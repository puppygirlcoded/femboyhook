#pragma once

#include "../../includes.h"

struct movement_t {
	void on_pre_create_move( );
	void on_post_create_move( );
	void on_end_scene( );
	void on_override_mouse_input( float* x, float* y );

	struct {
		bool successful_jb, successful_lb;

		struct {
			bool should_hit, should_duck, should_rotate = false;
			int tick_count = 0;

			int current_combo            = 0;

			bool should_indicate_success = false;
			c_vector predicted_position  = c_vector( );
		} edgebug;

		struct {
			bool surfing        = false;
			bool predicted      = false;
			bool should_duck    = false;
			int tick            = 0;
			c_vector view_point = c_vector( 0, 0, 0 );
			float forward_move  = 0.f;
			float side_move     = 0.f;

			c_user_cmd* user_cmd = nullptr;

			struct location_t {
				float die_time;
				float current_time;
				c_vector origin;
			};

			std::vector< location_t > location;

			bool should_indicate_success = false;
		} pixelsurf;

		bool should_indicate_edgejump_success    = false;
		bool should_indicate_minijump_success    = false;
		bool should_indicate_ladderbug_success   = false;
		bool should_indicate_stamina_hop_success = false;
		bool should_indicate_longjump_success    = false;
		bool should_indicate_fireman_success     = false;
	} movement_data;

	void on_attach( );
	void on_release( );
	bool has_invalid_movetypes( );
	bool has_invalid_predicted_movetypes( );
	float get_magic_value( );
};

inline movement_t movement = { };