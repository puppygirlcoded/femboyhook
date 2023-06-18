#pragma once

struct bbox_t {
	float m_left = 0.f, m_top = 0.f, m_right = 0.f, m_bottom = 0.f, m_width = 0.f, m_height = 0.f;
};

class c_base_entity;
class c_weapon_data;

struct players_t {
	void on_paint_traverse( );
	void on_post_screen_effects( );

private:
	bool get_bbox( c_base_entity* entity, bbox_t* box ) const;
	float player_alpha[ 65 ] = { };
	int player_health_interp[ 65 ] = { };

	struct player_data_t {
		bbox_t* m_box_data             = nullptr;
		c_base_entity* m_active_weapon = nullptr;
		c_weapon_data* m_weapon_data   = nullptr;
	} m_player_data[ 65 ];
};

inline players_t players = { };