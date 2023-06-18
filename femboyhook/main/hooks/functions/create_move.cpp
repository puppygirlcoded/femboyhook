#include "../hooks.h"

#include "../../features/aimbot/aimbot.h"
#include "../../features/backtrack/backtrack.h"
#include "../../features/misc/misc.h"
#include "../../features/movement/indicators/indicators.h"
#include "../../features/movement/movement.h"
#include "../../features/prediction/prediction.h"
#include "../../features/visuals/prediction/grenade_prediction.h"

static void __stdcall create_move( int sequence_number, float input_sample_frametime, bool is_active, bool* send_packet )
{
	static auto original = hooks.create_move_proxy.get_original< decltype( &n_detoured_functions::create_move_proxy ) >( );

	original( interfaces.m_client, 0, sequence_number, input_sample_frametime, is_active );

	c_user_cmd* cmd                   = memory.m_input->get_user_cmd( sequence_number );
	c_verified_user_cmd* verified_cmd = memory.m_input->get_verified_cmd( sequence_number );

	if ( !cmd || !verified_cmd || !is_active )
		return;

	globals.m_cmd = cmd;

	[ & ]( ) {
		if ( !globals.m_local || !globals.m_local->is_alive( ) || !globals.m_cmd || !cmd )
			return;

		misc.on_create_move( );
		indicators.on_create_move( );
		grenade_prediction.on_create_move( globals.m_cmd );
		movement.on_pre_create_move( );

		lag_comp.simulate( cmd );
		lag_comp.clear_incoming_sequences( );

		prediction_system.pre.set_pre_data( globals.m_local, cmd );
		if ( prediction_system.simulate( globals.m_local, cmd ) ) {
			aimbot.on_createmove( );

			prediction_system.end_simulate( globals.m_local );
			prediction_system.post.set_post_data( globals.m_local, cmd );
		}

		movement.on_post_create_move( );
	}( );

	cmd->m_view_point.normalize( );
	cmd->m_view_point.clamp( );

	globals.m_send_packet = send_packet;

	cmd->m_view_point_copy = cmd->m_view_point;
	cmd->m_buttons_copy    = cmd->m_buttons;

	verified_cmd->m_user_cmd = *cmd;
	verified_cmd->m_hash_crc = cmd->get_checksum( );
}

__declspec( naked ) void __fastcall n_detoured_functions::create_move_proxy( void* thisptr, int edx, int sequence_number,
                                                                             float input_sample_frametime, bool is_active )
{
	__asm
	{
		push	ebp
		mov		ebp, esp; 
		push	ebx; 
		push	esp; 
		push	dword ptr[is_active]; 
		push	dword ptr[input_sample_frametime]; 
		push	dword ptr[sequence_number];
		call	create_move
		pop		ebx
		pop		ebp
		retn	0Ch
	}
}