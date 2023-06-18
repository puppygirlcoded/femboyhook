#include "../hooks.h"

void __fastcall n_detoured_functions::particle_collection_simulate( c_particle_collection* this_ptr, void* edx )
{
	static auto original = hooks.particle_collection_simulate.get_original< decltype( &n_detoured_functions::particle_collection_simulate ) >( );

	if ( !interfaces.m_engine->is_in_game( ) ) {
		original( this_ptr, edx );
		return;
	}

	original( this_ptr, edx );

	c_particle_collection* root_colection = this_ptr;
	while ( root_colection->parent )
		root_colection = root_colection->parent;

	char* root_variable_name = root_colection->definition.object->name.buffer;

	switch ( fnv1a::hash( root_variable_name ) ) {
	case fnv1a::hash_const( ( "molotov_groundfire" ) ):
	case fnv1a::hash_const( ( "molotov_groundfire_00MEDIUM" ) ):
	case fnv1a::hash_const( ( "molotov_groundfire_00HIGH" ) ):
	case fnv1a::hash_const( ( "molotov_groundfire_fallback" ) ):
	case fnv1a::hash_const( ( "molotov_groundfire_fallback2" ) ):
	case fnv1a::hash_const( ( "molotov_explosion" ) ):
	case fnv1a::hash_const( ( "explosion_molotov_air" ) ):
	case fnv1a::hash_const( ( "extinguish_fire" ) ):
	case fnv1a::hash_const( ( "weapon_molotov_held" ) ):
	case fnv1a::hash_const( ( "weapon_molotov_fp" ) ):
	case fnv1a::hash_const( ( "weapon_molotov_thrown" ) ):
	case fnv1a::hash_const( ( "incgrenade_thrown_trail" ) ):
		for ( int i = 0; i < this_ptr->active_particles; i++ ) {
			float* color = this_ptr->particle_attributes.float_attribute_ptr( PARTICLE_ATTRIBUTE_TINT_RGB, i );

			float molotov_color[ 4 ] = { variables.visuals.world.modulate_fire_color.m_r / 255.f,
				                         variables.visuals.world.modulate_fire_color.m_g / 255.f,
				                         variables.visuals.world.modulate_fire_color.m_b / 255.f, 1.f };

			if ( variables.visuals.world.modulate_fire ) {
				color[ 0 ] = molotov_color[ 0 ];
				color[ 4 ] = molotov_color[ 1 ];
				color[ 8 ] = molotov_color[ 2 ];
			}
		}
		break;

	case fnv1a::hash_const( ( "explosion_smokegrenade" ) ):
	case fnv1a::hash_const( ( "explosion_smokegrenade_fallback" ) ):
		for ( int i = 0; i < this_ptr->active_particles; i++ ) {
			float* color = this_ptr->particle_attributes.float_attribute_ptr( PARTICLE_ATTRIBUTE_TINT_RGB, i );

			float smoke_color[ 4 ] = { variables.visuals.world.modulate_smoke_color.m_r / 255.f,
				                       variables.visuals.world.modulate_smoke_color.m_g / 255.f,
				                       variables.visuals.world.modulate_smoke_color.m_b / 255.f, 1.f };

			if ( variables.visuals.world.modulate_smoke ) {
				color[ 0 ] = smoke_color[ 0 ];
				color[ 4 ] = smoke_color[ 1 ];
				color[ 8 ] = smoke_color[ 2 ];
			}
		}
		break;
	case fnv1a::hash_const( ("blood_impact_basic") ):
	case fnv1a::hash_const( ("blood_impact_basic_fallback") ):
	case fnv1a::hash_const( ("blood_impact_chunks1") ):
	case fnv1a::hash_const( ("blood_impact_drops1") ):
	case fnv1a::hash_const( ("blood_impact_dust") ):
	case fnv1a::hash_const( ("blood_impact_goop_heavy") ):
	case fnv1a::hash_const( ("blood_impact_goop_light") ):
	case fnv1a::hash_const( ("blood_impact_goop_medium") ):
	case fnv1a::hash_const( ("blood_impact_green_01") ):
	case fnv1a::hash_const( ("blood_impact_headshot") ):
	case fnv1a::hash_const( ("blood_impact_headshot_01b") ):
	case fnv1a::hash_const( ("blood_impact_headshot_01c") ):
	case fnv1a::hash_const( ("blood_impact_headshot_01d") ):
	case fnv1a::hash_const( ("blood_impact_headshot_01e") ):
	case fnv1a::hash_const( ("blood_impact_heavy") ):
	case fnv1a::hash_const( ("blood_impact_light") ):
	case fnv1a::hash_const( ("blood_impact_light_headshot") ):
	case fnv1a::hash_const( ("blood_impact_medium") ):
	case fnv1a::hash_const( ("blood_impact_mist1") ):
	case fnv1a::hash_const( ("blood_impact_mist1_light") ):
	case fnv1a::hash_const( ("blood_impact_mist_heavy") ):
	case fnv1a::hash_const( ("blood_impact_red_01") ):
	case fnv1a::hash_const( ("blood_impact_red_01_backspray") ):
	case fnv1a::hash_const( ("blood_impact_red_01_chunk") ):
	case fnv1a::hash_const( ("blood_impact_red_01_drops") ):
	case fnv1a::hash_const( ("blood_impact_red_01_goop_a") ):
	case fnv1a::hash_const( ("blood_impact_red_01_goop_a_backup") ):
	case fnv1a::hash_const( ("blood_impact_red_01_goop_b") ):
	case fnv1a::hash_const( ("blood_impact_red_01_goop_c") ):
	case fnv1a::hash_const( ("blood_impact_red_01_mist") ):
	case fnv1a::hash_const( ("blood_impact_yellow_01") ):
	case fnv1a::hash_const( ("blood_pool") ):
	case fnv1a::hash_const( ("explosion_screen_c4_blood") ):
	case fnv1a::hash_const( ("explosion_screen_c4_blood2") ):
	case fnv1a::hash_const( ("explosion_screen_c4_blood_OLD") ):
	case fnv1a::hash_const( ("explosion_screen_c4_blood_distance_culled") ):
	case fnv1a::hash_const( ("impact_blood_glow") ):
	case fnv1a::hash_const( ("impact_blood_glow_light") ):
	case fnv1a::hash_const( ("impact_blood_glow_light_master") ):
	case fnv1a::hash_const( ("impact_blood_glow_normal_master") ):
	case fnv1a::hash_const( ("screen_blood_splatter") ):
	case fnv1a::hash_const( ("screen_blood_splatter_a") ):
	case fnv1a::hash_const( ("screen_blood_splatter_b") ):
		for ( int i = 0; i < this_ptr->active_particles; i++ ) {
			float* color = this_ptr->particle_attributes.float_attribute_ptr( PARTICLE_ATTRIBUTE_TINT_RGB, i );

			float blood_color[ 4 ] = { variables.visuals.world.modulate_blood_color.m_r / 255.f,
				                       variables.visuals.world.modulate_blood_color.m_g / 255.f,
				                       variables.visuals.world.modulate_blood_color.m_b / 255.f, 1.f };

			if ( variables.visuals.world.modulate_blood ) {
				color[ 0 ] = blood_color[ 0 ];
				color[ 4 ] = blood_color[ 1 ];
				color[ 8 ] = blood_color[ 2 ];
			}
		}
		break;

	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_fallback2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_main_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_main_sub1") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_main_sub2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_main_sub3") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_parent_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_silenced_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_vent") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_vent_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_vent_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_vent_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assaultrifle_vent_fire") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_assualtrifle_flame") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_fallback2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_main2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_autoshotgun_main3") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_awp") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_awp_tracer") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_main2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_main_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_huntingrifle_vent1") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_b") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_fallback2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_flash") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_main2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_main_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_tracers") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_para_vent1") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_core") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_core2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_elite") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_elite_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_fallback2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_main2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_main2_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_pistol_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_glow2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_glow3") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_shotgun_main2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_backup") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_fallback2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_glow") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_glow2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_glow3") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_glow_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_main") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_main2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_main_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_parent") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_parent_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_silenced") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_silenced_FP") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smg_silenced_FP_cheap") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smoke_medium") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smoke_small") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smoke_small2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smoke_small3") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smoke_small4") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_smoke_smallest") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks2") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks3") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks4") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks5") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks6") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_sparks7") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_taser") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_taser_fallback") ):
	case fnv1a::hash_const( ("weapon_muzzle_flash_taser_fallback2") ):
	case fnv1a::hash_const( ("weapon_muzzle_smoke") ):
	case fnv1a::hash_const( ("weapon_muzzle_smoke_b") ):
	case fnv1a::hash_const( ("weapon_muzzle_smoke_bVersion#2") ):
	case fnv1a::hash_const( ("weapon_muzzle_smoke_long#2") ):
	case fnv1a::hash_const( ("weapon_muzzle_smoke_long_b#2") ):
	case fnv1a::hash_const( ("weapon_muzzleflash_illumination") ):
	case fnv1a::hash_const( ("weapon_muzzleflash_illumination_add") ):
	case fnv1a::hash_const( ("weapon_sensorgren_beeplight") ):
	case fnv1a::hash_const( ("weapon_sensorgren_beeplight_inner") ):
	case fnv1a::hash_const( ("weapon_sensorgren_debris") ):
	case fnv1a::hash_const( ("weapon_sensorgren_detlight") ):
	case fnv1a::hash_const( ("weapon_sensorgren_detlight_child01") ):
	case fnv1a::hash_const( ("weapon_sensorgren_detlight_child02") ):
	case fnv1a::hash_const( ("weapon_sensorgren_detonate") ):
	case fnv1a::hash_const( ("weapon_sensorgren_glow") ):
	case fnv1a::hash_const( ("weapon_sensorgren_glowring") ):
	case fnv1a::hash_const( ("weapon_sensorgren_smoke") ):
	case fnv1a::hash_const( ("weapon_sensorgren_spark_01") ):
	case fnv1a::hash_const( ("weapon_sensorgren_spark_02") ):
	case fnv1a::hash_const( ("weapon_sensorgren_spark_02b") ):
	case fnv1a::hash_const( ("weapon_sensorgren_spark_03") ):
	case fnv1a::hash_const( ("weapon_sensorgren_wiggly") ):
	case fnv1a::hash_const( ("weapon_shell_casing_50cal") ):
	case fnv1a::hash_const( ("weapon_shell_casing_50cal_bonus") ):
	case fnv1a::hash_const( ("weapon_shell_casing_50cal_fallback") ):
	case fnv1a::hash_const( ("weapon_shell_casing_9mm") ):
	case fnv1a::hash_const( ("weapon_shell_casing_9mm_backup") ):
	case fnv1a::hash_const( ("weapon_shell_casing_9mm_fallback") ):
	case fnv1a::hash_const( ("weapon_shell_casing_9mm_fp") ):
	case fnv1a::hash_const( ("weapon_shell_casing_9mm_fp_fallback") ):
	case fnv1a::hash_const( ("weapon_shell_casing_candycorn") ):
	case fnv1a::hash_const( ("weapon_shell_casing_minigun") ):
	case fnv1a::hash_const( ("weapon_shell_casing_minigun_fallback") ):
	case fnv1a::hash_const( ("weapon_shell_casing_rifle") ):
	case fnv1a::hash_const( ("weapon_shell_casing_rifle_fallback") ):
	case fnv1a::hash_const( ("weapon_shell_casing_shotgun") ):
	case fnv1a::hash_const( ("weapon_shell_casing_shotgun_fallback") ):
	case fnv1a::hash_const( ("weapon_shell_casing_super_trail") ):
	case fnv1a::hash_const( ("weapon_shell_casing_super_trail_base") ):
	case fnv1a::hash_const( ("weapon_shell_casing_super_trail_glow") ):
	case fnv1a::hash_const( ("weapon_shell_casing_super_trail_smoke") ):
	case fnv1a::hash_const( ("weapon_shell_casing_super_trail_sparks") ):
	case fnv1a::hash_const( ("weapon_shell_casing_super_trail_sparks2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_assrifle2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_assrifle3") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_awp2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_awp3") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_para2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_para3") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_pistol1") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_pistol2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_pistol3") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_pistol_elite") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_rifle2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_rifle3") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_shotgun1") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_shotgun2") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_shotgun3") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_smg1") ):
	case fnv1a::hash_const( ("weapon_shell_eject_smoke_smg3") ):
	case fnv1a::hash_const( ("weapon_taser_flash") ):
	case fnv1a::hash_const( ("weapon_taser_flash_impact") ):
	case fnv1a::hash_const( ("weapon_taser_glow") ):
	case fnv1a::hash_const( ("weapon_taser_glow_impact") ):
	case fnv1a::hash_const( ("weapon_taser_sparks") ):
	case fnv1a::hash_const( ("weapon_taser_sparks_impact") ):
	case fnv1a::hash_const( ("weapon_tracers") ):
	case fnv1a::hash_const( ("weapon_tracers_50cal") ):
	case fnv1a::hash_const( ("weapon_tracers_50cal_glow") ):
	case fnv1a::hash_const( ("weapon_tracers_50cal_low") ):
	case fnv1a::hash_const( ("weapon_tracers_50cal_low_glow") ):
	case fnv1a::hash_const( ("weapon_tracers_assrifle") ):
	case fnv1a::hash_const( ("weapon_tracers_backup") ):
	case fnv1a::hash_const( ("weapon_tracers_mach") ):
	case fnv1a::hash_const( ("weapon_tracers_original") ):
	case fnv1a::hash_const( ("weapon_tracers_pistol") ):
	case fnv1a::hash_const( ("weapon_tracers_rifle") ):
	case fnv1a::hash_const( ("weapon_tracers_shot") ):
	case fnv1a::hash_const( ("weapon_tracers_smg") ):
	case fnv1a::hash_const( ("weapon_tracers_taser") ):
	case fnv1a::hash_const( ("weapon_tracers_taser_fallback") ):
	case fnv1a::hash_const( ("weapon_tracers_taser_fallback2") ):
	case fnv1a::hash_const( ("weapon_tracers_taser_wire1a") ):
	case fnv1a::hash_const( ("weapon_tracers_taser_wire1b") ):
	case fnv1a::hash_const( ("weapon_tracers_taser_wire2") ):
	case fnv1a::hash_const( ("ricochet_sparks") ):
	case fnv1a::hash_const( ("ricochet_sparks_contrast_glow") ):
	case fnv1a::hash_const( ("ricochet_sparks_contrast_glow_soft") ):
	case fnv1a::hash_const( ("ricochet_sparks_contrast_shadow") ):
	case fnv1a::hash_const( ("effects/tesla_glow_noz") ):
		for ( int i = 0; i < this_ptr->active_particles; i++ ) {
			float* color = this_ptr->particle_attributes.float_attribute_ptr( PARTICLE_ATTRIBUTE_TINT_RGB, i );

			float weapon_color[ 4 ] = { variables.visuals.world.modulate_weapon_effects_color.m_r / 255.f,
				                        variables.visuals.world.modulate_weapon_effects_color.m_g / 255.f,
				                        variables.visuals.world.modulate_weapon_effects_color.m_b / 255.f, 1.f };

			if ( variables.visuals.world.modulate_weapon_effects ) {
				color[ 0 ] = weapon_color[ 0 ];
				color[ 4 ] = weapon_color[ 1 ];
				color[ 8 ] = weapon_color[ 2 ];
			}
		}
		break;
	}
}