#pragma once

struct IDirect3DDevice9;
struct ImDrawList;
struct ImVec2;
struct ImFont;

#include "../../source_engine/classes/c_color.h"
#include "../../source_engine/classes/c_vector.h"
#include "../../source_engine/structs/matrix_t.h"

#include <imgui.h>

#include <any>
#include <deque>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <d3d9.h>

enum e_text_render_flags {
	text_render_flag_none       = 0,
	text_render_flag_dropshadow = 1,
	text_render_flag_outline    = 2
};

enum e_rect_render_flags {
	rect_render_flag_none = 0,
	rect_render_flag_inner_outline,
	rect_render_flag_outer_outline,
	rect_render_flag_full_outline
};

enum e_draw_type {
	draw_type_none = 0,
	draw_type_text,
	draw_type_rect,
	draw_type_rect_filled,
	draw_type_line,
	draw_type_texture,
	draw_type_gradient
};

struct draw_object_t {
	draw_object_t( const e_draw_type type, std::any&& obj ) : m_type( type ), m_obj( std::move( obj ) ) { }

	e_draw_type m_type = e_draw_type::draw_type_none;
	std::any m_obj     = { };
};

struct text_draw_object_t {
	ImFont* m_font                   = { };
	c_vector_2d m_position           = { };
	std::string m_text               = { };
	c_color m_color                  = { };
	c_color m_outline_color          = { };
	e_text_render_flags m_draw_flags = e_text_render_flags::text_render_flag_none;
};

struct rect_draw_object_t {
	c_vector_2d m_position           = { };
	c_vector_2d m_size               = { };
	c_color m_color                  = { };
	float m_rounding                 = 0.0f;
	int m_corners                    = 0;
	float m_thickness                = 1.0f;
	e_rect_render_flags m_draw_flags = e_rect_render_flags::rect_render_flag_none;
	c_color m_outline_color          = c_color( 0, 0, 0, 255 );
};

struct grad_rect_draw_object_t {
	c_vector_2d m_position { };
	c_vector_2d m_size = { };
	c_color m_color1    = { };
	c_color m_color2    = { };
	bool horizontal     = true;
};

struct line_draw_object_t {
	c_vector_2d m_start = { };
	c_vector_2d m_end   = { };
	c_color m_color     = { };
	float m_thickness   = 1.0f;
};

struct rect_filled_draw_object_t {
	c_vector_2d m_position = { };
	c_vector_2d m_size     = { };
	c_color m_color        = { };
	float m_rounding       = 0.0f;
	int m_draw_flags         = 0;
};

enum e_indicator_fonts {
	e_ink_free,
	e_verdana,
	e_tahoma,

	e_ind_font_max
};

struct texture_draw_object_t {
	c_vector_2d m_position = { };
	c_vector_2d m_size = { };
	c_color m_color = { };
	ImTextureID m_texture_id = { };
};

struct render_t {
	void clear_draw_data( )
	{
		if ( !this->m_draw_data.empty( ) )
			this->m_draw_data.clear( );
	}

	void on_end_scene( const std::function< void( ) >& function, IDirect3DDevice9* device );

	void swap_draw_data( )
	{
		std::unique_lock< std::shared_mutex > lock( this->m_mutex );

		this->m_draw_data.swap( this->m_thread_safe_draw_data );
	}

	void on_release( );

	void gradient_line( ImDrawList* draw_list, const c_vector_2d& p_min, const c_vector_2d& p_max, c_color color );

	void gradient_rect_filled( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& first_color,
	                           const c_color& second_color, bool horizontal = true );

	void rect_filled( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& color, float rounding = 0.0f,
	                  int draw_flags = 0 );

	void texture( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& color, ImTextureID texture_id );

	void circle_filled( ImDrawList* draw_list, const c_vector_2d& anchor, float radius, const c_color& color );

	void rect( ImDrawList* draw_list, const c_vector_2d& position, const c_vector_2d& size, const c_color& color, float rounding = 0.0f,
	           int corners = 0, float thickness = 1.0f, e_rect_render_flags draw_flags = e_rect_render_flags::rect_render_flag_none,
	           const c_color& outline_color = c_color( 0, 0, 0, 255 ) );

	void line( ImDrawList* draw_list, const c_vector_2d& start, const c_vector_2d& end, const c_color& color, float thickness = 1.0f );

	void text( ImDrawList* draw_list, ImFont* font, const c_vector_2d& position, const std::string& text, const c_color& color,
	           const c_color& outline_color, e_text_render_flags draw_flags = e_text_render_flags::text_render_flag_none );

	c_vector_2d measure_text( ImFont* font, const char* text );

	void circle( ImDrawList* draw_list, const c_vector_2d& anchor, float radius, const c_color& color, float thickness );

	void circle_filled_3d( const c_vector& origin, float radius, c_color color );

	int get_layer( );

	void set_layer( int layer );

	void set_clip( ImDrawList* draw_list, const c_vector_2d& min, const c_vector_2d& max, bool intersect = true );

	void set_clip_fullscreen( ImDrawList* draw_list );

	void pop_clip( ImDrawList* draw_list );

	void reset_clip( ImDrawList* draw_list );

	bool world_to_screen( const c_vector& origin, ImVec2& screen );

	void update_world_to_screen_matrix( );

	bool m_initialised = false;

	ImFont* m_segoe_ui = { };
	ImFont* m_indicator_fonts[ e_ind_font_max ];
	ImFont* m_flags_font = { };

	std::deque< draw_object_t > m_draw_data             = { };
	std::deque< draw_object_t > m_thread_safe_draw_data = { };
	std::shared_mutex m_mutex                           = { };
	IDirect3DDevice9* m_device                          = nullptr;


	const view_matrix_t& get_world_to_screen_matrix( );
	IDirect3DTexture9* rasterize_vector( char* data, const float scale );
};

inline render_t render = { };