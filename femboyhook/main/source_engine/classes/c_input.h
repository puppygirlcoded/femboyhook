#pragma once
#include "c_user_cmd.h"

#define multiplayer_backup 150

class c_input
{
public:
	char pad0[ 0xC ];
	bool m_track_ir_available = { };
	bool m_mouse_initialized  = { };
	bool m_mouse_active       = { };
	char pad1[ 0x9A ];
	bool m_camera_in_third_person = { };
	char pad2[ 0x2 ];
	c_vector m_camera_offset = { };
	char pad3[ 0x38 ];
	c_user_cmd* m_commands                   = { };
	c_verified_user_cmd* m_verified_commands = { };

	[[nodiscard]] c_user_cmd* get_user_cmd( const int sequence_number ) const
	{
		return &m_commands[ sequence_number % multiplayer_backup ];
	}

	[[nodiscard]] c_verified_user_cmd* get_verified_cmd( const int sequence_number ) const
	{
		return &m_verified_commands[ sequence_number % multiplayer_backup ];
	}
};
static_assert( sizeof( c_input ) == 0xF8 );
