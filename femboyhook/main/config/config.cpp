//#include "config.h"
//#include <nlohmann/json.hpp>
//
//static const std::filesystem::path config_path = xs( "femboyhook" );
//
//template< typename T >
//static T read_value( const nlohmann::json& src )
//{
//	if ( src.is_null( ) )
//		return T{ };
//
//	return src.get< T >( );
//}
//
//template< typename T >
//static void write_value( nlohmann::json& dest, const T& src )
//{
//	dest = src;
//}
//
//static void write_keybind( nlohmann::json& dest, const std::string& name, const key_bind_t& src )
//{
//	dest[ std::vformat( xs( "{}:key" ), std::make_format_args( name ) ) ]          = src.key;
//	dest[ std::vformat( xs( "{}:mode" ), std::make_format_args( name ) ) ]         = src.mode;
//	dest[ std::vformat( xs( "{}:toggle_state" ), std::make_format_args( name ) ) ] = src.toggle_state;
//}
//
//void load( const int index )
//{
//	nlohmann::json json;
//}
//
//void save( const int index )
//{
//	nlohmann::json json;
//
//	const auto aimbot  = xs( "aimbot" );
//	const auto general = xs( "general" );
//	write_value( json[ aimbot ][ xs( "enabled" ) ], variables.aimbot.enabled );
//	write_value( json[ aimbot ][ xs( "target_team" ) ], variables.aimbot.target_team );
//	write_value( json[ aimbot ][ xs( "target_selection" ) ], variables.aimbot.target_selection );
//	write_keybind( json[ aimbot ], xs( "activation_key" ), variables.aimbot.activation_key );
//
//	/* aimbot -> general */
//	[ & ] {
//		write_value( json[ aimbot ][ general ][ xs( "override_general" ) ], variables.aimbot.general.override_general );
//		write_value( json[ aimbot ][ general ][ xs( "silent" ) ], variables.aimbot.general.silent );
//		write_value( json[ aimbot ][ general ][ xs( "field_of_view" ) ], variables.aimbot.general.field_of_view );
//		write_value( json[ aimbot ][ general ][ xs( "compensate_recoil" ) ], variables.aimbot.general.compensate_recoil );
//		write_value( json[ aimbot ][ general ][ xs( "recoil_compensation:m_x" ) ], variables.aimbot.general.recoil_compensation.m_x );
//		write_value( json[ aimbot ][ general ][ xs( "recoil_compensation:m_y" ) ], variables.aimbot.general.recoil_compensation.m_y );
//		write_value( json[ aimbot ][ general ][ xs( "smoothness" ) ], variables.aimbot.general.smoothness );
//		write_value( json[ aimbot ][ general ][ xs( "target_visible_only" ) ], variables.aimbot.general.target_visible_only );
//		write_value( json[ aimbot ][ general ][ xs( "target_through_smoke" ) ], variables.aimbot.general.target_through_smoke );
//		write_value( json[ aimbot ][ general ][ xs( "target_enemy_in_air" ) ], variables.aimbot.general.target_enemy_in_air );
//		write_value( json[ aimbot ][ general ][ xs( "target_while_in_air" ) ], variables.aimbot.general.target_while_in_air );
//		write_value( json[ aimbot ][ general ][ xs( "target_while_flashed" ) ], variables.aimbot.general.target_while_flashed );
//		write_value( json[ aimbot ][ general ][ xs( "hitboxes" ) ], variables.aimbot.general.hitboxes );
//
//		const auto compensation = xs( "compensation" );
//		write_value( json[ aimbot ][ general ][ compensation ][ xs( "enabled" ) ], variables.aimbot.general.compensation.enabled );
//		write_value( json[ aimbot ][ general ][ compensation ][ xs( "silent" ) ], variables.aimbot.general.compensation.silent );
//		write_value( json[ aimbot ][ general ][ compensation ][ xs( "recoil_compensation:m_x" ) ],
//		             variables.aimbot.general.compensation.recoil_compensation.m_x );
//		write_value( json[ aimbot ][ general ][ compensation ][ xs( "recoil_compensation:m_y" ) ],
//		             variables.aimbot.general.compensation.recoil_compensation.m_y );
//	}( );
//}
//
//void config_system_t::check_directory( )
//{
//	if ( !std::filesystem::exists( config_path ) ) {
//		std::filesystem::create_directory( config_path );
//	} else {
//		this->fetch( );
//	}
//}
//
//void config_system_t::fetch( )
//{
//	this->m_configs.clear( );
//
//	for ( const auto& entry : std::filesystem::directory_iterator{ config_path } ) {
//		if ( !entry.is_regular_file( ) || entry.path( ).extension( ) != xs( ".json" ) )
//			continue;
//
//		const auto& file_name = entry.path( ).stem( ).string( );
//		this->m_configs.emplace_back( file_name );
//	}
//}
//void config_system_t::invoke( config_invoke_type_t invoke_type )
//{
//	switch ( invoke_type ) {
//	case config_invoke_type_t::invoke_save:
//		try {
//			save( this->m_selected_config );
//		} catch ( const std::exception& ex ) {
//			interfaces.m_engine->execute_command( std::vformat( xs( "error saving config: {}" ), std::make_format_args( ex.what( ) ) ).data( ) );
//		}
//		break;
//	case config_invoke_type_t::invoke_load:
//		try {
//			load( this->m_selected_config );
//		} catch ( const std::exception& ex ) {
//			interfaces.m_engine->execute_command( std::vformat( xs( "error loading config: {}" ), std::make_format_args( ex.what( ) ) ).data( ) );
//		}
//		break;
//	}
//}