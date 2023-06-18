#pragma once
#include <algorithm>
#include <cmath>

class c_angle
{
public:
	constexpr c_angle( float m_x = 0.f, float m_y = 0.f, float m_z = 0.f ) : m_x( m_x ), m_y( m_y ), m_z( m_z ) { }

	constexpr c_angle( const float* arrAngles ) : m_x( arrAngles[ 0 ] ), m_y( arrAngles[ 1 ] ), m_z( arrAngles[ 2 ] ) { }

	bool operator==( const c_angle& angle ) const
	{
		return this->is_equal( angle );
	}

	bool operator!=( const c_angle& angle ) const
	{
		return !this->is_equal( angle );
	}

	constexpr c_angle& operator=( const c_angle& angle )
	{
		this->m_x   = angle.m_x;
		this->m_y   = angle.m_y;
		this->m_z = angle.m_z;
		return *this;
	}

	constexpr c_angle& operator+=( const c_angle& angle )
	{
		this->m_x += angle.m_x;
		this->m_y += angle.m_y;
		this->m_z += angle.m_z;
		return *this;
	}

	constexpr c_angle& operator+=( const c_vector& vector )
	{
		this->m_x += vector.m_x;
		this->m_y += vector.m_y;
		this->m_z += vector.m_z;
		return *this;
	}

	constexpr c_angle& operator-=( const c_angle& angle )
	{
		this->m_x -= angle.m_x;
		this->m_y -= angle.m_y;
		this->m_z -= angle.m_z;
		return *this;
	}

	constexpr c_angle& operator*=( const c_angle& angle )
	{
		this->m_x *= angle.m_x;
		this->m_y *= angle.m_y;
		this->m_z *= angle.m_z;
		return *this;
	}

	constexpr c_angle& operator/=( const c_angle& angle )
	{
		this->m_x /= angle.m_x;
		this->m_y /= angle.m_y;
		this->m_z /= angle.m_z;
		return *this;
	}

	constexpr c_angle& operator+=( const float add )
	{
		this->m_x += add;
		this->m_y += add;
		this->m_z += add;
		return *this;
	}

	constexpr c_angle& operator-=( const float subtract )
	{
		this->m_x -= subtract;
		this->m_y -= subtract;
		this->m_z -= subtract;
		return *this;
	}

	constexpr c_angle& operator*=( const float multiply )
	{
		this->m_x *= multiply;
		this->m_y *= multiply;
		this->m_z *= multiply;
		return *this;
	}

	constexpr c_angle& operator/=( const float divide )
	{
		this->m_x /= divide;
		this->m_y /= divide;
		this->m_z /= divide;
		return *this;
	}

	c_angle operator+( const c_angle& angle ) const
	{
		return c_angle( this->m_x + angle.m_x, this->m_y + angle.m_y, this->m_z + angle.m_z );
	}

	c_angle operator-( const c_angle& angle ) const
	{
		return c_angle( this->m_x - angle.m_x, this->m_y - angle.m_y, this->m_z - angle.m_z );
	}

	c_angle operator*( const c_angle& angle ) const
	{
		return c_angle( this->m_x * angle.m_x, this->m_y * angle.m_y, this->m_z * angle.m_z );
	}

	c_angle operator/( const c_angle& angle ) const
	{
		return c_angle( this->m_x / angle.m_x, this->m_y / angle.m_y, this->m_z / angle.m_z );
	}

	c_angle operator+( const float add ) const
	{
		return c_angle( this->m_x + add, this->m_y + add, this->m_z + add );
	}

	c_angle operator-( const float subtract ) const
	{
		return c_angle( this->m_x - subtract, this->m_y - subtract, this->m_z - subtract );
	}

	c_angle operator*( const float multiply ) const
	{
		return c_angle( this->m_x * multiply, this->m_y * multiply, this->m_z * multiply );
	}

	c_angle operator/( const float divide ) const
	{
		return c_angle( this->m_x / divide, this->m_y / divide, this->m_z / divide );
	}

	[[nodiscard]] bool is_equal( const c_angle& angle, const float error_margin = std::numeric_limits< float >::epsilon( ) ) const
	{
		return ( std::fabsf( this->m_x - angle.m_x ) < error_margin && std::fabsf( this->m_y - angle.m_y ) < error_margin &&
		         std::fabsf( this->m_z - angle.m_z ) < error_margin );
	}

	[[nodiscard]] bool is_zero( ) const
	{
		return ( std::fpclassify( this->m_x ) == FP_ZERO && std::fpclassify( this->m_y ) == FP_ZERO && std::fpclassify( this->m_z ) == FP_ZERO );
	}

	c_angle clamp( )
	{
		this->m_x   = std::clamp( this->m_x, -89.f, 89.f );
		this->m_y   = std::clamp( this->m_y, -180.f, 180.f );
		this->m_z = std::clamp( this->m_z, -50.f, 50.f );
		return *this;
	}

	c_angle normalize( )
	{
		this->m_x   = std::isfinite( this->m_x ) ? std::remainderf( this->m_x, 360.f ) : 0.f;
		this->m_y   = std::isfinite( this->m_y ) ? std::remainderf( this->m_y, 360.f ) : 0.f;
		this->m_z = std::clamp( this->m_z, -50.f, 50.f );
		return *this;
	}

	c_angle mod( const float value )
	{
		this->m_x   = std::fmodf( this->m_x, value );
		this->m_y   = std::fmodf( this->m_y, value );
		this->m_z = std::fmodf( this->m_z, value );
		return *this;
	}

	c_vector as_vector() {
		return c_vector( this->m_x, this->m_y, this->m_z );
	}

public:
	float m_x = { }, m_y = { }, m_z = { };
};