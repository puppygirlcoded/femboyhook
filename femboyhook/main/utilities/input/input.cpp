#include "input.h"
#include "../../hooks/hooks.h"
#include "../../menu/menu.h"

#include <thread>

bool input_t::on_attach( )
{
	D3DDEVICE_CREATION_PARAMETERS creation_parameters = { };
	while ( FAILED( memory.m_device->GetCreationParameters( &creation_parameters ) ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	this->m_window = creation_parameters.hFocusWindow;

	if ( !this->m_window )
		return false;

	this->m_old_wnd_proc = reinterpret_cast< WNDPROC >(
		SetWindowLongPtrW( this->m_window, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( n_detoured_functions::wndproc ) ) );

	if ( !this->m_old_wnd_proc )
		return false;

	return true;
}

void input_t::on_release( )
{
	if ( this->m_old_wnd_proc ) {
		SetWindowLongPtrW( this->m_window, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( this->m_old_wnd_proc ) );
		this->m_old_wnd_proc = nullptr;
	}

	interfaces.m_input_system->enable_input( true );
}

bool input_t::on_wndproc( UINT msg, WPARAM wide_param, LPARAM long_param )
{
	if ( menu.m_opened && wide_param != VK_INSERT && wide_param != VK_END )
		return false;

	int current_key           = 0;
	e_key_state current_state = e_key_state::none;

	switch ( msg ) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if ( wide_param < 256U ) {
			current_key   = wide_param;
			current_state = e_key_state::down;
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if ( wide_param < 256U ) {
			current_key   = wide_param;
			current_state = e_key_state::up;
		}
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		current_key   = VK_LBUTTON;
		current_state = msg == WM_LBUTTONUP ? e_key_state::up : e_key_state::down;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		current_key   = VK_RBUTTON;
		current_state = msg == WM_RBUTTONUP ? e_key_state::up : e_key_state::down;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		current_key   = VK_MBUTTON;
		current_state = msg == WM_MBUTTONUP ? e_key_state::up : e_key_state::down;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		current_key   = ( GET_XBUTTON_WPARAM( wide_param ) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2 );
		current_state = msg == WM_XBUTTONUP ? e_key_state::up : e_key_state::down;
		break;
	default:
		return false;
	}

	this->m_key_state[ current_key ] =
		current_state == e_key_state::up && this->m_key_state[ current_key ] == e_key_state::down ? e_key_state::released : current_state;

	return true;
}