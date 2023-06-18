#include "edicts.h"
#include "../../../includes.h"
#include "../../../source_engine/enumerations/e_client_frame_stage.h"
#include "../../cache/cache.h"

void edicts_t::on_frame_stage_notify( int frame_stage )
{
	switch ( frame_stage ) {
	case (int)e_client_frame_stage::render_start: {
		/* sunset mode */
		[ & ]( ) {
			auto cl_csm_rot_override    = interfaces.m_console->get_convar( xs( "cl_csm_rot_override" ) );
			auto cl_csm_max_shadow_dist = interfaces.m_console->get_convar( xs( "cl_csm_max_shadow_dist" ) );
			auto cl_csm_rot_x           = interfaces.m_console->get_convar( xs( "cl_csm_rot_x" ) );
			auto cl_csm_rot_y           = interfaces.m_console->get_convar( xs( "cl_csm_rot_y" ) );

			if ( !variables.visuals.world.sunset_mode ) {
				cl_csm_rot_override->set_value( 0 );
				return;
			}

			cl_csm_max_shadow_dist->set_value( 800 );
			cl_csm_rot_override->set_value( 1 );
			cl_csm_rot_x->set_value( 0 );
			cl_csm_rot_y->set_value( 0 );
		}( );

	case ( int )e_client_frame_stage::render_end:
		/* wireframes */
		[ & ]( ) {
			std::array smoke_materials{ xs( "particle/vistasmokev1/vistasmokev1_emods" ), xs( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ),
				                        xs( "particle/vistasmokev1/vistasmokev1_fire" ), xs( "particle/vistasmokev1/vistasmokev1_smokegrenade" ) };

			for ( const auto p_mat : smoke_materials ) {
				const auto selected_material = interfaces.m_material_system->find_material( p_mat, xs( "Other textures" ) );
				selected_material->set_material_var_flag( material_var_wireframe, variables.visuals.world.wireframes & ( 1 << 0 ) );
			}

			std::array molotov_materials{ xs( "particle/fire_burning_character/fire_env_fire" ),
				                          xs( "particle/fire_burning_character/fire_env_fire_depthblend" ),
				                          xs( "particle/fire_burning_character/fire_burning_character_depthblend" ),
				                          xs( "particle/fire_burning_character/fire_burning_character" ),
				                          xs( "particle/fire_burning_character/fire_burning_character_nodepth" ),
				                          xs( "particle/particle_flares/particle_flare_001" ),
				                          xs( "particle/particle_flares/particle_flare_004" ),
				                          xs( "particle/particle_flares/particle_flare_004b_mod_ob" ),
				                          xs( "particle/particle_flares/particle_flare_004b_mod_z" ),
				                          xs( "particle/fire_explosion_1/fire_explosion_1_bright" ),
				                          xs( "particle/fire_explosion_1/fire_explosion_1b" ),
				                          xs( "particle/fire_particle_4/fire_particle_4" ),
				                          xs( "particle/fire_explosion_1/fire_explosion_1_oriented" ) };

			for ( const auto p_mat : molotov_materials ) {
				const auto selected_material = interfaces.m_material_system->find_material( p_mat, xs( "Other textures" ) );
				selected_material->set_material_var_flag( material_var_wireframe, variables.visuals.world.wireframes & ( 1 << 1 ) );
			}
			if ( variables.visuals.world.wireframes & ( 1 << 0 ) ) {
				static auto smoke_count =
					*reinterpret_cast< uint32_t** >( memory.m_modules[ e_module_names::client ].find_pattern( xs( "A3 ? ? ? ? 57 8B CB" ) ) + 1 );
				*( int* )smoke_count = 0;
			}
		}( );

	} break;
	}
}

void edicts_t::on_paint_traverse( )
{
	if ( !render.m_initialised )
		return;

	/* edicts esp */
	[&]() {
		for ( int i = 1; i <= interfaces.m_client_entity_list->get_highest_entity_index( ); i++ ) {
			auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( i ) );

			if ( !entity || entity->is_player( ) )
				continue;

			const auto client_class = entity->get_client_class( );
			if ( !client_class )
				continue;

			/* tone map - bloom */
			[ & ]( ) {
				if ( client_class->class_id != CEnvTonemapController )
					return;

				if ( !variables.visuals.world.bloom.enabled )
					return;

				const auto scale                  = variables.visuals.world.bloom.scale;
				const auto exposure               = variables.visuals.world.bloom.exposure;
				entity->use_custom_bloom_scale( ) = true;
				entity->custom_bloom_scale( )     = scale;

				entity->use_custom_auto_exposure_min( ) = true;
				entity->custom_auto_exposure_min( )     = exposure;

				entity->use_custom_auto_exposure_max( ) = true;
				entity->custom_auto_exposure_max( )     = exposure;
			}( );
		}
	}( );
}