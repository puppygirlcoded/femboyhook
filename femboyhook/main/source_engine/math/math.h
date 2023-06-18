#pragma once
#include "../classes/c_vector.h"
#include "../structs/matrix_t.h"

struct math_t {
	float m_pi = 3.1415926535897932384f;
	float m_rad_pi = 57.295779513082f;

	float deg_to_rad( float val );
	float rad_to_deg( float val );
	void sin_cos( const float radian, float* sin, float* cos );
	void angle_vectors( const c_vector& angles, c_vector* forward, c_vector* right = ( c_vector* )nullptr, c_vector* up = ( c_vector* )nullptr );
	c_vector vector_angles( const c_vector& start, const c_vector& end );
	c_vector vector_transform( const c_vector& vector, const matrix3x4_t& matrix );
	bool normalize_angles( c_vector& angles );
	int calculate_field_of_view( const c_vector& view_angles,const c_vector& destination, const c_vector& aim_angles );
	c_vector calculate_angle( const c_vector& source, const c_vector& destination, const c_vector& view_angles );
};

inline math_t math = { };