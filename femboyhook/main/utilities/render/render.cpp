#include "render.h"
#include "../../menu/gui/render_wrapper.h"
#include <numbers>
#include "../../includes.h"

#include "fonts/ink_free.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"

void render_t::on_end_scene( const std::function< void( ) >& function, IDirect3DDevice9* device )
{
	if ( !this->m_initialised ) {
		ImGui::CreateContext( );
		ImGui_ImplWin32_Init( input.m_window );
		ImGui_ImplDX9_Init( device );

		this->m_device = device;

		[ & ]( ) {
			ImGuiIO& io = ImGui::GetIO( );

			char windows_directory[ 64 ] = { };
			GetWindowsDirectoryA( windows_directory, sizeof( windows_directory ) );

			this->m_segoe_ui = io.Fonts->AddFontFromFileTTF(
				std::vformat( xs( "{}\\Fonts\\Segoeui.ttf" ), std::make_format_args( windows_directory ) ).c_str( ), 18.f );

			this->m_flags_font = io.Fonts->AddFontFromFileTTF(
				std::vformat( xs( "{}\\Fonts\\Segoeui.ttf" ), std::make_format_args( windows_directory ) ).c_str( ), 12.f );

			this->m_indicator_fonts[ e_indicator_fonts::e_ink_free ] =
				io.Fonts->AddFontFromMemoryCompressedTTF( f_inkfree_data, f_inkfree_size, 32.f );

			this->m_indicator_fonts[ e_indicator_fonts::e_verdana ] = io.Fonts->AddFontFromFileTTF(
				std::vformat( xs( "{}\\Fonts\\Verdanab.ttf" ), std::make_format_args( windows_directory ) ).c_str( ), 32.f );

			this->m_indicator_fonts[ e_indicator_fonts::e_tahoma ] = io.Fonts->AddFontFromFileTTF(
				std::vformat( xs( "{}\\Fonts\\Tahomabd.ttf" ), std::make_format_args( windows_directory ) ).c_str( ), 32.f );

			utils::render::fonts[ utils::render::FONT_SEGOE_UI_16 ] = io.Fonts->AddFontFromFileTTF(
				std::vformat( xs( "{}\\Fonts\\Segoeui.ttf" ), std::make_format_args( windows_directory ) ).c_str( ), 16.f );

			utils::render::fonts[ utils::render::FONT_SEGOE_UI_18 ] = io.Fonts->AddFontFromFileTTF(
				std::vformat( xs( "{}\\Fonts\\Segoeui.ttf" ), std::make_format_args( windows_directory ) ).c_str( ), 18.f );

			ImGuiFreeType::BuildFontAtlas( io.Fonts, 0x0 );
		}( );

		this->m_initialised = true;
	}

	ImGui_ImplDX9_NewFrame( );
	ImGui_ImplWin32_NewFrame( );
	ImGui::NewFrame( );

	ImDrawList* draw_list = ImGui::GetBackgroundDrawList( );

	/* render cached draw data */
	[ & ]( ) {
		std::unique_lock< std::shared_mutex > lock( this->m_mutex );

		if ( this->m_thread_safe_draw_data.empty( ) )
			return;

		for ( const draw_object_t& data : this->m_thread_safe_draw_data ) {
			if ( !data.m_obj.has_value( ) )
				continue;

			switch ( data.m_type ) {
			case e_draw_type::draw_type_text: {
				const auto& obj = std::any_cast< text_draw_object_t >( data.m_obj );
				this->text( draw_list, obj.m_font, obj.m_position, obj.m_text, obj.m_color, obj.m_outline_color, obj.m_draw_flags );
				break;
			}
			case e_draw_type::draw_type_rect: {
				const auto& obj = std::any_cast< rect_draw_object_t >( data.m_obj );
				this->rect( draw_list, obj.m_position, obj.m_size, obj.m_color, obj.m_rounding, obj.m_corners, obj.m_thickness, obj.m_draw_flags,
				            obj.m_outline_color );
				break;
			}
			case e_draw_type::draw_type_rect_filled: {
				const auto& obj = std::any_cast< rect_filled_draw_object_t >( data.m_obj );
				this->rect_filled( draw_list, obj.m_position, obj.m_size, obj.m_color, obj.m_rounding, obj.m_draw_flags );
				break;
			}
			case e_draw_type::draw_type_line: {
				const auto& obj = std::any_cast< line_draw_object_t >( data.m_obj );
				this->line( draw_list, obj.m_start, obj.m_end, obj.m_color, obj.m_thickness );
				break;
			}
			case e_draw_type::draw_type_texture: {
				const auto& obj = std::any_cast< texture_draw_object_t >( data.m_obj );
				this->texture( draw_list, obj.m_position, obj.m_size, obj.m_color, obj.m_texture_id );
				break;
			}
			case e_draw_type::draw_type_gradient: {
				const auto& obj = std::any_cast< grad_rect_draw_object_t >( data.m_obj );
				this->gradient_rect_filled( draw_list, obj.m_position, obj.m_size, obj.m_color1, obj.m_color2, obj.horizontal );
			}
			default:
				break;
			}
		}
	}( );

	ImGui::GetBackgroundDrawList( )->ChannelsSplit( 11 );

	function( );

	ImGui::GetBackgroundDrawList( )->ChannelsMerge( );

	ImGui::EndFrame( );
	ImGui::Render( );

	ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData( ) );
}

void render_t::on_release( )
{
	ImGui_ImplDX9_Shutdown( );
	ImGui_ImplWin32_Shutdown( );
	ImGui::DestroyContext( );
}

void render_t::gradient_line( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, c_color color )
{
	draw_list->AddRectFilledMultiColor( ImVec2( position.m_x, position.m_y ), ImVec2( position.m_x + size.m_x / 2, position.m_y + 1 ),
	                                    color.get_u32( 0.f ), color.get_u32( ), color.get_u32( ), color.get_u32( 0.f ) );

	draw_list->AddRectFilledMultiColor( ImVec2( position.m_x + size.m_x / 2, position.m_y ),
	                                    ImVec2( ( position.m_x + size.m_x / 2 ) + size.m_x / 2, position.m_y + 1 ), color.get_u32( ),
	                                    color.get_u32( 0.f ), color.get_u32( 0.f ), color.get_u32( ) );
}

void render_t::gradient_rect_filled( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& first_color,
                                     const c_color& second_color, bool horizontal )
{
	draw_list->AddRectFilledMultiColor( ImVec2( position.m_x, position.m_y ), ImVec2( position.m_x + size.m_x, position.m_y + size.m_y ),
	                                    first_color.get_u32( ), horizontal ? second_color.get_u32( ) : first_color.get_u32( ),
	                                    second_color.get_u32( ), horizontal ? first_color.get_u32( ) : second_color.get_u32( ) );
}

void render_t::rect_filled( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& color, float rounding,
                            int draw_flags )
{
	draw_list->AddRectFilled( ImVec2( position.m_x, position.m_y ), ImVec2( position.m_x + size.m_x, position.m_y + size.m_y ), color.get_u32( ),
	                          rounding, draw_flags );
}

void render_t::texture( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& color, ImTextureID texture_id )
{
	draw_list->AddImage( texture_id, ImVec2( position.m_x, position.m_y ), ImVec2( position.m_x + size.m_x, position.m_y + size.m_y ), ImVec2( 0, 0 ),
	                     ImVec2( 1, 1 ), color.get_u32( ) );
}

void render_t::circle_filled( ImDrawList* draw_list, const c_vector_2d& anchor, float radius, const c_color& color )
{
	draw_list->AddCircleFilled( ImVec2( anchor.m_x, anchor.m_y ), radius, color.get_u32( ), 0 );
}

void render_t::rect( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& color, float rounding, int corners,
                     float thickness, e_rect_render_flags draw_flags, const c_color& outline_color )
{
	const ImVec2 rectangle_draw_positions[ 2 ] = { ImVec2( position.m_x, position.m_y ), ImVec2( position.m_x + size.m_x, position.m_y + size.m_y ) };

	switch ( draw_flags ) {
	case e_rect_render_flags::rect_render_flag_inner_outline: {
		draw_list->AddRect( ImVec2( rectangle_draw_positions[ 0 ].x + 1, rectangle_draw_positions[ 0 ].y + 1 ),
		                    ImVec2( rectangle_draw_positions[ 1 ].x - 1, rectangle_draw_positions[ 1 ].y - 1 ), outline_color.get_u32( ), rounding,
		                    corners, thickness );
		break;
	}
	case e_rect_render_flags::rect_render_flag_outer_outline: {
		draw_list->AddRect( ImVec2( rectangle_draw_positions[ 0 ].x - 1, rectangle_draw_positions[ 0 ].y - 1 ),
		                    ImVec2( rectangle_draw_positions[ 1 ].x + 1, rectangle_draw_positions[ 1 ].y + 1 ), outline_color.get_u32( ), rounding,
		                    corners, thickness );
		break;
	}
	case e_rect_render_flags::rect_render_flag_full_outline: {
		draw_list->AddRect( ImVec2( rectangle_draw_positions[ 0 ].x + 1, rectangle_draw_positions[ 0 ].y + 1 ),
		                    ImVec2( rectangle_draw_positions[ 1 ].x - 1, rectangle_draw_positions[ 1 ].y - 1 ), outline_color.get_u32( ), rounding,
		                    corners, thickness );

		draw_list->AddRect( ImVec2( rectangle_draw_positions[ 0 ].x - 1, rectangle_draw_positions[ 0 ].y - 1 ),
		                    ImVec2( rectangle_draw_positions[ 1 ].x + 1, rectangle_draw_positions[ 1 ].y + 1 ), outline_color.get_u32( ), rounding,
		                    corners, thickness );
		break;
	}
	}

	draw_list->AddRect( ImVec2( rectangle_draw_positions[ 0 ].x, rectangle_draw_positions[ 0 ].y ),
	                    ImVec2( rectangle_draw_positions[ 1 ].x, rectangle_draw_positions[ 1 ].y ), color.get_u32( ), rounding, corners, thickness );
}

void render_t::text( ImDrawList* draw_list, ImFont* font, const c_vector_2d& position, const std::string& text, const c_color& color,
                     const c_color& outline_color, e_text_render_flags draw_flags )
{
	if ( !font )
		return;

	if ( draw_flags & e_text_render_flags::text_render_flag_dropshadow )
		draw_list->AddText( font, font->FontSize, ImVec2( position.m_x + 1.f, position.m_y + 1.f ), outline_color.get_u32( ), text.c_str( ) );
	else if ( draw_flags & e_text_render_flags::text_render_flag_outline ) {
		draw_list->AddText( font, font->FontSize, ImVec2( position.m_x + 1.f, position.m_y - 1.f ), outline_color.get_u32( ), text.c_str( ) );
		draw_list->AddText( font, font->FontSize, ImVec2( position.m_x - 1.f, position.m_y + 1.f ), outline_color.get_u32( ), text.c_str( ) );
	}

	draw_list->AddText( font, font->FontSize, ImVec2( position.m_x, position.m_y ), ImColor( color.m_r, color.m_g, color.m_b, color.m_a ),
	                    text.c_str( ) );
}

void corner_rect( float x1, float y1, float x2, float y2, const c_color& color ) { }

c_vector_2d render_t::measure_text( ImFont* font, const char* text )
{
	const auto size = font->CalcTextSizeA( font->FontSize, FLT_MAX, 0.0f, text );

	return { size.x, size.y };
}

void render_t::circle( ImDrawList* draw_list, const c_vector_2d& anchor, float radius, const c_color& color, float thickness )
{
	draw_list->AddCircle( ImVec2( anchor.m_x, anchor.m_y ), radius, color.get_u32( ), 0, thickness );
}

void render_t::line( ImDrawList* draw_list, const c_vector_2d& start, const c_vector_2d& end, const c_color& color, float thickness )
{
	draw_list->AddLine( ImVec2( start.m_x, start.m_y ), ImVec2( end.m_x, end.m_y ), color.get_u32( ), thickness );
}

int render_t::get_layer( )
{
	return ImGui::GetBackgroundDrawList( )->_Splitter._Current;
}

void render_t::set_layer( int layer )
{
	IM_ASSERT( layer <= 10 );

	ImGui::GetBackgroundDrawList( )->ChannelsSetCurrent( layer );
}

void render_t::set_clip( ImDrawList* draw_list, const c_vector_2d& min, const c_vector_2d& max, bool intersect )
{
	draw_list->PushClipRect( ImVec2( min.m_x, min.m_y ), ImVec2( max.m_x, max.m_y ), intersect );
}

void render_t::set_clip_fullscreen( ImDrawList* draw_list )
{
	draw_list->PushClipRectFullScreen( );
}

void render_t::pop_clip( ImDrawList* draw_list )
{
	draw_list->PopClipRect( );
}

void render_t::reset_clip( ImDrawList* draw_list )
{
	draw_list->_ClipRectStack.clear( );
	draw_list->PushClipRectFullScreen( );
}

bool render_t::world_to_screen( const c_vector& origin, ImVec2& screen )
{
	/* NOTE ~ float ~ call this everytime you want to access the view matrix un-threadsafely ? (lolz) */
	std::unique_lock< std::shared_mutex > lock( this->m_mutex );

	const view_matrix_t& world_to_screen_matrix = this->get_world_to_screen_matrix( );

	const float width = world_to_screen_matrix[ 3 ][ 0 ] * origin.m_x + world_to_screen_matrix[ 3 ][ 1 ] * origin.m_y +
	                    world_to_screen_matrix[ 3 ][ 2 ] * origin.m_z + world_to_screen_matrix[ 3 ][ 3 ];

	if ( width < 0.001f )
		return false;

	const float inverse = 1.0f / width;
	screen.x            = ( world_to_screen_matrix[ 0 ][ 0 ] * origin.m_x + world_to_screen_matrix[ 0 ][ 1 ] * origin.m_y +
                 world_to_screen_matrix[ 0 ][ 2 ] * origin.m_z + world_to_screen_matrix[ 0 ][ 3 ] ) *
	           inverse;
	screen.y = ( world_to_screen_matrix[ 1 ][ 0 ] * origin.m_x + world_to_screen_matrix[ 1 ][ 1 ] * origin.m_y +
	             world_to_screen_matrix[ 1 ][ 2 ] * origin.m_z + world_to_screen_matrix[ 1 ][ 3 ] ) *
	           inverse;

	const ImVec2 display_size = ImGui::GetIO( ).DisplaySize;
	screen.x                  = ( display_size.x * 0.5f ) + ( screen.x * display_size.x ) * 0.5f;
	screen.y                  = ( display_size.y * 0.5f ) - ( screen.y * display_size.y ) * 0.5f;
	return true;
}

static view_matrix_t view_matrix = { };

void render_t::update_world_to_screen_matrix( )
{
	view_matrix = interfaces.m_engine->world_to_screen_matrix( );
}

const view_matrix_t& render_t::get_world_to_screen_matrix( )
{
	return view_matrix;
}

IDirect3DTexture9* render_t::rasterize_vector( char* data, const float scale )
{
	NSVGimage* image = nsvgParse( data, "px", 96.0f );
	if ( !image ) {
		nsvgDelete( image );
	}

	NSVGrasterizer* rasterizer = nsvgCreateRasterizer( );
	if ( !rasterizer ) {
		nsvgDeleteRasterizer( rasterizer );
		nsvgDelete( image );
	}

	const int w = ( int )( image->width * scale );
	const int h = ( int )( image->height * scale );

	auto bytes = std::make_unique< uint8_t[] >( w * h * 4 );

	nsvgRasterize( rasterizer, image, 0, 0, scale, bytes.get( ), w, h, w * 4 );

	IDirect3DTexture9* texture;

	if ( this->m_device->CreateTexture( w, h, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &texture, NULL ) < 0 )
		return nullptr;

	D3DLOCKED_RECT tex_locked_rect;

	if ( texture->LockRect( 0, &tex_locked_rect, NULL, 0 ) != D3D_OK )
		return nullptr;

	for ( int y = 0; y < h; ++y ) {
		for ( int x = 0; x < w; ++x ) {
			const unsigned char* source_pixel = bytes.get( ) + w * 4 * y + x * 4;
			unsigned char* destination_pixel  = static_cast< unsigned char* >( tex_locked_rect.pBits ) + tex_locked_rect.Pitch * y + x * 4;

			destination_pixel[ 0 ] = source_pixel[ 2 ];
			destination_pixel[ 1 ] = source_pixel[ 1 ];
			destination_pixel[ 2 ] = source_pixel[ 0 ];
			destination_pixel[ 3 ] = source_pixel[ 3 ];
		}
	}

	texture->UnlockRect( 0 );

	return texture;
}

void render_t::circle_filled_3d( const c_vector& origin, float radius, c_color color )
{
	std::vector< c_vector > Points3D;
	float step = static_cast< float >( std::numbers::pi ) * 2.0f / 256;

	for ( float a = 0; a < ( std::numbers::pi * 2.0f ); a += step ) {
		c_vector start( radius * cosf( a ) + origin.m_x, radius * sinf( a ) + origin.m_y, origin.m_z );
		c_vector end( radius * cosf( a + step ) + origin.m_x, radius * sinf( a + step ) + origin.m_y, origin.m_z );

		ImVec2 out, out1, pos3d;

		if ( this->world_to_screen( end, out1 ) && this->world_to_screen( start, out ) ) {
			if ( this->world_to_screen( origin, pos3d ) ) {
				ImGui::GetBackgroundDrawList( )->AddTriangleFilled( out, out1, pos3d, color.get_u32( ) );
			}
		}
	}
}