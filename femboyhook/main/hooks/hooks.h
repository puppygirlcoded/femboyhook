#pragma once

#include <MinHook.h>

#include "../source_engine/enumerations/e_client_frame_stage.h"

#include "../includes.h"

class c_detour_hook
{
public:
	c_detour_hook( ) = default;

	explicit c_detour_hook( void* function, void* detour ) : m_base_fn( function ), m_replace_fn( detour ) { }

	bool create( void* function, void* detour )
	{
		m_base_fn = function;

		if ( !m_base_fn )
			return false;

		m_replace_fn = detour;

		if ( !m_replace_fn )
			return false;

		const MH_STATUS status = MH_CreateHook( m_base_fn, m_replace_fn, &m_original_fn );

		if ( status != MH_OK )
			throw std::runtime_error(
				std::vformat( xs( "failed to create hook function, status: {}\nbase function -> {:#08X}" ),
			                  std::make_format_args( MH_StatusToString( status ), reinterpret_cast< std::uintptr_t >( m_base_fn ) ) ) );

		if ( !this->replace( ) )
			return false;

		return true;
	}

	bool replace( )
	{
		if ( m_base_fn == nullptr )
			return false;

		if ( m_is_hooked )
			return false;

		const MH_STATUS status = MH_EnableHook( m_base_fn );

		if ( status != MH_OK )
			throw std::runtime_error(
				std::vformat( xs( "failed to enable hook function, status: {}\nbase function -> {:#08X} address" ),
			                  std::make_format_args( MH_StatusToString( status ), reinterpret_cast< std::uintptr_t >( m_base_fn ) ) ) );

		m_is_hooked = true;
		return true;
	}

	template< typename fn >
	fn get_original( )
	{
		return static_cast< fn >( m_original_fn );
	}

private:
	bool m_is_hooked    = false;
	void* m_base_fn     = nullptr;
	void* m_replace_fn  = nullptr;
	void* m_original_fn = nullptr;
};

struct hooks_t {
	bool on_attach( );

	inline void on_release( )
	{
		MH_DisableHook( MH_ALL_HOOKS );
		MH_RemoveHook( MH_ALL_HOOKS );
		MH_Uninitialize( );
	}

	template< typename T >
	static auto relative_to_absolute( const T& address ) noexcept
	{
		return address + 4 + *reinterpret_cast< int* >( address );
	}

	c_detour_hook alloc_key_values_memory          = { };
	c_detour_hook create_move_proxy                = { };
	c_detour_hook frame_stage_notify               = { };
	c_detour_hook on_add_entity                    = { };
	c_detour_hook on_remove_entity                 = { };
	c_detour_hook level_init_pre_entity            = { };
	c_detour_hook level_shutdown                   = { };
	c_detour_hook paint_traverse                   = { };
	c_detour_hook lock_cursor                      = { };
	c_detour_hook reset                            = { };
	c_detour_hook end_scene                        = { };
	c_detour_hook draw_print_text                  = { };
	c_detour_hook vsnprintf                        = { };
	c_detour_hook draw_model_execute               = { };
	c_detour_hook get_color_modulation             = { };
	c_detour_hook post_screen_effects              = { };
	c_detour_hook is_using_static_prop_debug_modes = { };
	c_detour_hook particle_collection_simulate     = { };
	c_detour_hook draw_view_model                  = { };
	c_detour_hook emit_sound                       = { };
	c_detour_hook push_notice                      = { };
	c_detour_hook override_mouse_input             = { };
	c_detour_hook get_vcollide                     = { };
	c_detour_hook override_view                    = { };
	c_detour_hook viewmodel_field_of_view          = { };
	c_detour_hook aspect_ratio                     = { };
	c_detour_hook is_connected                     = { };
	c_detour_hook is_hltv                          = { };
	c_detour_hook supports_full_depth              = { };
};

inline hooks_t hooks = { };

namespace n_detoured_functions
{
	void* __fastcall alloc_key_values_memory( c_key_values_system* thisptr, int edx, int size );
	void __fastcall create_move_proxy( void* thisptr, int edx, int sequence_number, float input_sample_frametime, bool is_active );
	void __fastcall frame_stage_notify( void* thisptr, int edx, e_client_frame_stage stage );
	void __fastcall on_add_entity( int ecx, int edx, void* handle_entity, unsigned int entity_handle );
	void __fastcall on_remove_entity( int ecx, int edx, void* handle_entity, unsigned int entity_handle );
	void __stdcall level_init_pre_entity( const char* map_name );
	void __fastcall level_shutdown( void* thisptr );
	void __fastcall paint_traverse( c_surface* thisptr, int edx, unsigned int panel, bool force_repaint, bool force );
	void __fastcall lock_cursor( void* thisptr, int edx );
	long __stdcall reset( IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* presentation_parameters );
	long __stdcall end_scene( IDirect3DDevice9* device );
	void __fastcall draw_print_text( int ecx, int edx, const wchar_t* text, int text_length, int draw_type );
	int __cdecl vsnprintf( char* dest, int text_length, const char* fmt, ... );
	void __fastcall draw_model_execute( uintptr_t ecx, uintptr_t edx, void* ctx, void* state, c_model_render_info* info, matrix3x4_t* matrix );
	void __fastcall get_color_modulation( void* ecx, void* edx, float* r, float* g, float* b );
	int __fastcall post_screen_effects( void* ecx, uintptr_t edx, int effect_id );
	bool __stdcall is_using_static_prop_debug_modes( );
	void __fastcall particle_collection_simulate( c_particle_collection* this_ptr, void* edx );
	void __fastcall draw_view_model( void* ecx, void* edx, c_view_setup& setup, bool draw_viewmodel, bool draw_scope_lens_mask );
	void __fastcall emit_sound( uintptr_t ecx, uintptr_t edx, void* filter, int index, int ichannel, const char* soundentry, unsigned int soundhash,
	                            const char* sample, float flvolume, int seed, float flattenuation, int iflags, int ipitch, const c_vector* origin,
	                            const c_vector* direction, void* utlvecorigins, bool updatepositions, float soundtime, int speakerentity, int unk );
	void __fastcall push_notice( int ecx, int edx, const char* text, int str_len, const char* null );
	void __fastcall override_mouse_input( int edx, int ecx, float* x, float* y );
	vcollide_t* __fastcall get_vcollide( void* edx, void* ecx, int vcollide_index );
	void __fastcall override_view( void* edx, void* ecx, c_view_setup* setup );
	float __stdcall viewmodel_field_of_view( );
	float __fastcall aspect_ratio( void* edx, void* ecx, int width, int height );
	bool __fastcall is_connected( void* edx, void* ecx );
	bool __fastcall is_hltv( void* ecx, void* edx );
	bool __stdcall supports_full_depth( );

	long __stdcall wndproc( HWND window, UINT msg, WPARAM wide_param, LPARAM long_param );
} // namespace n_detoured_functions
