#include "../hooks.h"
#include "../../features/backtrack/backtrack.h"

//struct hit_matrix_entry_t {
//	int entity_index;
//	c_model_render_info info;
//	draw_model_state_t state;
//	matrix3x4_t bone_to_world[ 128 ] = { };
//	float m_time;
//	matrix3x4_t model_to_world;
//};
//
//static std::vector< hit_matrix_entry_t > hit_matrixes;
//
//void add_hitmatrix( c_base_entity* entity, matrix3x4_t* bones )
//{
//	if ( !entity || !bones ) {
//		return;
//	}
//
//	auto& hit = hit_matrixes.emplace_back( );
//
//	std::memcpy( hit.bone_to_world, bones, entity->get_cached_bone_data( ).count( ) * sizeof( matrix3x4_t ) );
//	hit.m_time = memory.m_globals->m_real_time + 1.f;
//
//	static auto m_skin = netvars.find_in_data_map( entity->prediction_desc_map( ), fnv1a::hash( "m_nSkin" ) );
//	static auto m_body = netvars.find_in_data_map( entity->prediction_desc_map( ), fnv1a::hash( "m_nBody" ) );
//
//	hit.info.origin = entity->origin( );
//	hit.info.angles     = entity->abs_eye_angles( );
//
//	auto renderable = entity->renderable( );
//
//	if (!renderable) {
//		return;
//	}
//
//	auto model = renderable->get_model( );
//	if (!model) {
//		return;
//	}
//
//	//auto hdr = *( studio_hdr_t** )( entity->studio_hdr( ) );
//}

void __fastcall n_detoured_functions::draw_model_execute( uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info,
                                                          matrix3x4_t* matrix )
{
	static auto original = hooks.draw_model_execute.get_original< decltype( &n_detoured_functions::draw_model_execute ) >( );

	static c_material* material = nullptr;
	static c_material* overlay  = nullptr;
	if ( !material ) {
		material = interfaces.m_material_system->find_material( xs( "debug/debugdrawflat" ), nullptr );
		overlay  = interfaces.m_material_system->find_material( xs( "dev/glow_armsrace" ), nullptr );

		material->increment_reference_count( );
		overlay->increment_reference_count( );
	}

	const auto entity_index = info->entity_index;

	if ( info->flags != 1 || strstr( info->model->name, xs( "models/player" ) ) == nullptr ) {
		original( ecx, edx, ctx, state, info, matrix );
		return;
	}

	auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( entity_index ) );

	if ( entity == nullptr || !entity->is_player( ) ) {
		original( ecx, edx, ctx, state, info, matrix );
		return;
	}

	if ( entity->life_state( ) != 0 || entity->team( ) == globals.m_local->team( ) || entity == globals.m_local ) {
		original( ecx, edx, ctx, state, info, matrix );
		return;
	}

	const float invisible_color[ 4 ] = { variables.players.chams.invisible_color.m_r / 255.f, variables.players.chams.invisible_color.m_g / 255.f,
		                                 variables.players.chams.invisible_color.m_b / 255.f, variables.players.chams.invisible_color.m_a / 255.f };

	const float visible_color[ 4 ] = { variables.players.chams.visible_color.m_r / 255.f, variables.players.chams.visible_color.m_g / 255.f,
		                               variables.players.chams.visible_color.m_b / 255.f, variables.players.chams.visible_color.m_a / 255.f };

	const float backtrack_color[ 4 ] = { variables.players.chams.backtrack_color.m_r / 255.f, variables.players.chams.backtrack_color.m_g / 255.f,
		                                 variables.players.chams.backtrack_color.m_b / 255.f, variables.players.chams.backtrack_color.m_a / 255.f };

	const float overlay_color[ 4 ] = { variables.players.chams.overlay_color.m_r / 255.f, variables.players.chams.overlay_color.m_g / 255.f,
		                               variables.players.chams.overlay_color.m_b / 255.f, variables.players.chams.overlay_color.m_a / 255.f };

	if (variables.players.chams.backtrack) {
		auto record = &lag_comp.records[ info->entity_index ];
		if (record && !record->empty() && record->size() && lag_comp.valid(record->back().simulation_time)) {
			material->color_modulate( backtrack_color[ 0 ], backtrack_color[ 1 ], backtrack_color[ 2 ] );
			material->alpha_modulate( backtrack_color[ 3 ] );
			material->set_material_var_flag( material_var_ignorez, true );

			interfaces.m_model_render->override_material( material );
			original( ecx, edx, ctx, state, info, record->back().matrix );
		}
	}

	if ( variables.players.chams.invisible ) {
		material->color_modulate( invisible_color[ 0 ], invisible_color[ 1 ], invisible_color[ 2 ] );
		material->alpha_modulate( invisible_color[ 3 ] );
		material->set_material_var_flag( material_var_ignorez, true );

		interfaces.m_model_render->override_material( material );
		original( ecx, edx, ctx, state, info, matrix );
	} else {
		interfaces.m_model_render->override_material( nullptr );
		original( ecx, edx, ctx, state, info, matrix );
	}

	if ( variables.players.chams.visible ) {
		material->color_modulate( visible_color[ 0 ], visible_color[ 1 ], visible_color[ 2 ] );
		material->alpha_modulate( visible_color[ 3 ] );
		material->set_material_var_flag( material_var_ignorez, false );

		interfaces.m_model_render->override_material( material );
		original( ecx, edx, ctx, state, info, matrix );
	} else {
		interfaces.m_model_render->override_material( nullptr );
		original( ecx, edx, ctx, state, info, matrix );
	}

	if ( variables.players.chams.overlay ) {
		auto tint = overlay->find_var( xs( "$envmaptint" ), nullptr );
		if ( tint ) {
			tint->set_vector_value( overlay_color[ 0 ], overlay_color[ 1 ], overlay_color[ 2 ] );
		}

		auto alpha = overlay->find_var( xs( "$alpha" ), nullptr );
		if ( alpha ) {
			alpha->set_float_value( overlay_color[ 3 ] );
		}

		auto envmap = overlay->find_var( xs( "$envmapfresnelminmaxexp" ), nullptr );
		if ( envmap ) {
			envmap->set_vector_value( 0.f, 1.f, std::clamp< float >( ( 100.0f - variables.players.chams.wireframe_brightness ) * 0.2f, 1.f, 20.f ) );
		}

		overlay->set_material_var_flag( material_var_wireframe, variables.players.chams.wireframe_overlay );
		overlay->set_material_var_flag( material_var_ignorez, false );

		interfaces.m_model_render->override_material( overlay );
		original( ecx, edx, ctx, state, info, matrix );
	}

	original( ecx, edx, ctx, state, info, matrix );
	interfaces.m_model_render->override_material( nullptr );
}