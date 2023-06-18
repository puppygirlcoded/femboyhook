#include "../hooks.h"

void __fastcall n_detoured_functions::draw_print_text( int ecx, int edx, const wchar_t* text, int text_length, int draw_type )
{
	static auto original = hooks.draw_print_text.get_original< decltype( &n_detoured_functions::draw_print_text ) >( );

	if ( wcsncmp( text, xs( L"fps" ), 3 ) == 0 ) {
		const auto new_text = std::wstring( xs( L"femboyhook ~ " ) );
		original( ecx, edx, new_text.data( ), new_text.length( ), draw_type );
		return original( ecx, edx, text, text_length, draw_type );
	}

	return original( ecx, edx, text, text_length, draw_type );
}