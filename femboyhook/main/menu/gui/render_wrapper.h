#pragma once

#include <imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <algorithm>
#include <cmath>
#include <math.h>
#undef min
#undef max
namespace sdk
{

	namespace detail
	{

		inline double calc_color( double c, double t1, double t2 )
		{
			if ( c < 0 )
				c += 1.0;
			else if ( c > 1 )
				c -= 1.0;

			if ( 6.0 * c < 1.0 )
				return t1 + ( t2 - t1 ) * 6.0 * c;
			else if ( 2.0 * c < 1.0 )
				return t2;
			else if ( 3.0 * c < 2.0 )
				return t1 + ( t2 - t1 ) * ( 2.0 / 3.0 - c ) * 6.0;

			return t1;
		}

		inline void to_hsla( int r, int g, int b, int a, double& h, double& s, double& l, double& ad )
		{
			const auto float_r = r / 255.0;
			const auto float_g = g / 255.0;
			const auto float_b = b / 255.0;
			const auto float_a = a / 255.0;

			const auto min1  = std::min( float_r, float_g );
			const auto min   = std::min( min1, float_b );
			const auto max2  = std::max( float_r, float_g );
			const auto max   = std::max( max2, float_b );
			const auto delta = max - min;

			l = ( max + min ) / 2.0;

			if ( delta != 0 ) {
				if ( l < 0.5 ) {
					s = delta / ( max + min );
				} else {
					s = delta / ( 2.0 - max - min );
				}

				if ( float_r == max ) {
					h = ( float_g - float_b ) / delta;
				} else if ( float_g == max ) {
					h = 2.0 + ( float_b - float_r ) / delta;
				} else if ( float_b == max ) {
					h = 4.0 + ( float_r - float_g ) / delta;
				}
			}

			ad = float_a;
		}

		inline void to_rgba( double h, double s, double l, double a, int& r, int& g, int& b, int& ia )
		{
			if ( s == 0 ) {
				r = ( int )std::round( l * 255.0 );
				g = ( int )std::round( l * 255.0 );
				b = ( int )std::round( l * 255.0 );
			} else {
				const auto th = h / 6.0;

				auto t1 = 0.0;
				auto t2 = 0.0;

				if ( l < 0.5 ) {
					t2 = l * ( 1.0 + s );
				} else {
					t2 = ( l + s ) - ( l * s );
				}

				t1 = 2.0 * l - t2;

				const auto tr = calc_color( th + ( 1.0 / 3.0 ), t1, t2 );
				const auto tg = calc_color( th, t1, t2 );
				const auto tb = calc_color( th - ( 1.0 / 3.0 ), t1, t2 );

				r  = ( int )std::round( tr * 255.0 );
				g  = ( int )std::round( tg * 255.0 );
				b  = ( int )std::round( tb * 255.0 );
				ia = ( int )a * 255;
			}
		}

	} // namespace detail

	struct hsla {
		double h, s, l, a;

		inline hsla adjust_hue( double hue ) const
		{
			return hsla{ h + hue, s, l, a };
		}

		inline hsla adjust_sat( double sat ) const
		{
			return hsla{ h, s + sat, l, a };
		}

		inline hsla adjust_lum( double lum ) const
		{
			return hsla{ h, s, l + lum, a };
		}

		inline struct rgba to_rgba( ) const;
	};

	struct rgba {
		int r, g, b, a;

		rgba( ) = default;
		rgba( int scalar ) : r( scalar ), g( scalar ), b( scalar ), a( 255 ) { }
		rgba( int scalar, int a ) : r( scalar ), g( scalar ), b( scalar ), a( a ) { }
		rgba( int scalar, float a ) : r( scalar ), g( scalar ), b( scalar ), a( ( int )( a * 255 ) ) { }
		rgba( int r, int g, int b ) : r( r ), g( g ), b( b ), a( 255 ) { }
		rgba( int r, int g, int b, int a ) : r( r ), g( g ), b( b ), a( a ) { }
		rgba( int r, int g, int b, float a ) : r( r ), g( g ), b( b ), a( ( int )( a * 255 ) ) { }

		inline rgba adjust_alpha( int alpha ) const
		{
			return { r, g, b, alpha };
		}

		inline rgba adjust_alpha( float alpha ) const
		{
			return { r, g, b, ( int )( alpha * 255 ) };
		}

		inline hsla to_hsla( ) const
		{
			double h = 0.0, s = 0.0, l = 0.0, da = 0.0;

			detail::to_hsla( r, g, b, a, h, s, l, da );

			return { h, s, l, da };
		}
	};

	inline struct rgba hsla::to_rgba( ) const
	{
		int r = 0, g = 0, b = 0, a = 0;

		detail::to_rgba( h, s, l, a, r, g, b, a );

		return { r, g, b, a };
	}

	using color = typename rgba;

} // namespace sdk

#pragma once
#include <algorithm>
#include <cfloat>
#include <cmath>
#include <string_view>

namespace sdk
{

	inline bool compare( const float a, const float b, const float epsilon )
	{
		return fabs( a - b ) <= epsilon * std::max( fabs( a ), fabs( b ) );
	}

	struct vector2 {
		int x, y;

		vector2( ) = default;
		vector2( int value ) : x( value ), y( value ) { }
		vector2( int x, int y ) : x( x ), y( y ) { }

		inline friend vector2 operator+( const vector2& lhs, int rhs )
		{
			return { lhs.x + rhs, lhs.y + rhs };
		}

		inline friend vector2 operator-( const vector2& lhs, int rhs )
		{
			return { lhs.x - rhs, lhs.y - rhs };
		}

		inline friend vector2 operator*( const vector2& lhs, int rhs )
		{
			return { lhs.x * rhs, lhs.y * rhs };
		}

		inline friend vector2 operator/( const vector2& lhs, int rhs )
		{
			return { lhs.x / rhs, lhs.y / rhs };
		}

		inline friend vector2 operator+( const vector2& lhs, const vector2& rhs )
		{
			return { lhs.x + rhs.x, lhs.y + rhs.y };
		}

		inline friend vector2 operator-( const vector2& lhs, const vector2& rhs )
		{
			return { lhs.x - rhs.x, lhs.y - rhs.y };
		}

		inline friend vector2 operator*( const vector2& lhs, const vector2& rhs )
		{
			return { lhs.x * rhs.x, lhs.y * rhs.y };
		}

		inline friend vector2 operator/( const vector2& lhs, const vector2& rhs )
		{
			return { lhs.x / rhs.x, lhs.y / rhs.y };
		}

		inline vector2& operator+=( int rhs )
		{
			x += rhs;
			y += rhs;
			return *this;
		}

		inline vector2& operator-=( int rhs )
		{
			x -= rhs;
			y -= rhs;
			return *this;
		}

		inline vector2& operator*=( int rhs )
		{
			x *= rhs;
			y *= rhs;
			return *this;
		}

		inline vector2& operator/=( int rhs )
		{
			x /= rhs;
			y /= rhs;
			return *this;
		}

		inline vector2& operator+=( const vector2& rhs )
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		inline vector2& operator-=( const vector2& rhs )
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		inline vector2& operator*=( const vector2& rhs )
		{
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		inline vector2& operator/=( const vector2& rhs )
		{
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}
	};

	struct vector2f {
		float x, y;

		vector2f( ) = default;
		vector2f( float value ) : x( value ), y( value ) { }
		vector2f( float x, float y ) : x( x ), y( y ) { }

		inline friend vector2f operator+( const vector2f& lhs, float rhs )
		{
			return { lhs.x + rhs, lhs.y + rhs };
		}

		inline friend vector2f operator-( const vector2f& lhs, float rhs )
		{
			return { lhs.x - rhs, lhs.y - rhs };
		}

		inline friend vector2f operator*( const vector2f& lhs, float rhs )
		{
			return { lhs.x * rhs, lhs.y * rhs };
		}

		inline friend vector2f operator/( const vector2f& lhs, float rhs )
		{
			return { lhs.x / rhs, lhs.y / rhs };
		}

		inline friend vector2f operator+( const vector2f& lhs, const vector2f& rhs )
		{
			return { lhs.x + rhs.x, lhs.y + rhs.y };
		}

		inline friend vector2f operator-( const vector2f& lhs, const vector2f& rhs )
		{
			return { lhs.x - rhs.x, lhs.y - rhs.y };
		}

		inline friend vector2f operator*( const vector2f& lhs, const vector2f& rhs )
		{
			return { lhs.x * rhs.x, lhs.y * rhs.y };
		}

		inline friend vector2f operator/( const vector2f& lhs, const vector2f& rhs )
		{
			return { lhs.x / rhs.x, lhs.y / rhs.y };
		}

		inline vector2f& operator+=( float rhs )
		{
			x += rhs;
			y += rhs;
			return *this;
		}

		inline bool operator==( const vector2f& rhs ) const
		{
			return rhs.x == this->x && rhs.y == this->y;
		}

		inline bool operator!=( const vector2f& rhs )
		{
			return !( operator==( rhs ) );
		}

		inline vector2f& operator-=( float rhs )
		{
			x -= rhs;
			y -= rhs;
			return *this;
		}

		inline vector2f& operator*=( float rhs )
		{
			x *= rhs;
			y *= rhs;
			return *this;
		}

		inline vector2f& operator/=( float rhs )
		{
			x /= rhs;
			y /= rhs;
			return *this;
		}

		inline vector2f& operator+=( const vector2f& rhs )
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		inline vector2f& operator-=( const vector2f& rhs )
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		inline vector2f& operator*=( const vector2f& rhs )
		{
			x *= rhs.x;
			y *= rhs.y;
			return *this;
		}

		inline vector2f& operator/=( const vector2f& rhs )
		{
			x /= rhs.x;
			y /= rhs.y;
			return *this;
		}

		inline bool operator<( const vector2f& rhs ) const
		{
			return y < rhs.y && x < rhs.x;
		}

		inline bool operator>( const vector2f& rhs ) const
		{
			return y < rhs.y && x < rhs.x;
		}

		inline bool operator<=( const vector2f& rhs ) const
		{
			return y <= rhs.y && x <= rhs.x;
		}

		inline bool operator>=( const vector2f& rhs ) const
		{
			return y <= rhs.y && x <= rhs.x;
		}

		inline float dot( const vector2& rhs ) const
		{
			return x * rhs.x + y * rhs.y;
		}

		inline float dot( ) const
		{
			return x * x + y * y;
		}

		inline float dot( const sdk::vector2f& lhs, const sdk::vector2f& rhs ) const
		{
			return lhs.x * rhs.x + lhs.y * rhs.y;
		}

		inline bool is_empty( ) const
		{
			return x == 0.0f && y == 0.0f;
		}

		inline float length_squared( ) const
		{
			return x * x + y * y;
		}

		inline sdk::vector2f rotate( float theta ) const
		{
			const auto cos_theta = std::cos( theta );
			const auto sin_theta = std::sin( theta );
			return { cos_theta * x + sin_theta * y, -sin_theta * x + cos_theta * y };
		}

		inline double angle( const sdk::vector2f& rhs ) const
		{
			return std::acos( std::fmax( -1., std::fmin( 1., dot( this->normalized( ), rhs.normalized( ) ) ) ) );
		}

		inline float length( ) const
		{
			return sqrt( length_squared( ) );
		}

		inline vector2f normalized( ) const
		{
			const auto len = length( );

			return { x / len, y / len };
		}

		inline vector2f finite( ) const
		{
			if ( !std::isfinite( this->length( ) ) ) {
				return { };
			}

			return *this;
		}

		inline vector2f v_min( const vector2f& rhs ) const
		{
			if ( *this > rhs )
				return *this;

			return rhs;
		}

		inline vector2f v_max( const vector2f& rhs ) const
		{
			if ( *this < rhs )
				return *this;

			return rhs;
		}

		inline vector2f radius_clamp( float radius ) const
		{
			return *this - ( normalized( ) * radius );
		}

		inline vector2f clamp( float range ) const
		{
			return { std::clamp( this->x, -range, range ), std::clamp( this->y, -range, range ) };
		}
	};
} // namespace sdk

namespace utils::render
{

	enum font {
		FONT_SEGOE_UI_16,
		FONT_SEGOE_UI_18,
		FONT_FA_REGULAR_32,
		FONT_FA_SOLID_32,
		FONT_FA_BRANDS_32,
		FONT_MAX,
	};

	enum corner {
		CORNER_NONE         = 0,
		CORNER_TOP_LEFT     = 1 << 0,
		CORNER_TOP_RIGHT    = 1 << 1,
		CORNER_BOTTOM_LEFT  = 1 << 2,
		CORNER_BOTTOM_RIGHT = 1 << 3,
		CORNER_TOP          = CORNER_TOP_LEFT | CORNER_TOP_RIGHT,
		CORNER_BOTTOM       = CORNER_BOTTOM_LEFT | CORNER_BOTTOM_RIGHT,
		CORNER_LEFT         = CORNER_TOP_LEFT | CORNER_BOTTOM_LEFT,
		CORNER_RIGHT        = CORNER_TOP_RIGHT | CORNER_BOTTOM_RIGHT,
		CORNER_ALL          = ~CORNER_NONE,
	};

	inline ImFont* fonts[ utils::render::FONT_MAX ];

	using sdk::color;
	using sdk::vector2;
	using sdk::vector2f;

	uintptr_t get_window_handle( );

	bool is_ready( );
	bool setup( uintptr_t device_context );
	bool undo( );

	void start( );
	void end( );

	int get_layer( );

	void set_layer( int layer );
	void set_clip( const vector2& min, const vector2& max, bool intersect = true );
	void set_clip_foreground( const vector2& min, const vector2& max, bool intersect = true );
	void set_clip_fullscreen( );
	void pop_clip( );
	void pop_clip_foreground( );
	void reset_clip( );

	// clang-format off
    void line(const vector2& start, const vector2& end, const color& color, float thickness = 1.0f);
    void quad(const vector2& tl, const vector2& tr, const vector2& bl, const vector2& br, const color& color, float thickness = 1.0f);
    void rect(const vector2& position, const vector2& size, const color& color, float rounding = 0.0f, int corners = CORNER_ALL, float thickness = 1.0f);
    void rect_foreground(const vector2& position, const vector2& size, const color& color, float rounding = 0.0f, int corners = CORNER_ALL, float thickness = 1.0f);
    void triangle(const vector2& point1, const vector2& point2, const vector2& point3, const color& color, float thickness = 1.0f);
    void circle(const vector2& anchor, float radius, const color& color, float thickness = 1.0f);
    void circle(const vector2f& anchor, float radius, const color& color, float thickness = 1.0f);
    void text(const vector2& position, std::string_view text, int font, const color& color, float wrap_width = 0.0f, float font_size = -1.0f);
    void text_foreground(const vector2& position, std::string_view text, int font, const color& color, float wrap_width = 0.0f, float font_size = -1.0f);

    void fill_quad(const vector2& tl, const vector2& tr, const vector2& bl, const vector2& br, const color& color);
    void fill_rect(const vector2& position, const vector2& size, const color& color, float rounding = 0.0f, int corners = CORNER_ALL);
    void fill_rect_foreground(const vector2& position, const vector2& size, const color& color, float rounding = 0.0f, int corners = CORNER_ALL);
    void fill_gradient(const vector2& position, const vector2& size, const color& color1, const color& color2, bool horizontal = true);
    void fill_gradient_foreground(const vector2& position, const vector2& size, const color& color1, const color& color2, bool horizontal = true);
    void fill_triangle(const vector2& point1, const vector2& point2, const vector2& point3, const color& color);
    void fill_circle(const vector2& anchor, float radius, const color& color);
    void fill_circle_foreground(const vector2& anchor, float radius, const color& color);
    void fill_circle(const vector2f& anchor, float radius, const color& color);
	// clang-format on

	vector2 get_window_size( );
	vector2 measure_text( std::string_view text, int font, float wrap_width = 0.0f, float font_size = -1.0f );
	float get_fps( );
	void color_convert_rgb_to_hsv( float r, float g, float b, float& h, float& s, float& v );
	void color_convert_hsv_to_rgb( float h, float s, float v, float& r, float& g, float& b );
} // namespace utils::render
