#pragma once
#include "../../hashing/strings/xorstr.h"
#include "../../memory/memory.h"
#include "c_base_entity.h"
#include "c_user_cmd.h"

template< typename I >
__forceinline I v2( void* iface, uint32_t index )
{
	return ( I )( ( *( uint32_t** )iface )[ index ] );
}
class c_move_helper
{
private:
	virtual void unknown1( ) = 0;

public:
	virtual void set_host( c_base_entity* host ) = 0;

private:
	virtual void unknown2( ) = 0;
	virtual void unknown3( ) = 0;

public:
	virtual void process_impacts( ) = 0;
};

class player_move_data
{
public:
	bool first_run_of_instructions : 1;
	bool game_code_moved_player    : 1;
	int player_handle;
	int impulse_command;
	c_vector view_angles;
	c_vector abs_view_angles;
	int buttons;
	int old_buttons;
	float fw_move;
	float sd_move;
	float up_move;
	float max_speed;
	float client_max_speed;
	c_vector velocity;
	c_vector angles;
	c_vector old_angles;
	float step_height;
	c_vector wish_velocity;
	c_vector jump_velocity;
	c_vector constraint_center;
	float constraint_radius;
	float constraint_width;
	float constraint_speed_factor;
	float u0[ 5 ];
	c_vector abs_origin;
};

class virtual_game_movement
{
public:
	virtual ~virtual_game_movement( void ) { }
	virtual void process_movement( c_base_entity* player, player_move_data* move ) = 0;
	virtual void reset( void )                                                     = 0;
	virtual void start_track_prediction_errors( c_base_entity* player )            = 0;
	virtual void finish_track_prediction_errors( c_base_entity* player )           = 0;
	virtual void diff_print( char const* fmt, ... )                                = 0;
	virtual c_vector const& get_player_mins( bool ducked ) const                   = 0;
	virtual c_vector const& get_player_maxs( bool ducked ) const                   = 0;
	virtual c_vector const& get_player_view_offset( bool ducked ) const            = 0;
	virtual bool is_moving_player_stuck( void ) const                              = 0;
	virtual c_base_entity* get_moving_player( void ) const                         = 0;
	virtual void unblock_posher( c_base_entity* player, c_base_entity* pusher )    = 0;
	virtual void setup_movement_bounds( player_move_data* move )                   = 0;
};

class c_game_movement : public virtual_game_movement
{
public:
	virtual ~c_game_movement( void ) { }
};

class c_prediction
{
public:
	virtual ~c_prediction( void )                                                                      = 0;
	virtual void Init( void )                                                                          = 0;
	virtual void Shutdown( void )                                                                      = 0;
	virtual void Update( int startframe,            // World update ( un-modded ) most recently received
	                     bool validframe,           // Is frame data valid
	                     int incoming_acknowledged, // Last command acknowledged to have been run by server (un-modded)
	                     int outgoing_command       // Last command (most recent) sent to server (un-modded)
	                     )                                                                             = 0;
	virtual void PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet ) = 0;
	virtual void PostEntityPacketReceived( void )                                                      = 0;
	virtual void PostNetworkDataReceived( int commands_acknowledged )                                  = 0;
	virtual void OnReceivedUncompressedPacket( void )                                                  = 0;
	// The engine needs to be able to access a few predicted values
	virtual void GetViewOrigin( c_vector& org ) = 0;
	virtual void SetViewOrigin( c_vector& org ) = 0;
	virtual void GetViewAngles( c_vector& ang ) = 0;
	virtual void SetViewAngles( c_vector& ang ) = 0;

	virtual void get_local_view_angles( c_vector& ang ) = 0;
	virtual void set_local_view_angles( c_vector& ang ) = 0;

	__forceinline void run_command( c_base_entity* player, c_user_cmd* cmd, c_move_helper* helper )
	{
		return v2< void( __thiscall* )( void*, c_base_entity*, c_user_cmd*, c_move_helper* ) >( this, 19 )( this, player, cmd, helper );
	}
	__forceinline void check_moving_ground( c_base_entity* player, double frametime )
	{
		return v2< void( __thiscall* )( void*, c_base_entity*, double ) >( this, 18 )( this, player, frametime );
	}
	__forceinline void setup_move( c_base_entity* player, c_user_cmd* cmd, c_move_helper* helper, player_move_data* data )
	{
		return v2< void( __thiscall* )( void*, c_base_entity*, c_user_cmd*, c_move_helper*, void* ) >( this, 20 )( this, player, cmd, helper, data );
	}
	__forceinline void finish_move( c_base_entity* player, c_user_cmd* cmd, void* data )
	{
		return v2< void( __thiscall* )( void*, c_base_entity*, c_user_cmd*, void* ) >( this, 21 )( this, player, cmd, data );
	}
	__forceinline bool in_prediction( )
	{
		return v2< bool( __thiscall* )( void* ) >( this, 14 )( this );
	}
	__forceinline bool first_time_pedicted( )
	{
		return v2< bool( __thiscall* )( void* ) >( this, 15 )( this );
	}

	int32_t& predicted_commands( )
	{
		return *reinterpret_cast< int32_t* >( reinterpret_cast< uintptr_t >( this ) + 0x1C );
	}

	int32_t& previous_start_frame( )
	{
		return *reinterpret_cast< int32_t* >( reinterpret_cast< uintptr_t >( this ) + 0xC );
	}

	int32_t& server_commands_acknowledged( )
	{
		return *reinterpret_cast< int32_t* >( reinterpret_cast< uintptr_t >( this ) + 0x20 );
	}

	void restore_entity_to_predicted_frame( const int frame )
	{
		static auto pointer = reinterpret_cast< void( __stdcall* )( int, int ) >(
			memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8b ec 8b 4d ? 56 e8 ? ? ? ? 8b 75" ) ) );
		pointer( 0, frame );
	}

	/// purpose: return to original globals and stored data after running prediction, example we ran pred 60 times,
	/// come back 61 times and restore all values to original so we could re predict.
	void repredict( )
	{
		this->restore_entity_to_predicted_frame( this->predicted_commands( ) - 1 );
		this->previous_start_frame( ) = -1;
		this->predicted_commands( )   = 0;
	}

	char pad00[ 8 ];          // 0x0000
	bool mInPrediction;       // 0x0008
	bool mFirstTimePredicted; // 0x0009
	bool mEnginePaused;       // 0x000A
};