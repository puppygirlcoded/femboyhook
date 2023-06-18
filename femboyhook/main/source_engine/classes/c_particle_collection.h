#pragma once
#include <cstddef>
#include <minwindef.h>

template< class T >
struct cutl_reference {
	cutl_reference* next;
	cutl_reference* previous;
	T* object;
};

enum e_attribute_data_type {
	ATTRDATATYPE_NONE  = -1,
	ATTRDATATYPE_FLOAT = 0,
	ATTRDATATYPE_4V,
	ATTRDATATYPE_INT,
	ATTRDATATYPE_POINTER,

	ATTRDATATYPE_COUNT
};

#define MAX_PARTICLE_ATTRIBUTES 24

#define DEFPARTICLE_ATTRIBUTE( name, bit, datatype )                                                                                                 \
	const int PARTICLE_ATTRIBUTE_##name##_MASK                       = ( 1 << bit );                                                                 \
	const int PARTICLE_ATTRIBUTE_##name                              = bit;                                                                          \
	const e_attribute_data_type PARTICLE_ATTRIBUTE_##name##_DATATYPE = datatype;

DEFPARTICLE_ATTRIBUTE( TINT_RGB, 6, ATTRDATATYPE_4V );

DEFPARTICLE_ATTRIBUTE( ALPHA, 7, ATTRDATATYPE_FLOAT );

struct cutil_string_simple {
	char* buffer;
	int capacity;
	int grow_size;
	int length;
};

class c_particle_system
{
	BYTE pad_0[ 308 ];

public:
	cutil_string_simple name;
};

struct particle_attribute_address_table {
	float* attributes[ MAX_PARTICLE_ATTRIBUTES ];
	size_t float_strides[ MAX_PARTICLE_ATTRIBUTES ];

	FORCEINLINE float* float_attribute_ptr( int attribute, int particle_number ) const
	{
		int block_ofs = particle_number / 4;
		return attributes[ attribute ] + float_strides[ attribute ] * block_ofs + ( particle_number & 3 );
	}
};

class c_particle_collection
{
	BYTE pad_0[ 48 ]; // 0
public:
	int active_particles; // 48
private:
	BYTE pad_1[ 12 ]; // 52
public:
	cutl_reference< c_particle_system > definition; // 64
private:
	BYTE pad_2[ 60 ]; // 80
public:
	c_particle_collection* parent; // 136
private:
	BYTE pad_3[ 84 ]; // 140
public:
	particle_attribute_address_table particle_attributes; // 224
};