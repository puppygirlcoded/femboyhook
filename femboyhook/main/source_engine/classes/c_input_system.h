#pragma once

class c_input_system
{
private:
	enum e_indexes {
		_enable_input = 11
	};

public:
	void enable_input( bool enable )
	{
		using fn = void( __thiscall* )( c_input_system*, bool );
		return ( *( fn** )this )[ this->e_indexes::_enable_input ]( this, enable );
	}
};