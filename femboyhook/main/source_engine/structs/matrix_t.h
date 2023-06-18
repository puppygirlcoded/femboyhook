#pragma once
#include "../classes/c_vector.h"

using matrix3x3_t = float[ 3 ][ 3 ];
struct matrix3x4_t {
	matrix3x4_t( ) = default;

	constexpr matrix3x4_t( const float m00, const float m01, const float m02, const float m03, const float m10, const float m11, const float m12,
	                       const float m13, const float m20, const float m21, const float m22, const float m23 )
	{
		data[ 0 ][ 0 ] = m00;
		data[ 0 ][ 1 ] = m01;
		data[ 0 ][ 2 ] = m02;
		data[ 0 ][ 3 ] = m03;
		data[ 1 ][ 0 ] = m10;
		data[ 1 ][ 1 ] = m11;
		data[ 1 ][ 2 ] = m12;
		data[ 1 ][ 3 ] = m13;
		data[ 2 ][ 0 ] = m20;
		data[ 2 ][ 1 ] = m21;
		data[ 2 ][ 2 ] = m22;
		data[ 2 ][ 3 ] = m23;
	}

	constexpr matrix3x4_t( const c_vector& x_axis, const c_vector& y_axis, const c_vector& z_axis, const c_vector& origin )
	{
		init( x_axis, y_axis, z_axis, origin );
	}

	constexpr void init( const c_vector& forward, const c_vector& left, const c_vector& up, const c_vector& origin )
	{
		set_forward( forward );
		set_left( left );
		set_up( up );
		set_origin( origin );
	}

	constexpr void set_forward( const c_vector& forward )
	{
		this->data[ 0 ][ 0 ] = forward.m_x;
		this->data[ 1 ][ 0 ] = forward.m_y;
		this->data[ 2 ][ 0 ] = forward.m_z;
	}

	constexpr void set_left( const c_vector& left )
	{
		this->data[ 0 ][ 1 ] = left.m_x;
		this->data[ 1 ][ 1 ] = left.m_y;
		this->data[ 2 ][ 1 ] = left.m_z;
	}

	constexpr void set_up( const c_vector& up )
	{
		this->data[ 0 ][ 2 ] = up.m_x;
		this->data[ 1 ][ 2 ] = up.m_y;
		this->data[ 2 ][ 2 ] = up.m_z;
	}

	constexpr void set_origin( const c_vector& origin )
	{
		this->data[ 0 ][ 3 ] = origin.m_x;
		this->data[ 1 ][ 3 ] = origin.m_y;
		this->data[ 2 ][ 3 ] = origin.m_z;
	}

	constexpr void invalidate( )
	{
		for ( auto& sub_data : data ) {
			for ( auto& new_data : sub_data )
				new_data = std::numeric_limits< float >::infinity( );
		}
	}

	float* operator[]( const int index )
	{
		return data[ index ];
	}

	const float* operator[]( const int index ) const
	{
		return data[ index ];
	}

	[[nodiscard]] constexpr c_vector at( const int index ) const
	{
		return c_vector( data[ 0 ][ index ], data[ 1 ][ index ], data[ 2 ][ index ] );
	}

	float* base( )
	{
		return &data[ 0 ][ 0 ];
	}

	[[nodiscard]] const float* base( ) const
	{
		return &data[ 0 ][ 0 ];
	}

	float data[ 3 ][ 4 ] = { };
};

__declspec( align( 16 ) ) class matrix3x4a_t : public matrix3x4_t
{
public:
	matrix3x4a_t& operator=( const matrix3x4_t& mat_source )
	{
		std::copy_n( mat_source.base( ), sizeof( float ) * 3u * 4u, this->base( ) );
		return *this;
	}
};

struct view_matrix_t {
	view_matrix_t( ) = default;

	constexpr view_matrix_t( const float m00, const float m01, const float m02, const float m03, const float m10, const float m11, const float m12,
	                         const float m13, const float m20, const float m21, const float m22, const float m23, const float m30, const float m31,
	                         const float m32, const float m33 )
	{
		data[ 0 ][ 0 ] = m00;
		data[ 0 ][ 1 ] = m01;
		data[ 0 ][ 2 ] = m02;
		data[ 0 ][ 3 ] = m03;
		data[ 1 ][ 0 ] = m10;
		data[ 1 ][ 1 ] = m11;
		data[ 1 ][ 2 ] = m12;
		data[ 1 ][ 3 ] = m13;
		data[ 2 ][ 0 ] = m20;
		data[ 2 ][ 1 ] = m21;
		data[ 2 ][ 2 ] = m22;
		data[ 2 ][ 3 ] = m23;
		data[ 3 ][ 0 ] = m30;
		data[ 3 ][ 1 ] = m31;
		data[ 3 ][ 2 ] = m32;
		data[ 3 ][ 3 ] = m33;
	}

	constexpr view_matrix_t( const matrix3x4_t& mat_from, const c_vector_4d& additional_column = { } )
	{
		data[ 0 ][ 0 ] = mat_from.data[ 0 ][ 0 ];
		data[ 0 ][ 1 ] = mat_from.data[ 0 ][ 1 ];
		data[ 0 ][ 2 ] = mat_from.data[ 0 ][ 2 ];
		data[ 0 ][ 3 ] = mat_from.data[ 0 ][ 3 ];
		data[ 1 ][ 0 ] = mat_from.data[ 1 ][ 0 ];
		data[ 1 ][ 1 ] = mat_from.data[ 1 ][ 1 ];
		data[ 1 ][ 2 ] = mat_from.data[ 1 ][ 2 ];
		data[ 1 ][ 3 ] = mat_from.data[ 1 ][ 3 ];
		data[ 2 ][ 0 ] = mat_from.data[ 2 ][ 0 ];
		data[ 2 ][ 1 ] = mat_from.data[ 2 ][ 1 ];
		data[ 2 ][ 2 ] = mat_from.data[ 2 ][ 2 ];
		data[ 2 ][ 3 ] = mat_from.data[ 2 ][ 3 ];
		data[ 3 ][ 0 ] = additional_column.m_x;
		data[ 3 ][ 1 ] = additional_column.m_y;
		data[ 3 ][ 2 ] = additional_column.m_z;
		data[ 3 ][ 3 ] = additional_column.w;
	}

	float* operator[]( const int index )
	{
		return data[ index ];
	}

	const float* operator[]( const int index ) const
	{
		return data[ index ];
	}

	[[nodiscard]] constexpr c_vector_4d at( const int index ) const
	{
		return c_vector_4d( data[ 0 ][ index ], data[ 1 ][ index ], data[ 2 ][ index ], data[ 3 ][ index ] );
	}

	constexpr view_matrix_t& operator+=( const view_matrix_t& mat_add )
	{
		for ( int i = 0; i < 4; i++ ) {
			for ( int n = 0; n < 4; n++ )
				this->data[ i ][ n ] += mat_add[ i ][ n ];
		}

		return *this;
	}

	constexpr view_matrix_t& operator-=( const view_matrix_t& mat_subtract )
	{
		for ( int i = 0; i < 4; i++ ) {
			for ( int n = 0; n < 4; n++ )
				this->data[ i ][ n ] -= mat_subtract[ i ][ n ];
		}

		return *this;
	}

	view_matrix_t operator*( const view_matrix_t& multiply ) const
	{
		return view_matrix_t( data[ 0 ][ 0 ] * multiply.data[ 0 ][ 0 ] + data[ 0 ][ 1 ] * multiply.data[ 1 ][ 0 ] +
		                          data[ 0 ][ 2 ] * multiply.data[ 2 ][ 0 ] + data[ 0 ][ 3 ] * multiply.data[ 3 ][ 0 ],
		                      data[ 0 ][ 0 ] * multiply.data[ 0 ][ 1 ] + data[ 0 ][ 1 ] * multiply.data[ 1 ][ 1 ] +
		                          data[ 0 ][ 2 ] * multiply.data[ 2 ][ 1 ] + data[ 0 ][ 3 ] * multiply.data[ 3 ][ 1 ],
		                      data[ 0 ][ 0 ] * multiply.data[ 0 ][ 2 ] + data[ 0 ][ 1 ] * multiply.data[ 1 ][ 2 ] +
		                          data[ 0 ][ 2 ] * multiply.data[ 2 ][ 2 ] + data[ 0 ][ 3 ] * multiply.data[ 3 ][ 2 ],
		                      data[ 0 ][ 0 ] * multiply.data[ 0 ][ 3 ] + data[ 0 ][ 1 ] * multiply.data[ 1 ][ 3 ] +
		                          data[ 0 ][ 2 ] * multiply.data[ 2 ][ 3 ] + data[ 0 ][ 3 ] * multiply.data[ 3 ][ 3 ],

		                      data[ 1 ][ 0 ] * multiply.data[ 0 ][ 0 ] + data[ 1 ][ 1 ] * multiply.data[ 1 ][ 0 ] +
		                          data[ 1 ][ 2 ] * multiply.data[ 2 ][ 0 ] + data[ 1 ][ 3 ] * multiply.data[ 3 ][ 0 ],
		                      data[ 1 ][ 0 ] * multiply.data[ 0 ][ 1 ] + data[ 1 ][ 1 ] * multiply.data[ 1 ][ 1 ] +
		                          data[ 1 ][ 2 ] * multiply.data[ 2 ][ 1 ] + data[ 1 ][ 3 ] * multiply.data[ 3 ][ 1 ],
		                      data[ 1 ][ 0 ] * multiply.data[ 0 ][ 2 ] + data[ 1 ][ 1 ] * multiply.data[ 1 ][ 2 ] +
		                          data[ 1 ][ 2 ] * multiply.data[ 2 ][ 2 ] + data[ 1 ][ 3 ] * multiply.data[ 3 ][ 2 ],
		                      data[ 1 ][ 0 ] * multiply.data[ 0 ][ 3 ] + data[ 1 ][ 1 ] * multiply.data[ 1 ][ 3 ] +
		                          data[ 1 ][ 2 ] * multiply.data[ 2 ][ 3 ] + data[ 1 ][ 3 ] * multiply.data[ 3 ][ 3 ],

		                      data[ 2 ][ 0 ] * multiply.data[ 0 ][ 0 ] + data[ 2 ][ 1 ] * multiply.data[ 1 ][ 0 ] +
		                          data[ 2 ][ 2 ] * multiply.data[ 2 ][ 0 ] + data[ 2 ][ 3 ] * multiply.data[ 3 ][ 0 ],
		                      data[ 2 ][ 0 ] * multiply.data[ 0 ][ 1 ] + data[ 2 ][ 1 ] * multiply.data[ 1 ][ 1 ] +
		                          data[ 2 ][ 2 ] * multiply.data[ 2 ][ 1 ] + data[ 2 ][ 3 ] * multiply.data[ 3 ][ 1 ],
		                      data[ 2 ][ 0 ] * multiply.data[ 0 ][ 2 ] + data[ 2 ][ 1 ] * multiply.data[ 1 ][ 2 ] +
		                          data[ 2 ][ 2 ] * multiply.data[ 2 ][ 2 ] + data[ 2 ][ 3 ] * multiply.data[ 3 ][ 2 ],
		                      data[ 2 ][ 0 ] * multiply.data[ 0 ][ 3 ] + data[ 2 ][ 1 ] * multiply.data[ 1 ][ 3 ] +
		                          data[ 2 ][ 2 ] * multiply.data[ 2 ][ 3 ] + data[ 2 ][ 3 ] * multiply.data[ 3 ][ 3 ],

		                      data[ 3 ][ 0 ] * multiply.data[ 0 ][ 0 ] + data[ 3 ][ 1 ] * multiply.data[ 1 ][ 0 ] +
		                          data[ 3 ][ 2 ] * multiply.data[ 2 ][ 0 ] + data[ 3 ][ 3 ] * multiply.data[ 3 ][ 0 ],
		                      data[ 3 ][ 0 ] * multiply.data[ 0 ][ 1 ] + data[ 3 ][ 1 ] * multiply.data[ 1 ][ 1 ] +
		                          data[ 3 ][ 2 ] * multiply.data[ 2 ][ 1 ] + data[ 3 ][ 3 ] * multiply.data[ 3 ][ 1 ],
		                      data[ 3 ][ 0 ] * multiply.data[ 0 ][ 2 ] + data[ 3 ][ 1 ] * multiply.data[ 1 ][ 2 ] +
		                          data[ 3 ][ 2 ] * multiply.data[ 2 ][ 2 ] + data[ 3 ][ 3 ] * multiply.data[ 3 ][ 2 ],
		                      data[ 3 ][ 0 ] * multiply.data[ 0 ][ 3 ] + data[ 3 ][ 1 ] * multiply.data[ 1 ][ 3 ] +
		                          data[ 3 ][ 2 ] * multiply.data[ 2 ][ 3 ] + data[ 3 ][ 3 ] * multiply.data[ 3 ][ 3 ] );
	}

	constexpr void identity( )
	{
		for ( int i = 0; i < 4; i++ ) {
			for ( int n = 0; n < 4; n++ )
				this->data[ i ][ n ] = i == n ? 1.0f : 0.0f;
		}
	}

	const matrix3x4_t& as3x4( ) const
	{
		return *reinterpret_cast< const matrix3x4_t* >( this );
	}

	matrix3x4_t& as3x4( )
	{
		return *reinterpret_cast< matrix3x4_t* >( this );
	}

	float data[ 4 ][ 4 ] = { };
};
