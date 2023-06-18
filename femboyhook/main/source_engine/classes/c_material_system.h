#pragma once

class c_material;
using material_handle_t = unsigned short;

enum {
	MATERIAL_ADAPTER_NAME_LENGTH = 1 << 9
};

struct material_adapter_info_t {
	char driver_name[ MATERIAL_ADAPTER_NAME_LENGTH ];
	unsigned int vendor_id;
	unsigned int device_id;
	unsigned int sub_sys_id;
	unsigned int revision;
	int dx_support_level;
	int min_dx_support_level;
	int max_dx_support_level;
	unsigned int driver_version_high;
	unsigned int driver_version_low;
};

class c_texture;

class c_material_context
{
public:
	virtual void begin_render( ) = 0;
	virtual void end_render( )   = 0;

	void get_render_target_dimensions( int* width, int* height )
	{
		using fn = void( __thiscall* )( void*, int*, int* );
		return ( *( fn** )this )[ 8 ]( this, width, height );
	}

	void set_frame_buffer_copy_texture( c_texture* texture, int texture_index )
	{
		using fn = void( __thiscall* )( void*, c_texture*, int );
		return ( *( fn** )this )[ 20 ]( this, texture, texture_index );
	}

	void copy_render_target_to_texture_ex( c_texture* texture, int unk, c_vector_4d* dst, c_vector_4d* src )
	{
		using fn = void( __thiscall* )( void*, c_texture*, int, c_vector_4d*, c_vector_4d* );
		return ( *( fn** )this )[ 122 ]( this, texture, unk, dst, src );
	}

	void get_viewport( int* x, int* y, int* width, int* height )
	{
		using fn = void( __thiscall* )( void*, int*, int*, int*, int* );
		return ( *( fn** )this )[ 41 ]( this, x, y, width, height );
	}
};
class c_texture;

class c_material_system
{
private:
	enum e_indexes {
		_find_material           = 84,
		_first_material          = 86,
		_next_material           = 87,
		_invalid_material_handle = 88,
		_get_meterial            = 89,
		_get_materials_count     = 90,
		_find_texture            = 91,
		_get_render_context      = 115,
	};

public:
	c_material* find_material( char const* material_name, const char* group_name, bool complain = true, const char* complain_prefix = 0 )
	{
		using fn = c_material*( __thiscall* )( c_material_system*, char const*, const char*, bool, const char* );
		return ( *( fn** )this )[ this->_find_material ]( this, material_name, group_name, complain, complain_prefix );
	}

	material_handle_t first_material( )
	{
		using fn = material_handle_t( __thiscall* )( c_material_system* );
		return ( *( fn** )this )[ this->_first_material ]( this );
	}

	material_handle_t next_material( material_handle_t handle )
	{
		using fn = material_handle_t( __thiscall* )( c_material_system*, material_handle_t );
		return ( *( fn** )this )[ this->_next_material ]( this, handle );
	}

	material_handle_t invalid_material_handle( )
	{
		using fn = material_handle_t( __thiscall* )( c_material_system* );
		return ( *( fn** )this )[ this->_invalid_material_handle ]( this );
	}

	c_material* get_material( material_handle_t handle )
	{
		using fn = c_material*( __thiscall* )( c_material_system*, material_handle_t );
		return ( *( fn** )this )[ this->_get_meterial ]( this, handle );
	}

	int get_materials_count( )
	{
		using fn = int( __thiscall* )( c_material_system* );
		return ( *( fn** )this )[ this->_get_materials_count ]( this );
	}

	c_material_context* get_render_context() {
		using fn = c_material_context*( __thiscall* )( c_material_system* );
		return ( *( fn** )this )[ this->_get_render_context ]( this );
	}

	c_texture* find_texture(const char* name, const char* group_name = nullptr, bool complain = true, int additional_flags = 0) {
		using fn = c_texture*( __thiscall* )( c_material_system*, const char*, const char*, bool, int );
		return ( *( fn** )this )[ this->_find_texture ]( this, name, group_name, complain, additional_flags );
	}

	int get_current_adapter( )
	{
		using fn = int( __thiscall* )( void* );
		return ( *( fn** )this )[ 25 ]( this );
	}

	void get_display_adapter_info( int adapter, material_adapter_info_t* adapter_info )
	{
		using fn = void( __thiscall* )( void*, int, material_adapter_info_t* );
		return ( *( fn** )this )[ 26 ]( this, adapter, adapter_info );
	}
};