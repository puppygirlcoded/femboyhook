#include "../hooks.h"

float __stdcall n_detoured_functions::viewmodel_field_of_view() {
	static auto original = hooks.viewmodel_field_of_view.get_original< decltype( &viewmodel_field_of_view ) >( );

	if (interfaces.m_engine->is_in_game() && globals.m_local && globals.m_local->is_alive()) {
		return variables.visuals.local.viewmodel_field_of_view;
	}

	return original( );
}