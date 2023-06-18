#pragma once

class c_weapon_data;

class c_weapon_system
{
private:
	enum e_indexes {
		_get_weapon_data = 2
	};

public:
	c_weapon_data* get_weapon_data( short item_definition_index )
	{
		using fn = c_weapon_data*( __thiscall* )( c_weapon_system*, int );
		return ( *( fn** )this )[ this->e_indexes::_get_weapon_data ]( this, item_definition_index );
	}
};