#include "../hooks.h"

#include "../../features/visuals/players/players.h"
#include "../../features/visuals/edicts/edicts.h"
#include "../../menu/menu.h"

void __fastcall n_detoured_functions::paint_traverse( c_surface* thisptr, int edx, unsigned int panel, bool force_repaint, bool force )
{
	static auto original = hooks.paint_traverse.get_original< decltype( &n_detoured_functions::paint_traverse ) >( );

	const unsigned int panel_hash = fnv1a::hash( interfaces.m_panel->get_panel_name( panel ) );

	original( thisptr, edx, panel, force_repaint, force );

	if ( panel_hash == fnv1a::hash_const( "FocusOverlayPanel" ) ) {
		render.clear_draw_data( );

		if ( globals.m_local ) {
			players.on_paint_traverse( );
			edicts.on_paint_traverse( );
		}
		render.swap_draw_data( );
	}

	if (panel_hash == fnv1a::hash_const("MatSystemTopPanel")) {
		menu.handle_spotify( );
	}

}