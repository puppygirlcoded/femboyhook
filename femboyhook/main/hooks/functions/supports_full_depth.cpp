#include "../hooks.h"

bool __stdcall n_detoured_functions::supports_full_depth( )
{
	static auto original = hooks.supports_full_depth.get_original< decltype( &supports_full_depth ) >( );

	if ( !interfaces.m_material_system )
		return original(  );

	constexpr auto VENDORID_NVIDIA = 0x10DEu;
	constexpr auto VENDORID_AMD    = 0x1002u;
	constexpr auto VENDORID_INTEL  = 0x8086u;

	material_adapter_info_t info;
	auto adapter = interfaces.m_material_system->get_current_adapter( );
	interfaces.m_material_system->get_display_adapter_info( adapter, &info );

	if ( info.vendor_id == VENDORID_AMD )
		return false;

	return original(  );
}