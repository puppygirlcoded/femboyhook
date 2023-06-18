#include "../../includes.h"
#include "../steam_sdk/steam_api.h"

template< typename T >
T* interfaces_t::find( const void* module_handle, const std::string_view interface_name )
{
	const auto get_register_list = [ & ]( ) -> c_interface_register* {
		const void* create_interface_export = utilities.get_export_address( module_handle, xs( "CreateInterface" ) );

		if ( !create_interface_export )
			throw std::runtime_error( xs( "failed get createinterface address" ) );

		const std::uintptr_t create_interface_relative = reinterpret_cast< std::uintptr_t >( create_interface_export ) + 0x5;
		const std::uintptr_t create_interface = create_interface_relative + 4U + *reinterpret_cast< std::int32_t* >( create_interface_relative );
		return **reinterpret_cast< c_interface_register*** >( create_interface + 0x6 );
	};

	for ( auto register_data = get_register_list( ); register_data; register_data = register_data->m_next ) {
		if ( ( std::string_view( register_data->m_name ).compare( 0U, interface_name.length( ), interface_name ) == 0 &&
		       std::atoi( register_data->m_name + interface_name.length( ) ) > 0 ) ||
		     interface_name.compare( register_data->m_name ) == 0 ) {
			auto interface_address = register_data->m_create_fn( );

			console.print( xs( "found {:s} interface @ {:p}" ), register_data->m_name, interface_address );

			return static_cast< T* >( interface_address );
		}
	}

	console.print( xs( "failed to capture interface {:s}" ), interface_name );

	return nullptr;
}

bool interfaces_t::on_attach( )
{
	if ( ( this->m_client = this->find< c_client >( memory.m_modules[ e_module_names::client ].m_value, xs( "VClient" ) ) ) == nullptr )
		return false;

	if ( ( this->m_client_mode =
	           **reinterpret_cast< c_globals*** >( utilities.get_virtual_function< std::uintptr_t >( interfaces.m_client, 10 ) + 0x5 ) ) == nullptr )
		return false;

	// TODO: clean this shit up LOL
	this->m_hud_chat = *reinterpret_cast< c_hud_chat** >(
		reinterpret_cast< uintptr_t >( this->m_client_mode ) +
		*reinterpret_cast< uint8_t* >( memory.m_modules[ e_module_names::client ].find_pattern( xs( "E8 ? ? ? ? 8B 4F ? 85 C9 74 06 51" ) ) + 7 ) );
	if ( !this->m_hud_chat )
		return false;

	if ( ( this->m_engine = this->find< c_engine >( memory.m_modules[ e_module_names::engine ].m_value, xs( "VEngineClient" ) ) ) == nullptr )
		return false;

	if ( ( this->m_surface = this->find< c_surface >( memory.m_modules[ e_module_names::vguimatsurface ].m_value, xs( "VGUI_Surface" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_input_system =
	           this->find< c_input_system >( memory.m_modules[ e_module_names::inputsystem ].m_value, xs( "InputSystemVersion" ) ) ) == nullptr )
		return false;

	if ( ( this->m_engine_sound = this->find< void >( memory.m_modules[ e_module_names::engine ].m_value, xs( "IEngineSoundClient" ) ) ) == nullptr )
		return false;

	if ( ( this->m_client_entity_list =
	           this->find< c_client_entity_list >( memory.m_modules[ e_module_names::client ].m_value, xs( "VClientEntityList" ) ) ) == nullptr )
		return false;

	if ( ( this->m_panel = this->find< c_panel >( memory.m_modules[ e_module_names::vgui2 ].m_value, xs( "VGUI_Panel" ) ) ) == nullptr )
		return false;

	if ( ( this->m_material_system =
	           this->find< c_material_system >( memory.m_modules[ e_module_names::materialsystem ].m_value, xs( "VMaterialSystem" ) ) ) == nullptr )
		return false;

	if ( ( this->m_model_info = this->find< c_model_info >( memory.m_modules[ e_module_names::engine ].m_value, xs( "VModelInfoClient" ) ) ) ==
	     nullptr )
		return false;
	if ( ( this->m_event_manager =
	           this->find< i_game_event_manager2 >( memory.m_modules[ e_module_names::engine ].m_value, xs( "GAMEEVENTSMANAGER002" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_model_render = this->find< c_model_render >( memory.m_modules[ e_module_names::engine ].m_value, xs( "VEngineModel" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_console = this->find< c_console >( memory.m_modules[ e_module_names::vstdlib ].m_value, xs( "VEngineCvar" ) ) ) == nullptr )
		return false;

	if ( ( this->m_filesystem = this->find< c_file_system >( memory.m_modules[ e_module_names::filesystem ].m_value, xs( "VBaseFileSystem" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_localize = this->find< c_localize >( memory.m_modules[ e_module_names::localize ].m_value, xs( "Localize_" ) ) ) == nullptr )
		return false;

	if ( ( this->m_trace = this->find< c_engine_trace >( memory.m_modules[ e_module_names::engine ].m_value, xs( "EngineTraceClient" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_prediction = this->find< c_prediction >( memory.m_modules[ e_module_names::client ].m_value, xs( "VClientPrediction" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_game_movement = this->find< c_game_movement >( memory.m_modules[ e_module_names::client ].m_value, xs( "GameMovement001" ) ) ) ==
	     nullptr )
		return false;

	if ( ( this->m_model_cache = this->find< void >( memory.m_modules[ e_module_names::data_cache ].m_value, xs( "MDLCache" ) ) ) == nullptr )
		return false;

	if ( ( this->m_physics_collision =
	           this->find< c_physics_collision >( memory.m_modules[ e_module_names::physics ].m_value, xs( "VPhysicsCollision" ) ) ) == nullptr )
		return false;
	
	if ( ( this->m_effects =
	           this->find< c_effects >( memory.m_modules[ e_module_names::client ].m_value, xs( "IEffects" ) ) ) == nullptr )
		return false;

	if ( ( this->m_move_helper = **reinterpret_cast< c_move_helper*** >(
			   memory.m_modules[ e_module_names::client ].find_pattern( xs( "8B 0D ? ? ? ? 8B 46 08 68" ) ) + 2 ) ) == nullptr )
		return false;

	if ( ( this->m_glow_manager = reinterpret_cast< c_glow_manager* >( *reinterpret_cast< uintptr_t* >(
			   memory.m_modules[ e_module_names::client ].find_pattern( xs( "0F 11 05 ? ? ? ? 83 C8 01" ) ) + 3 ) ) ) == nullptr )
		return false;

	uintptr_t hSteamUser = ( ( uintptr_t( __cdecl* )( void ) )GetProcAddress( GetModuleHandle( xs("steam_api.dll") ), xs("SteamAPI_GetHSteamUser") ) )( );
	uintptr_t hSteamPipe = ( ( uintptr_t( __cdecl* )( void ) )GetProcAddress( GetModuleHandle( xs("steam_api.dll") ), xs("SteamAPI_GetHSteamPipe") ) )( );

	SteamClient          = ( ( ISteamClient * ( __cdecl* )( void )) GetProcAddress( GetModuleHandle( xs("steam_api.dll") ), xs("SteamClient") ) )( );
	SteamGameCoordinator = ( ISteamGameCoordinator* )SteamClient->GetISteamGenericInterface( hSteamUser, hSteamPipe, xs("SteamGameCoordinator001") );
	SteamUser            = ( ISteamUser* )SteamClient->GetISteamUser( hSteamUser, hSteamPipe, xs("SteamUser019") );
	SteamFriends         = SteamClient->GetISteamFriends( hSteamUser, hSteamPipe, xs("SteamFriends015") );
	SteamUtils           = SteamClient->GetISteamUtils( hSteamPipe, xs("SteamUtils009") );

	return true;
}