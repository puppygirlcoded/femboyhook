#include "imgui.h"
#include <array>
//#include <imgui_freetype.h>
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <optional>
#include <span>

// "warning C4005: 'APIENTRY': macro redefinition"
// clang-format off
#include <windows.h>


#include "render_wrapper.h"

static bool is_initialised;
static HWND window;
static ImFont* fonts[ utils::render::FONT_MAX ];

static std::optional< std::string > find_system_font( std::string_view font_name );

static ImFont* create_system_font( std::string_view font_name, float size, int flags = 0, std::initializer_list< ImWchar > glyph_ranges = { } );
static ImFont* create_memory_font( std::span< uint8_t > font_data, float size, int flags = 0, std::initializer_list< ImWchar > glyph_ranges = { } );

static ImVec2 convert_vector( const sdk::vector2& vec )
{
	return { ( float )vec.x, ( float )vec.y };
}

static ImVec2 convert_vector( const sdk::vector2f& vec )
{
	return { vec.x, vec.y };
}

uintptr_t utils::render::get_window_handle( )
{
	return ( uintptr_t )window;
}

bool utils::render::is_ready( )
{
	return is_initialised;
}

bool utils::render::setup( uintptr_t device_context )
{
	if ( is_initialised )
		return false;

	is_initialised = true;

	return true;
}

bool utils::render::undo( )
{
	if ( !is_initialised )
		return false;

	is_initialised = false;

	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );

	return true;
}

void utils::render::start( )
{
	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );
	ImGui::GetBackgroundDrawList( )->ChannelsSplit( 11 );
}

void utils::render::end( )
{
	ImGui::GetBackgroundDrawList( )->ChannelsMerge( );
	ImGui::EndFrame( );
	ImGui::Render( );

	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
}

int utils::render::get_layer( )
{
	return ImGui::GetBackgroundDrawList( )->_Splitter._Current;
}

void utils::render::set_layer( int layer )
{
	IM_ASSERT( layer <= 10 );

	ImGui::GetBackgroundDrawList( )->ChannelsSetCurrent( layer );
}

void utils::render::set_clip( const vector2& min, const vector2& max, bool intersect )
{
	ImGui::GetBackgroundDrawList( )->PushClipRect( convert_vector( min ), convert_vector( max ), intersect );
}

void utils::render::set_clip_foreground( const vector2& min, const vector2& max, bool intersect )
{
	ImGui::GetForegroundDrawList( )->PushClipRect( convert_vector( min ), convert_vector( max ), intersect );
}

void utils::render::set_clip_fullscreen( )
{
	ImGui::GetBackgroundDrawList( )->PushClipRectFullScreen( );
}

void utils::render::pop_clip( )
{
	ImGui::GetBackgroundDrawList( )->PopClipRect( );
}

void utils::render::pop_clip_foreground( )
{
	ImGui::GetForegroundDrawList( )->PopClipRect( );
}

void utils::render::reset_clip( )
{
	ImGui::GetBackgroundDrawList( )->_ClipRectStack.clear( );
	ImGui::GetBackgroundDrawList( )->PushClipRectFullScreen( );
}

void utils::render::line( const vector2& start, const vector2& end, const color& color, float thickness )
{
	ImGui::GetBackgroundDrawList( )->AddLine( convert_vector( start ), convert_vector( end ), IM_COL32( color.r, color.g, color.b, color.a ),
	                                          thickness );
}

void utils::render::quad( const vector2& tl, const vector2& tr, const vector2& bl, const vector2& br, const color& color, float thickness )
{
	ImGui::GetBackgroundDrawList( )->AddQuad( convert_vector( tl ), convert_vector( tr ), convert_vector( br ), convert_vector( bl ),
	                                          IM_COL32( color.r, color.g, color.b, color.a ), thickness );
}

void utils::render::rect( const vector2& position, const vector2& size, const color& color, float rounding, int corners, float thickness )
{
	ImGui::GetBackgroundDrawList( )->AddRect( convert_vector( position ), convert_vector( position + size ),
	                                          IM_COL32( color.r, color.g, color.b, color.a ), rounding, corners, thickness );
}
void utils::render::rect_foreground( const vector2& position, const vector2& size, const color& color, float rounding, int corners, float thickness )
{
	ImGui::GetForegroundDrawList( )->AddRect( convert_vector( position ), convert_vector( position + size ),
	                                          IM_COL32( color.r, color.g, color.b, color.a ), rounding, corners, thickness );
}

void utils::render::triangle( const vector2& point1, const vector2& point2, const vector2& point3, const color& color, float thickness )
{
	ImGui::GetBackgroundDrawList( )->AddTriangle( convert_vector( point1 ), convert_vector( point2 ), convert_vector( point3 ),
	                                              IM_COL32( color.r, color.g, color.b, color.a ), thickness );
}

void utils::render::circle( const vector2& anchor, float radius, const color& color, float thickness )
{
	ImGui::GetBackgroundDrawList( )->AddCircle( convert_vector( anchor ), radius, IM_COL32( color.r, color.g, color.b, color.a ), 0, thickness );
}

void utils::render::circle( const sdk::vector2f& anchor, float radius, const color& color, float thickness )
{
	ImGui::GetBackgroundDrawList( )->AddCircle( convert_vector( anchor ), radius, IM_COL32( color.r, color.g, color.b, color.a ), 0, thickness );
}

void utils::render::text( const vector2& position, std::string_view text, int font, const color& color, float wrap_width, float font_size )
{
	const auto font_ptr = fonts[ font ];

	if ( font_size < 0.0f )
		font_size = font_ptr->FontSize;

	// clang-format off

    ImGui::GetBackgroundDrawList()->AddText(font_ptr, font_size, convert_vector(position),
        IM_COL32(color.r, color.g, color.b, color.a), text.data(), nullptr, wrap_width);

	// clang-format on
}

void utils::render::text_foreground( const vector2& position, std::string_view text, int font, const color& color, float wrap_width, float font_size )
{
	const auto font_ptr = fonts[ font ];

	if ( font_size < 0.0f )
		font_size = font_ptr->FontSize;

	// clang-format off

    ImGui::GetForegroundDrawList()->AddText(font_ptr, font_size, convert_vector(position),
        IM_COL32(color.r, color.g, color.b, color.a), text.data(), nullptr, wrap_width);

	// clang-format on
}

void utils::render::fill_quad( const vector2& tl, const vector2& tr, const vector2& bl, const vector2& br, const color& color )
{
	ImGui::GetBackgroundDrawList( )->AddQuadFilled( convert_vector( tl ), convert_vector( tr ), convert_vector( br ), convert_vector( bl ),
	                                                IM_COL32( color.r, color.g, color.b, color.a ) );
}

void utils::render::fill_rect( const vector2& position, const vector2& size, const color& color, float rounding, int corners )
{
	ImGui::GetBackgroundDrawList( )->AddRectFilled( convert_vector( position ), convert_vector( position + size ),
	                                                IM_COL32( color.r, color.g, color.b, color.a ), rounding, corners );
}

void utils::render::fill_rect_foreground( const vector2& position, const vector2& size, const color& color, float rounding, int corners )
{
	ImGui::GetForegroundDrawList( )->AddRectFilled( convert_vector( position ), convert_vector( position + size ),
	                                                IM_COL32( color.r, color.g, color.b, color.a ), rounding, corners );
}

void utils::render::fill_gradient( const vector2& position, const vector2& size, const color& color1, const color& color2, bool horizontal )
{
	const auto color1_rgba = IM_COL32( color1.r, color1.g, color1.b, color1.a );
	const auto color2_rgba = IM_COL32( color2.r, color2.g, color2.b, color2.a );

	// clang-format off

    ImGui::GetBackgroundDrawList()->AddRectFilledMultiColor(convert_vector(position), convert_vector(position + size),
        color1_rgba, horizontal ? color2_rgba : color1_rgba, color2_rgba, horizontal ? color1_rgba : color2_rgba);

	// clang-format on
}

void utils::render::fill_gradient_foreground( const vector2& position, const vector2& size, const color& color1, const color& color2,
                                              bool horizontal )
{
	const auto color1_rgba = IM_COL32( color1.r, color1.g, color1.b, color1.a );
	const auto color2_rgba = IM_COL32( color2.r, color2.g, color2.b, color2.a );

	// clang-format off

    ImGui::GetForegroundDrawList()->AddRectFilledMultiColor(convert_vector(position), convert_vector(position + size),
        color1_rgba, horizontal ? color2_rgba : color1_rgba, color2_rgba, horizontal ? color1_rgba : color2_rgba);

	// clang-format on
}

void utils::render::fill_triangle( const vector2& point1, const vector2& point2, const vector2& point3, const color& color )
{
	ImGui::GetBackgroundDrawList( )->AddTriangleFilled( convert_vector( point1 ), convert_vector( point2 ), convert_vector( point3 ),
	                                                    IM_COL32( color.r, color.g, color.b, color.a ) );
}

void utils::render::fill_circle( const vector2& anchor, float radius, const color& color )
{
	ImGui::GetBackgroundDrawList( )->AddCircleFilled( convert_vector( anchor ), radius, IM_COL32( color.r, color.g, color.b, color.a ) );
}

void utils::render::fill_circle_foreground( const vector2& anchor, float radius, const color& color )
{
	ImGui::GetForegroundDrawList( )->AddCircleFilled( convert_vector( anchor ), radius, IM_COL32( color.r, color.g, color.b, color.a ) );
}

void utils::render::fill_circle( const vector2f& anchor, float radius, const color& color )
{
	ImGui::GetBackgroundDrawList( )->AddCircleFilled( convert_vector( anchor ), radius, IM_COL32( color.r, color.g, color.b, color.a ) );
}

sdk::vector2 utils::render::get_window_size( )
{
	const auto display_size = ImGui::GetIO( ).DisplaySize;

	return { ( int )display_size.x, ( int )display_size.y };
}

sdk::vector2 utils::render::measure_text( std::string_view text, int font, float wrap_width, float font_size )
{
	const auto font_ptr = fonts[ font ];

	if ( font_size < 0.0f )
		font_size = font_ptr->FontSize;

	const auto text_size = font_ptr->CalcTextSizeA( font_size, 10.0e+10f, wrap_width, text.data( ) );

	return { ( int )text_size.x, ( int )text_size.y };
	return { ( int )text_size.x, ( int )text_size.y };
}

static std::optional< std::string > find_system_font( std::string_view font_name )
{
	char windows_directory[ MAX_PATH ];

	if ( !GetWindowsDirectoryA( windows_directory, MAX_PATH ) )
		return std::nullopt;

	const auto fonts_directory = std::string( windows_directory ).append( "\\Fonts\\" );

	HKEY registry_key;

	if ( RegOpenKeyExA( HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registry_key ) )
		return std::nullopt;

	auto index = 0u;

	char value_name[ MAX_PATH ];
	char value_data[ MAX_PATH ];

	do {
		auto value_name_size = MAX_PATH;
		auto value_data_size = MAX_PATH;
		auto value_type      = 0u;

		const auto error = RegEnumValueA( registry_key, index, value_name, ( DWORD* )&value_name_size, nullptr, ( DWORD* )&value_type,
		                                  ( BYTE* )value_data, ( DWORD* )&value_data_size );

		index++;

		if ( error == ERROR_NO_MORE_ITEMS ) {
			RegCloseKey( registry_key );

			return std::nullopt;
		}

		if ( error || value_type != REG_SZ )
			continue;

		if ( _strnicmp( font_name.data( ), value_name, font_name.size( ) ) != 0 )
			continue;

		const auto result = fonts_directory + std::string( value_data, value_data_size );

		RegCloseKey( registry_key );

		return result;
	} while ( true );
}

static ImFont* create_system_font( std::string_view font_name, float size, int flags, std::initializer_list< ImWchar > glyph_ranges )
{
	const auto font_path_opt = find_system_font( font_name );

	if ( !font_path_opt.has_value( ) )
		return nullptr;

	const auto font_path = font_path_opt.value( );

	ImFontConfig font_config;

	font_config.GlyphRanges      = glyph_ranges.size( ) == 0 ? nullptr : glyph_ranges.begin( );
	font_config.FontBuilderFlags = flags;

	return ImGui::GetIO( ).Fonts->AddFontFromFileTTF( font_path.c_str( ), size, &font_config, nullptr );
}

static ImFont* create_memory_font( std::span< uint8_t > font_data, float size, int flags, std::initializer_list< ImWchar > glyph_ranges )
{
	ImFontConfig font_config;

	font_config.FontData         = std::malloc( font_data.size_bytes( ) );
	font_config.FontDataSize     = font_data.size_bytes( );
	font_config.SizePixels       = size;
	font_config.GlyphRanges      = glyph_ranges.size( ) == 0 ? nullptr : glyph_ranges.begin( );
	font_config.FontBuilderFlags = flags;

	memcpy( font_config.FontData, font_data.data( ), font_data.size_bytes( ) );

	return ImGui::GetIO( ).Fonts->AddFont( &font_config );
}

float utils::render::get_fps( )
{
	return 1000.0f / ImGui::GetIO( ).Framerate;
}

void utils::render::color_convert_rgb_to_hsv( float r, float g, float b, float& h, float& s, float& v )
{
	ImGui::ColorConvertRGBtoHSV( r, g, b, h, s, v );
}

void utils::render::color_convert_hsv_to_rgb( float h, float s, float v, float& r, float& g, float& b )
{
	ImGui::ColorConvertHSVtoRGB( h, s, v, r, g, b );
}
