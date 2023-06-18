#pragma once

#include "../classes/c_client.h"
#include "../classes/c_client_entity_list.h"
#include "../classes/c_console.h"
#include "../classes/c_effects.h"
#include "../classes/c_engine.h"
#include "../classes/c_file_system.h"
#include "../classes/c_game_event_manager.h"
#include "../classes/c_glow_manager.h"
#include "../classes/c_hud_chat.h"
#include "../classes/c_input_system.h"
#include "../classes/c_localize.h"
#include "../classes/c_material_system.h"
#include "../classes/c_model_info.h"
#include "../classes/c_panel.h"
#include "../classes/c_physics_collision.h"
#include "../classes/c_prediction.h"
#include "../classes/c_surface.h"
#include "../classes/c_trace.h"

#include <string_view>

using instantiate_interface_fn = void*( __cdecl* )( );

class c_interface_register
{
public:
	instantiate_interface_fn m_create_fn = { };
	const char* m_name                   = { };
	c_interface_register* m_next         = { };
};

struct interfaces_t {
	bool on_attach( );

	template< typename T >
	T* find( const void* module_handle, const std::string_view interface_name );

	c_client* m_client                         = nullptr;
	void* m_client_mode                        = nullptr;
	c_engine* m_engine                         = nullptr;
	c_surface* m_surface                       = nullptr;
	c_input_system* m_input_system             = nullptr;
	c_client_entity_list* m_client_entity_list = nullptr;
	c_localize* m_localize                     = nullptr;
	c_panel* m_panel                           = nullptr;
	c_material_system* m_material_system       = nullptr;
	c_model_info* m_model_info                 = nullptr;
	c_model_render* m_model_render             = nullptr;
	c_console* m_console                       = nullptr;
	c_file_system* m_filesystem                = nullptr;
	c_engine_trace* m_trace                    = nullptr;
	c_glow_manager* m_glow_manager             = nullptr;
	c_prediction* m_prediction                 = nullptr;
	c_game_movement* m_game_movement           = nullptr;
	c_move_helper* m_move_helper               = nullptr;
	void* m_model_cache                        = nullptr;
	void* m_engine_sound                       = nullptr;
	c_hud_chat* m_hud_chat                     = nullptr;
	c_physics_collision* m_physics_collision   = nullptr;
	i_game_event_manager2* m_event_manager     = nullptr;
	c_effects* m_effects                       = nullptr;
};

inline interfaces_t interfaces = { };