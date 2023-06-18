#pragma once

#include "../../../includes.h"


struct velocity_data_t {
	int velocity;
	bool edgebugged, jumpbugged, pixelsurfed, edgejumped, minijumped, longjump, staminahopped, ladderglidded, firemaned;
};

struct indicators_t {
	void on_create_move( );
	void on_end_scene( );

	std::vector< velocity_data_t > speed_history;
	std::vector< float > stamina_history;
	float indicator_time = 0.f;
};

inline indicators_t indicators = { };