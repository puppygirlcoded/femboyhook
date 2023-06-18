#pragma once
#include "c_vector.h"

class c_phys_convex;
class c_phys_polysoup;
class c_collision_query;
class c_physics_key_parser;
struct convertconvexparams_t;
class c_packed_physics_description;
class c_poly_hedron;

class c_phys_collide;

struct vcollide_t {
	unsigned short solid_count;
	unsigned short is_packed;
	unsigned short description_size;
	c_phys_collide** solids;
	char* key_values;
	void* user_data;
};

struct truncatedcone_t {
	c_vector origin;
	c_vector normal;
	float h;     // height of the cone (hl units)
	float theta; // cone angle (degrees)
};

class c_convex_info
{
public:
	virtual unsigned int get_contents( int convex_game_data ) = 0;
};

class c_physics_collision
{
public:
	virtual ~c_physics_collision( void ) { }

	virtual void function1( )  = 0;
	virtual void function2( )  = 0;
	virtual void function3( )  = 0;
	virtual void function4( )  = 0;
	virtual void function5( )  = 0;
	virtual void function6( )  = 0;
	virtual void function7( )  = 0;
	virtual void function8( )  = 0;
	virtual void function9( )  = 0;
	virtual void function10( ) = 0;
	virtual void function11( ) = 0;
	virtual void function12( ) = 0;
	virtual void function13( ) = 0;
	virtual void function14( ) = 0;
	virtual void function15( ) = 0;
	virtual void function16( ) = 0;
	virtual void function17( ) = 0;
	virtual void function18( ) = 0;
	virtual void function19( ) = 0;
	virtual void function20( ) = 0;
	virtual void function21( ) = 0;
	virtual void function22( ) = 0;
	virtual void function23( ) = 0;
	virtual void function24( ) = 0;
	virtual void function25( ) = 0;
	virtual void function26( ) = 0;
	virtual void function27( ) = 0;
	virtual void function28( ) = 0;
	virtual void function29( ) = 0;
	virtual void function30( ) = 0;
	virtual void function31( ) = 0;
	virtual void function32( ) = 0;
	virtual void function33( ) = 0;
	virtual void function34( ) = 0;
	virtual void function35( ) = 0;

	virtual void collide_load( vcollide_t* output, int solid_count, const char* buffer, int size, bool swap = false ) = 0;
	virtual void collide_unload( vcollide_t* pVCollide )                                                              = 0;
};