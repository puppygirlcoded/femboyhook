#include "math.h"

c_vector math_t::vector_transform( const c_vector& vector, const matrix3x4_t& matrix )
{
	return c_vector( vector.dot_product( matrix[ 0 ] ) + matrix[ 0 ][ 3 ], vector.dot_product( matrix[ 1 ] ) + matrix[ 1 ][ 3 ],
	                 vector.dot_product( matrix[ 2 ] ) + matrix[ 2 ][ 3 ] );
}

float math_t::deg_to_rad( float val )
{
	return val * ( m_pi / 180.f );
}

float math_t::rad_to_deg( float val )
{
	return val * ( 180.f / m_pi );
}

  void math_t::sin_cos( const float radian, float* sin, float* cos )
{
	*sin = std::sin( radian );
	*cos = std::cos( radian );
}
void math_t::angle_vectors( const c_vector& angles, c_vector* forward, c_vector* right, c_vector* up )
{
	float cp = std::cos( deg_to_rad( angles.m_x ) ), sp = std::sin( deg_to_rad( angles.m_x ) );
	float cy = std::cos( deg_to_rad( angles.m_y ) ), sy = std::sin( deg_to_rad( angles.m_y ) );
	float cr = std::cos( deg_to_rad( angles.m_z ) ), sr = std::sin( deg_to_rad( angles.m_z ) );

	if ( forward ) {
		forward->m_x = cp * cy;
		forward->m_y = cp * sy;
		forward->m_z = -sp;
	}

	if ( right ) {
		right->m_x = -1.f * sr * sp * cy + -1.f * cr * -sy;
		right->m_y = -1.f * sr * sp * sy + -1.f * cr * cy;
		right->m_z = -1.f * sr * cp;
	}

	if ( up ) {
		up->m_x = cr * sp * cy + -sr * -sy;
		up->m_y = cr * sp * sy + -sr * cy;
		up->m_z = cr * cp;
	}
}

c_vector math_t::vector_angles( const c_vector& start, const c_vector& end )
{
	const c_vector delta = end - start;

	const float magnitude = std::sqrt( delta.m_x * delta.m_x + delta.m_y * delta.m_y );
	const float pitch     = std::atan2( -delta.m_z, magnitude ) * m_rad_pi;
	const float yaw       = std::atan2( delta.m_y, delta.m_x ) * m_rad_pi;

	return c_vector( pitch, yaw, 0.0f );
}

bool math_t::normalize_angles( c_vector& angles )
{
	if ( std::isfinite( angles.m_x ) && std::isfinite( angles.m_y ) && std::isfinite( angles.m_z ) ) {
		angles.m_x = std::remainder( angles.m_x, 360.0f );
		angles.m_y = std::remainder( angles.m_y, 360.0f );

		return true;
	}

	return false;
}

int math_t::calculate_field_of_view( const c_vector& view_angles, const c_vector& destination, const c_vector& aim_angles )
{
	auto angle = calculate_angle( view_angles, destination, aim_angles );
	return std::hypotf( angle.m_x, angle.m_y );
}

c_vector math_t::calculate_angle(const c_vector& source, const c_vector& destination, const c_vector& view_angles) {
	c_vector delta = source - destination;
	c_vector angles;

	angles.m_x = rad_to_deg( atanf( delta.m_z / std::hypotf( delta.m_x, delta.m_y ) ) ) - view_angles.m_x;
	angles.m_y = rad_to_deg( atanf( delta.m_y / delta.m_x ) ) - view_angles.m_y;
	angles.m_z = 0.0f;

	if ( delta.m_x >= 0.0 )
		angles.m_y += 180.0f;

	return angles;
}