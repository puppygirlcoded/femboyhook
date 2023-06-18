#include "grenade_prediction.h"

enum nade_throw_act {
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP
};

void grenade_prediction_t::predict( c_user_cmd* ucmd )
{
	//	readability
	constexpr float restitution = 0.45f;
	constexpr float power[]     = { 1.0f, 1.0f, 0.5f, 0.0f };
	constexpr float velocity    = 403.0f * 0.9f;

	float step, gravity, new_velocity, unk01;
	int index{ }, grenade_act{ 1 };
	c_vector pos, thrown_direction, start, eye_origin;
	c_vector angles, thrown;

	//	first time setup
	static auto sv_gravity = interfaces.m_console->get_convar( xs("sv_gravity") );

	//	calculate step and actual gravity value
	gravity = sv_gravity->get_float( ) / 8.0f;
	step    = memory.m_globals->m_interval_per_tick;

	//	get local view and eye origin
	eye_origin = globals.m_local->get_eye_pos( );
	angles     = ucmd->m_view_point;

	//	copy current angles and normalise pitch
	thrown = angles;

	if ( thrown.m_x < 0 ) {
		thrown.m_x = -10 + thrown.m_x * ( ( 90 - 10 ) / 90.0f );
	} else {
		thrown.m_x = -10 + thrown.m_x * ( ( 90 + 10 ) / 90.0f );
	}

	//	find out how we're throwing the grenade
	auto primary_attack   = ucmd->m_buttons & in_attack;
	auto secondary_attack = ucmd->m_buttons & in_attack2;

	if ( primary_attack && secondary_attack ) {
		grenade_act = ACT_LOB;
	} else if ( secondary_attack ) {
		grenade_act = ACT_DROP;
	}

	//	apply 'magic' and modulate by velocity
	unk01 = power[ grenade_act ];

	unk01 = unk01 * 0.7f;
	unk01 = unk01 + 0.3f;

	new_velocity = velocity * unk01;

	//	here's where the fun begins
	math.angle_vectors( thrown, &thrown_direction );

	start            = eye_origin + thrown_direction * 16.0f;
	thrown_direction = ( thrown_direction * new_velocity ) + globals.m_local->velocity( );

	//	let's go ahead and predict
	for ( auto time = 0.0f; index < 500; time += step ) {
		pos = start + thrown_direction * step;

		//	setup trace
		trace_t trace;
		trace_filter filter;
		filter.skip = globals.m_local;

		interfaces.m_trace->run( ray_t{ start, pos }, MASK_SOLID, &filter, &trace );

		//	modify path if we have hit something
		if ( trace.fraction != 1.0f ) {
			thrown_direction = trace.plane.normal * -2.0f * thrown_direction.dot_product( trace.plane.normal ) + thrown_direction;

			thrown_direction *= restitution;

			pos = start + thrown_direction * trace.fraction * step;

			time += ( step * ( 1.0f - trace.fraction ) );
		}

		//	check for detonation
		auto detonate = detonated( reinterpret_cast<c_weapon*>(globals.m_local->get_active_weapon( )), time, trace );

		//	emplace nade point
		_points.at( index++ ) = c_nadepoint( start, pos, trace.fraction != 1.0f, true, trace.plane.normal, detonate );
		start                 = pos;

		//	apply gravity modifier
		thrown_direction.m_z -= gravity * trace.fraction * step;

		if ( detonate ) {
			break;
		}
	}

	//	invalidate all empty points and finish prediction
	for ( auto n = index; n < 500; ++n ) {
		_points.at( n ).m_valid = false;
	}

	_predicted = true;
}

bool grenade_prediction_t::detonated( c_weapon* weapon, float time, trace_t& trace )
{
	if ( !weapon ) {
		return true;
	}

	//	get weapon item index
	const auto index = weapon->item_definition_index( );

	switch ( index ) {
		//	flash and HE grenades only live up to 2.5s after thrown
	case 43:
	case 44:
		if ( time > 2.5f ) {
			return true;
		}
		break;

		//	fire grenades detonate on ground hit, or 3.5s after thrown
	case WEAPON_MOLOTOV:
	case 48:
		if ( trace.fraction != 1.0f && trace.plane.normal.m_z > 0.7f || time > 3.5f ) {
			return true;
		}
		break;

		//	decoy and smoke grenades were buggy in prediction, so i used this ghetto work-around
	case WEAPON_DECOY:
	case 45:
		if ( time > 2.5f ) {
			return true;
		}
		break;
	}

	return false;
}

void grenade_prediction_t::on_create_move( c_user_cmd* ucmd )
{
	if ( !variables.misc.local.grenade_prediction )
		return;

	if ( !( ucmd->m_buttons & in_attack ) && !( ucmd->m_buttons & in_attack2 ) ) {
		_predicted = false;
		return;
	}

	const static std::vector< int > nades{ WEAPON_FLASHBANG, WEAPON_SMOKEGRENADE, WEAPON_HEGRENADE, WEAPON_MOLOTOV, WEAPON_DECOY, WEAPON_INCGRENADE };

	//	grab local weapon
	auto weapon = globals.m_local->get_active_weapon( );

	if ( !weapon ) {
		return;
	}

	if ( std::find( nades.begin( ), nades.end( ), weapon->item_definition_index( ) ) != nades.end( ) ) {
		return predict( ucmd );
	}

	_predicted = false;
}

void grenade_prediction_t::on_end_scene( )
{
	if ( !variables.misc.local.grenade_prediction )
		return;

	if ( !interfaces.m_engine->is_in_game( ) || !globals.m_local || !globals.m_local->is_alive( ) )
		return;

	ImVec2 start, end;

	//	draw nade path
	if ( _predicted ) {
		for ( auto& p : _points ) {
			if ( !p.m_valid ) {
				break;
			}

			if ( render.world_to_screen( p.m_start, start ) && render.world_to_screen( p.m_end, end ) ) {
				render.line( ImGui::GetBackgroundDrawList( ), c_vector_2d( start.x, start.y ), c_vector_2d( end.x, end.y ),
				             variables.misc.local.grenade_prediction_color, 2.f );
				if ( p.m_detonate )
					render.circle_filled_3d( p.m_end, 90.f, variables.misc.local.grenade_prediction_color );
			}
		}
	}
}
