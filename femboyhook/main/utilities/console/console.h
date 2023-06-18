#pragma once
#include <format>
#include <iostream>

#include "../../hashing/imports/lazy_importer.h"
#include "../../hashing/strings/xorstr.h"
#include <spdlog/spdlog.h>

struct console_t {
	void on_attach( const char* window_title )
	{
#ifdef _DEBUG
		LI_FN( AllocConsole )( );

		freopen_s( reinterpret_cast< _iobuf** >( stdout ), "conout$", "w", static_cast< _iobuf* >( stdout ) );

		if ( window_title )
			LI_FN( SetConsoleTitleA )( window_title );
#endif
	}

	void on_release( )
	{
#ifdef _DEBUG
		fclose( static_cast< _iobuf* >( stdout ) );

		LI_FN( FreeConsole )( );
#endif
	}

	template< typename... Args_t >
	void print( const char* text, const Args_t&... args )
	{
#ifdef _DEBUG
		if constexpr ( sizeof...( args ) > 0 )
			spdlog::info( xs( "femboyhook ~ {:s}" ), std::vformat( text, std::make_format_args( args... ) ).c_str( ) );
		else
			spdlog::info( xs( "femboyhook ~ {:s}" ), text );
#endif
	}
};

inline console_t console = { };