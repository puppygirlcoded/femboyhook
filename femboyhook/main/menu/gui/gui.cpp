#include <map>
#include <stack>

#include "gui.h"
#include "input_wrapper.h"
#include "../menu.h"
#include "render_wrapper.h"
#include <WinUser.h>
#include <format>
#include <iostream>
#include <windows.h>

struct context {
	sdk::vector2 pos;
	sdk::vector2 size;
	sdk::vector2 min_size;

	sdk::vector2 interaction_reference;
	sdk::vector2 interaction_mouse_pos;

	bool is_dragging  = false;
	bool is_resizing  = false;
	bool group_switch = false;

	int blocking_hash    = 0;
	int tab_selected     = 0;
	int tab_selected_new = 0;
	int tab_count        = 0;

	int category_selected     = 0;
	int category_selected_new = 0;
	int category_count        = 0;

	int item_height        = 0;
	int tabs_offset        = 0;
	int group_offset_left  = 0;
	int group_offset_right = 0;

	sdk::color accent;
	sdk::color accent_dark;
	sdk::color accent_light;

	int copied_color_r, copied_color_g, copied_color_b, copied_color_a = 255;


	std::stack< sdk::vector2 > cursor_pos;
	std::stack< sdk::vector2 > working_area;
	std::map< int, float > animations;

	inline sdk::vector2 get_cursor( )
	{
		return cursor_pos.top( );
	}

	inline sdk::vector2 get_work_area( )
	{
		return working_area.top( );
	}

	inline sdk::vector2 pop_cursor( )
	{
		const auto top = cursor_pos.top( );

		cursor_pos.pop( );

		return top;
	}

	inline sdk::vector2 pop_work_area( )
	{
		const auto top = working_area.top( );

		working_area.pop( );

		return top;
	}

	inline void push_cursor( const sdk::vector2& pos )
	{
		cursor_pos.push( pos );
	}

	inline void push_work_area( const sdk::vector2& area_size )
	{
		working_area.push( area_size );
	}
};

static std::shared_ptr< context > ctx;

constexpr uint32_t fnv1a_32( const char* str, size_t count )
{
	return ( ( count ? fnv1a_32( str, count - 1 ) : 2166136261u ) ^ str[ count ] ) * 16777619u;
}

constexpr uint32_t fnv1a_32( std::string_view str )
{
	return fnv1a_32( str.data( ), str.size( ) );
}

constexpr uint32_t operator"" _hash( const char* str, size_t count )
{
	return fnv1a_32( str, count );
}

float gui::get_opacity( bool is_open )
{
	if ( ctx == nullptr )
		return 0.0f;

	return detail::handle_animation( *&ctx->animations[ "__opacity"_hash ], is_open ? 1.0f : 0.0f );
}

// sdk::color accent = { 233, 30, 99 };

bool gui::begin_window( std::string_view title, const sdk::vector2& initial_pos, const sdk::vector2& initial_size )
{
	using namespace utils;

	if ( ctx == nullptr ) {
		ctx = std::make_shared< context >( );

		ctx->pos      = initial_pos;
		ctx->size     = initial_size;
		ctx->min_size = initial_size;

		ctx->animations[ "__opacity"_hash ] = 0.0f;
		ctx->animations[ "__title"_hash ]   = 0.0f;
	}

	const auto opacity_int = ( int )( ctx->animations[ "__opacity"_hash ] * 255 );

	if ( opacity_int == 0 )
		return false;

	ctx->accent       = menu.accent.adjust_alpha( 255 );
	ctx->accent_light = ctx->accent.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );
	ctx->accent_dark  = ctx->accent.to_hsla( ).adjust_lum( -0.15 ).to_rgba( );

	render::set_layer( 1 );

	const auto is_interacting      = ctx->is_dragging || ctx->is_resizing;
	const auto title_size          = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto resize_grip_hovered = input_wrapper::is_hovering( ctx->pos + ctx->size - 4, ctx->pos + ctx->size + 4 );
	const auto title_bar_hovered =
		input_wrapper::is_hovering( ctx->pos, { ctx->pos.x + title_size.x + 24, ctx->pos.y + 24 } ) ||
		input_wrapper::is_hovering( { ctx->pos.x + ctx->tabs_offset - 4, ctx->pos.y }, { ctx->pos.x + ctx->size.x, ctx->pos.y + 24 } );

	const auto active_user_text = xs("build:"); // std::format("active user:");
	const auto username_text    = xs("release");    // std::format("{}", std::make_format_args("developer"));

	const auto active_user_text_size = render::measure_text( active_user_text, render::FONT_SEGOE_UI_18 );
	const auto username_text_size    = render::measure_text( username_text, render::FONT_SEGOE_UI_18 );

	if ( !is_interacting ) {
		if ( resize_grip_hovered )
			input_wrapper::set_cursor( input_wrapper::CURSOR_RESIZE_NWSE );

		if ( input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->interaction_mouse_pos = input_wrapper::get_mouse_pos( );

			if ( title_bar_hovered ) {
				ctx->interaction_reference = ctx->pos;
				ctx->is_dragging           = true;
			} else if ( resize_grip_hovered ) {
				ctx->interaction_reference = ctx->size;
				ctx->is_resizing           = true;
			}
		}
	} else if ( is_interacting && input_wrapper::is_mouse_down( input_wrapper::MOUSE_LEFT ) ) {
		if ( ctx->is_dragging ) {
			const auto window_size  = render::get_window_size( );
			const auto mouse_delta  = input_wrapper::get_mouse_pos( ) - ctx->interaction_mouse_pos;
			const auto new_position = ctx->interaction_reference + mouse_delta;

			ctx->pos = { std::clamp( new_position.x, 8, window_size.x - ctx->size.x - 8 ),
				         std::clamp( new_position.y, 8, window_size.y - ctx->size.y - 8 ) };
		} else if ( ctx->is_resizing ) {
			input_wrapper::set_cursor( input_wrapper::CURSOR_RESIZE_NWSE );

			const auto window_size = render::get_window_size( );
			const auto mouse_delta = input_wrapper::get_mouse_pos( ) - ctx->interaction_mouse_pos;
			const auto new_size    = ctx->interaction_reference + mouse_delta;

			ctx->size = { std::clamp( new_size.x, ctx->min_size.x, window_size.x - ctx->pos.x - 8 ),
				          std::clamp( new_size.y, ctx->min_size.y, window_size.y - ctx->pos.y - 8 ) };
		}
	} else {
		ctx->is_dragging = false;
		ctx->is_resizing = false;
	}

	const auto title_animation = detail::handle_animation( ctx->animations[ "__title"_hash ], ctx->is_dragging ? 1.0f : 0.0f );
	const auto title_color     = sdk::color{ 210 + ( int )( title_animation * 30 ), opacity_int };

	render::fill_rect( ctx->pos, ctx->size, { 12, 12, 12, opacity_int }, 3 );
	render::fill_rect( ctx->pos, { ctx->size.x, 24 }, { 15, 15, 15, opacity_int }, 3, render::CORNER_TOP );
	render::fill_rect( { ctx->pos.x, ctx->pos.y + ctx->size.y - 22 }, { ctx->size.x, 24 }, { 15, 15, 15, opacity_int }, 3, render::CORNER_BOTTOM );

	const auto categories_pos  = sdk::vector2{ ctx->pos.x + 8, ctx->pos.y + 31 };
	const auto categories_size = sdk::vector2{ 160, ctx->size.y - 62 };

	render::fill_rect( categories_pos, categories_size, { 12, 12, 12, opacity_int }, 3.f );
	render::rect( categories_pos - 1, categories_size + 2, { 20, 20, 20, opacity_int }, 3.f );

	render::rect( ctx->pos - 1, ctx->size + 2, { 20, 20, 20, opacity_int }, 3 );
	render::rect( ctx->pos - 1, { ctx->size.x + 2, 24 }, { 20, 20, 20, opacity_int }, 3, render::CORNER_TOP );
	render::rect( { ctx->pos.x - 1, ctx->pos.y + ctx->size.y - 23 }, { ctx->size.x + 2, 24 }, { 20, 20, 20, opacity_int }, 3, render::CORNER_BOTTOM );

	render::text( { ctx->pos.x + 8, ctx->pos.y + 10 - title_size.y / 2 }, title, render::FONT_SEGOE_UI_18, title_color );

	render::text( { ctx->pos.x + 8, ctx->pos.y + ctx->size.y - 12 - active_user_text_size.y / 2 }, active_user_text, render::FONT_SEGOE_UI_18,
	              { 210, 210, 210, opacity_int } );

	render::text( { ctx->pos.x + 8 + active_user_text_size.x + 5, ctx->pos.y + ctx->size.y - 12 - username_text_size.y / 2 }, username_text,
	              render::FONT_SEGOE_UI_18, ctx->accent.adjust_alpha( opacity_int ) );

	ctx->group_switch   = false;
	ctx->tab_count      = 0;
	ctx->category_count = 0;
	ctx->tabs_offset    = title_size.x + 24;

	ctx->push_work_area( { ctx->size.x - 160 - 24, categories_size.y } );
	ctx->push_cursor( { categories_size.x + 16, 31 } );

	return true;
}

bool gui::begin_tab( std::string_view title )
{
	using namespace utils;

	const auto opacity     = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int = ( int )( opacity * 255 );

	const auto is_active  = ctx->tab_selected == ctx->tab_count;
	const auto title_size = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto position   = sdk::vector2{ ctx->pos.x + 8 + ctx->tabs_offset, ctx->pos.y + 10 - title_size.y / 2 };

	const auto bg_position = sdk::vector2{ position.x - 5, ctx->pos.y + 3 };
	const auto bg_size     = sdk::vector2{ title_size.x + 10, 20 };
	const auto bg_hovered  = input_wrapper::is_hovering( bg_position, bg_position + bg_size );

	const auto tab_animation   = detail::handle_animation( ctx->animations[ "__tab"_hash + ctx->tab_count ], is_active    ? 1.0f
	                                                                                                         : bg_hovered ? 0.5f
	                                                                                                                      : 0.0f );
	const auto title_animation = detail::handle_animation( ctx->animations[ "__tab_active"_hash + ctx->tab_count ], is_active ? 1.0f : 0.0f );
	const auto title_color     = sdk::color{ 150 + ( int )( tab_animation * 60 ), opacity_int };

	if ( is_active || bg_hovered || title_animation > 0.0f ) {
		const auto bg_color      = sdk::color{ 12, ( int )( tab_animation * opacity * 255 ) };
		const auto outline_color = sdk::color{ 20, ( int )( tab_animation * opacity * 255 ) };

		const auto half_tab_width = bg_size.x / 2 + 1;
		const auto clip_position  = bg_position + sdk::vector2{ half_tab_width, 0 };

		render::rect( bg_position - 1, { bg_size.x + 2, bg_size.y + 1 }, outline_color, 3, render::CORNER_TOP );

		render::set_clip( { clip_position.x - ( int )( half_tab_width * title_animation ), clip_position.y - 1 },
		                  { clip_position.x + ( int )( half_tab_width * title_animation ) - 2, clip_position.y + 3 } );

		render::rect( bg_position - 1, { bg_size.x + 2, bg_size.y + 1 },
		              ctx->accent.adjust_alpha( ( int )( ( is_active ? opacity : title_animation ) * opacity * 255 ) ), 3, render::CORNER_TOP );

		render::pop_clip( );

		render::fill_rect( bg_position, bg_size + sdk::vector2{ 0, 1 }, bg_color, 3, render::CORNER_TOP );

		if ( bg_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->category_selected_new = 0;
			ctx->tab_selected_new      = ctx->tab_count;
		}
	}

	render::text( position, title, render::FONT_SEGOE_UI_18, title_color );

	ctx->tab_count += 1;
	ctx->tabs_offset += title_size.x + 18;

	return ctx->tab_selected == ctx->tab_count - 1;
}

bool gui::begin_category( std::string_view title )
{
	using namespace utils;

	constexpr auto category_spacing = 5;
	constexpr auto category_height  = 23;

	const auto category_offset = ( category_height + category_spacing ) * ctx->category_count;
	const auto category_pos    = sdk::vector2{ ctx->pos.x + 8 + category_spacing, ctx->pos.y + 31 + category_spacing + category_offset };
	const auto category_size   = sdk::vector2{ 130 - category_spacing * 2, category_height };
	const auto is_hovered      = input_wrapper::is_hovering( category_pos, { category_pos.x + category_size.x + 30, category_pos.y + category_size.y } );
	const auto opacity         = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int     = ( int )( opacity * 255 );
	const auto accent_light    = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );

	const auto is_active          = ctx->category_selected == ctx->category_count;
	const auto title_size         = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto category_animation = detail::handle_animation( ctx->animations[ "__category"_hash + ctx->category_count ], is_active    ? 1.0f
	                                                                                                                      : is_hovered ? 0.5f
	                                                                                                                                   : 0.0f );

	render::fill_gradient( category_pos, category_size, ctx->accent.adjust_alpha( ( int )( category_animation * opacity * 24 ) ),
	                       accent_light.adjust_alpha( 0 ) );
	render::fill_rect( category_pos, { 1, category_size.y }, ctx->accent.adjust_alpha( ( int )( category_animation * opacity * 255 ) ) );

	const auto title_color    = sdk::color{ 150 + ( int )( category_animation * 60 ), opacity_int };
	const auto title_position = sdk::vector2{ category_pos.x + 8, category_pos.y + category_height / 2 - title_size.y / 2 - 1 };

	render::text( title_position, title, render::FONT_SEGOE_UI_18, title_color );

	if ( is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) )
		ctx->category_selected_new = ctx->category_count;

	ctx->category_count += 1;

	return ctx->category_selected == ctx->category_count - 1;
}

bool gui::begin_group( std::string_view title )
{
	using namespace utils;

	const auto opacity_int     = ( int )( ctx->animations[ "__opacity"_hash ] * 255 );
	const auto title_size      = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto cursor_pos      = ctx->get_cursor( );
	const auto working_area    = ctx->get_work_area( );
	const auto max_group_width = working_area.x / 2 - 4;
	const auto group_pos       = ctx->group_switch ? sdk::vector2{ cursor_pos.x + max_group_width + 8, cursor_pos.y + ctx->group_offset_right }
	                                               : sdk::vector2{ cursor_pos.x, cursor_pos.y + ctx->group_offset_left };

	ctx->push_cursor( group_pos );
	ctx->push_cursor( { group_pos.x + 6, group_pos.y + 30 } );
	ctx->push_cursor( { group_pos.x + 6, group_pos.y + 30 } );
	ctx->push_work_area( { max_group_width - 12, INT_MAX } );

	render::set_layer( 3 );

	const auto title_position = sdk::vector2{ group_pos.x + 8, group_pos.y + 12 - title_size.y / 2 - 2 };

	render::text( ctx->pos + title_position, title, render::FONT_SEGOE_UI_18, { 210, 210, 210, opacity_int } );

	return true;
}

void gui::push_fake_group( ) { 
if ( ctx->group_switch ) {
		//ctx->group_offset_right += group_height + 8;
		ctx->group_switch = false;
	} else {
		//ctx->group_offset_left += group_height + 8;
		ctx->group_switch = true;
	}
}


void gui::finish_window( )
{
	ctx->tab_selected      = ctx->tab_selected_new;
	ctx->category_selected = ctx->category_selected_new;

	ctx->cursor_pos   = { };
	ctx->working_area = { };

	utils::render::set_layer( 0 );
}

void gui::finish_tab( )
{
	// @note: just a complementary method
	//        doesn't do anything (as of now)
}

void gui::finish_category( )
{
	ctx->group_switch       = false;
	ctx->group_offset_left  = 0;
	ctx->group_offset_right = 0;
}

void gui::finish_group( )
{
	using namespace utils;

	const auto opacity_int      = ( int )( ctx->animations[ "__opacity"_hash ] * 255 );
	const auto last_cursor_pos  = ctx->pop_cursor( );
	const auto first_cursor_pos = ctx->pop_cursor( );
	const auto group_pos        = ctx->pop_cursor( );
	const auto group_work_area  = ctx->pop_work_area( );
	const auto group_width      = group_work_area.x + 12;
	const auto group_height     = std::max( last_cursor_pos.y - first_cursor_pos.y + 6, 12 ) + 24;

	render::set_layer( 2 );

	render::fill_rect( ctx->pos + group_pos, { group_width, group_height }, { 12, 12, 12, opacity_int }, 3.f );
	render::fill_rect( ctx->pos + group_pos, { group_width, 24 }, { 15, 15, 15, opacity_int }, 3.f, ImDrawCornerFlags_Top );

	render::rect( ctx->pos + group_pos - 1, { group_width + 2, 24 }, { 20, 20, 20, opacity_int }, 3.f, ImDrawCornerFlags_Top );
	render::rect( ctx->pos + group_pos - 1, { group_width + 2, group_height + 2 }, { 20, 20, 20, opacity_int }, 3.f );

	if ( ctx->group_switch ) {
		ctx->group_offset_right += group_height + 8;
		ctx->group_switch = false;
	} else {
		ctx->group_offset_left += group_height + 8;
		ctx->group_switch = true;
	}
}

void gui::label( std::string_view title, int additional_spacing, const sdk::color& color )
{
	using namespace utils;

	const auto opacity_int = ( int )( ctx->animations[ "__opacity"_hash ] * 255 );
	const auto cursor_pos  = ctx->pop_cursor( );
	const auto title_size  = render::measure_text( title, render::FONT_SEGOE_UI_18 );

	render::text( ctx->pos + cursor_pos - sdk::vector2( 0, 5), title, render::FONT_SEGOE_UI_18, color.adjust_alpha( opacity_int ) );

	ctx->item_height = title_size.y;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + additional_spacing } );
}

void gui::button( std::string_view title, const std::function< void( ) >& callback )
{
	using namespace utils;

	const auto opacity      = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int  = ( int )( opacity * 255 );
	const auto cursor_pos   = ctx->pop_cursor( );
	const auto working_area = ctx->get_work_area( );
	const auto title_size   = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto button_size  = sdk::vector2{ working_area.x, 20 };
	const auto this_hash    = fnv1a_32( title );

	const auto is_hovered      = input_wrapper::is_hovering( ctx->pos + cursor_pos, ctx->pos + cursor_pos + button_size );
	const auto is_hovered_anim = is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered_anim ? 1.0f : 0.0f );
	const auto title_color     = sdk::color{ 180 + ( int )( 30 * hover_animation * opacity ), opacity_int };

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash && input_wrapper::is_mouse_released( input_wrapper::MOUSE_LEFT ) ) {
		if ( is_hovered )
			callback( );

		ctx->blocking_hash = 0;
	}

	// clang-format off

    render::fill_gradient(ctx->pos + cursor_pos, button_size,
        { 12 + (int)(4 * hover_animation * opacity), opacity_int },
        { 14 + (int)(3 * hover_animation * opacity), opacity_int }, false);

	// clang-format on

	const auto title_pos = sdk::vector2{
		ctx->pos.x + cursor_pos.x + button_size.x / 2 - title_size.x / 2,
		ctx->pos.y + cursor_pos.y + button_size.y / 2 - title_size.y / 2 - 1,
	};

	render::rect( ctx->pos + cursor_pos - 1, button_size + 2, { 20, 20, 20, opacity_int }, 3.0f );
	render::text( title_pos, title, render::FONT_SEGOE_UI_18, title_color );

	ctx->item_height = button_size.y;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + 5 } );
}
std::string trim_name( std::string_view name )
{
	if ( name.find( '#' ) > 0 )
		return std::string( name.substr( 0, name.find( '#' ) ) );
	else
		return std::string( name );
}
void gui::checkbox( std::string_view title, bool* value, bool default_value )
{
	using namespace utils;

	const auto opacity     = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int = ( int )( opacity * 255 );
	const auto cursor_pos  = ctx->pop_cursor( );
	const auto title_size  = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto title_pos   = sdk::vector2{ cursor_pos.x + 18, cursor_pos.y + 5 - title_size.y / 2 - 1 };
	const auto this_hash   = fnv1a_32( title );

	if ( ctx->animations.find( "hover"_hash + this_hash ) == ctx->animations.end( ) )
		*value = default_value;

	const auto is_hovered       = input_wrapper::is_hovering( ctx->pos + cursor_pos, ctx->pos + cursor_pos + sdk::vector2{ title_size.x + 18, 12 } );
	const auto is_hovered_anim  = !ctx->blocking_hash && is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation  = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], *value            ? 1.0f
	                                                                                                     : is_hovered_anim ? 0.5f
	                                                                                                                       : 0.0f );
	const auto toggle_animation = detail::handle_animation( ctx->animations[ "toggle"_hash + this_hash ], *value ? 1.0f : 0.0f );
	const auto title_color      = sdk::color{ 180 + ( int )( 30 * hover_animation * opacity ), opacity_int };

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash && input_wrapper::is_mouse_released( input_wrapper::MOUSE_LEFT ) ) {
		if ( is_hovered )
			*value = !*value;

		ctx->blocking_hash = 0;
	}

	// clang-format off

    render::fill_gradient(ctx->pos + cursor_pos, 10,
        { 12 + (int)(4 * hover_animation * opacity), opacity_int },
        { 14 + (int)(3 * hover_animation * opacity), opacity_int }, false);

	// clang-format on

	const auto accent      = ctx->accent.adjust_alpha( toggle_animation * opacity );
	const auto accent_dark = ctx->accent_dark.adjust_alpha( toggle_animation * opacity );

	render::fill_gradient( ctx->pos + cursor_pos, 10, accent, accent_dark, false );

	const auto text_pos = ctx->pos + title_pos;

	render::rect( ctx->pos + cursor_pos - 1, 12, { 20, 20, 20, opacity_int }, 3.0f );
	render::text( { text_pos.x, text_pos.y - 1 }, trim_name(title), render::FONT_SEGOE_UI_18, title_color );

	ctx->item_height = 12;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + 4 } );
}

void gui::slider( std::string_view title, float* value, float min_v, float max_v, float default_value, std::string_view format )
{
	using namespace utils;

	const auto opacity      = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int  = ( int )( opacity * 255 );
	const auto cursor_pos   = ctx->pop_cursor( );
	const auto working_area = ctx->get_work_area( );
	const auto value_str    = std::vformat( format.data( ), std::make_format_args( *value ) ); // fmt::format(format, *value);
	const auto title_size   = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto value_size   = render::measure_text( value_str, render::FONT_SEGOE_UI_18 );
	const auto slider_pos   = sdk::vector2{ cursor_pos.x, cursor_pos.y + title_size.y };
	const auto title_pos    = ctx->pos + cursor_pos - sdk::vector2{ 0, 3 };
	const auto slider_size  = sdk::vector2{ working_area.x, 6 };
	const auto fill_width   = ( int )detail::scale_value( *value, min_v, max_v, 0.0f, ( float )slider_size.x );
	const auto this_hash    = fnv1a_32( title );

	if ( ctx->animations.find( "hover"_hash + this_hash ) == ctx->animations.end( ) )
		*value = default_value;

	const auto is_active         = ctx->blocking_hash == this_hash;
	const auto is_hovered_slider = input_wrapper::is_hovering( ctx->pos + slider_pos, ctx->pos + slider_pos + slider_size );
	const auto is_hovered_text   = input_wrapper::is_hovering( title_pos, title_pos + title_size );
	const auto is_hovered        = is_hovered_slider || is_hovered_text;
	const auto is_hovered_anim   = !ctx->blocking_hash && is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation   = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered_anim ? 1.0f : 0.0f );
	const auto active_animation  = detail::handle_animation( ctx->animations[ "active"_hash + this_hash ], is_active ? 0.85f : 1.0f );
	const auto tooltip_animation = detail::handle_animation( ctx->animations[ "tooltip"_hash + this_hash ], is_active ? 1.0f : 0.0f );

	if ( !ctx->blocking_hash && is_hovered_slider && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash ) {
		const auto mouse_pos = input_wrapper::get_mouse_pos( );
		const auto new_value =
			detail::scale_value( ( float )( mouse_pos.x - ctx->pos.x - slider_pos.x ), 0.0f, ( float )slider_size.x, min_v, max_v );

		*value = std::clamp( new_value, min_v, max_v );

		if ( input_wrapper::is_mouse_released( input_wrapper::MOUSE_LEFT ) )
			ctx->blocking_hash = 0;
	}

	// clang-format off

    render::fill_gradient(ctx->pos + slider_pos, slider_size,
        { 12 + (int)(4 * hover_animation * opacity), opacity_int },
        { 14 + (int)(3 * hover_animation * opacity), opacity_int }, false);

	// clang-format on

	const auto accent      = ctx->accent.adjust_alpha( active_animation * opacity );
	const auto accent_dark = ctx->accent_dark.adjust_alpha( active_animation * opacity );

	render::fill_gradient( ctx->pos + slider_pos, { fill_width, slider_size.y }, accent, accent_dark, false );

	render::rect( ctx->pos + slider_pos - 1, slider_size + 2, { 20, 20, 20, opacity_int }, 3.0f );
	render::text( title_pos, trim_name( title ), render::FONT_SEGOE_UI_18, { 210, 210, 210, opacity_int } );

	const auto tooltip_opacity = ( int )( 255.0f * tooltip_animation * opacity );

	if ( tooltip_opacity > 0 ) {
		const auto mouse_pos    = input_wrapper::get_mouse_pos( );
		const auto tooltip_size = sdk::vector2{ value_size.x + 4, value_size.y };
		const auto tooltip_pos =
			ctx->pos + slider_pos +
			sdk::vector2{ std::clamp( fill_width - tooltip_size.x / 2, 0, slider_size.x - tooltip_size.x ), -tooltip_size.y - 4 };

		const auto previous_layer = render::get_layer( );

		render::set_layer( 4 );
		render::fill_rect( tooltip_pos, tooltip_size, { 12, 12, 12, tooltip_opacity }, 3.0f );
		render::rect( tooltip_pos - 1, tooltip_size + 2, { 20, 20, 20, tooltip_opacity }, 3.0f );
		render::text( { tooltip_pos.x + 2, tooltip_pos.y }, value_str, render::FONT_SEGOE_UI_18, { 210, 210, 210, tooltip_opacity } );
		render::set_layer( previous_layer );
	}

	ctx->item_height = title_size.y + slider_size.y;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + 7 } );
}

void gui::slider( std::string_view title, int* value, int min_v, int max_v, int default_value, std::string_view format )
{
	float value_float = ( float )*value;

	slider( title, &value_float, ( float )min_v, ( float )max_v, ( float )default_value, format );

	*value = ( int )value_float;
}

void gui::dropdown( std::string_view title, int* value, const std::vector< std::string >& elements, int default_value )
{
	using namespace utils;

	const auto opacity       = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int   = ( int )( opacity * 255 );
	const auto cursor_pos    = ctx->pop_cursor( );
	const auto working_area  = ctx->get_work_area( );
	const auto value_str     = elements.at( *value );
	const auto title_size    = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto value_size    = render::measure_text( value_str, render::FONT_SEGOE_UI_18 );
	const auto title_pos     = ctx->pos + cursor_pos - sdk::vector2{ 0, 2 };
	const auto dropdown_pos  = sdk::vector2{ title_pos.x, title_pos.y + title_size.y };
	const auto dropdown_size = sdk::vector2{ working_area.x, 20 };
	const auto dropdown_size_contents =
		sdk::vector2{ ( int )dropdown_size.x, ( int )dropdown_size.y + 4 + ( int )( elements.size( ) * dropdown_size.y ) };
	const auto this_hash    = fnv1a_32( title );
	const auto accent_light = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );

	render::text( title_pos - sdk::vector2{ 0, 3 }, trim_name( title ), render::FONT_SEGOE_UI_18, { 210, 210, 210, opacity_int } );

	const auto is_active                = ctx->blocking_hash == this_hash;
	const auto is_hovered_dropdown      = input_wrapper::is_hovering( dropdown_pos, dropdown_pos + dropdown_size );
	const auto is_hovered_open_dropdown = input_wrapper::is_hovering( dropdown_pos + sdk::vector2( 0, dropdown_size.y + 5 ),
	                                                          dropdown_pos + dropdown_size + sdk::vector2( 0, 5 ) +
	                                                              sdk::vector2( 0, ( int )dropdown_size.y * ( int )elements.size( ) ) );
	const auto is_hovered_text          = input_wrapper::is_hovering( title_pos, title_pos + title_size );
	const auto is_hovered               = is_hovered_dropdown || is_hovered_text;
	const auto is_hovered_anim          = !ctx->blocking_hash && is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation          = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered_anim ? 1.0f : 0.0f );
	const auto active_animation         = detail::handle_animation( ctx->animations[ "active"_hash + this_hash ], is_active ? 0.85f : 1.0f );
	const auto open_animation           = detail::handle_animation( ctx->animations[ "open"_hash + this_hash ], is_active ? 1.0f : 0.0f );

	if ( !ctx->blocking_hash && is_hovered_dropdown && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash ) {
		const auto mouse_pos = input_wrapper::get_mouse_pos( );

		// Change is_hovered_dropdown to dropdown_size_contents
		if ( input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) && !is_hovered_open_dropdown )
			ctx->blocking_hash = 0;
	}

	render::fill_gradient( dropdown_pos, dropdown_size, { 12 + ( int )( 4 * hover_animation * opacity ), opacity_int },
	                       { 14 + ( int )( 3 * hover_animation * opacity ), opacity_int }, false );

	render::rect( dropdown_pos - 1, dropdown_size + 2, { 20, 20, 20, opacity_int }, 3.0f );
	render::text( dropdown_pos + dropdown_size.y / 2 - value_size.y / 2 + sdk::vector2( 5, -1 ), value_str, render::FONT_SEGOE_UI_18,
	              { 210, 210, 210, opacity_int } );

	if ( ctx->blocking_hash == this_hash || open_animation > 0.1f ) {
		render::fill_rect_foreground( dropdown_pos + sdk::vector2{ 0, dropdown_size.y + 5 },
		                              sdk::vector2{ dropdown_size.x, ( int )dropdown_size.y * ( int )elements.size( ) },
		                              { 15, 15, 15, int( opacity_int * open_animation ) } );

		render::rect_foreground( dropdown_pos + sdk::vector2{ 0, dropdown_size.y + 5 },
		                         sdk::vector2{ dropdown_size.x, ( int )dropdown_size.y * ( int )elements.size( ) },
		                         { 20, 20, 20, int( opacity_int * open_animation ) } );

		auto option = [ & ]( const std::string& label, sdk::vector2 pos, sdk::vector2 size, bool active ) -> bool {
			const auto this_hash              = fnv1a_32( label );
			const auto hovered                = input_wrapper::is_hovering( pos, pos + size );
			const auto label_active_animation = detail::handle_animation( ctx->animations[ "open_label"_hash + this_hash ], active    ? 1.0f
			                                                                                                                : hovered ? 0.5f
			                                                                                                                          : 0.0f );
			const auto title_color            = sdk::color{ 150 + ( int )( label_active_animation * 60 ), int( opacity_int * open_animation ) };

			render::fill_gradient_foreground( pos, size,
			                                  ctx->accent.adjust_alpha( ( int )( label_active_animation * opacity * 24 * open_animation ) ),
			                                  accent_light.adjust_alpha( 0 ) );
			render::fill_rect_foreground( pos, sdk::vector2( 1, size.y ),
			                              ctx->accent.adjust_alpha( label_active_animation * open_animation * opacity ) );
			render::text_foreground( pos + size.y / 2 - render::measure_text( label, render::FONT_SEGOE_UI_18 ).y / 2 + sdk::vector2( 4, -1 ), label,
			                         render::FONT_SEGOE_UI_18, title_color );

			return hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT );
		};

		for ( size_t i = 0; i < elements.size( ); i++ ) {
			const auto m_pos = dropdown_pos + sdk::vector2{ 0, dropdown_size.y + 5 + ( ( int )dropdown_size.y * ( int )i ) };

			if ( option( elements.at( i ), m_pos, dropdown_size, *value == i ) ) {
				*value = i;
			}
		}
	}

	ctx->item_height = title_size.y + dropdown_size.y;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + 4 } );
}

void gui::multi_dropdown( std::string_view title, int* value, const std::vector< std::string >& elements )
{
	using namespace utils;

	const auto opacity      = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int  = ( int )( opacity * 255 );
	const auto cursor_pos   = ctx->pop_cursor( );
	const auto working_area = ctx->get_work_area( );
	auto value_str          = std::string( "" );

	if ( *value != 0 )
		for ( size_t i = 0, j = 0; i < elements.size( ); i++ ) {
			if ( *value & ( 1 << i ) ) {
				if ( j )
					value_str += ", " + elements[ i ];
				else
					value_str = elements[ i ];

				j++;
			}
		}

	const auto title_size    = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto value_size    = render::measure_text( value_str, render::FONT_SEGOE_UI_18 );
	const auto title_pos     = ctx->pos + cursor_pos - sdk::vector2{ 0, 2};
	const auto dropdown_pos  = sdk::vector2{ title_pos.x, title_pos.y + title_size.y };
	const auto dropdown_size = sdk::vector2{ working_area.x, 20 };
	const auto dropdown_size_contents =
		sdk::vector2{ ( int )dropdown_size.x, ( int )dropdown_size.y + 4 + ( int )( elements.size( ) * dropdown_size.y ) };
	const auto this_hash    = fnv1a_32( title );
	const auto accent_light = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );

	render::text( title_pos - sdk::vector2{ 0, 3 }, title, render::FONT_SEGOE_UI_18, { 210, 210, 210, opacity_int } );

	const auto is_active                = ctx->blocking_hash == this_hash;
	const auto is_hovered_dropdown      = input_wrapper::is_hovering( dropdown_pos, dropdown_pos + dropdown_size );
	const auto is_hovered_open_dropdown = input_wrapper::is_hovering( dropdown_pos + sdk::vector2( 0, dropdown_size.y + 5 ),
	                                                          dropdown_pos + dropdown_size + sdk::vector2( 0, 5 ) +
	                                                              sdk::vector2( 0, ( int )dropdown_size.y * ( int )elements.size( ) ) );
	const auto is_hovered_text          = input_wrapper::is_hovering( title_pos, title_pos + title_size );
	const auto is_hovered               = is_hovered_dropdown || is_hovered_text;
	const auto is_hovered_anim          = !ctx->blocking_hash && is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation          = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered_anim ? 1.0f : 0.0f );
	const auto active_animation         = detail::handle_animation( ctx->animations[ "active"_hash + this_hash ], is_active ? 0.85f : 1.0f );
	const auto open_animation           = detail::handle_animation( ctx->animations[ "open"_hash + this_hash ], is_active ? 1.0f : 0.0f );

	if ( !ctx->blocking_hash && is_hovered_dropdown && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash ) {
		const auto mouse_pos = input_wrapper::get_mouse_pos( );

		// Change is_hovered_dropdown to dropdown_size_contents
		if ( input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) && !is_hovered_open_dropdown )
			ctx->blocking_hash = 0;
	}

	render::fill_gradient( dropdown_pos, dropdown_size, { 12 + ( int )( 4 * hover_animation * opacity ), opacity_int },
	                       { 14 + ( int )( 3 * hover_animation * opacity ), opacity_int }, false );

	render::rect( dropdown_pos - 1, dropdown_size + 2, { 20, 20, 20, opacity_int }, 3.0f );

	render::set_clip( dropdown_pos, dropdown_pos + dropdown_size );
	render::text( dropdown_pos + dropdown_size.y / 2 - value_size.y / 2 + sdk::vector2( 5, -1 ), value_str, render::FONT_SEGOE_UI_18,
	              { 210, 210, 210, opacity_int } );
	render::pop_clip( );

	if ( ctx->blocking_hash == this_hash || open_animation > 0.1f ) {
		render::fill_rect_foreground( dropdown_pos + sdk::vector2{ 0, dropdown_size.y + 5 },
		                              sdk::vector2{ dropdown_size.x, ( int )dropdown_size.y * ( int )elements.size( ) },
		                              { 15, 15, 15, int( opacity_int * open_animation ) } );

		render::rect_foreground( dropdown_pos + sdk::vector2{ 0, dropdown_size.y + 5 },
		                         sdk::vector2{ dropdown_size.x, ( int )dropdown_size.y * ( int )elements.size( ) },
		                         { 20, 20, 20, int( opacity_int * open_animation ) } );

		auto option = [ & ]( const std::string& label, sdk::vector2 pos, sdk::vector2 size, bool active ) -> bool {
			const auto this_hash              = fnv1a_32( label );
			const auto hovered                = input_wrapper::is_hovering( pos, pos + size );
			const auto label_active_animation = detail::handle_animation( ctx->animations[ "open_label"_hash + this_hash ], active    ? 1.0f
			                                                                                                                : hovered ? 0.5f
			                                                                                                                          : 0.0f );
			const auto title_color            = sdk::color{ 150 + ( int )( label_active_animation * 60 ), int( opacity_int * open_animation ) };

			render::fill_gradient_foreground( pos, size,
			                                  ctx->accent.adjust_alpha( ( int )( label_active_animation * opacity * 24 * open_animation ) ),
			                                  accent_light.adjust_alpha( 0 ) );
			render::fill_rect_foreground( pos, sdk::vector2( 1, size.y ),
			                              ctx->accent.adjust_alpha( label_active_animation * open_animation * opacity ) );
			render::text_foreground( pos + size.y / 2 - render::measure_text( label, render::FONT_SEGOE_UI_18 ).y / 2 + sdk::vector2( 4, -1 ), label,
			                         render::FONT_SEGOE_UI_18, title_color );

			return hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT );
		};

		for ( size_t i = 0; i < elements.size( ); i++ ) {
			const auto m_pos = dropdown_pos + sdk::vector2{ 0, dropdown_size.y + 5 + ( ( int )dropdown_size.y * ( int )i ) };

			if ( option( elements.at( i ), m_pos, dropdown_size, *value & ( 1 << i ) ) ) {
				if ( *value & ( 1 << i ) ) {
					*value &= ~( 1 << i );
				} else {
					*value |= ( 1 << i );
				}
			}
		}
	}

	ctx->item_height = title_size.y + dropdown_size.y;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + 4 } );
}

void gui::color_picker( std::string_view title, sdk::color* value, bool show_alpha_bar, const sdk::color& default_value )
{
	using namespace utils;

	const auto opacity         = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int     = ( int )( opacity * 255 );
	const auto cursor_pos      = ctx->pos + ctx->cursor_pos.top( );
	const auto working_area    = ctx->get_work_area( );
	const auto preview_size    = sdk::vector2( 18, 10 );
	const auto preview_pos     = sdk::vector2{ cursor_pos.x + working_area.x - preview_size.x, cursor_pos.y };
	const auto this_hash       = fnv1a_32( title );
	const auto this_hash_right = fnv1a_32( title ) + 0x4;
	const auto accent_light    = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );
	const auto open_pos        = sdk::vector2( preview_pos.x + preview_size.x + 5, preview_pos.y );
	const auto open_size       = sdk::vector2( 158 + 5 + 15, 158 + ( show_alpha_bar ? 19 : 0 ) );
	const auto open_size2      = sdk::vector2( 50, 34 );
	const auto single_area     = sdk::vector2( 50, 17 );

	const auto is_active        = ctx->blocking_hash == this_hash;
	const auto is_active_right  = ctx->blocking_hash == this_hash_right;
	const auto is_hovered       = input_wrapper::is_hovering( preview_pos, preview_pos + preview_size );
	const auto is_hovered_open  = input_wrapper::is_hovering( open_pos, open_pos + open_size );
	const auto is_hovered_open2 = input_wrapper::is_hovering( open_pos, open_pos + open_size2 );
	const auto is_hovered_copy  = input_wrapper::is_hovering( open_pos, open_pos + single_area ) && is_active_right;
	const auto is_hovered_paste =
		input_wrapper::is_hovering( open_pos + sdk::vector2( 0, single_area.y ), open_pos + sdk::vector2( 0, single_area.y ) + single_area ) &&
		is_active_right;
	const auto is_hovered_anim  = !ctx->blocking_hash && is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation  = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered_anim ? 1.0f : 0.0f );
	const auto active_animation = detail::handle_animation( ctx->animations[ "active"_hash + this_hash ], is_active ? 0.85f : 1.0f );
	const auto open_animation   = detail::handle_animation( ctx->animations[ "open"_hash + this_hash ], is_active ? 1.0f : 0.0f );
	const auto open_animation2  = detail::handle_animation( ctx->animations[ "open"_hash + this_hash_right ], is_active_right ? 1.0f : 0.0f );
	const auto copy_animation   = detail::handle_animation( ctx->animations[ "copy"_hash + this_hash_right ], is_hovered_copy ? 1.0f : 0.0f );
	const auto paste_animation  = detail::handle_animation( ctx->animations[ "paste"_hash + this_hash_right ], is_hovered_paste ? 1.0f : 0.0f );
	render::fill_rect( preview_pos, preview_size, sdk::color{ value->r, value->g, value->b, opacity_int }, 3.0f );
	render::fill_gradient( preview_pos, preview_size, { 12, 0 }, { 14, int( opacity_int / 1.5 ) }, false );
	render::rect( preview_pos - 1, preview_size + 2, sdk::color{ 20, opacity_int }, 3.0f );

	static float full_r = 0.f, full_g = 0.f, full_b = 0.f;
	bool skip = false;
	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash                          = this_hash;
		ctx->animations[ "block"_hash + this_hash ] = 0;
		skip                                        = true;
	} else if ( ctx->blocking_hash == this_hash ) {
		float h, s, v, r, g, b, a;
		render::color_convert_rgb_to_hsv( value->r / 255.f, value->g / 255.f, value->b / 255.f, h, s, v );

		render::color_convert_hsv_to_rgb( h, 1.f, 1.f, full_r, full_g, full_b );
		r = value->r / 255.f;
		g = value->g / 255.f;
		b = value->b / 255.f;
		a = value->a / 255.f;

		const auto hsv_rect     = sdk::vector2( 150, 150 );
		const auto hue_rect     = sdk::vector2( 15, 150 );
		const auto alpha_rect   = sdk::vector2( 150, 15 );
		const auto hovering_hsv = input_wrapper::is_hovering( open_pos + 4, open_pos + 4 + hsv_rect );
		const auto hovering_hue =
			input_wrapper::is_hovering( sdk::vector2( open_pos.x + 158, open_pos.y + 4 ), sdk::vector2( open_pos.x + 158, open_pos.y + 4 ) + hue_rect );
		const auto hovering_alpha =
			input_wrapper::is_hovering( sdk::vector2( open_pos.x + 4, open_pos.y + 158 ), sdk::vector2( open_pos.x + 4, open_pos.y + 158 ) + alpha_rect ) &&
			show_alpha_bar;

		if ( ctx->animations[ "block"_hash + this_hash ] == 0 && hovering_hsv && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 1;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 0 && hovering_hue && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 2;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 0 && hovering_alpha && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 3;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 1 ) {
			s = ( float )( input_wrapper::get_mouse_pos( ).x - ( open_pos.x + 4 ) ) / hsv_rect.x;
			v = 1.f - ( float )( input_wrapper::get_mouse_pos( ).y - ( open_pos.y + 4 ) ) / hsv_rect.y;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 2 ) {
			h = ( float )( input_wrapper::get_mouse_pos( ).y - ( open_pos.y + 4 ) ) / hsv_rect.y;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 3 ) {
			a = ( float )( input_wrapper::get_mouse_pos( ).x - ( open_pos.x + 4 ) ) / hsv_rect.x;
		}

		if ( !input_wrapper::is_mouse_down( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 0;
		}

		h = std::clamp( h, 0.001f, 0.999f );
		s = std::clamp( s, 0.001f, 0.999f );
		v = std::clamp( v, 0.001f, 0.999f );
		a = std::clamp( a, 0.001f, 0.999f );

		render::color_convert_hsv_to_rgb( h, s, v, r, g, b );

		if ( ctx->animations[ "block"_hash + this_hash ] != 0 ) {
			value->r = r * 255;
			value->g = g * 255;
			value->b = b * 255;
			value->a = a * 255;
		}

		if ( input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) && !is_hovered_open && !is_hovered )
			ctx->blocking_hash = 0;
	}

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_RIGHT ) ) {
		ctx->blocking_hash = this_hash_right;
	} else if ( ctx->blocking_hash == this_hash_right ) {
		if ( is_hovered_copy && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->copied_color_r = value->r;
			ctx->copied_color_g = value->g;
			ctx->copied_color_b = value->b;
			ctx->copied_color_a = value->a;
			ctx->blocking_hash = 0;
		}

		if ( is_hovered_paste && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			value->r         = ctx->copied_color_r;
			value->g         = ctx->copied_color_g;
			value->b         = ctx->copied_color_b;
			value->a         = ctx->copied_color_a;
			ctx->blocking_hash = 0;
		}

		if ( !is_hovered_open2 && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) )
			ctx->blocking_hash = 0;
	}

	if ( is_active || open_animation > 0.1f ) {
		float h, s, v;
		render::color_convert_rgb_to_hsv( value->r / 255.f, value->g / 255.f, value->b / 255.f, h, s, v );
		const sdk::color col_hues[ 6 + 1 ] = {
			sdk::color( 255, 0, 0, int( opacity_int * open_animation ) ), sdk::color( 255, 255, 0, int( opacity_int * open_animation ) ),
			sdk::color( 0, 255, 0, int( opacity_int * open_animation ) ), sdk::color( 0, 255, 255, int( opacity_int * open_animation ) ),
			sdk::color( 0, 0, 255, int( opacity_int * open_animation ) ), sdk::color( 255, 0, 255, int( opacity_int * open_animation ) ),
			sdk::color( 255, 0, 0, int( opacity_int * open_animation ) )
		};

		render::fill_rect_foreground( open_pos, open_size, { 12, int( opacity_int * open_animation ) }, 3.f );
		render::rect_foreground( open_pos, open_size, { 20, int( opacity_int * open_animation ) }, 3.f );

		// HSV
		render::fill_gradient_foreground( open_pos + 4, 150, { 255, int( opacity_int * open_animation ) },
		                                  { int( full_r * 255 ), int( full_g * 255 ), int( full_b * 255 ), int( opacity_int * open_animation ) },
		                                  true );
		render::fill_gradient_foreground( open_pos + 4, 150, { 0, 0 }, { 0, int( opacity_int * open_animation ) }, false );

		// Hue
		for ( int i = 0; i < 6; i++ ) {
			render::fill_gradient_foreground( open_pos + sdk::vector2( 158, 4 + ( 150 / 6 * i ) ), sdk::vector2( 15, 150 / 6 ), col_hues[ i ],
			                                  col_hues[ i + 1 ], false );
		}

		if ( show_alpha_bar ) {
			render::fill_gradient_foreground( sdk::vector2( open_pos.x + 4, open_pos.y + 158 ), sdk::vector2( 150, 15 ),
			                                  { 0, int( opacity_int * open_animation ) },
			                                  { value->r, value->g, value->b, int( opacity_int * open_animation ) } );
		}

		auto hue_r   = sdk::vector2( open_pos.x + 158, std::clamp( ( int )( open_pos.y + 4 + ( h * 150 ) ), open_pos.y + 4, open_pos.y + 4 + 145 ) );
		auto alpha_r = sdk::vector2( std::clamp( ( int )( open_pos.x + 4 + ( ( value->a / 255.f ) * 150 ) ), open_pos.x + 4, open_pos.x + 4 + 145 ),
		                             open_pos.y + 158 );

		render::fill_rect_foreground( hue_r, sdk::vector2( 15, 5 ), { value->r, value->g, value->b, int( opacity_int * open_animation ) } );
		render::rect_foreground( hue_r, sdk::vector2( 15, 5 ), { 20, int( opacity_int * open_animation ) } );

		render::rect_foreground( open_pos + 4, 150, { 20, int( opacity_int * open_animation ) } );
		render::rect_foreground( open_pos + sdk::vector2( 158, 4 ), sdk::vector2( 15, 150 ), { 20, int( opacity_int * open_animation ) } );

		render::fill_circle_foreground( sdk::vector2( open_pos + 4 + sdk::vector2( s * 150, ( 1.f - v ) * 150 ) ), 5.f,
		                                { 42, int( opacity_int * open_animation ) } );
		render::fill_circle_foreground( sdk::vector2( open_pos + 4 + sdk::vector2( s * 150, ( 1.f - v ) * 150 ) ), 4.f,
		                                { 22, int( opacity_int * open_animation ) } );
		render::fill_circle_foreground( sdk::vector2( open_pos + 4 + sdk::vector2( s * 150, ( 1.f - v ) * 150 ) ), 3.f,
		                                { value->r, value->g, value->b, int( opacity_int * open_animation ) } );

		if ( show_alpha_bar ) {
			render::fill_rect_foreground( alpha_r, sdk::vector2( 5, 15 ), { value->r, value->g, value->b, int( opacity_int * open_animation ) } );
			render::rect_foreground( alpha_r, sdk::vector2( 5, 15 ), { 20, int( opacity_int * open_animation ) } );

			render::rect_foreground( sdk::vector2( open_pos.x + 4, open_pos.y + 158 ), sdk::vector2( 150, 15 ),
			                         { 20, int( opacity_int * open_animation ) } );
		}
	}

	if ( is_active_right || open_animation2 > 0.1f ) {
		render::fill_rect_foreground( open_pos, open_size2, { 12, int( opacity_int * open_animation2 ) }, 3.0f );
		render::text_foreground( open_pos + sdk::vector2( open_size2.x, open_size2.y / 2 ) / 2 -
		                             render::measure_text( "copy", render::FONT_SEGOE_UI_16 ) / 2,
		                         "copy", render::FONT_SEGOE_UI_16, { 200 + int( 55 * copy_animation ), int( opacity_int * open_animation2 ) } );
		render::text_foreground( open_pos + sdk::vector2( open_size2.x, open_size2.y / 2 ) / 2 -
		                             render::measure_text( "paste", render::FONT_SEGOE_UI_16 ) / 2 + sdk::vector2( 0, open_size2.y / 2 ) -
		                             sdk::vector2( 0, 1 ),
		                         "paste", render::FONT_SEGOE_UI_16, { 200 + int( 55 * paste_animation ), int( opacity_int * open_animation2 ) } );
		render::rect_foreground( open_pos, open_size2, { 20, int( opacity_int * open_animation2 ) }, 3.0f );
		render::fill_rect_foreground( sdk::vector2( open_pos.x, open_pos.y + open_size2.y / 2 ), sdk::vector2( open_size2.x, 1 ),
		                              { 20, int( opacity_int * open_animation2 ) } );
	}
}

void gui::color_picker( std::string_view title, c_color* value, bool show_alpha_bar, const c_color& default_value )
{
	using namespace utils;

	const auto opacity         = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int     = ( int )( opacity * 255 );
	const auto cursor_pos      = ctx->pos + ctx->cursor_pos.top( );
	const auto working_area    = ctx->get_work_area( );
	const auto preview_size    = sdk::vector2( 18, 10 );
	const auto preview_pos     = sdk::vector2{ cursor_pos.x + working_area.x - preview_size.x, cursor_pos.y };
	const auto this_hash       = fnv1a_32( title );
	const auto this_hash_right = fnv1a_32( title ) + 0x4;
	const auto accent_light    = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );
	const auto open_pos        = sdk::vector2( preview_pos.x + preview_size.x + 5, preview_pos.y );
	const auto open_size       = sdk::vector2( 158 + 5 + 15, 158 + ( show_alpha_bar ? 19 : 0 ) );
	const auto open_size2      = sdk::vector2( 50, 34 );
	const auto single_area     = sdk::vector2( 50, 17 );

	const auto is_active        = ctx->blocking_hash == this_hash;
	const auto is_active_right  = ctx->blocking_hash == this_hash_right;
	const auto is_hovered       = input_wrapper::is_hovering( preview_pos, preview_pos + preview_size );
	const auto is_hovered_open  = input_wrapper::is_hovering( open_pos, open_pos + open_size );
	const auto is_hovered_open2 = input_wrapper::is_hovering( open_pos, open_pos + open_size2 );
	const auto is_hovered_copy  = input_wrapper::is_hovering( open_pos, open_pos + single_area ) && is_active_right;
	const auto is_hovered_paste =
		input_wrapper::is_hovering( open_pos + sdk::vector2( 0, single_area.y ), open_pos + sdk::vector2( 0, single_area.y ) + single_area ) &&
		is_active_right;
	const auto is_hovered_anim  = !ctx->blocking_hash && is_hovered || ctx->blocking_hash == this_hash;
	const auto hover_animation  = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered_anim ? 1.0f : 0.0f );
	const auto active_animation = detail::handle_animation( ctx->animations[ "active"_hash + this_hash ], is_active ? 0.85f : 1.0f );
	const auto open_animation   = detail::handle_animation( ctx->animations[ "open"_hash + this_hash ], is_active ? 1.0f : 0.0f );
	const auto open_animation2  = detail::handle_animation( ctx->animations[ "open"_hash + this_hash_right ], is_active_right ? 1.0f : 0.0f );
	const auto copy_animation   = detail::handle_animation( ctx->animations[ "copy"_hash + this_hash_right ], is_hovered_copy ? 1.0f : 0.0f );
	const auto paste_animation  = detail::handle_animation( ctx->animations[ "paste"_hash + this_hash_right ], is_hovered_paste ? 1.0f : 0.0f );
	render::fill_rect( preview_pos, preview_size, sdk::color{ value->m_r, value->m_g, value->m_b, opacity_int }, 3.0f );
	render::fill_gradient( preview_pos, preview_size, { 14, 0 }, { 16, int( opacity_int / 1.5 ) }, false );
	render::rect( preview_pos - 1, preview_size + 2, sdk::color{ 20, opacity_int }, 3.0f );

	static float full_r = 0.f, full_g = 0.f, full_b = 0.f;
	bool skip = false;
	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash                          = this_hash;
		ctx->animations[ "block"_hash + this_hash ] = 0;
		skip                                        = true;
	} else if ( ctx->blocking_hash == this_hash ) {
		float h, s, v, r, g, b, a;
		render::color_convert_rgb_to_hsv( value->m_r / 255.f, value->m_g / 255.f, value->m_b / 255.f, h, s, v );

		render::color_convert_hsv_to_rgb( h, 1.f, 1.f, full_r, full_g, full_b );
		r = value->m_r / 255.f;
		g = value->m_g / 255.f;
		b = value->m_b / 255.f;
		a = value->m_a / 255.f;

		const auto hsv_rect     = sdk::vector2( 150, 150 );
		const auto hue_rect     = sdk::vector2( 15, 150 );
		const auto alpha_rect   = sdk::vector2( 150, 15 );
		const auto hovering_hsv = input_wrapper::is_hovering( open_pos + 4, open_pos + 4 + hsv_rect );
		const auto hovering_hue =
			input_wrapper::is_hovering( sdk::vector2( open_pos.x + 158, open_pos.y + 4 ), sdk::vector2( open_pos.x + 158, open_pos.y + 4 ) + hue_rect );
		const auto hovering_alpha =
			input_wrapper::is_hovering( sdk::vector2( open_pos.x + 4, open_pos.y + 158 ), sdk::vector2( open_pos.x + 4, open_pos.y + 158 ) + alpha_rect ) &&
			show_alpha_bar;

		if ( ctx->animations[ "block"_hash + this_hash ] == 0 && hovering_hsv && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 1;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 0 && hovering_hue && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 2;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 0 && hovering_alpha && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 3;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 1 ) {
			s = ( float )( input_wrapper::get_mouse_pos( ).x - ( open_pos.x + 4 ) ) / hsv_rect.x;
			v = 1.f - ( float )( input_wrapper::get_mouse_pos( ).y - ( open_pos.y + 4 ) ) / hsv_rect.y;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 2 ) {
			h = ( float )( input_wrapper::get_mouse_pos( ).y - ( open_pos.y + 4 ) ) / hsv_rect.y;
		}

		if ( ctx->animations[ "block"_hash + this_hash ] == 3 ) {
			a = ( float )( input_wrapper::get_mouse_pos( ).x - ( open_pos.x + 4 ) ) / hsv_rect.x;
		}

		if ( !input_wrapper::is_mouse_down( input_wrapper::MOUSE_LEFT ) ) {
			ctx->animations[ "block"_hash + this_hash ] = 0;
		}

		h = std::clamp( h, 0.001f, 0.999f );
		s = std::clamp( s, 0.001f, 0.999f );
		v = std::clamp( v, 0.001f, 0.999f );
		a = std::clamp( a, 0.001f, 0.999f );

		render::color_convert_hsv_to_rgb( h, s, v, r, g, b );

		if ( ctx->animations[ "block"_hash + this_hash ] != 0 ) {
			value->m_r = r * 255;
			value->m_g = g * 255;
			value->m_b = b * 255;
			value->m_a = a * 255;
		}

		if ( input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) && !is_hovered_open && !is_hovered )
			ctx->blocking_hash = 0;
	}

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_RIGHT ) ) {
		ctx->blocking_hash = this_hash_right;
	} else if ( ctx->blocking_hash == this_hash_right ) {
		if ( is_hovered_copy && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->copied_color_r = value->m_r;
			ctx->copied_color_g = value->m_g;
			ctx->copied_color_b = value->m_b;
			ctx->copied_color_a = value->m_a;
			ctx->blocking_hash = 0;
		}

		if ( is_hovered_paste && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			value->m_r         = ctx->copied_color_r;
			value->m_g         = ctx->copied_color_g;
			value->m_b         = ctx->copied_color_b;
			value->m_a         = ctx->copied_color_a;
			ctx->blocking_hash = 0;
		}

		if ( !is_hovered_open2 && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) )
			ctx->blocking_hash = 0;
	}

	if ( is_active || open_animation > 0.1f ) {
		float h, s, v;
		render::color_convert_rgb_to_hsv( value->m_r / 255.f, value->m_g / 255.f, value->m_b / 255.f, h, s, v );
		const sdk::color col_hues[ 6 + 1 ] = {
			sdk::color( 255, 0, 0, int( opacity_int * open_animation ) ), sdk::color( 255, 255, 0, int( opacity_int * open_animation ) ),
			sdk::color( 0, 255, 0, int( opacity_int * open_animation ) ), sdk::color( 0, 255, 255, int( opacity_int * open_animation ) ),
			sdk::color( 0, 0, 255, int( opacity_int * open_animation ) ), sdk::color( 255, 0, 255, int( opacity_int * open_animation ) ),
			sdk::color( 255, 0, 0, int( opacity_int * open_animation ) )
		};

		render::fill_rect_foreground( open_pos, open_size, { 12, int( opacity_int * open_animation ) }, 3.f );
		render::rect_foreground( open_pos, open_size, { 20, int( opacity_int * open_animation ) }, 3.f );

		// HSV
		render::fill_gradient_foreground( open_pos + 4, 150, { 255, int( opacity_int * open_animation ) },
		                                  { int( full_r * 255 ), int( full_g * 255 ), int( full_b * 255 ), int( opacity_int * open_animation ) },
		                                  true );
		render::fill_gradient_foreground( open_pos + 4, 150, { 0, 0 }, { 0, int( opacity_int * open_animation ) }, false );

		// Hue
		for ( int i = 0; i < 6; i++ ) {
			render::fill_gradient_foreground( open_pos + sdk::vector2( 158, 4 + ( 150 / 6 * i ) ), sdk::vector2( 15, 150 / 6 ), col_hues[ i ],
			                                  col_hues[ i + 1 ], false );
		}

		if ( show_alpha_bar ) {
			render::fill_gradient_foreground( sdk::vector2( open_pos.x + 4, open_pos.y + 158 ), sdk::vector2( 150, 15 ),
			                                  { 0, int( opacity_int * open_animation ) },
			                                  { value->m_r, value->m_g, value->m_b, int( opacity_int * open_animation ) } );
		}

		auto hue_r   = sdk::vector2( open_pos.x + 158, std::clamp( ( int )( open_pos.y + 4 + ( h * 150 ) ), open_pos.y + 4, open_pos.y + 4 + 145 ) );
		auto alpha_r = sdk::vector2( std::clamp( ( int )( open_pos.x + 4 + ( ( value->m_a / 255.f ) * 150 ) ), open_pos.x + 4, open_pos.x + 4 + 145 ),
		                             open_pos.y + 158 );

		render::fill_rect_foreground( hue_r, sdk::vector2( 15, 5 ), { value->m_r, value->m_g, value->m_b, int( opacity_int * open_animation ) } );
		render::rect_foreground( hue_r, sdk::vector2( 15, 5 ), { 20, int( opacity_int * open_animation ) } );

		render::rect_foreground( open_pos + 4, 150, { 20, int( opacity_int * open_animation ) } );
		render::rect_foreground( open_pos + sdk::vector2( 158, 4 ), sdk::vector2( 15, 150 ), { 20, int( opacity_int * open_animation ) } );

		render::fill_circle_foreground( sdk::vector2( open_pos + 4 + sdk::vector2( s * 150, ( 1.f - v ) * 150 ) ), 5.f,
		                                { 42, int( opacity_int * open_animation ) } );
		render::fill_circle_foreground( sdk::vector2( open_pos + 4 + sdk::vector2( s * 150, ( 1.f - v ) * 150 ) ), 4.f,
		                                { 22, int( opacity_int * open_animation ) } );
		render::fill_circle_foreground( sdk::vector2( open_pos + 4 + sdk::vector2( s * 150, ( 1.f - v ) * 150 ) ), 3.f,
		                                { value->m_r, value->m_g, value->m_b, int( opacity_int * open_animation ) } );

		if ( show_alpha_bar ) {
			render::fill_rect_foreground( alpha_r, sdk::vector2( 5, 15 ),
			                              { value->m_r, value->m_g, value->m_b, int( opacity_int * open_animation ) } );
			render::rect_foreground( alpha_r, sdk::vector2( 5, 15 ), { 20, int( opacity_int * open_animation ) } );

			render::rect_foreground( sdk::vector2( open_pos.x + 4, open_pos.y + 158 ), sdk::vector2( 150, 15 ),
			                         { 20, int( opacity_int * open_animation ) } );
		}
	}

	if ( is_active_right || open_animation2 > 0.1f ) {
		render::fill_rect_foreground( open_pos, open_size2, { 12, int( opacity_int * open_animation2 ) }, 3.0f );
		render::text_foreground( open_pos + sdk::vector2( open_size2.x, open_size2.y / 2 ) / 2 -
		                             render::measure_text( "copy", render::FONT_SEGOE_UI_16 ) / 2,
		                         "copy", render::FONT_SEGOE_UI_16, { 200 + int( 55 * copy_animation ), int( opacity_int * open_animation2 ) } );
		render::text_foreground( open_pos + sdk::vector2( open_size2.x, open_size2.y / 2 ) / 2 -
		                             render::measure_text( "paste", render::FONT_SEGOE_UI_16 ) / 2 + sdk::vector2( 0, open_size2.y / 2 ) -
		                             sdk::vector2( 0, 1 ),
		                         "paste", render::FONT_SEGOE_UI_16, { 200 + int( 55 * paste_animation ), int( opacity_int * open_animation2 ) } );
		render::rect_foreground( open_pos, open_size2, { 20, int( opacity_int * open_animation2 ) }, 3.0f );
		render::fill_rect_foreground( sdk::vector2( open_pos.x, open_pos.y + open_size2.y / 2 ), sdk::vector2( open_size2.x, 1 ),
		                              { 20, int( opacity_int * open_animation2 ) } );
	}
}

auto get_key_name = [ & ]( const int vk_key, char* buffer ) {
	if ( vk_key == 0 ) {
		strcpy( buffer, "..." );
		return;
	}

	switch ( vk_key ) {
	case VK_XBUTTON1:
		strcpy( buffer, ( "mouse four" ) );
		return;
	case VK_XBUTTON2:
		strcpy( buffer, ( "mouse five" ) );
		return;
	case VK_LBUTTON:
		strcpy( buffer, ( "mouse one" ) );
		return;
	case VK_MBUTTON:
		strcpy( buffer, ( "mouse three" ) );
		return;
	case VK_RBUTTON:
		strcpy( buffer, ( "mouse two" ) );
		return;
	}

	auto scan_code = MapVirtualKeyA( vk_key, MAPVK_VK_TO_VSC );

	switch ( vk_key ) {
	case VK_LEFT:
	case VK_UP:
	case VK_RIGHT:
	case VK_DOWN:
	case VK_RCONTROL:
	case VK_RMENU:
	case VK_LWIN:
	case VK_RWIN:
	case VK_APPS:
	case VK_PRIOR:
	case VK_NEXT:
	case VK_END:
	case VK_HOME:
	case VK_INSERT:
	case VK_DELETE:
	case VK_DIVIDE:
	case VK_NUMLOCK:
		scan_code |= KF_EXTENDED;
	}

	if ( auto result = GetKeyNameTextA( scan_code << 16, buffer, 128 ); result == 0 ) {
		strcpy( buffer, "..." );
		return;
	}

	for ( size_t i = 0; i < strlen( buffer ); i++ )
		buffer[ i ] = tolower( buffer[ i ] );
};

void gui::key_bind( std::string_view title, key_bind_t* value, bool allow_keyboard, bool allow_mouse, int default_value )
{
	using namespace utils;

	const auto this_hash     = fnv1a_32( title );
	const auto this_hash_key = fnv1a_32( title ) + 0x4;
	const auto opacity       = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int   = ( int )( opacity * 255 );
	const auto is_active     = ctx->blocking_hash == this_hash;
	char buf[ 128 ]          = { 0 };
	get_key_name( value->key, buf );
	const auto k               = std::vformat( "{}", std::make_format_args( value->key <= 0 ? "none" : buf) );
	const auto render_str      = is_active ? "..." : k;
	const auto render_str_size = render::measure_text( render_str, render::FONT_SEGOE_UI_16 );
	const auto cursor_pos      = ctx->pos + ctx->cursor_pos.top( ) - 1;
	const auto working_area    = ctx->get_work_area( );
	const auto preview_size    = sdk::vector2( render_str_size.x + 8, 12 );
	const auto preview_pos     = sdk::vector2{ cursor_pos.x + working_area.x - preview_size.x + 2, cursor_pos.y };
	const auto accent_light    = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );
	const auto hover_size      = sdk::vector2( 50, 50 );
	const auto hover_pos       = sdk::vector2( preview_pos.x + preview_size.x + 4, preview_pos.y );
	const auto hover_active    = ctx->blocking_hash == this_hash_key;
	const auto single_size     = sdk::vector2( 50, 50 / 3 );

	const auto is_hovered        = input_wrapper::is_hovering( preview_pos, preview_pos + preview_size );
	const auto is_hovered_open   = input_wrapper::is_hovering( hover_pos, hover_pos + hover_size );
	const auto is_hovered_always = input_wrapper::is_hovering( hover_pos, hover_pos + single_size );
	const auto is_hovered_held =
		input_wrapper::is_hovering( hover_pos + sdk::vector2( 0, single_size.y ), hover_pos + sdk::vector2( 0, single_size.y ) + single_size );
	const auto is_hovered_toggle =
		input_wrapper::is_hovering( hover_pos + sdk::vector2( 0, single_size.y * 2 ), hover_pos + sdk::vector2( 0, single_size.y * 2 ) + single_size );
	const auto hover_animation = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered ? 1.0f : 0.0f );
	const auto open_animation  = detail::handle_animation( ctx->animations[ "open"_hash + this_hash ], hover_active ? 1.0f : 0.0f );

	const auto always_animation = detail::handle_animation( ctx->animations[ "hover_always"_hash + this_hash ], is_hovered_always ? 1.0f : 0.0f );
	const auto held_animation   = detail::handle_animation( ctx->animations[ "hover_held"_hash + this_hash ], is_hovered_held ? 1.0f : 0.0f );
	const auto toggle_animation = detail::handle_animation( ctx->animations[ "hover_toggle"_hash + this_hash ], is_hovered_toggle ? 1.0f : 0.0f );

	const auto active_always_animation =
		detail::handle_animation( ctx->animations[ "active_always"_hash + this_hash ], ( value->mode == e_always_on ) ? 1.0f : 0.0f );
	const auto active_held_animation =
		detail::handle_animation( ctx->animations[ "active_held"_hash + this_hash ], ( value->mode == e_held ) ? 1.0f : 0.0f );
	const auto active_toggle_animation =
		detail::handle_animation( ctx->animations[ "active_toggle"_hash + this_hash ], ( value->mode == e_toggle ) ? 1.0f : 0.0f );

	render::fill_rect( preview_pos, preview_size, { 12, opacity_int }, 3.0f );
	render::rect( preview_pos, preview_size, { 20 + int( 8 * hover_animation ), opacity_int }, 3.0f );

	render::text( preview_pos + preview_size / 2 - render::measure_text( render_str, render::FONT_SEGOE_UI_16 ) / 2 - sdk::vector2( 0, 2 ),
	              render_str, render::FONT_SEGOE_UI_16, { 200 + int( 55 * hover_animation ), opacity_int } );

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash ) {
		for ( int i = 1; i < 0xff; i++ ) {
			if ( i == VK_CANCEL )
				continue;

			if ( input_wrapper::is_key_pressed( i ) ) {
				value->key = i;
				if ( i == VK_ESCAPE )
					value->key = -1;

				ctx->blocking_hash = 0;
			}
		}

		if ( !is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->blocking_hash = 0;
		}
	}

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_RIGHT ) ) {
		ctx->blocking_hash = this_hash_key;
	} else if ( ctx->blocking_hash == this_hash_key ) {
		if ( is_hovered_always && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			value->mode        = e_always_on;
			ctx->blocking_hash = 0;
		}

		if ( is_hovered_held && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			value->mode        = e_held;
			ctx->blocking_hash = 0;
		}

		if ( is_hovered_toggle && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			value->mode        = e_toggle;
			ctx->blocking_hash = 0;
		}

		if ( !is_hovered_open && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) )
			ctx->blocking_hash = 0;
	}

	if ( hover_active || open_animation > 0.1f ) {
		render::fill_rect_foreground( hover_pos, hover_size, { 12, int( opacity_int * open_animation ) }, 3.0f );
		render::rect_foreground( hover_pos, hover_size, { 20, int( opacity_int * open_animation ) }, 3.0f );
		render::fill_rect_foreground( hover_pos + sdk::vector2( 0, hover_size.y / 3 ), sdk::vector2( hover_size.x, 1 ),
		                              { 20, int( opacity_int * open_animation ) } );
		render::fill_rect_foreground( hover_pos + sdk::vector2( 0, ( hover_size.y / 3 ) * 2 ), sdk::vector2( hover_size.x, 1 ),
		                              { 20, int( opacity_int * open_animation ) } );
		render::text_foreground( hover_pos + single_size / 2 - render::measure_text( "always", render::FONT_SEGOE_UI_16 ) / 2, "always",
		                         render::FONT_SEGOE_UI_16,
		                         { 200 + int( 55 * always_animation ), int( opacity_int * open_animation * ( 1.f - active_always_animation ) ) } );
		render::text_foreground(
			hover_pos + single_size / 2 - render::measure_text( "hold", render::FONT_SEGOE_UI_16 ) / 2 + sdk::vector2( 0, single_size.y ), "hold",
			render::FONT_SEGOE_UI_16, { 200 + int( 55 * held_animation ), int( opacity_int * open_animation * ( 1.f - active_held_animation ) ) } );
		render::text_foreground( hover_pos + single_size / 2 - render::measure_text( "toggle", render::FONT_SEGOE_UI_16 ) / 2 +
		                             sdk::vector2( 0, single_size.y * 2 ),
		                         "toggle", render::FONT_SEGOE_UI_16,
		                         { 200 + int( 55 * toggle_animation ), int( opacity_int * open_animation * ( 1.f - active_toggle_animation ) ) } );

		render::text_foreground( hover_pos + single_size / 2 - render::measure_text( "always", render::FONT_SEGOE_UI_16 ) / 2, "always",
		                         render::FONT_SEGOE_UI_16, ctx->accent.adjust_alpha( opacity * open_animation * active_always_animation ) );
		render::text_foreground( hover_pos + single_size / 2 - render::measure_text( "hold", render::FONT_SEGOE_UI_16 ) / 2 +
		                             sdk::vector2( 0, single_size.y ),
		                         "hold", render::FONT_SEGOE_UI_16, ctx->accent.adjust_alpha( opacity * open_animation * active_held_animation ) );
		render::text_foreground( hover_pos + single_size / 2 - render::measure_text( "toggle", render::FONT_SEGOE_UI_16 ) / 2 +
		                             sdk::vector2( 0, single_size.y * 2 ),
		                         "toggle", render::FONT_SEGOE_UI_16, ctx->accent.adjust_alpha( opacity * open_animation * active_toggle_animation ) );
	}
}

static std::string s_to_lower( std::string s )
{
	std::transform( s.begin( ), s.end( ), s.begin( ), []( unsigned char c ) { return tolower( c ); } );
	return s;
}

void gui::text_input( std::string_view title, std::string* value, bool is_protected )
{
	using namespace utils;

	const auto this_hash    = fnv1a_32( title );
	const auto opacity      = ctx->animations[ "__opacity"_hash ];
	const auto opacity_int  = ( int )( opacity * 255 );
	const auto is_active    = ctx->blocking_hash == this_hash;
	const auto cursor_pos   = ctx->pop_cursor( );
	const auto working_area = ctx->get_work_area( );

	const auto title_size   = render::measure_text( title, render::FONT_SEGOE_UI_18 );
	const auto value_size   = render::measure_text( *value, render::FONT_SEGOE_UI_18 );
	const auto title_pos    = ctx->pos + cursor_pos;
	const auto input_pos    = sdk::vector2{ title_pos.x, title_pos.y + title_size.y + 3 };
	const auto input_size   = sdk::vector2{ working_area.x, 20 };
	const auto accent_light = ctx->accent_light.to_hsla( ).adjust_lum( 0.15 ).to_rgba( );

	const auto is_hovered      = input_wrapper::is_hovering( input_pos, input_pos + input_size );
	const auto hover_animation = detail::handle_animation( ctx->animations[ "hover"_hash + this_hash ], is_hovered || is_active ? 1.0f : 0.0f );

	if ( !ctx->blocking_hash && is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
		ctx->blocking_hash = this_hash;
	} else if ( ctx->blocking_hash == this_hash ) {
		for ( size_t i = 0; i < 255; i++ ) {
			if ( input_wrapper::is_key_pressed( i ) ) {
				if ( i == VK_ESCAPE || i == VK_RETURN || i == VK_INSERT || i == VK_DELETE || i == VK_CONTROL ) {
					ctx->blocking_hash = 0;
					continue;
				}

				if ( i == VK_SPACE ) {
					*value = value->append( " " );
					continue;
				}

				if ( i == VK_SHIFT ) {
					continue;
				}

				if ( i == VK_BACK ) {
					*value = value->substr( 0, strlen( value->data( ) ) - 1 );
					continue;
				}

				 std::string nigger = {};
				 nigger.push_back( ( char )( ( char )i ) );
				 *value =  s_to_lower( value->append( nigger ) );
			}
		}

		if ( !is_hovered && input_wrapper::is_mouse_clicked( input_wrapper::MOUSE_LEFT ) ) {
			ctx->blocking_hash = 0;
		}
	}

	render::text( title_pos, title, render::FONT_SEGOE_UI_18, { 210, 210, 210, opacity_int } );

	render::fill_gradient( input_pos, input_size, { 12 + ( int )( 4 * hover_animation * opacity ), opacity_int },
	                       { 14 + ( int )( 3 * hover_animation * opacity ), opacity_int }, false );

	render::rect( input_pos - 1, input_size + 2, { 20, 20, 20, opacity_int }, 3.0f );

	render::text( input_pos + input_size.y / 2 - value_size.y / 2 + sdk::vector2( 5, -2 ), *value, render::FONT_SEGOE_UI_18, { 200, opacity_int } );

	ctx->item_height = title_size.y + input_size.y + 4;
	ctx->push_cursor( { cursor_pos.x, cursor_pos.y + ctx->item_height + 5 } );
}
