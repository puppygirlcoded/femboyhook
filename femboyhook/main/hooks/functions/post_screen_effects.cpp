#include "../hooks.h"

#include "../../features/visuals/motion_blur/motion_blur.h"
#include "../../features/visuals/players/players.h"

int __fastcall n_detoured_functions::post_screen_effects( void* ecx, uintptr_t edx, int effect_id )
{
	static auto original = hooks.post_screen_effects.get_original< decltype( &n_detoured_functions::post_screen_effects ) >( );

	if ( variables.visuals.motion_blur.render_on_viewmodel )
		motion_blur.simulate( *reinterpret_cast< c_view_setup* >( effect_id ) );

	players.on_post_screen_effects( );

	return original( ecx, edx, effect_id );
}