#include "../hooks.h"

#include "../../features/misc/misc.h"
#include "../../features/movement/indicators/indicators.h"
#include "../../features/movement/movement.h"
#include "../../features/visuals/prediction/grenade_prediction.h"
#include "../../menu/menu.h"
struct vertex_t {
	float m_x, m_y, m_z, m_rhw;
	std::uint32_t m_color;
};
std::uint32_t to_d3d( const c_color& col )
{
	return D3DCOLOR_RGBA( col.m_r, col.m_g, col.m_b, col.m_a );
}
long __stdcall n_detoured_functions::end_scene( IDirect3DDevice9* device )
{
	static auto original      = hooks.end_scene.get_original< decltype( &n_detoured_functions::end_scene ) >( );
	static void* used_address = nullptr;
	static auto return_adr    = _ReturnAddress( );

	// make sure we just draw once per all endscene calls thru the game
	if ( _ReturnAddress( ) == return_adr )
		return end_scene( device );

	if ( !used_address ) {
		MEMORY_BASIC_INFORMATION memory_basic_information = { };

		VirtualQuery( _ReturnAddress( ), &memory_basic_information, sizeof( MEMORY_BASIC_INFORMATION ) );

		char module_name[ MAX_PATH ] = { };
		GetModuleFileName( static_cast< HMODULE >( memory_basic_information.AllocationBase ), module_name, MAX_PATH );

		if ( strstr( module_name, xs( "gameoverlayrenderer.dll" ) ) != nullptr )
			used_address = _ReturnAddress( );
	}

	if ( _ReturnAddress( ) != used_address )
		return original( device );

	render.on_end_scene(
		[ & ]( ) {
			menu.on_end_scene( );
			misc.on_end_scene( );
			grenade_prediction.on_end_scene( );
			indicators.on_end_scene( );
			movement.on_end_scene( );
		},
		device );

	return end_scene( device );
}
