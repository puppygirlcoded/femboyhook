#pragma once

#include "../../includes.h"

// this had to be a class to inherit c_game_event_listener :cry:

class event_listener_t : public i_game_event_listener2
{
public:
	void fire_game_event( c_game_event* event );
	void on_attach( );
	void on_release( );
};

extern event_listener_t event_listener;