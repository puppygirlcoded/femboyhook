#pragma once
#include <string.h>

namespace fnv1a
{
	constexpr unsigned int basis = 0x811c9dc5;
	constexpr unsigned int prime = 0x1000193;

	consteval unsigned int hash_const( const char* string, const unsigned int value = basis ) noexcept
	{
		return ( string[ 0 ] == '\0' ) ? value : hash_const( &string[ 1 ], ( value ^ static_cast< unsigned int >( string[ 0 ] ) ) * prime );
	}

	inline unsigned int hash( const char* string, unsigned int value = basis )
	{
		for ( unsigned int i = 0u; i < strlen( string ); ++i ) {
			value ^= string[ i ];
			value *= prime;
		}

		return value;
	}
} // namespace fnv1a
