#pragma once

#include <cstddef>
#include <d3d9.h>

struct c_image_texture {
	std::byte pad0[ 0xC ];
	IDirect3DTexture9* texture;
};

class c_texture
{
private:
	std::byte pad0[ 0x50 ];

public:
	c_image_texture** texture_handle;

	int get_actual_width( )
	{
		using original_fn = int( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 3 ]( this );
	}

	int get_actual_height( )
	{
		using original_fn = int( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 4 ]( this );
	}

	void increment_reference_count( )
	{
		using original_fn = void( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 10 ]( this );
	}

	void deincrement_reference_count( )
	{
		using original_fn = void( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 11 ]( this );
	}
};