#include "../hooks.h"

bool __fastcall n_detoured_functions::is_connected( void* edx, void* ecx )
{
	static auto original = hooks.is_connected.get_original< decltype( &is_connected ) >( );
	static void* get_inventory_state = memory.m_modules[ e_module_names::client ].find_pattern( xs( "84 C0 75 05 B0 01 5F" ) ) - 2;

	// if unlock inventory is enabled and if the game is checking if we're in game or not when accessing the inventory.
	if ( variables.misc.helpers.unlock_inventory && _ReturnAddress( ) == get_inventory_state ) {
		return false;
	}

	return original( edx, ecx );
}