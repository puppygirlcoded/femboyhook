#pragma once
#include <cstdint>

using crc32_t = std::uint32_t;

namespace crc32
{
	void init( crc32_t* crc );
	void process_buffer( crc32_t* crc, const void* p, int len );
	void final( crc32_t* crc );
	crc32_t get_table_entry( unsigned int slot );

	inline crc32_t process_single_buffer( const void* p, int len )
	{
		crc32_t crc;
		crc32::init( &crc );
		crc32::process_buffer( &crc, p, len );
		crc32::final( &crc );
		return crc;
	}
} // namespace crc32
