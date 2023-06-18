#include "prediction.h"

bool prediction_system_t::can_simulate( )
{
	return ( interfaces.m_engine->is_in_game( ) && globals.m_local && globals.m_cmd && globals.m_local->is_alive() && memory.m_client_state->m_delta_tick != -1);
}

void prediction_system_t::create_backup( )
{
	this->backup.current_time = memory.m_globals->m_current_time;
	this->backup.frame_time   = memory.m_globals->m_frame_time;
}
// lol
template< unsigned int index, typename tret, typename... T >
__forceinline static auto call_virtual( void* thisptr, T... arglist ) -> tret
{
	using fn = tret( __thiscall* )( void*, decltype( arglist )... );
	return ( *static_cast< fn** >( thisptr ) )[ index ]( thisptr, arglist... );
}

void prediction_system_t::post_think( c_base_entity* entity )
{
	static const auto post_think_vphysics =
		c_address( memory.m_modules[ e_module_names::client ].find_pattern( xs( "E8 ? ? ? ? 8B CF E8 ? ? ? ? 8B 06" ) ) )
			.relative( )
			.as< bool( __thiscall* )( c_base_entity* ) >( );

	static const auto simulate_player_simulated_effects =
		c_address( memory.m_modules[ e_module_names::client ].find_pattern( xs( "E8 ? ? ? ? 8B 06 8B CE 5F 5E FF A0 88" ) ) )
			.relative( )
			.as< void( __thiscall* )( c_base_entity* ) >( );

	call_virtual< 33, void >( interfaces.m_model_cache );

	if ( entity->is_alive( ) || *reinterpret_cast< std::uint32_t* >( uintptr_t( entity ) + 0x9AE1 ) ) {
		entity->update_colision_bounds( );

		if ( entity->flags( ) & fl_on_ground )
			entity->fall_velocity( ) = 0;

		//if ( entity->get_sequence( ) == -1 )
		//	entity->set_sequence( 0 );

		entity->studio_frame_advance( );

		post_think_vphysics( entity );
	}
	simulate_player_simulated_effects( entity );

	call_virtual< 34, void >( interfaces.m_model_cache );
}

bool prediction_system_t::simulate( c_base_entity* entity, c_user_cmd* cmd )
{
	if ( !this->can_simulate( ) )
		return false;

	if ( !entity )
		return false;

	*entity->current_cmd( ) = cmd;
	entity->last_cmd( )     = *cmd;

	if ( !this->misc.random_seed )
		this->misc.random_seed = *reinterpret_cast< int** >(
			memory.m_modules[ e_module_names::client ].find_pattern( xs( "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04" ) ) + 2 );

	*this->misc.random_seed = cmd->m_random_seed & 0x7FFFFFFF;

	this->create_backup( );

	memory.m_globals->m_current_time = entity->tick_base( ) * memory.m_globals->m_interval_per_tick;
	memory.m_globals->m_frame_time   = memory.m_globals->m_interval_per_tick;

	this->is_simulating = true;

	interfaces.m_move_helper->set_host( entity );

	interfaces.m_game_movement->start_track_prediction_errors( entity );

	if ( cmd->m_impluse )
		*reinterpret_cast< std::uint32_t* >( uintptr_t( entity ) + 0x320C ) = cmd->m_impluse;

	// should process buttons here.

	interfaces.m_prediction->check_moving_ground( entity, memory.m_globals->m_frame_time );

	interfaces.m_prediction->set_local_view_angles( cmd->m_view_point );

	if ( entity->physics_run_think( 0 ) )
		entity->pre_think( );

	const auto next_think_tick = static_cast< int >( *reinterpret_cast< std::uint32_t* >( uintptr_t( entity ) + 0xFC ) );

	if ( next_think_tick > 0 && next_think_tick <= static_cast< int >( *reinterpret_cast< std::uint32_t* >( uintptr_t( entity ) + 0x3440 ) ) &&
	     entity->is_alive( ) ) {
		*reinterpret_cast< std::uint32_t* >( uintptr_t( entity ) + 0xFC ) = -1;
		// player->think();
		( *( void( __thiscall** )( c_base_entity* ) )( *reinterpret_cast< std::uint32_t* >( uintptr_t( entity ) + 556 ) ) )( entity );
	}

	interfaces.m_prediction->setup_move( entity, cmd, interfaces.m_move_helper, &this->backup.move_data );
	interfaces.m_game_movement->process_movement( entity, &this->backup.move_data );
	interfaces.m_prediction->finish_move( entity, cmd, &this->backup.move_data );
	interfaces.m_move_helper->process_impacts( );

	this->post_think( entity );

	this->predicted.set_predicted_data( globals.m_local, cmd );

	interfaces.m_prediction->mInPrediction       = this->backup.in_prediction;
	interfaces.m_prediction->mFirstTimePredicted = this->backup.in_first_prediction;

	return true;
}

void prediction_system_t::end_simulate( c_base_entity* entity )
{
	if ( !this->can_simulate( ) )
		return;

	interfaces.m_game_movement->finish_track_prediction_errors( entity );
	interfaces.m_prediction->mInPrediction = false;

	interfaces.m_move_helper->set_host( nullptr );

	this->is_simulating = false;

	memory.m_globals->m_current_time = this->backup.current_time;
	memory.m_globals->m_frame_time   = this->backup.frame_time;

	*this->misc.random_seed = -1;

	interfaces.m_game_movement->reset( );

	if ( !interfaces.m_prediction->mEnginePaused && memory.m_globals->m_frame_time > 0.f ) {
		++entity->tick_base( );
	}
}