#pragma once
#include "../../../includes.h"

struct grenade_prediction_t {
	class c_nadepoint
	{
	public:
		c_nadepoint( )
		{
			m_valid = false;
		}

		c_nadepoint( c_vector start, c_vector end, bool plane, bool valid, c_vector normal, bool detonate )
		{
			m_start    = start;
			m_end      = end;
			m_plane    = plane;
			m_valid    = valid;
			m_normal   = normal;
			m_detonate = detonate;
		}

		c_vector m_start, m_end, m_normal;
		bool m_valid, m_plane, m_detonate;
	};

	std::array< c_nadepoint, 500 > _points{ };
	bool _predicted = false;
	void predict( c_user_cmd* ucmd );
	bool detonated( c_weapon* weapon, float time, trace_t& trace );
	void on_create_move( c_user_cmd* ucmd );
	void on_end_scene( );
};

inline grenade_prediction_t grenade_prediction;