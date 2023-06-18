#pragma once

#include "../../hashing/other/crc32.h"
#include "c_angle.h"
#include "c_vector.h"

#pragma pack( push, 4 )
class c_user_cmd
{
public:
	virtual ~c_user_cmd( ) { }
	int m_command_number       = { };
	int m_tick_count           = { };
	c_vector m_view_point      = { };
	c_vector m_aim_direction   = { };
	float m_forward_move       = { };
	float m_side_move          = { };
	float m_up_move            = { };
	int m_buttons              = { };
	std::uint8_t m_impluse     = { };
	int m_weapon_select        = { };
	int m_weapon_sub_type      = { };
	int m_random_seed          = { };
	short m_mouse_delta_x      = { };
	short m_mouse_delta_y      = { };
	bool m_has_been_predicted  = { };
	c_vector m_view_point_copy = { };
	int m_buttons_copy         = { };
	int unk1                   = { };
	int unk2                   = { };

	[[nodiscard]] crc32_t get_checksum( ) const
	{
		crc32_t hash_crc = 0UL;

		crc32::init( &hash_crc );
		crc32::process_buffer( &hash_crc, &m_command_number, sizeof( m_command_number ) );
		crc32::process_buffer( &hash_crc, &m_tick_count, sizeof( m_tick_count ) );
		crc32::process_buffer( &hash_crc, &m_view_point, sizeof( m_view_point ) );
		crc32::process_buffer( &hash_crc, &m_aim_direction, sizeof( m_aim_direction ) );
		crc32::process_buffer( &hash_crc, &m_forward_move, sizeof( m_forward_move ) );
		crc32::process_buffer( &hash_crc, &m_side_move, sizeof( m_side_move ) );
		crc32::process_buffer( &hash_crc, &m_up_move, sizeof( m_up_move ) );
		crc32::process_buffer( &hash_crc, &m_buttons, sizeof( m_buttons ) );
		crc32::process_buffer( &hash_crc, &m_impluse, sizeof( m_impluse ) );
		crc32::process_buffer( &hash_crc, &m_weapon_select, sizeof( m_weapon_select ) );
		crc32::process_buffer( &hash_crc, &m_weapon_sub_type, sizeof( m_weapon_sub_type ) );
		crc32::process_buffer( &hash_crc, &m_random_seed, sizeof( m_random_seed ) );
		crc32::process_buffer( &hash_crc, &m_mouse_delta_x, sizeof( m_mouse_delta_x ) );
		crc32::process_buffer( &hash_crc, &m_mouse_delta_y, sizeof( m_mouse_delta_y ) );
		crc32::final( &hash_crc );

		return hash_crc;
	}
};
static_assert( sizeof( c_user_cmd ) == 0x64 );

class c_verified_user_cmd
{
public:
	c_user_cmd m_user_cmd = { };
	crc32_t m_hash_crc    = { };
};
static_assert( sizeof( c_verified_user_cmd ) == 0x68 );
#pragma pack( pop )