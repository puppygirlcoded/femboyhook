#include "../hooks.h"

#include "../../features/cache/cache.h"

void __stdcall n_detoured_functions::level_init_pre_entity( const char* map_name )
{
	static auto original = hooks.level_init_pre_entity.get_original< decltype( &n_detoured_functions::level_init_pre_entity ) >( );

	original( map_name );

	/* clearing cached edicts / players */
	[ & ]( ) {
		if ( !cache.m_edicts.empty( ) )
			cache.m_edicts.clear( );

		if ( !cache.m_players.empty( ) )
			cache.m_players.clear( );
	}( );
}