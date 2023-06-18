#pragma once
#include <any>
#include <deque>
#include <filesystem>
#include <string>
#include <vector>

#include "../includes.h"

enum config_invoke_type_t {
	invoke_save,
	invoke_load
};

struct config_system_t {
	int m_selected_config = -1;

	std::vector< std::string > m_configs;

	void load_default( );

	void check_directory( );

	void fetch( );

	void save( const int index );

	void load( const int index );

	void create( const std::string& name );

	void invoke( config_invoke_type_t invoke_type );
};

inline config_system_t config_system = { };