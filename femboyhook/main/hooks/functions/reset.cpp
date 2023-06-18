#include "../hooks.h"

long __stdcall n_detoured_functions::reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_parameters )
{
	static auto original = hooks.reset.get_original< decltype( &n_detoured_functions::reset ) >( );

	if ( !render.m_initialised )
		return original( device, presentation_parameters );

	ImGui_ImplDX9_InvalidateDeviceObjects( );

	const HRESULT reset_result = original( device, presentation_parameters );

	if ( reset_result == D3D_OK )
		ImGui_ImplDX9_CreateDeviceObjects( );

	return reset_result;
}