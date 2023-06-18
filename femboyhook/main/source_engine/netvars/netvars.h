#pragma once
#include <unordered_map>

#include "../../hashing/strings/fnv1a.h"
#include "../datatypes/datamap.h"
#include "../datatypes/datatable.h"

template< typename T >
constexpr T get_vfunc( void* class_base, int index )
{
	return ( *( T** )class_base )[ index ];
}

#define add_variable_offset( type, function_name, netvar, additional )                                                                         \
	[[nodiscard]] std::add_lvalue_reference_t< type > function_name( )                                                                              \
	{                                                                                                                                                \
		static unsigned int hashed_value = fnv1a::hash_const( netvar );                                                                               \
		static std::uintptr_t offset  = netvars.m_props[ hashed_value ].m_offset;                                                           \
		return *( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset + additional );                                \
	}

#define add_variable( type, function_name, netvar ) add_variable_offset( type, function_name, netvar, 0U )

#define add_pvariable_offset( type, function_name, netvar, additional )                                                                        \
	[[nodiscard]] std::add_pointer_t< type > function_name( )                                                                                       \
	{                                                                                                                                                \
		static constexpr unsigned int hashed_value = fnv1a::hash_const( netvar );                                                                               \
		static std::uintptr_t offset  = netvars.m_props[ hashed_value ].m_offset;                                                           \
		return ( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset + additional );                                 \
	}

#define add_pvariable( type, function_name, netvar ) add_pvariable_offset( type, function_name, netvar, 0U )

#define add_resource_variable( type, function_name, netvar )                                                                                    \
	[[nodiscard]] std::add_lvalue_reference_t< type > function_name( int nIndex )                                                                   \
	{                                                                                                                                                \
		static constexpr unsigned int hashed_value = fnv1a::hash_const( netvar );                                                                               \
		static std::uintptr_t offset  = netvars.m_props[ hashed_value ].m_offset;                                                           \
		return *( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset + nIndex * sizeof( type ) );                    \
	}

#define add_datafield( type, function_name, data_map, data_field )                                                                                   \
	[[nodiscard]] std::add_lvalue_reference_t< type > function_name( )                                                                              \
	{                                                                                                                                                \
		static constexpr unsigned int hashed_value = fnv1a::hash_const( data_field );                                                                            \
		static std::uintptr_t offset  = netvars.find_in_data_map( data_map, hashed_value );                                                        \
		return *( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset );                                              \
	}

#define add_pdatafield( type, function_name, data_map, data_field )                                                                                  \
	[[nodiscard]] std::add_pointer_t< type > function_name( )                                                                                       \
	{                                                                                                                                                \
		static constexpr unsigned int hashed_value = fnv1a::hash_const( data_field );                                                                            \
		static std::uintptr_t offset  = netvars.find_in_data_map( data_map, hashed_value );                                                        \
		return ( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset );                                               \
	}

#define add_offset( type, function_name, offset )                                                                                                \
	[[nodiscard]] std::add_lvalue_reference_t< type > function_name( )                                                                              \
	{                                                                                                                                                \
		return *( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset );                                              \
	}

#define add_poffset( type, function_name, offset )                                                                                               \
	[[nodiscard]] std::add_pointer_t< type > function_name( )                                                                                       \
	{                                                                                                                                                \
		return ( std::add_pointer_t< type > )( reinterpret_cast< std::uintptr_t >( this ) + offset );                                               \
	}
#define MEMBER_FUNC_ARGS( ... )                                                                                                                      \
	( this, __VA_ARGS__ );                                                                                                                           \
	}
#define add_virtual_function( i, f, s )                                                                                                                     \
	auto f                                                                                                                                           \
	{                                                                                                                                                \
		return get_vfunc< s >( this, i ) MEMBER_FUNC_ARGS

struct netvars_t {
	struct netvar_object_t {
		recv_prop_t* m_recv_prop = nullptr;
		unsigned int m_offset    = 0U;
	};

	bool on_attach( );

	unsigned int find_in_data_map( data_map_t* map, const unsigned int field_hash );

	int get_prop( recv_table_t* recvTable, const char* propName, recv_prop_t** prop );

	std::unordered_map< unsigned int, netvar_object_t > m_props = { };
private:
	void store_props( const char* class_name, recv_table_t* recv_table, const std::uintptr_t offset );
};

inline netvars_t netvars = { };