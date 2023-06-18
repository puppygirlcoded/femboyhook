#include "../hooks.h"
#include "../../features/cache/cache.h"

void __fastcall n_detoured_functions::on_add_entity( int ecx, int edx, void* handle_entity, unsigned int entity_handle )
{
	static auto original = hooks.on_add_entity.get_original< decltype( &n_detoured_functions::on_add_entity ) >( );

	cache.on_add_entity( reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity_from_handle( entity_handle ) ) );

	original( ecx, edx, handle_entity, entity_handle );
}