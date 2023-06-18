#include "../hooks.h"

#include "../../hashing/strings/xorstr.h"

int __cdecl n_detoured_functions::vsnprintf( char* dest, int text_length, const char* fmt, ... )
{
	int v4 = { }; // ecx

	va_list args = { };
	va_start( args, fmt );

	if ( strstr( fmt, xs( "fps:" ) ) ) {
		fmt = interfaces.m_engine->is_in_game( ) ? xs( "fps: %5i / %4.1f var / %i ms" ) : xs( "fps: %5i / var: %4.1f ms / (v%u%s)" );
		v4  = _vsnprintf( dest, text_length, fmt, args );
	} else if ( strstr( fmt, xs( "loss:" ) ) ) {
		fmt = xs( " %3i%% lost / %2i%% choked" );
		v4  = _vsnprintf( dest, text_length, fmt, args );
	} else if ( strstr( fmt, xs( "tick:" ) ) ) {
		fmt = xs( "tr:%5.1f / " );
		v4  = _vsnprintf( dest, text_length, fmt, args );
	} else if ( strstr( fmt, xs( "sv:" ) ) ) {
		fmt = xs( "sv:%5.1f %s%4.1f ms" );
		v4  = _vsnprintf( dest, text_length, fmt, args );
	} else
		v4 = _vsnprintf( dest, text_length, fmt, args );

	va_end( args );

	if ( ( v4 < 0 ) || ( text_length > 0 && v4 >= text_length ) ) {
		v4                      = text_length - 1;
		dest[ text_length - 1 ] = '\0';
	}

	return v4;
}