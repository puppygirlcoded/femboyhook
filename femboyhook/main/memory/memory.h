#pragma once

#include <d3d9.h>
#include <vector>

#include "../source_engine/classes/c_client_state.h"
#include "../source_engine/classes/c_globals.h"
#include "../source_engine/classes/c_input.h"
#include "../source_engine/classes/c_key_values_system.h"
#include "../source_engine/classes/c_weapon_system.h"

enum e_module_names : int {
	client = 0,
	engine,
	vgui2,
	vguimatsurface,
	localize,
	server,
	panorama,
	materialsystem,
	vstdlib,
	shaderapidx9,
	inputsystem,
	filesystem,
	data_cache,
	physics,
	max
};

struct module_handle_info_t {
	const char* m_name = { };
	void* m_value      = { };

	std::uint8_t* find_pattern( const char* signature );
};

class c_address
{
public:
	__forceinline c_address( ) : m_address{ } { };
	__forceinline ~c_address( ){ };

	__forceinline c_address( std::size_t address ) : m_address{ address } { }
	__forceinline c_address( void* address ) : m_address{ reinterpret_cast< std::size_t >( address ) } { }

	inline operator std::size_t( )
	{
		return m_address;
	}

	inline operator void*( )
	{
		return reinterpret_cast< void* >( m_address );
	}

	inline bool operator==( c_address address )
	{
		return as< std::size_t >( ) == address.as< std::size_t >( );
	}

	inline bool operator!=( c_address address )
	{
		return as< std::size_t >( ) != address.as< std::size_t >( );
	}

	template< typename t = c_address >
	inline t to( )
	{
		return *( t* )m_address;
	}

	template< typename t = c_address >
	inline t as( )
	{
		return m_address ? ( t )m_address : t( );
	}

	template< typename t = c_address >
	inline t at( std::size_t offset )
	{
		return m_address ? *( t* )( m_address + offset ) : t( );
	}

	template< typename t = c_address >
	inline t add( std::size_t offset )
	{
		return m_address ? ( t )( m_address + offset ) : t( );
	}

	template< typename t = c_address >
	inline t sub( std::size_t offset )
	{
		return m_address ? ( t )( m_address - offset ) : t( );
	}

	template< typename t = c_address >
	static __forceinline t RVA( c_address base, size_t offset )
	{
		return base.as< t >( offset );
	}

	template< typename t = c_address >
	inline t get( std::size_t dereferences = 1 )
	{
		if ( !m_address )
			return t( );

		std::size_t object = m_address;

		while ( dereferences-- )
			if ( object )
				object = *reinterpret_cast< std::size_t* >( object );

		return ( t )object;
	}

	template< typename t = std::size_t >
	inline void set( const t& value )
	{
		if ( !m_address )
			return;

		*( t* )m_address = value;
	}

	template< typename t = c_address >
	inline t relative( std::size_t offset = 0x1 )
	{
		if ( !m_address )
			return t( );

		std::size_t new_address = m_address + offset;

		std::int32_t relative_offset = *reinterpret_cast< std::int32_t* >( new_address );

		if ( !relative_offset )
			return t( );

		return ( t )( new_address + sizeof( std::size_t ) + relative_offset );
	}

	template< typename t = c_address >
	__forceinline t rel32( size_t offset )
	{
		uintptr_t out;
		uint32_t r;

		if ( !m_address )
			return t{ };

		out = m_address + offset;
		r   = *( uint32_t* )out;
		if ( !r )
			return t{ };
		out = ( out + 4 ) + r;

		return ( t )out;
	}

protected:
	std::size_t m_address;
};

struct memory_t {
	bool on_attach( const std::vector< const char* >& modules );

	bool on_attach( );

	std::vector< module_handle_info_t > m_modules = { };

	c_input* m_input                         = nullptr;
	c_key_values_system* m_key_values_system = nullptr;
	c_client_state* m_client_state           = nullptr;
	c_globals* m_globals                     = nullptr;
	c_weapon_system* m_weapon_system         = nullptr;
	IDirect3DDevice9* m_device               = nullptr;
};

inline memory_t memory = { };