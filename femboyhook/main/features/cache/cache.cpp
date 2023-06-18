#include "cache.h"
#include "../../globals.h"
#include "../../source_engine/interfaces/interfaces.h"

void cache_t::on_add_entity( c_base_entity* entity )
{
	if ( !entity || entity->index( ) == interfaces.m_engine->get_local_player( ) )
		return;

	if ( !entity->is_player( ) ) {
		const auto it_found = std::ranges::find( this->m_edicts, entity );
		if ( it_found == this->m_edicts.cend( ) && entity->index( ) >= 1 && entity->index( ) <= 2048 )
			this->m_edicts.push_back( entity );
	} else {
		const auto it_found = std::ranges::find( this->m_players, entity );
		if ( it_found == this->m_players.cend( ) && entity->index( ) >= 1 && entity->index( ) <= 64 )
			this->m_players.push_back( entity );
	}
}

void cache_t::on_remove_entity( c_base_entity* entity )
{
	if ( !entity || entity->index( ) == interfaces.m_engine->get_local_player( ) )
		return;

	if ( !entity->is_player( ) ) {
		const auto it_found = std::ranges::find( this->m_edicts, entity );
		if ( it_found != this->m_edicts.cend( ) )
			this->m_edicts.erase( it_found );
	} else {
		const auto it_found = std::ranges::find( this->m_players, entity );
		if ( it_found != this->m_players.cend( ) )
			this->m_players.erase( it_found );
	}
}

void cache_t::enumerate( e_cache_types type, const std::function< void( c_base_entity* ) >& function ) const
{
	for ( const auto entity : type == e_cache_types::cache_type_edicts ? this->m_edicts : this->m_players ) {
		if ( entity )
			function( entity );
	}
}