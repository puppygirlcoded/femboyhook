#pragma once

enum e_keybind_modes {
	e_always_on,
	e_held,
	e_toggle
};

struct key_bind_t {
	int key;
	int mode;

	bool toggle_state;

	bool get_key_state( )
	{
		if ( mode == e_always_on )
			return true;

		if ( key <= 0 ) {
			return false;
		}

		if ( mode == e_toggle && input.is_key_released( key ) ) {
			toggle_state = !toggle_state;
		}

		switch ( mode ) {
		case e_always_on:
			return true;
			break;
		case e_held:
			return input.is_key_down( key );
			break;
		case e_toggle:
			return toggle_state;
			break;
		}

		return false;
	}
};

// store here for the menu configuration and aimbot.cpp
struct aimbot_value_t {
	bool override_general           = false;
	bool silent                     = false;
	int field_of_view               = 20;
	bool compensate_recoil          = false;
	c_vector_2d recoil_compensation = { 25, 50 };
	int smoothness                  = 35;
	bool target_visible_only        = false;
	bool target_through_smoke       = false;
	bool target_enemy_in_air        = false;
	bool target_while_in_air        = false;
	bool target_while_flashed       = false;

	int hitboxes = 0;

	struct {
		bool enabled                    = false;
		bool silent                     = false;
		c_vector_2d recoil_compensation = { 25, 50 };
	} compensation;
};

struct variables_t {
	// aimbot
	struct {
		bool enabled              = false;
		bool target_team          = false;
		int target_selection      = 0;
		key_bind_t activation_key = key_bind_t( 1, e_held );

		aimbot_value_t general;
		aimbot_value_t rifles;
		aimbot_value_t snipers;
		aimbot_value_t pistols;
	} aimbot;

	// players
	struct {
		bool draw_on_team = false;

		bool bounding_box          = false;
		c_color bounding_box_color = c_color( 255, 255, 255 );
		int bounding_box_type      = 0;

		bool filled_bounding_box                 = false;
		c_color filled_bounding_box_color_top    = c_color( 255, 255, 255, 20 );
		c_color filled_bounding_box_color_bottom = c_color( 255, 31, 131, 20 );

		bool name          = false;
		c_color name_color = c_color( 255, 255, 255 );

		bool weapon_name          = false;
		c_color weapon_name_color = c_color( 255, 255, 255 );

		bool weapon_icon          = false;
		c_color weapon_icon_color = c_color( 255, 255, 255 );

		bool skeleton          = false;
		bool skeleton_outline  = false;
		c_color skeleton_color = c_color( 255, 255, 255 );

		bool health_bar          = false;
		c_color health_bar_color = c_color( 50, 255, 50 );

		bool gradient_health_bar                 = false;
		c_color gradient_health_bar_color_top    = c_color( 255, 255, 255 );
		c_color gradient_health_bar_color_bottom = c_color( 255, 31, 131 );

		bool viewangles           = false;
		float viewangles_distance = 75.f;
		c_color viewangles_color  = c_color( 255, 255, 255 );

		int flags = 0;

		// chams
		struct {
			bool visible          = false;
			c_color visible_color = c_color( 255, 255, 255 );

			bool invisible          = false;
			c_color invisible_color = c_color( 255, 255, 255 );

			bool backtrack          = false;
			c_color backtrack_color = c_color( 255, 255, 255 );

			bool overlay             = false;
			c_color overlay_color    = c_color( 255, 255, 255 );
			bool wireframe_overlay   = false;
			int wireframe_brightness = 100;
		} chams;

		// glow
		struct {
			bool visible          = false;
			c_color visible_color = c_color( 255, 255, 255 );

			bool invisible          = false;
			c_color invisible_color = c_color( 255, 255, 255 );
		} glow;
	} players;

	// visuals
	struct {
		struct {
			bool modulate_world          = false;
			c_color modulate_world_color = c_color( 33, 31, 33 );

			bool modulate_props          = false;
			c_color modulate_props_color = c_color( 68, 66, 68 );

			bool modulate_sky          = false;
			c_color modulate_sky_color = c_color( 53, 51, 53 );

			bool modulate_fire          = false;
			c_color modulate_fire_color = c_color( 50, 125, 255 );

			bool modulate_smoke          = false;
			c_color modulate_smoke_color = c_color( 255, 125, 50 );

			bool modulate_blood          = false;
			c_color modulate_blood_color = c_color( 125, 50, 255 );

			bool modulate_weapon_effects          = false;
			c_color modulate_weapon_effects_color = c_color( 50, 255, 125 );

			int skybox_changer = 0;
			bool sunset_mode   = false;

			int wireframes = 0;

			int precipitation = 0;

			struct {
				bool enabled   = false;
				float scale    = 0.5f;
				float exposure = 0.4f;
			} bloom;

			bool modulate_fog          = false;
			c_color modulate_fog_color = c_color( 255, 220, 255 );
			float fog_distance         = 1.f;
			float fog_density          = 0.2f;
		} world;

		struct {
			bool velocity          = false;
			c_color velocity_color = c_color( 255, 255, 255 );
			bool stamina           = false;
			c_color stamina_color  = c_color( 255, 31, 131 );
			bool previous_speeds   = false;

			bool velocity_graph          = false;
			c_color velocity_graph_color = c_color( 255, 255, 255 );

			bool stamina_graph          = false;
			c_color stamina_graph_color = c_color( 255, 31, 131 );

			int keybinds           = 0;
			c_color keybinds_color = c_color( 255, 31, 131 );
			bool include_on_graph  = false;

			int chat_logs    = 0;
			int offset       = 0;
			int font         = 0;
			bool font_shadow = false;

			bool edgebug_hitsound   = false;
			bool pixelsurf_hitsound = false;

			bool pixelsurf_line           = false;
			float pixelsurf_line_duration = 2.f;
			c_color pixelsurf_line_color  = c_color( 255, 31, 131 );
		} indicators;

		struct {
			bool enabled             = false;
			bool render_on_viewmodel = false;
			bool forward_blur        = false;
			float rotation_intensity = 1.f;
			float blur_strength      = 1.f;
			float falling_min        = 10.f;
			float falling_max        = 20.f;
		} motion_blur;

		struct {
			int field_of_view              = 90;
			int viewmodel_field_of_view    = 60;
			float aspect_ratio             = 0.f;
			bool recoil_crosshair          = false;
			c_color recoil_crosshair_color = c_color( 255, 255, 255 );
			bool remove_scope              = false;
		} local;

	} visuals;

	// movement
	struct {
		bool bunny_hop     = false;
		bool infinite_duck = false;

		bool edgejump           = false;
		key_bind_t edgejump_key = key_bind_t( 0, e_held );

		bool minijump           = false;
		key_bind_t minijump_key = key_bind_t( 0, e_held );

		bool jumpbug           = false;
		key_bind_t jumpbug_key = key_bind_t( 0, e_held );

		bool edgebug            = false;
		bool advanced_edgebug   = false;
		key_bind_t edgebug_key  = key_bind_t( 0, e_held );
		int edgebug_ticks       = 32;
		int edgebug_lock_amount = 0;

		bool blockbot           = false;
		key_bind_t blockbot_key = key_bind_t( 0, e_held );

		bool autoalign           = false;
		key_bind_t autoalign_key = key_bind_t( 0, e_held );

		bool pixelsurf                  = false;
		key_bind_t pixelsurf_key        = key_bind_t( 0, e_held );
		int pixelsurf_ticks             = 16;
		bool pixelsurf_free_look        = false;
		bool pixelsurf_hold_needed_keys = false;

		bool stamina_hop           = false;
		key_bind_t stamina_hop_key = key_bind_t( 0, e_held );
		int stamina_hop_ticks      = 3;

		bool ladderbug           = false;
		key_bind_t ladderbug_key = key_bind_t( 0, e_held );

		bool longjump           = false;
		key_bind_t longjump_key = key_bind_t( 0, e_held );
		int longjump_wait_time  = 2;

		bool fireman           = false;
		key_bind_t fireman_key = key_bind_t( 0, e_held );

		bool mouse_strafe_limiter              = false;
		bool mouse_strafe_limiter_affect_pitch = false;
		float mouse_strafe_limiter_value       = 20.f;
		key_bind_t mouse_strafe_limiter_key    = key_bind_t( 0, e_held );
	} movement;

	// misc
	struct {
		bool watermark            = true;
		bool spotify_on_watermark = false;
		bool spotify_statistics   = false;
		bool discord_presence     = false;

		struct {
			bool reveal_ranks          = false;
			bool clantag               = false;
			bool auto_accept           = false;
			bool anti_afk              = false;
			bool disable_panorama_blur = false;

			bool use_spammer           = false;
			key_bind_t use_spammer_key = key_bind_t( 0, e_held );

			bool zeus_bot           = false;
			key_bind_t zeus_bot_key = key_bind_t( 0, e_held );

			bool zoom           = false;
			key_bind_t zoom_key = key_bind_t( 0, e_held );
			int zoom_amount     = 40;

			bool radar_reveal     = false;
			bool unlock_inventory = false;
			bool team_damage      = false;
		} helpers;

		struct {
			bool headshot_spoofer = false;
			bool hitsound         = false;
			bool kill_say         = false;
			bool kill_effect      = false;

			bool hitmarker          = false;
			c_color hitmarker_color = c_color( 255, 255, 255 );

		} events;

		struct {
			bool left_hand_knife             = false;
			bool remove_sway                 = false;
			c_color grenade_prediction_color = c_color( 180, 180, 180 );
			bool grenade_prediction          = false;
			bool spectator_list              = false;
		} local;

	} misc;

	// skins
	struct {
		bool enabled = false;

		int t_knife  = 0;
		int ct_knife = 0;
	} skins;
};

inline variables_t variables = { };