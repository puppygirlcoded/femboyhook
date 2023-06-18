#pragma once

#include "render_wrapper.h"

namespace utils::input_wrapper
{

	using sdk::vector2;

	enum mouse_button {
		MOUSE_LEFT,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,
		MOUSE_SIDE1,
		MOUSE_SIDE2,
	};

	enum mouse_cursor {
		CURSOR_NONE  = -1,
		CURSOR_ARROW = 0,
		CURSOR_TEXT_INPUT,
		CURSOR_RESIZE_ALL,
		CURSOR_RESIZE_NS,
		CURSOR_RESIZE_EW,
		CURSOR_RESIZE_NESW,
		CURSOR_RESIZE_NWSE,
		CURSOR_HAND,
		CURSOR_NOT_ALLOWED,
	};

	bool is_ready( );
	bool setup( );
	bool undo( );

	void enable_raw_input( bool enabled );
	void enable_cursor( bool state );
	void set_cursor( int cursor );

	float get_mouse_wheel( );

	float get_avg_frame_time( );
	float get_last_frame_time( );

	vector2 get_mouse_pos( );
	vector2 get_mouse_delta( );

	bool is_hovering( const vector2& min, const vector2& max );

	bool is_mouse_clicked( int key, bool repeat = false );
	bool is_mouse_double_clicked( int key );
	bool is_mouse_down( int key );
	bool is_mouse_released( int key );

	int vk_to_mouse( int vk );

	bool is_key_pressed( int key, bool repeat = true );
	bool is_key_down( int key );
	bool is_key_released( int key );
	void update( const sdk::vector2f& raw_pos );
	sdk::vector2f get_raw_mouse_pos( );
} // namespace utils::input
