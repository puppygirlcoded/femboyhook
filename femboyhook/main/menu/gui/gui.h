#pragma once

#include <algorithm>
#include <cstdint>
#include <functional>
#include <string_view>

#include "../../includes.h"
#include "../../utilities/input/input.h"
#include "render_wrapper.h"
#include "input_wrapper.h"


namespace gui
{

	namespace detail
	{

		template< typename T >
		inline T scale_value( T value, T in_min, T in_max, T out_min, T out_max )
		{
			return ( T )( ( value - in_min ) * ( out_max - out_min ) / ( float )( in_max - in_min ) + out_min );
		}

		inline sdk::color blend_color( const sdk::color& start, const sdk::color& end, float rate )
		{
			const auto diff_r = end.r - start.r;
			const auto diff_g = end.g - start.g;
			const auto diff_b = end.b - start.b;
			const auto diff_a = end.a - start.a;

			return { ( int )std::clamp( start.r + diff_r * rate, 0.0f, 255.0f ), ( int )std::clamp( start.g + diff_g * rate, 0.0f, 255.0f ),
				     ( int )std::clamp( start.b + diff_b * rate, 0.0f, 255.0f ), ( int )std::clamp( start.a + diff_a * rate, 0.0f, 255.0f ) };
		}

		inline float handle_animation( const float&& value, float target, float rate = 0.065f, float rewind_rate = 0.09f )
		{
			const auto interval = ( 1.0f / ( target < value ? rewind_rate : rate ) ) * utils::input_wrapper::get_last_frame_time( );
			const auto delta    = target - value;

			return std::clamp( value + delta * interval, 0.0f, 1.0f );
		}

		inline float handle_animation( float& value, float target, float rate = 0.065f, float rewind_rate = 0.09f )
		{
			value = handle_animation( ( float )value, target, rate, rewind_rate );

			return value;
		}

	} // namespace detail

	float get_opacity( bool is_open );

	bool begin_window( std::string_view title, const sdk::vector2& initial_pos, const sdk::vector2& initial_size );
	bool begin_tab( std::string_view title );
	bool begin_category( std::string_view title );
	bool begin_group( std::string_view title );
	void push_fake_group( );

	void finish_window( );
	void finish_tab( );
	void finish_category( );
	void finish_group( );

	void label( std::string_view title, int additional_spacing = 0, const sdk::color& color = { 210, 210, 210 } );
	void button( std::string_view title, const std::function< void( ) >& callback );
	void checkbox( std::string_view title, bool* value, bool default_value = false );
	void slider( std::string_view title, float* value, float min_v, float max_v, float default_value = 0.0f, std::string_view format = "{:0.1f}" );
	void slider( std::string_view title, int* value, int min_v, int max_v, int default_value = 0, std::string_view format = "{}" );
	void dropdown( std::string_view title, int* value, const std::vector< std::string >& elements, int default_value = 0 );
	void multi_dropdown( std::string_view title, int* value, const std::vector< std::string >& elements );
	void color_picker( std::string_view title, sdk::color* value, bool show_alpha_bar, const sdk::color& default_value = { 255, 255, 255, 255 } );
	void color_picker( std::string_view title, c_color* value, bool show_alpha_bar, const c_color& default_value );
	void key_bind( std::string_view title, key_bind_t* value, bool allow_keyboard, bool allow_mouse, int default_value = 0 );
	void text_input( std::string_view title, std::string* value, bool is_protected = false );
} // namespace gui
