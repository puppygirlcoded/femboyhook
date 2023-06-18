#pragma once
#include "../../../includes.h"

struct motion_blur_history_t {
	motion_blur_history_t( )
	{
		last_time_update                = 0.0f;
		previous_pitch                  = 0.0f;
		previous_yaw                    = 0.0f;
		previous_position               = { 0.0f, 0.0f, 0.0f };
		previous_frame_basis_vectors    = { };
		no_rotational_motion_blur_until = 0.0f;
	}

	float last_time_update;
	float previous_pitch;
	float previous_yaw;
	c_vector previous_position;
	matrix3x4_t previous_frame_basis_vectors;
	float no_rotational_motion_blur_until;
};

struct motion_blur_t {
	void simulate( c_view_setup view_setup );
};

inline motion_blur_t motion_blur = { };