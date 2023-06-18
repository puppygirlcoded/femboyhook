#pragma once

#include "gui/render_wrapper.h"

struct menu_t {
	void handle_spotify( );
	void on_end_scene( );

	std::string current_spotify_song;

	bool m_opened = true;

	sdk::color accent = { 255, 31, 131 };
};

inline menu_t menu = { };