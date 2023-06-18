#pragma once

#include "../../source_engine/classes/c_game_event_manager.h"

struct misc_t {
	struct {
		float hitmarker_time = 0.f;
		struct {
			int damage;
			int kills;
		} team_info;
	} event_data;

	void on_frame_stage_notify( );
	void on_end_scene( );
	void on_create_move( );
	void on_fire_game_event( c_game_event* event );
	void on_player_hurt( c_game_event* event );
	void on_player_death( c_game_event* event );
	void on_attach( );
	void on_release( );
};

inline misc_t misc = { };