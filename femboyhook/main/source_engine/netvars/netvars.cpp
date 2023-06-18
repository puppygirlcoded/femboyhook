#include "netvars.h"

#include "../../includes.h"

bool netvars_t::on_attach( )
{
	m_props.clear( );

	for ( auto client_class = interfaces.m_client->get_all_classes( ); client_class; client_class = client_class->m_next ) {
		if ( !client_class->m_recv_table )
			continue;

		this->store_props( client_class->m_network_name, client_class->m_recv_table, 0U );
	}

	return !this->m_props.empty( );
}

void netvars_t::store_props( const char* class_name, recv_table_t* recv_table, const std::uintptr_t offset )
{
	const unsigned int class_hash      = fnv1a::hash( class_name );
	const unsigned int split_hash      = fnv1a::hash( "->", class_hash );
	const unsigned int base_class_hash = fnv1a::hash( "baseclass", split_hash );

	for ( int i = { }; i < recv_table->m_props_count; ++i ) {
		const auto current_prop = &recv_table->m_props[ i ];
		if ( !current_prop )
			continue;

		const unsigned int total_hash = fnv1a::hash( current_prop->m_var_name, split_hash );
		if ( total_hash == base_class_hash )
			continue;

		/* NOTE ~ float ~ prints the class name and the variable name, example "CBasePlayer->m_iHealth" */
		/* console.print( "{:s}->{:s}", class_name, current_prop->m_var_name ); */

		if ( isdigit( current_prop->m_var_name[ 0 ] ) )
			continue;

		if ( const auto child_table = current_prop->m_data_table; child_table && child_table->m_props_count > 0 &&
		                                                          child_table->m_net_table_name[ 0 ] == 'D' &&
		                                                          current_prop->m_recv_type == e_send_prop_type::dpt_datatable ) {
			this->store_props( class_name, child_table, static_cast< std::uintptr_t >( current_prop->m_offset ) + offset );
		}

		const std::uintptr_t total_offset = static_cast< std::uintptr_t >( current_prop->m_offset ) + offset;
		if ( !this->m_props[ total_hash ].m_offset )
			this->m_props[ total_hash ] = { current_prop, total_offset };
	}
}

std::uintptr_t netvars_t::find_in_data_map( data_map_t* map, const unsigned int field_hash )
{
	while ( map ) {
		for ( int i = 0; i < map->m_data_fields; i++ ) {
			if ( !map->m_data_desc[ i ].m_field_name )
				continue;

			if ( fnv1a::hash( map->m_data_desc[ i ].m_field_name ) == field_hash )
				return map->m_data_desc[ i ].m_field_offset[ td_offset_normal ];

			if ( map->m_data_desc[ i ].m_field_type == e_field_types::field_embedded ) {
				if ( map->m_data_desc[ i ].m_type_description ) {
					if ( const auto offset = this->find_in_data_map( map->m_data_desc[ i ].m_type_description, field_hash ); offset != 0U )
						return offset;
				}
			}
		}

		map = map->m_base_map;
	}

	return 0U;
}

int netvars_t::get_prop( recv_table_t* recvTable, const char* propName, recv_prop_t** prop )
{
	int extraOffset = 0;

	for ( auto i = 0; i < recvTable->m_props_count; i++ ) {
		const auto recvProp = &recvTable->m_props[ i ];
		const auto child    = recvProp->m_data_table;

		if ( child && ( child->m_props_count > 0 ) ) {
			int tmp = get_prop( child, propName, prop );
			if ( tmp )
				extraOffset += ( recvProp->m_offset + tmp );
		}

		if ( strcmp( recvProp->m_var_name, propName ) )
			continue;

		if ( prop )
			*prop = recvProp;

		return ( recvProp->m_offset + extraOffset );
	}

	return extraOffset;
}