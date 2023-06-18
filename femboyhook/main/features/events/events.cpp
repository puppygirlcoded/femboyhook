#include "events.h"
#include "../misc/misc.h"

event_listener_t event_listener;

void event_listener_t::on_attach( )
{
	debug_id = EVENT_DEBUG_ID_INIT;
	interfaces.m_event_manager->add_listener( this, xs( "player_hurt" ), false );
	interfaces.m_event_manager->add_listener( this, xs( "player_death" ), false );
}

void event_listener_t::on_release( )
{
	interfaces.m_event_manager->remove_listener( this );
}

void event_listener_t::fire_game_event( c_game_event* event )
{
	const auto event_name = event->get_name( );
	event->set_int( xs( "headshot" ), 1 );
	misc.on_fire_game_event( event );

	switch ( fnv1a::hash( event_name ) ) {
	case fnv1a::hash_const( "player_hurt" ):
		misc.on_player_hurt( event );
		break;
	case fnv1a::hash_const( "player_death" ):
		misc.on_player_death( event );
		break;
	}
}