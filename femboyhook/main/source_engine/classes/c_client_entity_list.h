#pragma once

#include <cstdint>

class c_client_networkable;
class c_client_unknown;
class c_client_entity;

class c_client_entity_list
{
public:
	virtual c_client_networkable* get_client_networkable( int index )                              = 0;
	virtual c_client_networkable* get_client_networkable_from_handle( std::uintptr_t networkable ) = 0;
	virtual c_client_unknown* get_client_unknown_from_handle( std::uintptr_t unknown )             = 0;
	virtual c_client_entity* get_client_entity( int index )                                        = 0;
	virtual c_client_entity* get_client_entity_from_handle( std::uintptr_t handle )                = 0;
	virtual int number_of_entities( bool include_non_networkable )                                 = 0;
	virtual int get_highest_entity_index( )                                                        = 0;
	virtual void set_max_entities( int max )                                                       = 0;
	virtual int get_max_entities( )                                                                = 0;
};
