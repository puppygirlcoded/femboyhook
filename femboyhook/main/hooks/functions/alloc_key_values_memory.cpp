#include "../hooks.h"

void* __fastcall n_detoured_functions::alloc_key_values_memory( c_key_values_system* thisptr, int edx, int size )
{
	static auto original = hooks.alloc_key_values_memory.get_original< decltype( &n_detoured_functions::alloc_key_values_memory ) >( );

	static const auto key_values_alloc_engine =
		utilities.get_absolute_address( reinterpret_cast< std::uintptr_t >(
			memory.m_modules[ e_module_names::engine ].find_pattern( xs( "E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C" ) ) + 0x1 ) ) +
		0x4A;

	static const auto key_values_alloc_client =
		utilities.get_absolute_address( reinterpret_cast< std::uintptr_t >(
			memory.m_modules[ e_module_names::client ].find_pattern( xs( "E8 ? ? ? ? 83 C4 08 84 C0 75 10" ) ) + 0x1 ) ) +
		0x3E;

	if ( const std::uintptr_t return_address = reinterpret_cast< std::uintptr_t >( _ReturnAddress( ) );
	     return_address == key_values_alloc_engine || return_address == key_values_alloc_client )
		return nullptr;

	return original( thisptr, edx, size );
}