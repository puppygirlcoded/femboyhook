#pragma once

#include "source_engine/classes/c_base_entity.h"
#include "source_engine/classes/c_user_cmd.h"
#include "source_engine/structs/matrix_t.h"
#include "source_engine/classes/c_view_setup.h"

struct globals_t {
	c_base_entity* m_local      = nullptr;
	c_user_cmd* m_cmd           = nullptr;
	bool m_send_packet          = false;
	view_matrix_t m_view_matrix = { };
	c_view_setup* m_view_setup = nullptr;
};

inline globals_t globals = { };