#pragma once

#include <windows.h>
#include <array>

enum class e_key_state : int
{
	none = 0,
	down,
	up,
	released
};

struct input_t {
	HWND m_window          = nullptr;
	WNDPROC m_old_wnd_proc = nullptr;

	std::array< e_key_state, 256U > m_key_state = { };

	bool on_attach( );
	void on_release( );
	bool on_wndproc( UINT msg, WPARAM wide_param, LPARAM long_param );

	bool is_key_down( const std::uint32_t button_code )
	{
		return this->m_key_state[ button_code ] == e_key_state::down;
	}

	bool is_key_released( const std::uint32_t button_code )
	{
		if ( this->m_key_state[ button_code ] == e_key_state::released ) {
			this->m_key_state[ button_code ] = e_key_state::up;
			return true;
		}

		return false;
	}
};

inline input_t input = { };