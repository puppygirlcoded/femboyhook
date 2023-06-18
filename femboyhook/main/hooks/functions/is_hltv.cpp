#include "../hooks.h"

bool __fastcall n_detoured_functions::is_hltv(void* ecx, void* edx) {
	static auto original = hooks.is_hltv.get_original< decltype( &is_hltv ) >( );

	static auto radar_reveal =
		memory.m_modules[ e_module_names::client ].find_pattern( xs( "84 C0 0F 85 ? ? ? ? 8B CB E8 ? ? ? ? 84 C0 0F 85 ? ? ? ? 83 BF" ) );

	if (variables.misc.helpers.radar_reveal && globals.m_local && _ReturnAddress() == radar_reveal) {
		return true;
	}

	return original( ecx, edx );
}