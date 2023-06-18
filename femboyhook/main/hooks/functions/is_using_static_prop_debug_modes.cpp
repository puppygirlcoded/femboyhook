#include "../hooks.h"

bool __stdcall n_detoured_functions::is_using_static_prop_debug_modes( ) {
	return variables.visuals.world.modulate_props;
}