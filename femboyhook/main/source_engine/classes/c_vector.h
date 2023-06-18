#pragma once
// @credits: https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/public/mathlib/vector.h

// used: isfinite, fmodf, sqrtf
#include <cmath>
// used: numeric_limits
#include <algorithm>
#include <limits>

class c_vector_2d
{
public:
	constexpr c_vector_2d( float m_x = 0.f, float m_y = 0.f ) : m_x( m_x ), m_y( m_y ) { }

	[[nodiscard]] bool is_zero( ) const
	{
		return ( std::fpclassify( this->m_x ) == FP_ZERO && std::fpclassify( this->m_y ) == FP_ZERO );
	}

	c_vector_2d operator*( float multiply )
	{
		return c_vector_2d( this->m_x * multiply, this->m_y * multiply );
	}

public:
	float m_x = { }, m_y = { };
};
class c_vector
{
public:
	constexpr c_vector( float m_x = 0.f, float m_y = 0.f, float m_z = 0.f ) : m_x( m_x ), m_y( m_y ), m_z( m_z ) { }

	constexpr c_vector( const float* vector ) : m_x( vector[ 0 ] ), m_y( vector[ 1 ] ), m_z( vector[ 2 ] ) { }

	constexpr c_vector( const c_vector_2d& vector_2d ) : m_x( vector_2d.m_x ), m_y( vector_2d.m_y ), m_z( 0.0f ) { }

	void init( float _x, float _y, float _z )
	{
		m_x = _x;
		m_y = _y;
		m_z = _z;
	}

	[[nodiscard]] bool is_valid( ) const
	{
		return std::isfinite( this->m_x ) && std::isfinite( this->m_y ) && std::isfinite( this->m_z );
	}

	constexpr void invalidate( )
	{
		this->m_x = this->m_y = this->m_z = std::numeric_limits< float >::infinity( );
	}

	void clamp( void )
	{
		this->m_x = std::clamp( m_x, -89.f, 89.f );
		this->m_y = std::clamp( std::remainder( m_y, 360.f ), -180.f, 180.f );
		this->m_z = std::clamp( m_z, -50.f, 50.f );
	}

	c_vector clamped( )
	{
		auto clamped = *this;
		clamped.clamp( );
		return clamped;
	}
	c_vector normalize( )
	{
		this->m_x = std::isfinite( this->m_x ) ? std::remainderf( this->m_x, 360.f ) : 0.f;
		this->m_y = std::isfinite( this->m_y ) ? std::remainderf( this->m_y, 360.f ) : 0.f;
		this->m_z = std::clamp( this->m_z, -50.f, 50.f );
		return *this;
	}
	[[nodiscard]] float* data( )
	{
		return reinterpret_cast< float* >( this );
	}

	[[nodiscard]] const float* data( ) const
	{
		return reinterpret_cast< float* >( const_cast< c_vector* >( this ) );
	}

	float& operator[]( const std::size_t i )
	{
		return this->data( )[ i ];
	}

	const float& operator[]( const std::size_t i ) const
	{
		return this->data( )[ i ];
	}

	bool operator==( const c_vector& vector ) const
	{
		return this->is_equal( vector );
	}

	bool operator!=( const c_vector& vector ) const
	{
		return !this->is_equal( vector );
	}

	constexpr c_vector& operator=( const c_vector& vector )
	{
		this->m_x = vector.m_x;
		this->m_y = vector.m_y;
		this->m_z = vector.m_z;
		return *this;
	}

	constexpr c_vector& operator=( const c_vector_2d& vector2D )
	{
		this->m_x = vector2D.m_x;
		this->m_y = vector2D.m_y;
		this->m_z = 0.0f;
		return *this;
	}

	constexpr c_vector& operator+=( const c_vector& vector )
	{
		this->m_x += vector.m_x;
		this->m_y += vector.m_y;
		this->m_z += vector.m_z;
		return *this;
	}

	constexpr c_vector& operator-=( const c_vector& vector )
	{
		this->m_x -= vector.m_x;
		this->m_y -= vector.m_y;
		this->m_z -= vector.m_z;
		return *this;
	}

	constexpr c_vector& operator*=( const c_vector& vector )
	{
		this->m_x *= vector.m_x;
		this->m_y *= vector.m_y;
		this->m_z *= vector.m_z;
		return *this;
	}

	constexpr c_vector& operator/=( const c_vector& vector )
	{
		this->m_x /= vector.m_x;
		this->m_y /= vector.m_y;
		this->m_z /= vector.m_z;
		return *this;
	}

	constexpr c_vector& operator+=( const float add )
	{
		this->m_x += add;
		this->m_y += add;
		this->m_z += add;
		return *this;
	}

	constexpr c_vector& operator-=( const float subtract )
	{
		this->m_x -= subtract;
		this->m_y -= subtract;
		this->m_z -= subtract;
		return *this;
	}

	constexpr c_vector& operator*=( const float multiply )
	{
		this->m_x *= multiply;
		this->m_y *= multiply;
		this->m_z *= multiply;
		return *this;
	}

	constexpr c_vector& operator/=( const float divide )
	{
		this->m_x /= divide;
		this->m_y /= divide;
		this->m_z /= divide;
		return *this;
	}

	c_vector& operator/=( int divide )
	{
		m_x /= divide;
		m_y /= divide;
		m_z /= divide;

		return *this;
	}

	c_vector operator+( const c_vector& vector ) const
	{
		return c_vector( this->m_x + vector.m_x, this->m_y + vector.m_y, this->m_z + vector.m_z );
	}

	c_vector operator-( const c_vector& vector ) const
	{
		return c_vector( this->m_x - vector.m_x, this->m_y - vector.m_y, this->m_z - vector.m_z );
	}

	c_vector operator*( const c_vector& vector ) const
	{
		return c_vector( this->m_x * vector.m_x, this->m_y * vector.m_y, this->m_z * vector.m_z );
	}

	c_vector operator/( const c_vector& vecDivide ) const
	{
		return c_vector( this->m_x / vecDivide.m_x, this->m_y / vecDivide.m_y, this->m_z / vecDivide.m_z );
	}

	c_vector operator+( const float add ) const
	{
		return c_vector( this->m_x + add, this->m_y + add, this->m_z + add );
	}

	c_vector operator-( const float subtract ) const
	{
		return c_vector( this->m_x - subtract, this->m_y - subtract, this->m_z - subtract );
	}

	c_vector operator*( const float multiply ) const
	{
		return c_vector( this->m_x * multiply, this->m_y * multiply, this->m_z * multiply );
	}

	c_vector operator/( const float divide ) const
	{
		return c_vector( this->m_x / divide, this->m_y / divide, this->m_z / divide );
	}

	[[nodiscard]] bool is_equal( const c_vector& vector, const float error_margin = std::numeric_limits< float >::epsilon( ) ) const
	{
		return ( std::fabsf( this->m_x - vector.m_x ) < error_margin && std::fabsf( this->m_y - vector.m_y ) < error_margin &&
		         std::fabsf( this->m_z - vector.m_z ) < error_margin );
	}

	[[nodiscard]] bool is_zero( ) const
	{
		return ( std::fpclassify( this->m_x ) == FP_ZERO && std::fpclassify( this->m_y ) == FP_ZERO && std::fpclassify( this->m_z ) == FP_ZERO );
	}

	[[nodiscard]] c_vector_2d to_vector_2d( ) const
	{
		return c_vector_2d( this->m_x, this->m_y );
	}

	[[nodiscard]] float length( ) const
	{
		return std::sqrtf( this->length_squared( ) );
	}

	[[nodiscard]] constexpr float length_squared( ) const
	{
		return dot_product( *this );
	}

	[[nodiscard]] float length_2d( ) const
	{
		return std::sqrtf( this->length_2d_squared( ) );
	}

	[[nodiscard]] constexpr float length_2d_squared( ) const
	{
		return ( this->m_x * this->m_x + this->m_y * this->m_y );
	}

	[[nodiscard]] float dist_to( const c_vector& vector ) const
	{
		return ( *this - vector ).length( );
	}

	[[nodiscard]] constexpr float dist_to_squared( const c_vector& vector ) const
	{
		return ( *this - vector ).length_squared( );
	}

	[[nodiscard]] c_vector normalized( ) const
	{
		c_vector vector = *this;
		vector.normalize_in_place( );
		return vector;
	}

	void normalize1( )
	{
		auto floorf = [ & ]( float x ) { return float( ( int )x ); };

		auto roundf = [ & ]( float x ) {
			float t;

			// if (!isfinite(x))
			//    return x;

			if ( x >= 0.0 ) {
				t = floorf( x );
				if ( t - x <= -0.5 )
					t += 1.0;
				return t;
			} else {
				t = floorf( -x );
				if ( t + x <= -0.5 )
					t += 1.0;
				return -t;
			}
		};

		auto absf = []( float x ) { return x > 0.f ? x : -x; };

		auto x_rev = this->m_x / 360.f;
		if ( this->m_x > 180.f || this->m_x < -180.f ) {
			x_rev = absf( x_rev );
			x_rev = roundf( x_rev );

			if ( this->m_x < 0.f )
				this->m_x = ( this->m_x + 360.f * x_rev );

			else
				this->m_x = ( this->m_x - 360.f * x_rev );
		}

		auto y_rev = this->m_y / 360.f;
		if ( this->m_y > 180.f || this->m_y < -180.f ) {
			y_rev = absf( y_rev );
			y_rev = roundf( y_rev );

			if ( this->m_y < 0.f )
				this->m_y = ( this->m_y + 360.f * y_rev );

			else
				this->m_y = ( this->m_y - 360.f * y_rev );
		}

		auto z_rev = this->m_z / 360.f;
		if ( this->m_z > 180.f || this->m_z < -180.f ) {
			z_rev = absf( z_rev );
			z_rev = roundf( z_rev );

			if ( this->m_z < 0.f )
				this->m_z = ( this->m_z + 360.f * z_rev );

			else
				this->m_z = ( this->m_z - 360.f * z_rev );
		}
	}

	c_vector normalized1( )
	{
		c_vector ret = *this;
		ret.normalize1( );

		return ret;
	}
	[[nodiscard]] float normalize_movement( ) const
	{
		c_vector vector     = *this;
		float vector_length = vector.length( );
		if ( vector_length != 0.0f )
			vector /= vector_length;
		else
			vector.m_x = vector.m_y = vector.m_z = 0.0f;

		return vector_length;
	}

	float normalize_in_place( )
	{
		const float length = this->length( );
		const float radius = 1.0f / ( length + std::numeric_limits< float >::epsilon( ) );

		this->m_x *= radius;
		this->m_y *= radius;
		this->m_z *= radius;

		return length;
	}

	[[nodiscard]] constexpr float dot_product( const c_vector& vector ) const
	{
		return ( this->m_x * vector.m_x + this->m_y * vector.m_y + this->m_z * vector.m_z );
	}

	[[nodiscard]] constexpr c_vector cross_product( const c_vector& vector ) const
	{
		return c_vector( this->m_y * vector.m_z - this->m_z * vector.m_y, this->m_z * vector.m_x - this->m_x * vector.m_z,
		                 this->m_x * vector.m_y - this->m_y * vector.m_x );
	}

public:
	float m_x = { }, m_y = { }, m_z = { };
};

class c_vector_4d
{
public:
	constexpr c_vector_4d( float m_x = 0.f, float m_y = 0.f, float m_z = 0.f, float w = 0.f ) : m_x( m_x ), m_y( m_y ), m_z( m_z ), w( w ) { }

public:
	float m_x = { }, m_y = { }, m_z = { }, w = { };
};

class __declspec( align( 16 ) ) c_vector_aligned : public c_vector
{
public:
	c_vector_aligned( ) = default;

	explicit c_vector_aligned( const c_vector& vector )
	{
		this->m_x = vector.m_x;
		this->m_y = vector.m_y;
		this->m_z = vector.m_z;
		this->m_w = 0.f;
	}

	constexpr c_vector_aligned& operator=( const c_vector& vector )
	{
		this->m_x = vector.m_x;
		this->m_y = vector.m_y;
		this->m_z = vector.m_z;
		this->m_w = 0.f;
		return *this;
	}

public:
	float m_w = { };
};