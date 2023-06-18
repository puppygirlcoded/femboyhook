#include "../../features/misc/misc.h"
#include "../../features/visuals/edicts/edicts.h"
#include "../../features/skins/skins.h"
#include "../hooks.h"

void __fastcall n_detoured_functions::frame_stage_notify( void* thisptr, int edx, e_client_frame_stage stage )
{
	static auto original = hooks.frame_stage_notify.get_original< decltype( &n_detoured_functions::frame_stage_notify ) >( );

	misc.on_frame_stage_notify( );

	if ( !interfaces.m_engine->is_in_game( ) )
		return original( thisptr, edx, stage );

	c_base_entity* local = globals.m_local =
		reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( interfaces.m_engine->get_local_player( ) ) );
	if ( !local )
		return original( thisptr, edx, stage );

	if ( stage == e_client_frame_stage::start ) {
		render.update_world_to_screen_matrix( );
	}

	edicts.on_frame_stage_notify( ( int )stage );
	skins.on_frame_stage_notify( ( int )stage );

	original( thisptr, edx, stage );
}