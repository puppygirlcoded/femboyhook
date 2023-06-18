#include "../hooks.h"

void __fastcall n_detoured_functions::override_view( void* edx, void* ecx, c_view_setup* setup )
{
	static auto original = hooks.override_view.get_original< decltype( &override_view ) >( );

	globals.m_view_setup = setup;

	if ( setup && interfaces.m_engine->is_in_game( ) && globals.m_local && globals.m_local->is_alive( ) ) {

		static float zoom_interp = setup->fov;
		const auto should_zoom   = variables.misc.helpers.zoom && variables.misc.helpers.zoom_key.get_key_state( );
		const float zoom_static  = should_zoom ? variables.misc.helpers.zoom_amount : variables.visuals.local.field_of_view;
		if ( zoom_interp != zoom_static )
			zoom_interp += ( zoom_static - zoom_interp ) * 0.1f;

		setup->fov = zoom_interp;
	}

	original( edx, ecx, setup );
}