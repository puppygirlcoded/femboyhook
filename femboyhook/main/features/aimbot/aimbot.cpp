#include "../../includes.h"
#include "../cache/cache.h"

#include "../variables.h"

#include "aimbot.h"

aimbot_value_t aimbot_t::get_current_settings( )
{
	const auto general_settings = variables.aimbot.general;
	const auto rifle_settings   = variables.aimbot.rifles;
	const auto sniper_settings  = variables.aimbot.snipers;
	const auto pistol_settings  = variables.aimbot.pistols;

	const auto active_weapon = globals.m_local->get_active_weapon( );
	if ( !active_weapon ) {
		return general_settings;
	}

	const auto weapon_type = active_weapon->get_weapon_type( );
	if ( !weapon_type ) {
		return general_settings;
	}

	const auto is_rifle  = weapon_type == WEAPON_TYPE_RIFLE;
	const auto is_sniper = weapon_type == WEAPON_TYPE_SNIPER;
	const auto is_pistol = weapon_type == WEAPON_TYPE_PISTOL;

	if ( is_rifle && rifle_settings.override_general ) {
		return rifle_settings;
	}

	if ( is_sniper && sniper_settings.override_general ) {
		return sniper_settings;
	}

	if ( is_pistol && pistol_settings.override_general ) {
		return pistol_settings;
	}

	return general_settings;
}

int aimbot_t::get_best_bone( c_base_entity* entity, float fov )
{
	auto best_hitbox = -1;
	auto best_fov    = fov;

	const auto hitbox_selection = get_current_settings( ).hitboxes;

	std::vector< int > hitboxes;

	// TODO: add more bones

	// head
	if ( hitbox_selection & ( 1 << 0 ) ) {
		// actual head
		hitboxes.emplace_back( 0 );
		// neck
		hitboxes.emplace_back( 1 );
	}

	// chest
	if ( hitbox_selection & ( 1 << 1 ) ) {
		hitboxes.emplace_back( 5 );
		hitboxes.emplace_back( 6 );
	}

	const auto eye_pos = globals.m_local->get_eye_pos( );

	for ( auto& bone : hitboxes ) {
		c_vector aim_pos = entity->get_hitbox_position( bone );

		if ( aim_pos.is_zero( ) ) {
			continue;
		}

		auto aim_angles = math.vector_angles( eye_pos, aim_pos );

		if ( !math.normalize_angles( aim_angles ) ) {
			continue;
		}

		if ( !globals.m_local->can_see_player( entity, aim_pos ) && this->get_current_settings( ).target_visible_only ) {
			continue;
		}

		const auto current_fov = math.calculate_field_of_view( eye_pos, aim_angles, globals.m_cmd->m_view_point );

		if ( current_fov != INT_MAX && current_fov != INT_MIN ) {
			best_hitbox = bone;
			break;
		}

		if ( current_fov < best_fov ) {
			best_hitbox = bone;
			best_fov    = current_fov;
		}
	}

	return best_hitbox;
}

target_info_t aimbot_t::get_best_player( c_base_entity* entity, float fov, const int target_selection_type )
{
	int best_distance = INT_MAX;
	int best_health   = INT_MAX;
	int best_fov      = fov;

	if ( !entity )
		return { -1, nullptr };

	const auto entity_index = entity->index( );
	const auto eye_pos      = globals.m_local->get_eye_pos( );

	const auto distance        = ( entity->origin( ) - globals.m_local->origin( ) ).length( );
	const auto health          = entity->health( );
	const auto best_bone       = this->get_best_bone( entity, fov );
	const auto hitbox_position = entity->get_hitbox_position( best_bone );
	const auto field_of_view   = math.calculate_field_of_view( eye_pos, hitbox_position, globals.m_cmd->m_view_point );

	switch ( target_selection_type ) {
	case 0: // distance
		if ( distance < best_distance ) {
			best_distance = distance;
			return { entity_index, entity };
		}
		break;
	case 1: // fov
		if ( field_of_view < best_fov ) {
			best_fov = field_of_view;
			return { entity_index, entity };
		}
		break;
	case 2: // health
		if ( health < best_health ) {
			best_health = health;
			return { entity_index, entity };
		}
		break;
	}

	return { -1, nullptr };
}

void aimbot_t::target_players( )
{
	if ( !variables.aimbot.enabled || !variables.aimbot.activation_key.get_key_state( ) )
		return;

	const auto active_weapon = globals.m_local->get_active_weapon( );
	if ( !active_weapon ) {
		return;
	}

	const auto weapon_type = active_weapon->get_weapon_type( );
	if ( !weapon_type ) {
		return;
	}

	const auto current_settings      = this->get_current_settings( );
	const auto target_selection_type = variables.aimbot.target_selection;
	const auto eye_pos               = globals.m_local->get_eye_pos( );

	if ( weapon_type == WEAPON_TYPE_KNIFE ) {
		return;
	}

	auto line_goes_through_smoke = [ & ]( c_vector start, c_vector end ) -> bool {
		static auto fn = ( bool ( * )( c_vector m_start, c_vector m_end ) )memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0" ) );

		return fn( start, end );
	};

	cache.enumerate( e_cache_types::cache_type_player, [ & ]( c_base_entity* entity ) {
		if ( !entity )
			return;

		if ( !entity->is_alive( ) ) {
			return;
		}

		if ( entity->team( ) == globals.m_local->team( ) && !variables.aimbot.target_team )
			return;

		const auto entity_index      = entity->index( );
		const auto best_player_index = this->get_best_player( entity, current_settings.field_of_view, target_selection_type );

		if ( entity_index != best_player_index.entity_index ) {
			return;
		}

		if ( !( entity->flags( ) & fl_on_ground ) && !current_settings.target_enemy_in_air )
			return;

		if ( !( globals.m_local->flags( ) & fl_on_ground ) && !current_settings.target_while_in_air )
			return;

		if ( globals.m_local->flash_duration( ) > 1.f && !current_settings.target_while_flashed )
			return;

		const auto best_bone = this->get_best_bone( entity, current_settings.field_of_view );
		if ( best_bone == -1 ) {
			return;
		}

		const auto target_position = entity->get_hitbox_position( best_bone );
		if ( target_position.is_zero( ) ) {
			return;
		}

		if ( line_goes_through_smoke( eye_pos, target_position ) && !current_settings.target_through_smoke )
			return;

		auto calculated_angle = math.calculate_angle( eye_pos, target_position, globals.m_cmd->m_view_point ).clamped( );

		if ( current_settings.smoothness != 0 ) {
			calculated_angle /= current_settings.smoothness;
		}

		const auto recoil_scale = interfaces.m_console->get_convar( xs( "weapon_recoil_scale" ) )
		                              ->get_float( );
		// we dont wanna compensate again or we're just gonna miss lolz
		if ( current_settings.compensate_recoil && !current_settings.compensation.enabled ) {
			if ( current_settings.smoothness != 0 )
				calculated_angle -= ( globals.m_local->aim_punch_angle( ) * ( recoil_scale * ( current_settings.recoil_compensation.m_x / 100.f ),
				                                              recoil_scale * ( current_settings.recoil_compensation.m_y / 100.f ) ) ) /
				                    current_settings.smoothness;
			else
				calculated_angle -= ( globals.m_local->aim_punch_angle( ) * ( recoil_scale * ( current_settings.recoil_compensation.m_x / 100.f ),
				                                                              recoil_scale * ( current_settings.recoil_compensation.m_y / 100.f ) ) );
		}

		globals.m_cmd->m_view_point += calculated_angle.clamped( );

		if ( !current_settings.silent ) {
			interfaces.m_engine->set_view_angles( globals.m_cmd->m_view_point );
		}
	} );
}

void aimbot_t::on_createmove( )
{
	/* standalone compensation */
	[ & ]( ) {
		const auto current_settings = this->get_current_settings( );

		if ( !current_settings.compensation.enabled || !( globals.m_cmd->m_buttons & in_attack ) )
			return;
		const auto recoil_scale = interfaces.m_console->get_convar( xs( "weapon_recoil_scale" ) )->get_float( );
		const auto recoil       = ( globals.m_local->aim_punch_angle( ) * ( recoil_scale * ( current_settings.recoil_compensation.m_x / 100.f ),
                                                                      recoil_scale * ( current_settings.recoil_compensation.m_y / 100.f ) ) );

		globals.m_cmd->m_view_point -= c_vector( recoil.m_x, recoil.m_y, 0.f );

		if ( !current_settings.compensation.silent ) {
			interfaces.m_engine->set_view_angles( globals.m_cmd->m_view_point );
		}
	}( );

	this->target_players( );
}