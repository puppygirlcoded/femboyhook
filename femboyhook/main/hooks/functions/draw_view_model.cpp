#include "../hooks.h"

#include "../../features/visuals/motion_blur/motion_blur.h"

void __fastcall n_detoured_functions::draw_view_model( void* ecx, void* edx, c_view_setup& setup, bool draw_viewmodel, bool draw_scope_lens_mask ) { 
	static auto original = hooks.draw_view_model.get_original< decltype( &n_detoured_functions::draw_view_model ) >( );

	if (!variables.visuals.motion_blur.render_on_viewmodel)
		motion_blur.simulate( setup );

	return original( ecx, edx, setup, draw_viewmodel, draw_scope_lens_mask );
}