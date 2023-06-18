#pragma once

class c_panel
{
private:
	enum e_indexes {
		_get_panel_name = 36
	};

public:
	const char* get_name( )
	{
		using fn = const char*( __thiscall* )( c_panel* );
		return ( *( fn** )this )[ this->e_indexes::_get_panel_name ]( this );
	}
};