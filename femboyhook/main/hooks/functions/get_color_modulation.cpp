#include "../hooks.h"

void __fastcall n_detoured_functions::get_color_modulation( void* ecx, void* edx, float* r, float* g, float* b ) { 
	static auto original = hooks.get_color_modulation.get_original< decltype( &n_detoured_functions::get_color_modulation ) >( );

	original( ecx, edx, r, g, b);

	const auto material = reinterpret_cast< c_material* >( ecx );
	if ( !material || material->is_error_material( ) )
		return original( ecx, edx, r, g, b );

	const auto group = fnv1a::hash( material->get_texture_group_name( ) );

	const auto is_sky   = group == fnv1a::hash( xs( "SkyBox textures" ) );
	const auto is_world = group == fnv1a::hash( xs( "World textures" ) ) && !is_sky;
	const auto is_prop  = group == fnv1a::hash( xs( "StaticProp textures" ) );

	static c_color last_world_color = variables.visuals.world.modulate_world_color;
	if (variables.visuals.world.modulate_world && is_world) {
		*r = variables.visuals.world.modulate_world_color.m_r / 255.f;
		*g = variables.visuals.world.modulate_world_color.m_g / 255.f;
		*b = variables.visuals.world.modulate_world_color.m_b / 255.f;
		last_world_color = variables.visuals.world.modulate_world_color;
	}

	static c_color last_prop_color = variables.visuals.world.modulate_props_color;
	if (variables.visuals.world.modulate_props && is_prop) {
		*r = variables.visuals.world.modulate_props_color.m_r / 255.f;
		*g = variables.visuals.world.modulate_props_color.m_g / 255.f;
		*b = variables.visuals.world.modulate_props_color.m_b / 255.f;
		last_prop_color = variables.visuals.world.modulate_props_color;
	}

	static c_color last_sky_color = variables.visuals.world.modulate_sky_color;
	if (variables.visuals.world.modulate_sky && is_sky) {
		*r = variables.visuals.world.modulate_sky_color.m_r / 255.f;
		*g = variables.visuals.world.modulate_sky_color.m_g / 255.f;
		*b = variables.visuals.world.modulate_sky_color.m_b / 255.f;
		last_sky_color = variables.visuals.world.modulate_sky_color;

	}
}