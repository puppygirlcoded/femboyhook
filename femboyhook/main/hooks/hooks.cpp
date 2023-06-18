#include "hooks.h"

bool hooks_t::on_attach( )
{
	auto initialise_hook = []( c_detour_hook& detour_class, void* function, void* detour, const char* hook_name ) -> bool {
		if ( !detour_class.create( function, detour ) ) {
			console.print( xs( "failed to hook {:s} @ {:p}" ), hook_name, function );
			return false;
		} else
			console.print( xs( "{:s} hooked @ {:p}" ), hook_name, function );

		return true;
	};

	try {
		static const auto alloc_key_values_memory_address = utilities.get_virtual_function( memory.m_key_values_system, 2 );
		static const auto create_move_address             = utilities.get_virtual_function( interfaces.m_client, 22 );
		static const auto frame_stage_notify_address      = utilities.get_virtual_function( interfaces.m_client, 37 );
		static const auto on_add_entity_address =
			reinterpret_cast< void* >( memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 51 8B 45 0C 53 56 8B F1 57" ) ) );
		static const auto is_using_static_prop_debug_modes_address = reinterpret_cast< void* >(
			this->relative_to_absolute( memory.m_modules[ e_module_names::engine ].find_pattern( xs( "E8 ? ? ? ? 84 C0 8B 45 08" ) ) + 1 ) );
		static const auto particle_collection_simulate_address = reinterpret_cast< void* >(
			this->relative_to_absolute( memory.m_modules[ e_module_names::client ].find_pattern( xs( "E8 ? ? ? ? 8B 0E 83 C1 10" ) ) + 1 ) );
		static const auto on_remove_entity_address = reinterpret_cast< void* >(
			memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 51 8B 45 0C 53 8B D9 56 57 83 F8 FF 75 07" ) ) );
		static const auto get_color_modulation_address =
			reinterpret_cast< void* >( memory.m_modules[ e_module_names::materialsystem ].find_pattern( xs( "55 8B EC 83 EC ? 56 8B F1 8A 46" ) ) );
		static const auto level_init_pre_entity_address = utilities.get_virtual_function( interfaces.m_client, 5 );
		static const auto level_shutdown_address        = utilities.get_virtual_function( interfaces.m_client, 7 );
		static const auto paint_traverse_address        = utilities.get_virtual_function( interfaces.m_panel, 41 );
		static const auto emit_sound_address            = utilities.get_virtual_function( interfaces.m_engine_sound, 5 );
		static const auto lock_cursor_address           = utilities.get_virtual_function( interfaces.m_surface, 67 );
		static const auto reset_address                 = utilities.get_virtual_function( memory.m_device, 16 );
		static const auto end_scene_address             = utilities.get_virtual_function( memory.m_device, 42 );
		static const auto post_screen_effects_address   = utilities.get_virtual_function( interfaces.m_client_mode, 44 );
		static const auto draw_model_execute_address    = utilities.get_virtual_function( interfaces.m_model_render, 21 );
		static const auto vsnprint_address              = reinterpret_cast< void* >(
            memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 51 56 8B 75 0C 8D 45 14 57 8B 7D 08 8B D6 50 51 FF 75 10 8B CF E8 "
		                                                                                           "? ? ? ? 83 C4 0C 85 C0 78 08 85 F6 7E 0C 3B C6 7C 08 8D 46 FF" ) ) );
		static const auto draw_view_model_address = reinterpret_cast< void* >( memory.m_modules[ e_module_names::client ].find_pattern(
			xs( "53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 81 ? ? ? ? ? 56 57 FF" ) ) );
		static const auto push_notice_address     = reinterpret_cast< void* >(
            memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 83 E4 F8 B8 ? ? ? ? E8 ? ? ? ? 53 8B D9 8B 0D" ) ) );
		static const auto draw_print_text_address      = utilities.get_virtual_function( interfaces.m_surface, 28 );
		static const auto override_mouse_input_address = utilities.get_virtual_function( interfaces.m_client_mode, 23 );
		static const auto get_vcollide_address         = utilities.get_virtual_function( interfaces.m_model_info, 6 );
		static const auto override_view_address        = utilities.get_virtual_function( interfaces.m_client_mode, 18 );
		static const auto viewmodel_fov_address        = utilities.get_virtual_function( interfaces.m_client_mode, 35 );
		static const auto aspect_ratio_address         = utilities.get_virtual_function( interfaces.m_engine, 101 );
		static const auto is_connected_address         = utilities.get_virtual_function( interfaces.m_engine, 27 );
		static const auto is_hltv_address              = utilities.get_virtual_function( interfaces.m_engine, 93 );
		static const auto supports_full_depth_address  = reinterpret_cast< void* >(
            memory.m_modules[ e_module_names::shaderapidx9 ].find_pattern( xs( "A1 ? ? ? ? A8 ? 75 ? 83 C8 ? B9 ? ? ? ? 68 ? ? ? ? A3" ) ) );

		if ( MH_Initialize( ) != MH_OK ) {
			throw std::runtime_error( xs( "failed initialize minhook" ) );
			return false;
		}

		initialise_hook( hooks.alloc_key_values_memory, alloc_key_values_memory_address, &n_detoured_functions::alloc_key_values_memory,
		                 xs( "IKeyValuesSystem::AllocKeyValuesMemory()" ) );
		initialise_hook( hooks.create_move_proxy, create_move_address, &n_detoured_functions::create_move_proxy, xs( "CHLClient::CreateMove()" ) );
		initialise_hook( hooks.frame_stage_notify, frame_stage_notify_address, &n_detoured_functions::frame_stage_notify,
		                 xs( "CHLClient::FrameStageNotify()" ) );
		initialise_hook( hooks.on_add_entity, on_add_entity_address, &n_detoured_functions::on_add_entity, xs( "IClientEntityList::OnAddEntity()" ) );
		initialise_hook( hooks.on_remove_entity, on_remove_entity_address, &n_detoured_functions::on_remove_entity,
		                 xs( "IClientEntityList::OnRemoveEntity()" ) );
		initialise_hook( hooks.level_init_pre_entity, level_init_pre_entity_address, &n_detoured_functions::level_init_pre_entity,
		                 xs( "CHLClient::LevelInitPreEntity()" ) );
		initialise_hook( hooks.level_shutdown, level_shutdown_address, &n_detoured_functions::level_shutdown, xs( "CHLClient::LevelShutdown()" ) );
		initialise_hook( hooks.paint_traverse, paint_traverse_address, &n_detoured_functions::paint_traverse, xs( "IPanel::PaintTraverse()" ) );
		initialise_hook( hooks.lock_cursor, lock_cursor_address, &n_detoured_functions::lock_cursor, xs( "ISurface::LockCursor()" ) );
		initialise_hook( hooks.reset, reset_address, &n_detoured_functions::reset, xs( "IDirect3DDevice9::Reset()" ) );
		initialise_hook( hooks.end_scene, end_scene_address, &n_detoured_functions::end_scene, xs( "IDirect3DDevice9::EndScene()" ) );
		initialise_hook( hooks.vsnprintf, vsnprint_address, &n_detoured_functions::vsnprintf, xs( "vsnprintf" ) );
		initialise_hook( hooks.draw_print_text, draw_print_text_address, &n_detoured_functions::draw_print_text, xs( "ISurface::DrawPrintText()" ) );
		initialise_hook( hooks.draw_model_execute, draw_model_execute_address, &n_detoured_functions::draw_model_execute,
		                 xs( "IModelRenderInfo::DrawModelExecute()" ) );
		initialise_hook( hooks.get_color_modulation, get_color_modulation_address, &n_detoured_functions::get_color_modulation,
		                 xs( "IMaterialSystem::GetColorModulation()" ) );
		initialise_hook( hooks.post_screen_effects, post_screen_effects_address, &n_detoured_functions::post_screen_effects,
		                 xs( "ClientMode::DoPostScreenEffects()" ) );
		initialise_hook( hooks.is_using_static_prop_debug_modes, is_using_static_prop_debug_modes_address,
		                 &n_detoured_functions::is_using_static_prop_debug_modes, xs( "EngineClient::IsUsingStaticPropDebugModes()" ) );
		initialise_hook( hooks.particle_collection_simulate, particle_collection_simulate_address,
		                 &n_detoured_functions::particle_collection_simulate, xs( "CHLClient::ParticleCollectionSimulate()" ) );
		initialise_hook( hooks.draw_view_model, draw_view_model_address, &n_detoured_functions::draw_view_model, xs( "CHLClient::DrawViewModel()" ) );
		initialise_hook( hooks.emit_sound, emit_sound_address, &n_detoured_functions::emit_sound, xs( "EngineClient::EmitSound()" ) );
		initialise_hook( hooks.push_notice, push_notice_address, &n_detoured_functions::push_notice, xs( "CHLClient::PushNotice()" ) );
		initialise_hook( hooks.override_mouse_input, override_mouse_input_address, &n_detoured_functions::override_mouse_input,
		                 xs( "ClientMode::OverrideMouseInput()" ) );
		initialise_hook( hooks.get_vcollide, get_vcollide_address, &n_detoured_functions::get_vcollide, xs( "CModelInfo::GetVCollide()" ) );
		initialise_hook( hooks.override_view, override_view_address, &n_detoured_functions::override_view, xs( "OverrideView()" ) );
		initialise_hook( hooks.viewmodel_field_of_view, viewmodel_fov_address, &n_detoured_functions::viewmodel_field_of_view,
		                 xs( "ViewmodelFieldOfView()" ) );
		initialise_hook( hooks.aspect_ratio, aspect_ratio_address, &n_detoured_functions::aspect_ratio, xs( "AspectRatio()" ) );
		initialise_hook( hooks.is_connected, is_connected_address, &n_detoured_functions::is_connected, xs( "IVEngineClient::IsConnected()" ) );
		initialise_hook( hooks.is_hltv, is_hltv_address, &n_detoured_functions::is_hltv, xs( "IVEngineClient::IsHLTV()" ) );
		initialise_hook( hooks.supports_full_depth, supports_full_depth_address, &n_detoured_functions::supports_full_depth,
		                 xs( "SupportsResolveDepth()" ) );

		if ( interfaces.m_engine->is_in_game( ) )
			memory.m_client_state->m_delta_tick = -1;

		return true;
	} catch ( const std::exception& ex ) {
		console.print( xs( "{:s}" ), ex.what( ) );

#ifdef _DEBUG
		_RPT0( _CRT_ERROR, ex.what( ) );
#else
		std::abort( );
#endif
	}

	return false;
}