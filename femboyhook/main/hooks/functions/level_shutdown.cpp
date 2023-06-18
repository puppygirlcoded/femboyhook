#include "../hooks.h"

#include "../../features/cache/cache.h"

void __fastcall n_detoured_functions::level_shutdown( void* thisptr )
{
	static auto original = hooks.level_shutdown.get_original< decltype( &n_detoured_functions::level_shutdown ) >( );

	/* clearing cached edicts / players */
	[ & ]( ) {
		if ( !cache.m_edicts.empty( ) )
			cache.m_edicts.clear( );

		if ( !cache.m_players.empty( ) )
			cache.m_players.clear( );
	}( );

	original( thisptr );
}