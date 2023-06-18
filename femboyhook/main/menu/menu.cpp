#include "menu.h"
#include "../config/config.h"
#include "gui/gui.h"
#include "gui/input_wrapper.h"
#include "gui/render_wrapper.h"
#include <psapi.h>

#include "../includes.h"

void menu_t::handle_spotify( )
{
	static HWND spotify_hwnd    = nullptr;
	static float last_hwnd_time = 0.f;
	if ( ( !spotify_hwnd || spotify_hwnd == INVALID_HANDLE_VALUE ) && last_hwnd_time < memory.m_globals->m_real_time + 2.f ) {
		for ( HWND hwnd = GetTopWindow( 0 ); hwnd; hwnd = GetWindow( hwnd, GW_HWNDNEXT ) ) {
			last_hwnd_time = memory.m_globals->m_real_time;

			if ( !( IsWindowVisible )( hwnd ) )
				continue;

			int length = ( GetWindowTextLengthW )( hwnd );
			if ( length == 0 )
				continue;

			WCHAR filename[ 300 ];
			DWORD pid;
			( GetWindowThreadProcessId )( hwnd, &pid );

			const auto spotify_handle = ( OpenProcess )( PROCESS_QUERY_INFORMATION, FALSE, pid );
			( K32GetModuleFileNameExW )( spotify_handle, nullptr, filename, 300 );

			std::wstring sane_filename{ filename };

			( CloseHandle )( spotify_handle );

			if ( sane_filename.find( ( L"Spotify.exe" ) ) != std::string::npos )
				spotify_hwnd = hwnd;
		}
	} else if ( spotify_hwnd && spotify_hwnd != INVALID_HANDLE_VALUE ) {
		WCHAR title[ 300 ];

		if ( !( GetWindowTextW )( spotify_hwnd, title, 300 ) ) {
			spotify_hwnd = nullptr;
		} else {
			std::wstring sane_title{ title };
			std::string song( sane_title.begin( ), sane_title.end( ) );
			if ( sane_title.find( ( L"-" ) ) != std::string::npos ) {
				this->current_spotify_song = song;
			} else if ( sane_title.find( ( L"Advertisment" ) ) != std::string::npos ) {
				this->current_spotify_song = xs( "advertisment" );
			} else if ( sane_title.find( ( L"Spotify" ) ) != std::string::npos ) {
				this->current_spotify_song = xs( "paused" );
			} else {
				this->current_spotify_song = xs( "advertisment" );
			}
		}
	}

	static std::string old_song = this->current_spotify_song;
	if ( old_song != this->current_spotify_song ) {
		if ( variables.misc.spotify_statistics ) {
			if ( this->current_spotify_song.find( xs( "paused" ) ) != std::string::npos )
				interfaces.m_hud_chat->chat_printf( xs( "#fbh#_spotify_paused" ) );
			else if ( this->current_spotify_song.find( xs( "advertisment" ) ) != std::string::npos )
				interfaces.m_hud_chat->chat_printf( xs( "#fbh#_spotify_advertisement" ) );
			else
				interfaces.m_hud_chat->chat_printf( xs( "#fbh#_spotify_switch" ) );
		}

		old_song = this->current_spotify_song;
	}
}

void menu_t::on_end_scene( )
{
	const auto opacity   = gui::get_opacity( m_opened );
	const auto dim_alpha = ( int )( opacity * 140 );

	const static auto initial_menu_size = sdk::vector2{ 600, 430 };
	const static auto initial_menu_pos  = sdk::vector2{ 32 };

	if ( gui::begin_window( xs( "femboyhook" ), initial_menu_pos, initial_menu_size ) ) {
		auto aimbot_tab = [ & ]( const std::string& title, aimbot_value_t& aimbot_value, bool show_override_from_default = false ) -> void {
			if ( gui::begin_group( xs( "global" ) ) ) {
				gui::key_bind( xs( "aimbot key" ), &variables.aimbot.activation_key, true, true, 1 );
				gui::checkbox( xs( "enable aimbot" ), &variables.aimbot.enabled );
				gui::dropdown( xs( "target selection" ), &variables.aimbot.target_selection,
				               { xs( "distance" ), xs( "field of view" ), xs( "health" ) } );
				gui::finish_group( );
			}

			if ( gui::begin_group( title ) ) {
				if ( show_override_from_default )
					gui::checkbox( xs( "override general" ), &aimbot_value.override_general );

				if ( show_override_from_default && aimbot_value.override_general ) {
					gui::checkbox( xs( "silent aim" ), &aimbot_value.silent );
					gui::multi_dropdown( xs( "hitboxes" ), &aimbot_value.hitboxes, { xs( "head" ), xs( "chest" ), xs( "pelvis" ), xs( "legs" ) } );
					gui::slider( xs( "field of view" ), &aimbot_value.field_of_view, 0, 180, 20 );
					gui::slider( xs( "smoothness" ), &aimbot_value.smoothness, 0, 100, 35 );
				} else if ( !show_override_from_default ) {
					gui::checkbox( xs( "silent aim" ), &aimbot_value.silent );
					gui::multi_dropdown( xs( "hitboxes" ), &aimbot_value.hitboxes, { xs( "head" ), xs( "chest" ), xs( "pelvis" ), xs( "legs" ) } );
					gui::slider( xs( "field of view" ), &aimbot_value.field_of_view, 0, 180, 20 );
					gui::slider( xs( "smoothness" ), &aimbot_value.smoothness, 0, 100, 35 );
				}

				gui::finish_group( );
			}

			if ( gui::begin_group( xs( "compensation" ) ) ) {
				gui::checkbox( xs( "compensate recoil" ), &aimbot_value.compensate_recoil );
				if ( aimbot_value.compensate_recoil ) {
					gui::slider( xs( "compensate x#aim" ), &aimbot_value.recoil_compensation.m_x, 0, 100, 25 );
					gui::slider( xs( "compensate y#aim" ), &aimbot_value.recoil_compensation.m_y, 0, 100, 50 );
				}

				gui::finish_group( );
			}

			if ( gui::begin_group( xs( "conditions" ) ) ) {
				gui::checkbox( xs( "target visible only" ), &aimbot_value.target_visible_only );
				gui::checkbox( xs( "target through smoke" ), &aimbot_value.target_through_smoke );
				gui::checkbox( xs( "target in air" ), &aimbot_value.target_enemy_in_air );
				gui::checkbox( xs( "target while in air" ), &aimbot_value.target_while_in_air );
				gui::checkbox( xs( "target while flashed" ), &aimbot_value.target_while_flashed );
				gui::checkbox( xs( "target team" ), &variables.aimbot.target_team );
				gui::finish_group( );
			}

			if ( gui::begin_group( xs( "standalone compensation" ) ) ) {
				gui::checkbox( xs( "standalone recoil" ), &aimbot_value.compensation.enabled );
				if ( aimbot_value.compensation.enabled ) {
					gui::checkbox( xs( "silent compensation" ), &aimbot_value.compensation.silent );
					gui::slider( xs( "compensate x" ), &aimbot_value.compensation.recoil_compensation.m_x, 0, 100, 25 );
					gui::slider( xs( "compensate y" ), &aimbot_value.compensation.recoil_compensation.m_y, 0, 100, 50 );
				}

				gui::finish_group( );
			}
		};

		if ( gui::begin_tab( xs( "aimbot" ) ) ) {
			if ( gui::begin_category( xs( "general" ) ) ) {
				aimbot_tab( xs( "general" ), variables.aimbot.general );
				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "rifles" ) ) ) {
				aimbot_tab( xs( "rifles" ), variables.aimbot.rifles, true );
				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "snipers" ) ) ) {
				aimbot_tab( xs( "snipers" ), variables.aimbot.snipers, true );

				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "pistols" ) ) ) {
				aimbot_tab( xs( "pistols" ), variables.aimbot.pistols, true );

				gui::finish_category( );
			}
			gui::finish_tab( );
		}

		if ( gui::begin_tab( xs( "esp" ) ) ) {
			if ( gui::begin_category( xs( "players" ) ) ) {
				if ( gui::begin_group( xs( "general" ) ) ) {
					gui::checkbox( xs( "draw on team" ), &variables.players.draw_on_team );
					gui::color_picker( xs( "name_color" ), &variables.players.name_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "name" ), &variables.players.name );

					if ( variables.players.filled_bounding_box )
						gui::color_picker( xs( "filed_box_top" ), &variables.players.filled_bounding_box_color_top, true,
						                   c_color( 255, 255, 255, 40 ) );
					else
						gui::color_picker( xs( "bounding_box_color" ), &variables.players.bounding_box_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "bounding box" ), &variables.players.bounding_box );
					if ( variables.players.bounding_box ) {
						gui::color_picker( xs( "filed_box_bot" ), &variables.players.filled_bounding_box_color_bottom, true,
						                   c_color( 255, 31, 131, 40 ) );
						gui::checkbox( xs( "filled box" ), &variables.players.filled_bounding_box );
						gui::dropdown( xs( "bounding box type" ), &variables.players.bounding_box_type, { xs( "outline" ), xs( "corner" ) } );
					}

					if ( variables.players.gradient_health_bar )
						gui::color_picker( xs( "grad_health_top" ), &variables.players.gradient_health_bar_color_top, false,
						                   c_color( 255, 31, 131 ) );
					else
						gui::color_picker( xs( "health_color" ), &variables.players.health_bar_color, false, c_color( 50, 255, 50 ) );
					gui::checkbox( xs( "health bar" ), &variables.players.health_bar );
					if ( variables.players.health_bar ) {
						gui::color_picker( xs( "grad_health_bot" ), &variables.players.gradient_health_bar_color_bottom, false,
						                   c_color( 255, 31, 131 ) );
						gui::checkbox( xs( "gradient healthbar" ), &variables.players.gradient_health_bar );
					}
					gui::multi_dropdown( xs( "flags" ), &variables.players.flags,
					                     { xs( "flash kill" ), xs( "armor" ), xs( "flashed" ), /* xs( "bomb" ),*/ xs( "kit" ), xs( "scoped" ) } );
					gui::color_picker( xs( "skeleton_color" ), &variables.players.skeleton_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "skeleton" ), &variables.players.skeleton );
					// if ( variables.players.skeleton ) {
					//	gui::checkbox( xs( "skeleton dropshadow" ), &variables.players.skeleton_outline );
					// }

					gui::color_picker( xs( "weapon_name_color" ), &variables.players.weapon_name_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "weapon name" ), &variables.players.weapon_name );

					gui::color_picker( xs( "weapon_icon_color" ), &variables.players.weapon_icon_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "weapon icon" ), &variables.players.weapon_icon );

					gui::color_picker( xs( "viewangles_color" ), &variables.players.viewangles_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "viewangles" ), &variables.players.viewangles );
					if ( variables.players.viewangles )
						gui::slider( xs( "viewangles distance" ), &variables.players.viewangles_distance, 50.f, 120.f, 75.f, xs( "{:.1f}" ) );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "model" ) ) ) {
					gui::color_picker( xs( "chams.visible_color" ), &variables.players.chams.visible_color, true, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "visible" ), &variables.players.chams.visible );

					gui::color_picker( xs( "chams.invisible_color" ), &variables.players.chams.invisible_color, true, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "invisible" ), &variables.players.chams.invisible );

					gui::color_picker( xs( "chams.backtrack_color" ), &variables.players.chams.backtrack_color, true, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "backtrack" ), &variables.players.chams.backtrack );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "glow" ) ) ) {
					gui::color_picker( xs( "glow.visible_color" ), &variables.players.glow.visible_color, true, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "glow#glow" ), &variables.players.glow.visible );
					// gui::color_picker( xs( "glow.invisible_color" ), &variables.players.glow.invisible_color, true, c_color( 255, 255, 255 ) );
					// gui::checkbox( xs( "invisible#glow" ), &variables.players.glow.invisible );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "model overlay" ) ) ) {
					gui::color_picker( xs( "chams.overlay_color" ), &variables.players.chams.overlay_color, true, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "overlay" ), &variables.players.chams.overlay );
					gui::checkbox( xs( "wireframe overlay" ), &variables.players.chams.wireframe_overlay );
					if ( variables.players.chams.overlay ) {
						gui::slider( xs( "overlay intensity" ), &variables.players.chams.wireframe_brightness, 0, 100, 100 );
					}

					gui::finish_group( );
				}
				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "edicts" ) ) ) {
				if ( gui::begin_group( xs( "general" ) ) ) {
					gui::finish_group( );
				}

				gui::finish_category( );
			}
			gui::finish_tab( );
		}

		if ( gui::begin_tab( xs( "visuals" ) ) ) {
			if ( gui::begin_category( xs( "general" ) ) ) {
				if ( gui::begin_group( xs( "world" ) ) ) {
					gui::color_picker( xs( "world.modulate_world" ), &variables.visuals.world.modulate_world_color, false, c_color( 33, 31, 33 ) );
					gui::checkbox( xs( "modulate world" ), &variables.visuals.world.modulate_world );

					gui::color_picker( xs( "world.modulate_props" ), &variables.visuals.world.modulate_props_color, false, c_color( 68, 66, 68 ) );
					gui::checkbox( xs( "modulate props" ), &variables.visuals.world.modulate_props );

					gui::color_picker( xs( "world.modulate_sky" ), &variables.visuals.world.modulate_sky_color, false, c_color( 53, 51, 53 ) );
					gui::checkbox( xs( "modulate sky" ), &variables.visuals.world.modulate_sky );
					gui::checkbox( xs( "sunset mode" ), &variables.visuals.world.sunset_mode );

					gui::dropdown( xs( "skybox changer" ), &variables.visuals.world.skybox_changer,
					               { xs( "default" ), xs( "night" ), xs( "lunar" ) } );
					gui::multi_dropdown( xs( "wireframes" ), &variables.visuals.world.wireframes, { xs( "smokes" ), xs( "molotovs" ) } );
					 gui::dropdown( xs( "precipitation" ), &variables.visuals.world.precipitation, { xs( "clear" ), xs( "rain" ), xs("storm"), xs( "snow" ) } );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "modulation" ) ) ) {
					gui::color_picker( xs( "world.modulate_fire" ), &variables.visuals.world.modulate_fire_color, false, c_color( 50, 125, 255 ) );
					gui::checkbox( xs( "modulate fire" ), &variables.visuals.world.modulate_fire );

					gui::color_picker( xs( "world.modulate_smoke" ), &variables.visuals.world.modulate_smoke_color, false, c_color( 255, 125, 50 ) );
					gui::checkbox( xs( "modulate smoke" ), &variables.visuals.world.modulate_smoke );

					gui::color_picker( xs( "world.modulate_blood" ), &variables.visuals.world.modulate_blood_color, false, c_color( 125, 50, 255 ) );
					gui::checkbox( xs( "modulate blood" ), &variables.visuals.world.modulate_blood );

					gui::color_picker( xs( "world.modulate_weapon_effects" ), &variables.visuals.world.modulate_weapon_effects_color, false,
					                   c_color( 50, 255, 125 ) );
					gui::checkbox( xs( "modulate weapon effects" ), &variables.visuals.world.modulate_weapon_effects );

					gui::color_picker( xs( "world.modulate_fog" ), &variables.visuals.world.modulate_fog_color, false, c_color( 255, 220, 255 ) );
					gui::checkbox( xs( "modulate fog" ), &variables.visuals.world.modulate_fog );
					if ( variables.visuals.world.modulate_fog ) {
						gui::slider( xs( "density" ), &variables.visuals.world.fog_density, 0.f, 1.f, 0.3f );
						gui::slider( xs( "distance" ), &variables.visuals.world.fog_distance, 5.f, 50.f, 30.f );
					}
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "local" ) ) ) {
					gui::slider( xs( "field of view" ), &variables.visuals.local.field_of_view, 90, 120, 90 );
					gui::slider( xs( "viewmodel" ), &variables.visuals.local.viewmodel_field_of_view, 60, 120, 60 );
					gui::slider( xs( "aspect ratio" ), &variables.visuals.local.aspect_ratio, 0.f, 3.f, 0.f, xs( "{:.1f}" ) );

					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "motion blur" ) ) ) {
					gui::checkbox( xs( "enable#motion_blur" ), &variables.visuals.motion_blur.enabled );
					if ( variables.visuals.motion_blur.enabled ) {
						gui::checkbox( xs( "render over viewmodel" ), &variables.visuals.motion_blur.render_on_viewmodel );
						gui::checkbox( xs( "forward blur" ), &variables.visuals.motion_blur.forward_blur );
						gui::slider( xs( "blur strength" ), &variables.visuals.motion_blur.blur_strength, 0.f, 5.f, 1.f, xs( "{:.1f}" ) );
						gui::slider( xs( "rotation intensity" ), &variables.visuals.motion_blur.rotation_intensity, 0.f, 5.f, 1.f, xs( "{:.1f}" ) );
						gui::slider( xs( "minimum fall" ), &variables.visuals.motion_blur.falling_min, 1.f, 40.f, 10.f, xs( "{:.1f}" ) );
						gui::slider( xs( "maximum fall" ), &variables.visuals.motion_blur.falling_max, 1.f, 40.f, 20.f, xs( "{:.1f}" ) );
					}
					gui::finish_group( );
				}
				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "others" ) ) ) {
				if ( gui::begin_group( xs( "bloom" ) ) ) {
					gui::checkbox( xs( "enable#bloom" ), &variables.visuals.world.bloom.enabled );
					if ( variables.visuals.world.bloom.enabled ) {
						gui::slider( xs( "amount" ), &variables.visuals.world.bloom.scale, 0.f, 1.f, 0.5f );
						gui::slider( xs( "exposure" ), &variables.visuals.world.bloom.exposure, 0.f, 5.f, 0.4f );
					}

					gui::finish_group( );
				}
				gui::finish_category( );
			}
			gui::finish_tab( );
		}

		if ( gui::begin_tab( xs( "misc" ) ) ) {
			if ( gui::begin_category( xs( "general" ) ) ) {
				if ( gui::begin_group( xs( "general" ) ) ) {
					gui::checkbox( xs( "watermark" ), &variables.misc.watermark, true );
					if ( variables.misc.watermark ) {
						gui::checkbox( xs( "display spotify song" ), &variables.misc.spotify_on_watermark );
					}
					gui::checkbox( xs( "discord presence" ), &variables.misc.discord_presence );
					gui::checkbox( xs( "spotify updates" ), &variables.misc.spotify_statistics );

					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "events" ) ) ) {
					gui::checkbox( xs( "hitsound" ), &variables.misc.events.hitsound );
					gui::color_picker( xs( "hitmarker_color" ), &variables.misc.events.hitmarker_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "hitmarker" ), &variables.misc.events.hitmarker );
					gui::checkbox( xs( "kill say" ), &variables.misc.events.kill_say );
					gui::checkbox( xs( "kill effect" ), &variables.misc.events.kill_effect );
					//gui::checkbox( xs( "headshot spoofer" ), &variables.misc.events.headshot_spoofer );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "helpers" ) ) ) {
					gui::checkbox( xs( "clan tag" ), &variables.misc.helpers.clantag );
					gui::checkbox( xs( "auto accept" ), &variables.misc.helpers.auto_accept );
					gui::checkbox( xs( "reveal ranks" ), &variables.misc.helpers.reveal_ranks );
					gui::checkbox( xs( "reveal on radar" ), &variables.misc.helpers.radar_reveal );
					gui::checkbox( xs( "unlock inventory access" ), &variables.misc.helpers.unlock_inventory );
					gui::checkbox( xs( "anti afk" ), &variables.misc.helpers.anti_afk );
					gui::checkbox( xs( "disable panorama blur" ), &variables.misc.helpers.disable_panorama_blur );
					gui::checkbox( xs( "team damage" ), &variables.misc.helpers.team_damage );

					gui::key_bind( xs( "use_spammer_key" ), &variables.misc.helpers.use_spammer_key, true, true );
					gui::checkbox( xs( "use spammer" ), &variables.misc.helpers.use_spammer );

					gui::key_bind( xs( "blockbot_key" ), &variables.movement.blockbot_key, true, true );
					gui::checkbox( xs( "blockbot" ), &variables.movement.blockbot );

					gui::key_bind( xs( "autozeus_key" ), &variables.misc.helpers.zeus_bot_key, true, true );
					gui::checkbox( xs( "auto zeus" ), &variables.misc.helpers.zeus_bot );

					gui::key_bind( xs( "zoom_key" ), &variables.misc.helpers.zoom_key, true, true );
					gui::checkbox( xs( "zoom" ), &variables.misc.helpers.zoom );
					if ( variables.misc.helpers.zoom ) {
						gui::slider( xs( "zoom amount" ), &variables.misc.helpers.zoom_amount, 20, variables.visuals.local.field_of_view, 40 );
					}
					//gui::button( xs( "show hidden convars" ), [ & ] {
					//	auto p = **reinterpret_cast< c_con_command_base*** >( interfaces.m_console + 0x34 );
					//	if ( p )
					//		for ( auto c = p->next; c != nullptr; c = c->next ) {
					//			c_con_command_base* cmd = c;
					//			cmd->flags &= ~FCVAR_DEVELOPMENTONLY;
					//			cmd->flags &= ~FCVAR_HIDDEN;
					//		}
					//	interfaces.m_engine->execute_command( xs( "echo fbh | unlocked hidden convars" ) );
					//} );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "local" ) ) ) {
					gui::checkbox( xs( "spectator list" ), &variables.misc.local.spectator_list );
					gui::checkbox( xs( "left hand knife" ), &variables.misc.local.left_hand_knife );
					gui::checkbox( xs( "remove sway" ), &variables.misc.local.remove_sway );
					gui::color_picker( xs( "recoil_crosshair_color" ), &variables.visuals.local.recoil_crosshair_color, false,
					                   c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "recoil crosshair" ), &variables.visuals.local.recoil_crosshair );
					gui::checkbox( xs( "remove scope" ), &variables.visuals.local.remove_scope );

					gui::color_picker( xs( "grenade_prediction_color" ), &variables.misc.local.grenade_prediction_color, false,
					                   c_color( 180, 180, 180 ) );
					gui::checkbox( xs( "grenade prediction" ), &variables.misc.local.grenade_prediction );

					gui::finish_group( );
				}
				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "movement" ) ) ) {
				if ( gui::begin_group( xs( "standard" ) ) ) {
					gui::checkbox( xs( "bunny hop" ), &variables.movement.bunny_hop );
					gui::checkbox( xs( "infinite duck" ), &variables.movement.infinite_duck );
					gui::key_bind( xs( "edgejump_key" ), &variables.movement.edgejump_key, true, true );
					gui::checkbox( xs( "edge jump" ), &variables.movement.edgejump );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "general" ) ) ) {
					gui::key_bind( xs( "jumpbug_key" ), &variables.movement.jumpbug_key, true, true );
					gui::checkbox( xs( "jump bug" ), &variables.movement.jumpbug );

					gui::key_bind( xs( "autoalign_key" ), &variables.movement.autoalign_key, true, true );
					gui::checkbox( xs( "auto align" ), &variables.movement.autoalign );

					gui::key_bind( xs( "longjump_key" ), &variables.movement.longjump_key, true, true );
					gui::checkbox( xs( "long jump" ), &variables.movement.longjump );
					if ( variables.movement.longjump ) {
						gui::slider( xs( "wait time#lj_time" ), &variables.movement.longjump_wait_time, 0, 5, 2, xs( "{} ticks" ) );
					}

					gui::key_bind( xs( "minijump_key" ), &variables.movement.minijump_key, true, true );
					gui::checkbox( xs( "mini jump" ), &variables.movement.minijump );

					gui::key_bind( xs( "ladderbug_key" ), &variables.movement.ladderbug_key, true, true );
					gui::checkbox( xs( "ladder glide" ), &variables.movement.ladderbug );

					gui::key_bind( xs( "staminahop_key" ), &variables.movement.stamina_hop_key, true, true );
					gui::checkbox( xs( "stamina hop" ), &variables.movement.stamina_hop );
					if ( variables.movement.stamina_hop ) {
						gui::slider( xs( "ticks" ), &variables.movement.stamina_hop_ticks, 1, 6, 3 );
					}

					gui::key_bind( xs( "fireman_key" ), &variables.movement.fireman_key, true, true );
					gui::checkbox( xs( "fire man" ), &variables.movement.fireman );

					gui::key_bind( xs( "msl_key" ), &variables.movement.mouse_strafe_limiter_key, true, true );
					gui::checkbox( xs( "mouse strafe limiter" ), &variables.movement.mouse_strafe_limiter );
					if ( variables.movement.mouse_strafe_limiter ) {
						gui::checkbox( xs( "affect pitch" ), &variables.movement.mouse_strafe_limiter_affect_pitch );
						gui::slider( xs( "strafe limit" ), &variables.movement.mouse_strafe_limiter_value, 0.f, 100.f, 20.f );
					}

					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "edgebug" ) ) ) {
					gui::key_bind( xs( "edgebug_key" ), &variables.movement.edgebug_key, true, true );
					gui::checkbox( xs( "enable#eb_enable" ), &variables.movement.edgebug );
					if ( variables.movement.edgebug ) {
						gui::checkbox( xs( "advanced detection" ), &variables.movement.advanced_edgebug );
						gui::slider( xs( "prediction ticks#edgebug ticks" ), &variables.movement.edgebug_ticks, 1, 64, 32 );
						gui::slider( xs( "lock amount#eb_lock_amount" ), &variables.movement.edgebug_lock_amount, 0, 100, 0, ( xs( "{}%" ) ) );
					}
					gui::finish_group( );
				}

				gui::push_fake_group( );

				if ( gui::begin_group( xs( "pixelsurf" ) ) ) {
					gui::key_bind( xs( "pixelsurf_key" ), &variables.movement.pixelsurf_key, true, true );
					gui::checkbox( xs( "enable#ps_enable" ), &variables.movement.pixelsurf );
					if ( variables.movement.pixelsurf ) {
						gui::checkbox( xs( "free look" ), &variables.movement.pixelsurf_free_look );
						gui::checkbox( xs( "hold keys" ), &variables.movement.pixelsurf_hold_needed_keys );
						gui::slider( xs( "prediction ticks#pixelsurf ticks" ), &variables.movement.pixelsurf_ticks, 1, 64, 16 );
					}

					gui::finish_group( );
				}

				gui::finish_category( );
			}

			if ( gui::begin_category( xs( "indicators" ) ) ) {
				if ( gui::begin_group( xs( "general" ) ) ) {
					gui::color_picker( xs( "velocity_color" ), &variables.visuals.indicators.velocity_color, false, c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "velocity" ), &variables.visuals.indicators.velocity );

					gui::color_picker( xs( "stamina_color" ), &variables.visuals.indicators.stamina_color, false, c_color( 255, 31, 131 ) );
					gui::checkbox( xs( "stamina" ), &variables.visuals.indicators.stamina );

					gui::checkbox( xs( "previous speed" ), &variables.visuals.indicators.previous_speeds );

					gui::color_picker( xs( "keybinds_color" ), &variables.visuals.indicators.keybinds_color, false, c_color( 255, 31, 131 ) );
					gui::multi_dropdown( xs( "keybinds" ), &variables.visuals.indicators.keybinds,
					                     { xs( "jump bug" ), xs( "edge bug" ), xs( "pixel surf" ), xs( "mini jump" ), xs( "edge jump" ),
					                       xs( "long jump" ), xs( "stamina hop" ), xs( "ladder glide" ), xs( "fire man" ), xs( "key strokes" ) } );

					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "feedback" ) ) ) {
					gui::multi_dropdown( xs( "chat logs" ), &variables.visuals.indicators.chat_logs,
					                     { xs( "on edgebug" ), xs( "on jumpbug" ), xs( "on pixelsurf" ), xs( "on edgejump" ), xs( "on longjump" ),
					                       xs( "on minijump" ), xs( "on ladderglide" ), xs( "on staminahop" ), xs( "on fireman" ) } );

					gui::checkbox( xs( "edgebug hitsound" ), &variables.visuals.indicators.edgebug_hitsound );
					gui::checkbox( xs( "pixelsurf hitsound" ), &variables.visuals.indicators.pixelsurf_hitsound );
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "graphs" ) ) ) {
					gui::color_picker( xs( "velocity_graph_color" ), &variables.visuals.indicators.velocity_graph_color, false,
					                   c_color( 255, 255, 255 ) );
					gui::checkbox( xs( "velocity graph" ), &variables.visuals.indicators.velocity_graph );

					gui::color_picker( xs( "stamina_graph_color" ), &variables.visuals.indicators.stamina_graph_color, false,
					                   c_color( 255, 31, 131 ) );
					gui::checkbox( xs( "stamina graph" ), &variables.visuals.indicators.stamina_graph );

					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "world" ) ) ) {
					gui::color_picker( xs( "pixelsurf_line_color" ), &variables.visuals.indicators.pixelsurf_line_color, false,
					                   c_color( 255, 31, 131 ) );
					gui::checkbox( xs( "draw line when surfing" ), &variables.visuals.indicators.pixelsurf_line );
					if ( variables.visuals.indicators.pixelsurf_line ) {
						gui::slider( xs( "line duration" ), &variables.visuals.indicators.pixelsurf_line_duration, 0.f, 10.f, 2.f );
					}
					gui::finish_group( );
				}

				if ( gui::begin_group( xs( "font" ) ) ) {
					gui::slider( xs( "offset" ), &variables.visuals.indicators.offset, -200, 200, 0, xs( "{}px" ) );

					gui::dropdown( xs( "font" ), &variables.visuals.indicators.font, { xs( "default" ), xs( "verdana" ), xs( "tahoma" ) } );

					gui::checkbox( xs( "font shadow" ), &variables.visuals.indicators.font_shadow );
					gui::finish_group( );
				}

				gui::finish_category( );
			}

			gui::finish_tab( );
		}

		if ( gui::begin_tab( xs( "other" ) ) ) {
			if ( gui::begin_category( xs( "general" ) ) ) {
				if ( gui::begin_group( xs( "ui" ) ) ) {
					gui::color_picker( xs( "menu_color" ), &accent, false );
					gui::label( xs( "menu accent" ) );
					// gui::button( "button", [ & ] {} );
					gui::finish_group( );
				}
				gui::finish_category( );
			}

			static std::string current_config_name = { };
			static int selected_config             = 0;
			static std::string current_config_file = { };

			if ( gui::begin_category( xs( "configs" ) ) ) {
				if ( gui::begin_group( xs( "general" ) ) ) {
					
					gui::finish_group( );
				}
				gui::finish_category( );
			}

			// current_config_name = !config.m_file_names.empty( ) ? config.m_file_names[ selected_config ] : "";

			// if ( gui::begin_category( xs( "config" ) ) ) {
			//	if ( gui::begin_group( xs( "general" ) ) ) {
			//		static std::string fart = { };
			//		gui::text_input( xs( "test" ), &fart );
			//
			//		gui::text_input( xs( "config file name" ), &current_config_file );
			//
			//		gui::button( xs( "create config" ), [] {
			//			if ( !config.save( current_config_file ) )
			//				console.print( "failed to create {:s}", current_config_file );
			//
			//			current_config_file.clear( );
			//			config.refresh( );
			//		} );
			//
			//		gui::button( xs( "save config" ), [] {
			//			if ( !config.save( current_config_name ) )
			//				console.print( "failed to save {:s}", current_config_name );
			//		} );
			//
			//		gui::button( xs( "load config" ), [] {
			//			if ( !config.load( current_config_name ) )
			//				console.print( "failed to load {:s}", current_config_name );
			//		} );
			//
			//		gui::finish_group( );
			//	}
			//	gui::finish_category( );
			// }
			gui::finish_tab( );
		}
		gui::finish_window( );
	}
}