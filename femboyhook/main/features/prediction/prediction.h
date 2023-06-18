#pragma once
#include "../../includes.h"

struct prediction_system_t {
	struct {
		float current_time;
		float frame_time;
		int tick_count;
		bool in_first_prediction;
		bool in_prediction;

		player_move_data move_data{ };
	} backup;

	struct {
		int* random_seed;
	} misc;

	struct {
		c_user_cmd* cmd;
		c_base_entity* local;
		int flags;
		float z_origin;
		int movetype;
		float fall_velocity;
		int velocity_len;
		float stamina_len;

		void set_pre_data( c_base_entity* m_local, c_user_cmd* m_cmd )
		{
			this->cmd = new c_user_cmd(*m_cmd);
			this->local         = &*m_local;
			this->flags         = m_local->flags( );
			this->movetype      = m_local->move_type( );
			this->fall_velocity = m_local->velocity( ).m_z;
			this->velocity_len  = m_local->velocity( ).length_2d( ) + 0.5;
			this->stamina_len   = m_local->stamina( ).length_2d( );
			this->z_origin      = m_local->origin( ).m_z;
		}
	} pre;

	struct {
		c_user_cmd* cmd;
		c_base_entity* local;
		int flags;
		float z_origin;
		int movetype;
		float fall_velocity;
		int velocity_len;
		float stamina_len;

		void set_predicted_data( c_base_entity* m_local, c_user_cmd* m_cmd )
		{
			this->cmd           = new c_user_cmd( *m_cmd );
			this->local         = &*m_local;
			this->flags         = m_local->flags( );
			this->movetype      = m_local->move_type( );
			this->fall_velocity = m_local->velocity( ).m_z;
			this->velocity_len  = m_local->velocity( ).length_2d( ) + 0.5;
			this->stamina_len   = m_local->stamina( ).length_2d( );
			this->z_origin      = m_local->origin( ).m_z;
		}
	} predicted;

	struct {
		c_user_cmd* cmd;
		c_base_entity* local;
		int flags;
		float z_origin;
		int movetype;
		float fall_velocity;
		int velocity_len;
		float stamina_len;

		void set_post_data( c_base_entity* m_local, c_user_cmd* m_cmd )
		{
			this->cmd           = new c_user_cmd( *m_cmd );
			this->local = &*m_local;
			this->flags         = m_local->flags( );
			this->movetype      = m_local->move_type( );
			this->fall_velocity = m_local->velocity( ).m_z;
			this->velocity_len  = m_local->velocity( ).length_2d( ) + 0.5;
			this->stamina_len   = m_local->stamina( ).length_2d( );
			this->z_origin      = m_local->origin( ).m_z;
		}
	} post;

	bool is_simulating = false;

	bool simulate( c_base_entity* entity, c_user_cmd* cmd );
	void end_simulate( c_base_entity* entity );
	void create_backup( );
	void post_think( c_base_entity* entity );
	bool can_simulate( );
};

inline prediction_system_t prediction_system;