#include "../hooks.h"

float __fastcall n_detoured_functions::aspect_ratio(void* edx, void* ecx, int width, int height) {
	static auto original = hooks.aspect_ratio.get_original< decltype( &aspect_ratio ) >( );

	if (interfaces.m_engine->is_in_game() && globals.m_local && variables.visuals.local.aspect_ratio != 0.0f) {
		return variables.visuals.local.aspect_ratio;
	}

	return original( edx, ecx, width, height );
}