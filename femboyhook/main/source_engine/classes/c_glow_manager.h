#include "c_vector.h"
#include "c_base_entity.h"
#include "c_color.h"
class c_glow_object_definition
{
public:
	void set( c_color col )
	{
		color                  = c_vector( col.m_r / 255.f, col.m_g / 255.f, col.m_b / 255.f );
		alpha                  = col.m_a / 255.f;
		render_when_occluded   = true;
		render_when_unoccluded = false;
		bloom_amount           = 1.0f;
	}
	bool unused( )
	{
		return next_free_slot != -2;
	}

	int next_free_slot;
	c_base_entity* entity;
	c_vector color;
	float alpha;
	char unknown0[ 8 ];
	float bloom_amount;
	char unknown1[ 4 ];
	bool render_when_occluded;
	bool render_when_unoccluded;
	bool full_bloom_render;
	char unknown2[ 5 ];
	int glow_style;
	int splitscreen_slot;
};

class c_glow_manager
{
public:
	c_glow_object_definition* objects;
	char pad[ 8 ];
	int size;
};