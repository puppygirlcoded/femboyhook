#include "../hooks.h"

vcollide_t* __fastcall n_detoured_functions::get_vcollide( void* edx, void* ecx, int vcollide_index )
{
	static auto original = hooks.get_vcollide.get_original< decltype( &get_vcollide ) >( );

	return original( edx, ecx, vcollide_index );
}