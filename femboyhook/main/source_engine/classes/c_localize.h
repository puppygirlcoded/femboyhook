#pragma once

class c_localize
{
private:
	enum e_indexes {
		_find = 11
	};

public:
	wchar_t* find( const char* token_name )
	{
		using fn = wchar_t*( __thiscall* )( c_localize*, const char* );
		return ( *( fn** )this )[ this->e_indexes::_find ]( this, token_name );
	}
};