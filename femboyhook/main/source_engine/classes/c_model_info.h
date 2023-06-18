#pragma once

#include "c_vector.h"
#include "../structs/matrix_t.h"
#include "c_material.h"

using quaternion_t = typename float[4];

struct studio_box_t {
	int bone;
	int group;
	c_vector mins;
	c_vector maxs;
	int name_index;
	int pad01[ 3 ];
	float radius;
	int pad02[ 4 ];
};

struct studio_bone_t {
	int name_index;
	int parent;
	int bone_controller[ 6 ];

	c_vector position;
	quaternion_t quaternion;
	c_vector rotation;
	c_vector position_scale;
	c_vector rotation_scale;

	matrix3x4_t pose_to_bone;
	quaternion_t quaternion_alignement;

	int flags;
	int proc_type;
	int proc_index;
	int physics_bone;
	int surface_prop_idx;
	int contents;
	int surf_prop_lookup;
	int unused[ 7 ];
};

struct studio_hitbox_set_t {
	int name_index;
	int hitbox_count;
	int hitbox_index;

	inline studio_box_t* get_hitbox( int index ) const
	{
		return ( studio_box_t* )( ( uintptr_t )this + hitbox_index ) + index;
	}
};

struct studio_hdr_t {
	int id;
	int version;
	long checksum;
	char name_char_array[ 64 ];
	int length;
	c_vector eye_position;
	c_vector illium_position;
	c_vector hull_mins;
	c_vector hull_maxs;
	c_vector mins;
	c_vector maxs;
	int flags;
	int bones_count;
	int bone_index;
	int bone_controllers_count;
	int bone_controller_index;
	int hitbox_sets_count;
	int hitbox_set_index;
	int local_anim_count;
	int local_anim_index;
	int local_seq_count;
	int local_seq_index;
	int activity_list_version;
	int events_indexed;
	int textures_count;
	int texture_index;

	inline studio_hitbox_set_t* get_hitbox_set( int index ) const
	{
		if ( index > hitbox_sets_count )
			return nullptr;

		return ( studio_hitbox_set_t* )( ( uintptr_t )this + hitbox_set_index ) + index;
	}

	inline studio_box_t* get_hitbox( int index, int set_index ) const
	{
		const auto set = get_hitbox_set( set_index );

		if ( !set )
			return nullptr;

		return set->get_hitbox( index );
	}

	inline studio_bone_t* get_bone( int index ) const
	{
		if ( index > bones_count )
			return nullptr;

		return ( studio_bone_t* )( ( uintptr_t )this + bone_index ) + index;
	}
};

struct c_model {
	void* handle;
	char name[ 255 ];
};

struct c_model_render_info {
	c_vector origin;
	c_vector angles;
	char pad[ 0x4 ];
	void* renderable;
	const c_model* model;
	const view_matrix_t* model_to_world;
	const view_matrix_t* lighting_offset;
	const c_vector* lighting_origin;
	int flags;
	int entity_index;
	int skin;
	int body;
	int hitboxset;
	unsigned short instance;
};

class c_model_render {
private:
	enum indexes {
		_override_material = 1,
		_is_forced_material_override = 2,
	};

public:
	void override_material( c_material* material ) {
		using fn = void( __thiscall* )( c_model_render*, c_material*, int, int );
		return ( *( fn** )this )[ this->_override_material ]( this, material, 0, 0 );
	}

	bool is_forced_material_override( ) {
		using fn = bool( __thiscall* )( c_model_render* );
		return ( *( fn** )this )[ this->_is_forced_material_override ]( this );
	}
};

class c_model_info {
private:
	enum indexes {
		_get_model = 1,
		_get_model_index = 2,
		_get_model_name = 3,
		_get_studio_model = 32,

	};
public:
	c_model* get_model( int index ) {
		using original_fn = c_model*( __thiscall* )( c_model_info*, int );
		return ( *( original_fn** )this )[ this->_get_model ]( this, index );
	}

	int get_model_index( const char* filename ) {
		using original_fn = int( __thiscall* )( c_model_info*, const char* );
		return ( *( original_fn** )this )[ this->_get_model_index ]( this, filename );
	}

	const char* get_model_name( const c_model* model ) {
		using original_fn = const char*( __thiscall* )( c_model_info*, const c_model* );
		return ( *( original_fn** )this )[ this->_get_model_name ]( this, model );
	}

	studio_hdr_t* get_studio_model( const c_model* model ) {
		using original_fn = studio_hdr_t*( __thiscall* )( c_model_info*, const c_model* );
		return ( *( original_fn** )this )[ this->_get_studio_model ]( this, model );
	}
};

struct studio_hw_data_t;
using studio_decal_handle_t = void*;
struct draw_model_state_t {
	studio_hdr_t* m_studio_hdr;
	studio_hw_data_t* m_studio_hw_data;
	void* m_renderable;
	const matrix3x4_t* m_model_to_world;
	studio_decal_handle_t m_decals;
	int m_draw_flags;
	int m_lod;
};