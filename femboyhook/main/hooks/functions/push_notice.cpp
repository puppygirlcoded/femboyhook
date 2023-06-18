#include "../hooks.h"

#include "../../menu/menu.h"
#include "../../features/movement/movement.h"

std::string rgb_to_hex( int r, int g, int b, bool bWithHash )
{
	std::stringstream ss;
	if ( bWithHash )
		ss << "#";

	ss << std::hex << ( r << 16 | g << 8 | b );
	return ss.str( );
}

void __fastcall n_detoured_functions::push_notice( int ecx, int edx, const char* text, int str_len, const char* null )
{
	static auto original = hooks.push_notice.get_original< decltype( &n_detoured_functions::push_notice ) >( );

	auto print_text = []( int m_ecx, int m_edx, std::string str ) {
		std::string this_str = std::vformat(
			xs( "<font color=\"{}\">fbh</font><font color=\"#7d7d7d\"> |</font><font color=\"{}\"> {}</font>" ),
			std::make_format_args( rgb_to_hex( menu.accent.r, menu.accent.g, menu.accent.b, true ), rgb_to_hex( 190, 190, 190, true ), str ) );

		return original( m_ecx, m_edx, this_str.data( ), strlen( this_str.data( ) ), this_str.data( ) );
	};

	auto spotify_text = []( int m_ecx, int m_edx, std::string str ) {
		std::string this_str = std::vformat( xs( "<font color=\"#1DB954\">spotify</font><font "
		                                         "color=\"#7d7d7d\"> |</font><font color=\"{}\"> {}</font>" ),
		                                     std::make_format_args( rgb_to_hex( 190, 190, 190, true ), str ) );

		return original( m_ecx, m_edx, this_str.data( ), strlen( this_str.data( ) ), this_str.data( ) );
	};

	// std::string str = "<img src=\"https://cdn.discordapp.com/avatars/223411475807600640/f27abe87b365a421859647dd4c3395d8.png\" alt=\"image\"
	// style=\"width:100px;height:100px;\">"; return original( ecx, edx, str.data( ), strlen( str.data( ) ), str.data( ) );

	switch ( fnv1a::hash( text ) ) {
	case fnv1a::hash_const( ( "#fbh#_print_eb" ) ):
		if ( movement.movement_data.edgebug.current_combo > 1)
			return print_text( ecx, edx,
			                   std::vformat( xs( "edgebugged - {} combo" ), std::make_format_args( movement.movement_data.edgebug.current_combo ) ) );
		else
			return print_text( ecx, edx, xs( "edgebugged" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_jb" ) ):
		return print_text( ecx, edx, xs( "jumpbugged" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_ps" ) ):
		return print_text( ecx, edx, xs( "pixelsurfed" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_ej" ) ):
		return print_text( ecx, edx, xs( "edgejumped" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_mj" ) ):
		return print_text( ecx, edx, xs( "minijumped" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_lg" ) ):
		return print_text( ecx, edx, xs( "glided" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_sh" ) ):
		return print_text( ecx, edx, xs( "hopped" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_lj" ) ):
		return print_text( ecx, edx, xs( "crouched" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_print_af" ) ):
		return print_text( ecx, edx, xs( "extinguished" ) );
		break;

	case fnv1a::hash_const( ( "#fbh#_spotify_paused" ) ):
		return spotify_text( ecx, edx, xs( "now paused" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_spotify_advertisement" ) ):
		return spotify_text( ecx, edx, xs( "advertisement is now playing" ) );
		break;
	case fnv1a::hash_const( ( "#fbh#_spotify_switch" ) ):
		return spotify_text( ecx, edx, std::vformat( xs( "now playing: {}" ), std::make_format_args( menu.current_spotify_song ) ) );
		break;
	}

	original( ecx, edx, text, str_len, null );
}