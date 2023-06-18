#pragma once
#include <corecrt_io.h>

template< typename c_stream >
class c_archivex final
{
public:
	constexpr explicit c_archivex( c_stream& stream ) noexcept : stream{ stream } { }

	template< typename T >
	constexpr const auto& operator<<( const T& item ) const noexcept
	{
		stream.write( reinterpret_cast< const char* >( &item ), sizeof( item ) );
		return *this;
	}

	template< typename T >
	constexpr const auto& operator>>( T& item ) const noexcept
	{
		stream.read( reinterpret_cast< char* >( &item ), sizeof( item ) );
		return *this;
	}

private:
	c_stream& stream;
};