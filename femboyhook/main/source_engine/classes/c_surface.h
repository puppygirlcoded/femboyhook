#pragma once

class c_surface
{
	enum e_indexes {
		_unlock_cursor = 66
	};

public:
	void unlock_cursor( ) {
		using fn = void( __thiscall* )( c_surface* );
		return ( *( fn** )this )[ this->e_indexes::_unlock_cursor ]( this );
	}
};