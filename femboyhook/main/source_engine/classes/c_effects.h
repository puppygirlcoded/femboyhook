#pragma once
#include "c_vector.h"

class c_effects
{
public:
	virtual ~c_effects( ){ };

	virtual void beam( const c_vector& start, const c_vector& end, int model_index, int halo_index, unsigned char frame_start,
	                   unsigned char frame_rate, float life, unsigned char width, unsigned char end_width, unsigned char fade_length,
	                   unsigned char noise, unsigned char red, unsigned char green, unsigned char blue, unsigned char brightness,
	                   unsigned char speed ) = 0;

	virtual void smoke( const c_vector& origin, int model_index, float scale, float framerate ) = 0;

	virtual void sparks( const c_vector& position, int magnitude = 1, int trail_length = 1, const c_vector* vec_dir = NULL ) = 0;

	virtual void dust( const c_vector& pos, const c_vector& dir, float size, float speed ) = 0;
};