#pragma once

struct edicts_t {
	void on_frame_stage_notify( int frame_stage );
	void on_paint_traverse( );
};

inline edicts_t edicts = { };