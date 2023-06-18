#pragma once
#include <cmath>
struct ImVec4;
class c_color
{
public:
	c_color( ) = default;
	c_color( int scalar ) : m_r( scalar ), m_g( scalar ), m_b( scalar ), m_a( 255 ) { }
	c_color( int scalar, int a ) : m_r( scalar ), m_g( scalar ), m_b( scalar ), m_a( a ) { }
	c_color( int scalar, float a ) : m_r( scalar ), m_g( scalar ), m_b( scalar ), m_a( ( int )( a * 255 ) ) { }
	c_color( int r, int g, int b ) : m_r( r ), m_g( g ), m_b( b ), m_a( 255 ) { }
	c_color( int r, int g, int b, int a ) : m_r( r ), m_g( g ), m_b( b ), m_a( a ) { }
	c_color( int r, int g, int b, float a ) : m_r( r ), m_g( g ), m_b( b ), m_a( ( int )( a * 255 ) ) { }

	inline c_color adjust_alpha( int alpha ) const
	{
		return { m_r, m_g, m_b, alpha };
	}

	inline c_color adjust_alpha( float alpha ) const
	{
		return { m_r, m_g, m_b, ( int )( alpha * 255 ) };
	}

	[[nodiscard]] unsigned int get_u32( const float alpha_multiplier = 1.0f ) const;

	[[nodiscard]] ImVec4 get_vec4( const float alpha_multiplier = 1.0f ) const;

	bool operator!=( const c_color& color ) const
	{
		return ( m_r != color.m_r || m_g != color.m_g || m_b != color.m_b || m_a != color.m_a);
	}

	static c_color blend(c_color a, c_color b, float t) {
		
		const int m_r = std::lerp( a.m_r, b.m_r, t );
		const int m_g = std::lerp( a.m_g, b.m_g, t );
		const int m_b = std::lerp( a.m_b, b.m_b, t );
		const int m_a = std::lerp( a.m_a, b.m_a, t );
		return c_color( m_r, m_g, m_b, m_a );
	}

	int m_r = { }, m_g = { }, m_b = { }, m_a = { };
};