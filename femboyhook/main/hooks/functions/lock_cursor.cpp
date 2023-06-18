#include "../hooks.h"
#include "../../menu/menu.h"

void __fastcall n_detoured_functions::lock_cursor( void* thisptr, int edx )
{
	static auto original = hooks.lock_cursor.get_original< decltype( &n_detoured_functions::lock_cursor ) >( );

	if ( menu.m_opened ) {
		interfaces.m_surface->unlock_cursor( );
		return;
	}

	original( thisptr, edx );
}