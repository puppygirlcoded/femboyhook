#pragma once

enum e_move_types : int {
	move_type_none = 0,
	move_type_isometric,
	move_type_walk,
	move_type_step,
	move_type_fly,
	move_type_flygravity,
	move_type_vphysics,
	move_type_push,
	move_type_noclip,
	move_type_ladder,
	move_type_observer,
	move_type_custom,
	move_type_last     = move_type_custom,
	move_type_max_bits = 4
};