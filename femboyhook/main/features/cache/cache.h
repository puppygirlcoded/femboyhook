#pragma once

#include <functional>
#include <vector>

#include "../../source_engine/classes/c_base_entity.h"

enum e_cache_types {
	cache_type_player = 0,
	cache_type_edicts
};

struct cache_t {
	cache_t( )
	{
		this->m_edicts.reserve( 2048 );
		this->m_players.reserve( 64 );
	}

	void on_add_entity( c_base_entity* entity );
	void on_remove_entity( c_base_entity* entity );

	void enumerate( e_cache_types type, const std::function< void( c_base_entity* ) >& function ) const;

	std::vector< c_base_entity* > m_edicts  = { };
	std::vector< c_base_entity* > m_players = { };
};

inline cache_t cache = { };