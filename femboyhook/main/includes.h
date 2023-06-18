#pragma once

#include "globals.h"
#include "memory/memory.h"
#include "utilities/console/console.h"
#include "utilities/input/input.h"
#include "utilities/render/render.h"
#include "utilities/utilities.h"
#include "menu/gui/render_wrapper.h"
#include "menu/gui/input_wrapper.h"

#include "features/variables.h"

#include "globals.h"

#include "source_engine/c_utl_vector.h"
#include "source_engine/classes/c_color.h"
#include "source_engine/classes/c_client_class.h"
#include "source_engine/classes/c_user_cmd.h"
#include "source_engine/enumerations/e_command_buttons.h"
#include "source_engine/enumerations/e_entity_flags.h"
#include "source_engine/enumerations/e_move_types.h"
#include "source_engine/enumerations/e_item_definition_index.h"
#include "source_engine/interfaces/interfaces.h"
#include "source_engine/math/math.h"
#include "source_engine/structs/matrix_t.h"
#include "source_engine/structs/player_info_t.h"
#include "source_engine/classes/c_model_info.h"
#include "source_engine/classes/c_material.h"
#include "source_engine/classes/c_console.h"
#include "source_engine/classes/c_convar.h"
#include "source_engine/classes/c_trace.h"
#include "source_engine/classes/c_particle_collection.h"
#include "source_engine/classes/c_view_setup.h"
#include "source_engine/classes/c_texture.h"
#include "source_engine/classes/c_physics_collision.h"
#include "source_engine/classes/c_game_event_manager.h"

#include "hashing/strings/xorstr.h"

#include <nlohmann/json.hpp>

#include <imgui.h>
#include <misc/freetype/imgui_freetype.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <spdlog/spdlog.h>