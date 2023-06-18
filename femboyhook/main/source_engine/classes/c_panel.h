#pragma once

class c_panel
{
public:
	const char* get_panel_name( int panel_index )
	{
		using fn = const char*( __thiscall* )( c_panel*, int );
		return ( *( fn** )this )[ 36 ]( this, panel_index );
	}
};