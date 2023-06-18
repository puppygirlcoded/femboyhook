#pragma once

struct aimbot_value_t;

struct target_info_t {
	int entity_index;
	c_base_entity* entity;
};

struct aimbot_t {
	aimbot_value_t get_current_settings( );

	int get_best_bone( c_base_entity* entity, float fov );
	target_info_t get_best_player( c_base_entity* entity, float fov, const int target_selection_type );

	void target_players( );

	void on_createmove( );
};

inline aimbot_t aimbot = { };