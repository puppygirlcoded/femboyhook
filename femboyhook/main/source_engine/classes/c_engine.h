#pragma once

#include "../structs/matrix_t.h"
#include "c_angle.h"

struct player_info_t;

class c_net_channel_info
{
public:
	enum {
		GENERIC = 0,  // must be first and is default group
		LOCALPLAYER,  // bytes for local player entity update
		OTHERPLAYERS, // bytes for other players update
		ENTITIES,     // all other entity bytes
		SOUNDS,       // game sounds
		EVENTS,       // event messages
		USERMESSAGES, // user messages
		ENTMESSAGES,  // entity messages
		VOICE,        // voice data
		STRINGTABLE,  // a stringtable update
		MOVE,         // client move cmds
		STRINGCMD,    // string command
		SIGNON,       // various signondata
		TOTAL,        // must be last and is not a real group
	};

	virtual const char* get_name( void ) const     = 0; // get channel name
	virtual const char* get_address( void ) const  = 0; // get channel IP address as string
	virtual float get_time( void ) const           = 0; // current net time
	virtual float get_time_connected( void ) const = 0; // get connection time in seconds
	virtual int get_buffer_size( void ) const      = 0; // netchannel packet history size
	virtual int get_data_rate( void ) const        = 0; // send data rate in byte/sec

	virtual bool is_loop_back( void ) const  = 0; // true if loopback channel
	virtual bool is_timing_out( void ) const = 0; // true if timing out
	virtual bool is_play_back( void ) const  = 0; // true if demo playback

	virtual float get_latency( int flow ) const                                         = 0; // current latency (RTT), more accurate but jittering
	virtual float get_average_latency( int flow ) const                                 = 0; // average packet latency in seconds
	virtual float get_average_loss( int flow ) const                                    = 0; // avg packet loss[0..1]
	virtual float get_average_choke( int flow ) const                                   = 0; // avg packet choke[0..1]
	virtual float get_average_data( int flow ) const                                    = 0; // data flow in bytes/sec
	virtual float get_average_packets( int flow ) const                                 = 0; // avg packets/sec
	virtual int get_total_data( int flow ) const                                        = 0; // total flow in/out in bytes
	virtual int get_sequence_number( int flow ) const                                   = 0; // last send seq number
	virtual bool is_valid_packet( int flow, int frame_number ) const                    = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float get_packet_time( int flow, int frame_number ) const                   = 0; // time when packet was send
	virtual int get_packet_bytes( int flow, int frame_number, int group ) const         = 0; // group size of this packet
	virtual bool get_stream_progress( int flow, int* received, int* total ) const       = 0; // TCP progress if transmitting
	virtual float get_since_last_time_recieved( void ) const                            = 0; // get time since last recieved packet in seconds
	virtual float get_command_interpolation_ammount( int flow, int frame_number ) const = 0;
	virtual void get_packet_response_latency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;
	virtual void get_remote_framerate( float* pflFrameTime, float* pflFrameTimeStdDeviation ) const                 = 0;

	virtual float get_timeout_seconds( ) const = 0;
};

class c_engine
{
private:
	enum e_indexes {
		_get_screen_size        = 5,
		_get_local_player       = 12,
		_is_in_game             = 26,
		_is_connected           = 27,
		_world_to_screen_matrix = 37,
		_get_level_name         = 52,
		_get_level_name_short   = 53,
		_get_player_info        = 8,
		_set_view_angles        = 19,
		_get_net_channel_info   = 78,
		_execute_command        = 108,
	};

public:
	bool is_in_game( )
	{
		using fn = bool( __thiscall* )( c_engine* );
		return ( *( fn** )this )[ this->e_indexes::_is_in_game ]( this );
	}

	bool is_connected( )
	{
		using fn = bool( __thiscall* )( c_engine* );
		return ( *( fn** )this )[ this->e_indexes::_is_connected ]( this );
	}

	int get_local_player( )
	{
		using fn = int( __thiscall* )( c_engine* );
		return ( *( fn** )this )[ this->e_indexes::_get_local_player ]( this );
	}

	const char* get_level_name( )
	{
		using fn = const char*( __thiscall* )( c_engine* );
		return ( *( fn** )this )[ this->e_indexes::_get_level_name ]( this );
	}

	const char* get_level_name_short( )
	{
		using fn = const char*( __thiscall* )( c_engine* );
		return ( *( fn** )this )[ this->e_indexes::_get_level_name_short ]( this );
	}
	int get_player_for_user_id( int user_id )
	{
		using original_fn = int( __thiscall* )( c_engine*, int );
		return ( *( original_fn** )this )[ 9 ]( this, user_id );
	}
	const view_matrix_t& world_to_screen_matrix( )
	{
		using fn = const view_matrix_t&( __thiscall* )( c_engine* );
		return ( *( fn** )this )[ this->e_indexes::_world_to_screen_matrix ]( this );
	}

	bool get_player_information( int entity_index, player_info_t* player_information )
	{
		using fn = bool( __thiscall* )( c_engine*, int, player_info_t* );
		return ( *( fn** )this )[ this->e_indexes::_get_player_info ]( this, entity_index, player_information );
	}

	void set_view_angles( c_vector& angles )
	{
		using fn = void( __thiscall* )( c_engine*, c_vector& );
		return ( *( fn** )this )[ this->e_indexes::_set_view_angles ]( this, std::ref( angles ) );
	}

	c_net_channel_info* get_net_channel_info( )
	{
		using original_fn = c_net_channel_info*( __thiscall* )( c_engine* );
		return ( *( original_fn** )this )[ this->e_indexes::_get_net_channel_info ]( this );
	}

	void get_screen_size( int& w, int& h )
	{
		using fn = void( __thiscall* )( c_engine*, int&, int& );
		return ( *( fn** )this )[ this->e_indexes::_get_screen_size ]( this, w, h );
	}

	void execute_command( const char* buffer )
	{
		using fn = void( __thiscall* )( c_engine*, const char* );
		return ( *( fn** )this )[ this->e_indexes::_execute_command ]( this, buffer );
	}
};