#include "../../features/movement/movement.h"
#include "../hooks.h"

void __fastcall n_detoured_functions::override_mouse_input( int edx, int ecx, float* x, float* y )
{
	static auto original = hooks.override_mouse_input.get_original< decltype( &override_mouse_input ) >( );

	if ( movement.movement_data.edgebug.should_hit ) {
		*x *= ( 1.f - ( variables.movement.edgebug_lock_amount / 100.f ) );
	}

	movement.on_override_mouse_input( x, y );

	return original( edx, ecx, x, y );
}