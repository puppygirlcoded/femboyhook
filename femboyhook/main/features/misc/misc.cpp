#include "misc.h"
#include "../../includes.h"
#include "../../menu/menu.h"

#include "../../source_engine/steam_sdk/steam_api.h"
#include <discord_rpc.h>

void misc_t::on_frame_stage_notify( )
{
	/*if ( variables.misc.discord_presence ) {
	    DiscordRichPresence presence = { };
	    std::memset( &presence, 0, sizeof( presence ) );
	    presence.largeImageKey = "fbh_1";
	    presence.state         = xs( "version 2" );
	    presence.instance      = 1;
	    Discord_UpdatePresence( &presence );
	} else {
	    Discord_ClearPresence( );
	}*/

	/* skybox changer */
	[ & ]( ) {
		if ( !variables.visuals.world.skybox_changer )
			return;

		static auto load_named_sky = reinterpret_cast< void( __fastcall* )( const char* ) >(
			memory.m_modules[ e_module_names::engine ].find_pattern( xs( "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" ) ) );

		switch ( variables.visuals.world.skybox_changer ) {
		case 1:
			load_named_sky( xs( "sky_csgo_night02" ) );
			break;
		case 2:
			load_named_sky( xs( "sky_lunacy" ) );
			break;
		}
	}( );
}

void misc_t::on_end_scene( )
{
	/* watermark */
	[ & ]( ) {
		if ( !variables.misc.watermark )
			return;

		auto str = std::string( xs( "femboyhook beta" ) );

		if ( variables.misc.spotify_on_watermark ) {
			str += std::vformat( xs( " | {}" ), std::make_format_args( menu.current_spotify_song ) );
		}

		auto text_size    = utils::render::measure_text( str, utils::render::FONT_SEGOE_UI_18 );
		auto rect_size    = text_size + sdk::vector2( 10, 5 );
		auto display_size = sdk::vector2( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );

		utils::render::fill_rect( sdk::vector2( display_size.x - rect_size.x, 0 ), rect_size, { 0, 100 } );
		utils::render::fill_gradient( sdk::vector2( display_size.x - rect_size.x, 0 ), rect_size, menu.accent.adjust_alpha( 60 ),
		                              menu.accent.adjust_alpha( 0 ) );
		utils::render::text( sdk::vector2( display_size.x - rect_size.x + 5, 1 ), str, utils::render::FONT_SEGOE_UI_18, { 255 } );
		utils::render::fill_rect( sdk::vector2( display_size.x - rect_size.x - 3, 0 ), sdk::vector2( 3, rect_size.y ), menu.accent );
		utils::render::fill_gradient( sdk::vector2( display_size.x - rect_size.x - 3, 0 ), sdk::vector2( 3, rect_size.y ), sdk::color( 0, 0 ),
		                              sdk::color( 0, 150 ), false );
	}( );

	/* spectator list */
	[ & ]( ) {
		if ( !variables.misc.local.spectator_list )
			return;

		struct spectator_t {
			std::string name;
			uint64_t xuid;
			int team;
			bool is_bot;
		};
		std::vector< spectator_t > spectators;

		static std::map< uint64_t, LPDIRECT3DTEXTURE9 > profile_picture_stack;
		const auto push_profile_picture = []( LPDIRECT3DTEXTURE9 byte, uint64_t xuid ) -> void {
			auto profile_picture = profile_picture_stack.find( xuid );
			if ( profile_picture == profile_picture_stack.end( ) ) {
				profile_picture_stack.insert( { xuid, byte } );
				profile_picture = profile_picture_stack.find( xuid );
			}
		};

		// retarted code XD
		const auto copy_convert = [ & ]( const uint8_t* rgba, uint8_t* out, const size_t size ) {
			auto in  = reinterpret_cast< const uint32_t* >( rgba );
			auto buf = reinterpret_cast< uint32_t* >( out );
			for ( auto i = 0u; i < ( size / 4 ); ++i ) {
				const auto pixel = *in++;
				*buf++           = ( pixel & 0xFF00FF00 ) | ( ( pixel & 0xFF0000 ) >> 16 ) | ( ( pixel & 0xFF ) << 16 );
			}
		};

		const auto get_steam_profile_picture = [ & ]( int64_t xuid ) -> LPDIRECT3DTEXTURE9 {
			LPDIRECT3DTEXTURE9 temp_image;

			int steam_image = SteamFriends->GetSmallFriendAvatar( CSteamID( ( uint64 )xuid ) );

			if ( steam_image == -1 )
				return nullptr;

			uint32 image_width, image_height;

			if ( !SteamUtils->GetImageSize( steam_image, &image_width, &image_height ) )
				return nullptr;

			const int image_size_to_byte = image_width * image_height * 4;
			uint8* image_rgba            = new uint8[ image_size_to_byte ];

			if ( !SteamUtils->GetImageRGBA( steam_image, image_rgba, image_size_to_byte ) ) {
				delete[] image_rgba;
				return nullptr;
			}

			auto res = render.m_device->CreateTexture( image_width, image_height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &temp_image,
			                                           nullptr );

			std::vector< uint8_t > texData;
			texData.resize( image_width * image_height * 4u );

			copy_convert( image_rgba, texData.data( ), image_width * image_height * 4u );

			D3DLOCKED_RECT rect;

			if ( !temp_image )
				return 0;

			res      = temp_image->LockRect( 0, &rect, nullptr, D3DLOCK_DISCARD );
			auto src = texData.data( );
			auto dst = reinterpret_cast< uint8_t* >( rect.pBits );

			for ( auto y = 0u; y < image_height; ++y ) {
				std::copy( src, src + ( image_width * 4 ), dst );

				src += image_width * 4;
				dst += rect.Pitch;
			}
			res = temp_image->UnlockRect( 0 );
			delete[] image_rgba;

			return temp_image;
		};

		const auto get_profile_picture = []( int64_t xuid ) -> LPDIRECT3DTEXTURE9 {
			auto profile_picture = profile_picture_stack.find( xuid );
			if ( profile_picture == profile_picture_stack.end( ) ) {
				return 0x0;
			}

			return profile_picture->second;
		};

		// lol
		if ( interfaces.m_engine->is_in_game( ) )
			for ( int i = 1; i < 32; i++ ) {
				const auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( i ) );
				player_info_t player_info;

				if ( !entity )
					continue;

				const auto observer_target = entity->observer_target( );
				if ( observer_target == 0xFFFFFFFF )
					continue;

				const auto target = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( observer_target ) );
				if ( !target || target->is_dormant( ) )
					continue;

				interfaces.m_engine->get_player_information( entity->index( ), &player_info );

				spectator_t t;

				t.name   = player_info.m_name;
				t.xuid   = player_info.m_xuid;
				t.is_bot = player_info.m_fakeplayer;
				t.team   = entity->team( );
				spectators.push_back( t );
			}

		static float alpha = 1.f;
		alpha = ImClamp( alpha + ( 3.f * ( 1.f / 0.5f * ImGui::GetIO( ).DeltaTime ) * ( !spectators.empty( ) || menu.m_opened ? 1.f : -1.f ) ), 0.0f,
		                 1.f );

		if ( alpha < 0.1f )
			return;
		// TODO: recode this, it was made in an example window lol
		const auto draw_list = ImGui::GetBackgroundDrawList( );
		static ImVec2 pos    = { 125, 125 };
		static ImVec2 size   = { 170, 24 + 5 };

		static bool start_dragging = false;
		static ImVec2 old_mouse_pos;
		ImVec2 delta = ImGui::GetIO( ).MousePos - old_mouse_pos;
		bool hovering_drag_area =
			utils::input_wrapper::is_hovering( sdk::vector2{ ( int )pos.x, ( int )pos.y },
		                                       sdk::vector2{ ( int )pos.x, ( int )pos.y } + sdk::vector2{ ( int )size.x, 24 } ) &&
			menu.m_opened;

		if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) && hovering_drag_area )
			start_dragging = true;

		if ( start_dragging && ImGui::IsMouseDown( ImGuiMouseButton_Left ) ) {
			pos += delta;
		} else if ( start_dragging && !ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
			start_dragging = false;

		old_mouse_pos = ImGui::GetIO( ).MousePos;

		pos = ImClamp( pos, ImVec2( 0.f, 0.f ), ImGui::GetIO( ).DisplaySize - size );

		const auto alpha_a   = static_cast< int >( alpha * 255 );
		ImColor accent       = ImColor( menu.accent.r, menu.accent.g, menu.accent.b, static_cast< int >( 15 * alpha ) );
		ImColor accent_no_bg = ImColor( menu.accent.r, menu.accent.g, menu.accent.b, 0 );

		draw_list->AddRectFilled( pos, pos + size, ImColor( 18, 18, 18, alpha_a ), 4.f );
		draw_list->AddRectFilledMultiColor( pos + ImVec2( 1, 1 ), pos + ImVec2( size.x / 2, 24 ) - ImVec2( 1, 1 ), accent, accent_no_bg, accent_no_bg,
		                                    accent );
		draw_list->AddRect( pos, pos + size, ImColor( 24, 24, 24, alpha_a ), 4.f );
		draw_list->AddRectFilled( pos + ImVec2( 0, 23 ), pos + ImVec2( size.x, 24 ), ImColor( 24, 24, 24, alpha_a ) );
		draw_list->AddText( pos + ImVec2( size.x / 2, 10 ) - ImGui::CalcTextSize( xs( "spectators" ) ) / 2, ImColor( 200, 200, 200, alpha_a ),
		                    xs( "spectators" ) );

		std::vector< std::string > spectator_handles;

		auto add_spectator = [ & ]( std::string spectator_name, LPDIRECT3DTEXTURE9 profile_picture ) -> void {
			draw_list->AddImageRounded( profile_picture, pos + ImVec2( 5, 24 + 5 + ( 21 * spectators.size( ) ) ),
			                            pos + ImVec2( 5, 24 + 5 + ( 21 * spectators.size( ) ) ) + ImVec2( 16, 16 ), ImVec2( 0, 0 ), ImVec2( 1, 1 ),
			                            ImColor( 255, 255, 255, alpha_a ), 3.f );
			draw_list->AddText( pos + ImVec2( 26, 24 + 5 + ( 21 * spectators.size( ) ) ), ImColor( 220, 220, 220, alpha_a ), spectator_name.data( ) );

			spectator_handles.push_back( spectator_name );
		};

		for ( const auto spectator : spectators ) {
			auto av = get_profile_picture( spectator.xuid );
			if ( !av ) { // OH NO WE HAVENT SAVED THE OPPS MUG SHOT! LETS SAVE IT!
				push_profile_picture( get_steam_profile_picture( spectator.xuid ), spectator.xuid );
			}
			auto profile_picture = get_profile_picture( spectator.xuid );
			add_spectator( spectator.name, profile_picture );
		}

		size.y = 24 + 5 + ( ( 16 + 5 ) * spectator_handles.size( ) );
	}( );

	/* team damage */
	[ & ]( ) {
		if ( !variables.misc.helpers.team_damage )
			return;

		auto reset_info = [ & ]( ) {
			this->event_data.team_info.damage = 0;
			this->event_data.team_info.kills  = 0;
		};

		if ( !interfaces.m_engine->is_in_game( ) ) {
			reset_info( );
		}

		const auto draw_list = ImGui::GetBackgroundDrawList( );
		static ImVec2 pos    = { 300, 125 };
		static ImVec2 size   = { 170, 24 + 5 };

		static bool start_dragging = false;
		static ImVec2 old_mouse_pos;
		ImVec2 delta = ImGui::GetIO( ).MousePos - old_mouse_pos;
		bool hovering_drag_area =
			utils::input_wrapper::is_hovering( sdk::vector2{ ( int )pos.x, ( int )pos.y },
		                                       sdk::vector2{ ( int )pos.x, ( int )pos.y } + sdk::vector2{ ( int )size.x, 24 } ) &&
			menu.m_opened;

		if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) && hovering_drag_area )
			start_dragging = true;

		if ( start_dragging && ImGui::IsMouseDown( ImGuiMouseButton_Left ) ) {
			pos += delta;
		} else if ( start_dragging && !ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
			start_dragging = false;

		old_mouse_pos = ImGui::GetIO( ).MousePos;

		pos = ImClamp( pos, ImVec2( 0.f, 0.f ), ImGui::GetIO( ).DisplaySize - size );

		ImColor accent       = ImColor( menu.accent.r, menu.accent.g, menu.accent.b, 15 );
		ImColor accent_no_bg = ImColor( menu.accent.r, menu.accent.g, menu.accent.b, 0 );

		draw_list->AddRectFilled( pos, pos + size, ImColor( 18, 18, 18 ), 4.f );
		draw_list->AddRectFilledMultiColor( pos + ImVec2( 1, 1 ), pos + ImVec2( size.x / 2, 24 ) - ImVec2( 1, 1 ), accent, accent_no_bg, accent_no_bg,
		                                    accent );
		draw_list->AddRect( pos, pos + size, ImColor( 24, 24, 24 ), 4.f );
		draw_list->AddRectFilled( pos + ImVec2( 0, 23 ), pos + ImVec2( size.x, 24 ), ImColor( 24, 24, 24 ) );
		draw_list->AddText( pos + ImVec2( size.x / 2, 10 ) - ImGui::CalcTextSize( xs( "team damage" ) ) / 2, ImColor( 200, 200, 200 ),
		                    xs( "team damage" ) );

		std::vector< std::string > items;

		auto add_item = [ & ]( std::string item_desc ) -> void {
			draw_list->AddText( pos + ImVec2( 5, 19 + 5 + ( 21 * items.size( ) ) ), ImColor( 220, 220, 220 ), item_desc.data( ) );

			items.push_back( item_desc );
		};

		add_item( std::vformat( xs( "total damage: {}/300" ), std::make_format_args( this->event_data.team_info.damage ) ) );
		add_item( std::vformat( xs( "total kills: {}/3" ), std::make_format_args( this->event_data.team_info.kills ) ) );

		size.y = 24 + 5 + ( ( 14 + 5 ) * items.size( ) );
	}( );

	if ( !interfaces.m_engine->is_in_game( ) || !globals.m_local->is_alive( ) )
		return;

	/* recoil crosshair */
	[ & ]( ) {
		if ( !variables.visuals.local.recoil_crosshair )
			return;

		if ( !globals.m_view_setup )
			return;

		auto screen_size = c_vector_2d( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );
		auto position    = c_vector_2d( screen_size.m_x / 2.f, screen_size.m_y / 2.f );

		auto aim_punch = globals.m_local->aim_punch_angle( );

		position.m_x -= ( screen_size.m_x / globals.m_view_setup->fov ) * aim_punch.m_y;
		position.m_y += ( screen_size.m_y / globals.m_view_setup->fov ) * aim_punch.m_x;

		const auto should_show = globals.m_local->shots_fired( ) > 1;
		static float alpha     = 0.f;
		if ( alpha != should_show )
			alpha += ( should_show - alpha ) * ( 1.f / 0.2f ) * ImGui::GetIO( ).DeltaTime;

		if ( alpha > 0.1f ) {
			auto draw_cross = [ & ]( c_vector_2d position, int size, c_color color ) -> void {
				const auto draw_list = ImGui::GetBackgroundDrawList( );
				// top left -> bottom right
				render.line( draw_list, c_vector_2d( position.m_x - size, position.m_y - size ),
				             c_vector_2d( position.m_x + size, position.m_y + size ), color, 2.f );

				// bottom left -> top right
				render.line( draw_list, c_vector_2d( position.m_x - size, position.m_y + size ),
				             c_vector_2d( position.m_x + size, position.m_y - size ), color, 2.f );
			};

			draw_cross( position, 4, variables.visuals.local.recoil_crosshair_color.adjust_alpha( alpha ) );
		}
	}( );

	/* hitmarker */
	[ & ]( ) {
		if ( !variables.misc.events.hitmarker ) {
			this->event_data.hitmarker_time = 0.f;
			return;
		}

		this->event_data.hitmarker_time -= 1.f * ( 1.f / 0.6f ) * ImGui::GetIO( ).DeltaTime;

		const auto alpha = this->event_data.hitmarker_time / 1.5f;

		if ( alpha < 0.1f )
			return;

		auto draw_cross = [ & ]( c_vector_2d position, int size, c_color color ) -> void {
			const auto draw_list = ImGui::GetBackgroundDrawList( );
			// top left -> bottom right
			render.line( draw_list, c_vector_2d( position.m_x - size, position.m_y - size ), c_vector_2d( position.m_x + size, position.m_y + size ),
			             color, 2.f );

			// bottom left -> top right
			render.line( draw_list, c_vector_2d( position.m_x - size, position.m_y + size ), c_vector_2d( position.m_x + size, position.m_y - size ),
			             color, 2.f );
		};

		auto screen_size = c_vector_2d( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );
		auto position    = c_vector_2d( screen_size.m_x / 2.f, screen_size.m_y / 2.f );

		draw_cross( position, 6, variables.misc.events.hitmarker_color.adjust_alpha( alpha ) );
	}( );

	/* custom scope */
	[ & ]( ) {
		if ( !variables.visuals.local.remove_scope )
			return;

		if ( !globals.m_local->is_scoped( ) )
			return;

		const auto active_weapon = globals.m_local->get_active_weapon( );
		if ( !active_weapon )
			return;

		const auto weapon_type = active_weapon->get_weapon_type( );

		if ( weapon_type != WEAPON_TYPE_SNIPER )
			return;

		const auto draw_list   = ImGui::GetBackgroundDrawList( );
		const auto screen_size = c_vector_2d( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );

		render.rect_filled( draw_list, c_vector_2d( 0, screen_size.m_y / 2 ), c_vector_2d( screen_size.m_x, 1 ), c_color( 0, 0, 0 ) );
		render.rect_filled( draw_list, c_vector_2d( screen_size.m_x / 2, 0 ), c_vector_2d( 1, screen_size.m_y ), c_color( 0, 0, 0 ) );
	}( );
}

void misc_t::on_create_move( )
{
	/* reveal ranks */
	[ & ]( ) {
		if ( !variables.misc.helpers.reveal_ranks || input.is_key_down( VK_TAB ) )
			return;

		interfaces.m_client->dispatch_user_message( 50, 0, 0 );
	}( );

	/* clan tag */
	[ & ]( ) {
		const static auto clantag_fn = reinterpret_cast< int( __fastcall* )( const char*, const char* ) >(
			memory.m_modules[ e_module_names::engine ].find_pattern( xs( "53 56 57 8B DA 8B F9 FF 15" ) ) );

		static float next_update_time = -1.f;
		const auto current_time       = memory.m_globals->m_current_time;

		if ( current_time >= next_update_time ) {
			// TODO: make smth animated for this
			const auto str = variables.misc.helpers.clantag ? xs( "femboyhook /" ) : xs( "" );
			clantag_fn( str, str );

			// update every second
			next_update_time = current_time + 1.f;
		}
	}( );

	/* anti afk */
	[ & ]( ) {
		if ( !variables.misc.helpers.anti_afk )
			return;

		if ( globals.m_cmd->m_command_number % 2 )
			globals.m_cmd->m_buttons |= 1 << 27;
	}( );

	/* use spammer */
	[ & ]( ) {
		if ( !variables.misc.helpers.use_spammer || !variables.misc.helpers.use_spammer_key.get_key_state( ) )
			return;

		// TODO: make this a config related item
		const auto delay = 2;

		if ( globals.m_cmd->m_tick_count % delay == 1 )
			globals.m_cmd->m_buttons |= in_use;
		else
			globals.m_cmd->m_buttons &= ~in_use;
	}( );

	/* panorama blur */
	[ & ]( ) {
		static auto panorama_disable_blur = interfaces.m_console->get_convar( xs( "@panorama_disable_blur" ) );
		panorama_disable_blur->set_value( variables.misc.helpers.disable_panorama_blur );
	}( );

	/* auto zeus bot */
	[ & ]( ) {
		if ( !variables.misc.helpers.zeus_bot || !variables.misc.helpers.zeus_bot_key.get_key_state( ) )
			return;

		float max_dist  = FLT_MAX;
		int closest_ent = -1;

		for ( int i = 0; i < interfaces.m_client_entity_list->get_highest_entity_index( ); i++ ) {
			const auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( i ) );
			if ( !entity || !entity->is_alive( ) || entity->team( ) == globals.m_local->team( ) || !entity->is_player( ) || entity->is_dormant( ) )
				continue;

			const auto dist = entity->origin( ).dist_to( globals.m_local->origin( ) );
			if ( dist < max_dist ) {
				max_dist    = dist;
				closest_ent = i;
			}
		}

		if ( closest_ent == -1 )
			return;

		if ( max_dist > 150 )
			return;

		const auto target = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( closest_ent ) );
		if ( !target )
			return;

		const auto weapon = globals.m_local->get_active_weapon( );
		if ( !weapon )
			return;

		if ( weapon->item_definition_index( ) != WEAPON_TASER )
			return;

		const auto body_position = target->get_hitbox_position( 5 );
		if ( body_position == c_vector( 0, 0, 0 ) )
			return;

		if ( !globals.m_local->can_see_player( target, body_position ) )
			return;

		auto target_angle = math.calculate_angle( globals.m_local->get_eye_pos( ), body_position, globals.m_cmd->m_view_point ).clamped( );

		// dont target if we're holding attack
		if ( globals.m_cmd->m_buttons & in_attack )
			return;

		globals.m_cmd->m_view_point += target_angle.clamped( );

		globals.m_cmd->m_buttons |= in_attack;
	}( );

	/* left hand knife */
	[ & ]( ) {
		if ( !variables.misc.local.left_hand_knife ) {
			interfaces.m_console->get_convar( xs( "cl_righthand" ) )->set_value( true );
			return;
		}

		const auto weapon = globals.m_local->get_active_weapon( );
		if ( !weapon ) {
			interfaces.m_console->get_convar( xs( "cl_righthand" ) )->set_value( true );
			return;
		}

		const auto is_holding_knife = weapon->get_weapon_type( ) == WEAPON_TYPE_KNIFE;

		interfaces.m_console->get_convar( xs( "cl_righthand" ) )->set_value( !is_holding_knife );
	}( );

	/* remove sway */
	[ & ]( ) {
		if ( !variables.misc.local.remove_sway ) {
			interfaces.m_console->get_convar( xs( "cl_wpn_sway_scale" ) )
				->set_value( interfaces.m_console->get_convar( xs( "cl_wpn_sway_scale" ) )->default_value );
			return;
		}

		interfaces.m_console->get_convar( xs( "cl_wpn_sway_scale" ) )->set_value( 0.f );
	}( );

	/* fog modulation */
	[ & ]( ) {
		// ik this is the shit way but meh detected either way so LOL

		static auto fog_override = interfaces.m_console->get_convar( xs( "fog_override" ) );

		if ( !variables.visuals.world.modulate_fog ) {
			if ( fog_override->get_int( ) )
				fog_override->set_value( false );

			return;
		}

		if ( !fog_override->get_int( ) )
			fog_override->set_value( true );

		static auto fog_start = interfaces.m_console->get_convar( xs( "fog_start" ) );

		if ( fog_start->get_int( ) )
			fog_start->set_value( 0 );

		static auto fog_end = interfaces.m_console->get_convar( xs( "fog_end" ) );

		if ( fog_end->get_int( ) != ( variables.visuals.world.fog_distance * 100.f ) )
			fog_end->set_value( ( variables.visuals.world.fog_distance * 100.f ) );

		static auto fog_maxdensity = interfaces.m_console->get_convar( xs( "fog_maxdensity" ) );

		if ( fog_maxdensity->get_float( ) != variables.visuals.world.fog_density )
			fog_maxdensity->set_value( variables.visuals.world.fog_density );

		const auto color = std::vformat( xs( "{} {} {}" ), std::make_format_args( variables.visuals.world.modulate_fog_color.m_r,
		                                                                          variables.visuals.world.modulate_fog_color.m_g,
		                                                                          variables.visuals.world.modulate_fog_color.m_b ) );

		static auto fog_color = interfaces.m_console->get_convar( xs( "fog_color" ) );

		if ( strcmp( fog_color->string, color.data( ) ) )
			fog_color->set_value( color.data( ) );
	}( );
}

void misc_t::on_fire_game_event( c_game_event* event )
{
	/* headshot spoofer */
	/* thanks axine :D */
	[ & ] {
		if ( !variables.misc.events.headshot_spoofer )
			return;

		const auto attacker_id = event->get_int( xs( "attacker" ) );
		const auto player_idx  = interfaces.m_engine->get_player_for_user_id( attacker_id );
		const auto local_index = interfaces.m_engine->get_local_player( );

		// if ( player_idx == local_index )
		event->set_int( xs( "headshot" ), 1 );
	}( );
}

void misc_t::on_player_hurt( c_game_event* event )
{
	const auto user_id = interfaces.m_engine->get_player_for_user_id( event->get_int( xs( "userid" ) ) );
	const auto user    = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( user_id ) );
	const auto attacker =
		interfaces.m_client_entity_list->get_client_entity( interfaces.m_engine->get_player_for_user_id( event->get_int( xs( "attacker" ) ) ) );

	if ( !user || !attacker )
		return;

	/* attacker -> local */
	[ & ]( ) {
		if ( attacker != globals.m_local )
			return;

		/* team info - damage */
		[ & ]( ) {
			if ( user->team( ) != globals.m_local->team( ) )
				return;

			this->event_data.team_info.damage += event->get_int( xs( "dmg_health" ) );
		}( );

		/* hitsound */
		[ & ]( ) {
			if ( !variables.misc.events.hitsound )
				return;

			interfaces.m_engine->execute_command( xs( "play resource/warning.wav" ) );
		}( );

		/* hitmarker */
		[ & ]( ) {
			if ( !variables.misc.events.hitmarker )
				return;

			this->event_data.hitmarker_time = 1.5f;
		}( );
	}( );
}

void misc_t::on_player_death( c_game_event* event )
{
	const auto user_id = interfaces.m_engine->get_player_for_user_id( event->get_int( xs( "userid" ) ) );
	const auto user    = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( user_id ) );
	const auto attacker =
		interfaces.m_client_entity_list->get_client_entity( interfaces.m_engine->get_player_for_user_id( event->get_int( xs( "attacker" ) ) ) );

	if ( !user || !attacker )
		return;

	/* attacker -> local */
	[ & ]( ) {
		if ( attacker != globals.m_local )
			return;

		/* team info - kills */
		[ & ]( ) {
			if ( user->team( ) != globals.m_local->team( ) )
				return;

			this->event_data.team_info.kills += 1;
		};

		/* killsay */
		[ & ]( ) {
			if ( !variables.misc.events.kill_say )
				return;

			const std::vector< std::string > messages = { xs( "You just got filled up UwU" ),    xs( "Get creampie'd -w-" ),
				                                          xs( "Cute ass, I just fucked it :3" ), xs( "fbh da fuck up cuh" ),
				                                          xs( "shitter got clapped" ),           xs( "ur third world" ) };

			interfaces.m_engine->execute_command(
				std::vformat( xs( "say {}" ), std::make_format_args( messages.at( rand( ) % messages.size( ) ) ) ).data( ) );
		}( );

		/* kill effect - sparks */
		[ & ]( ) {
			if ( !variables.misc.events.kill_effect )
				return;

			const auto position = user->get_bone_position( 8 );
			interfaces.m_effects->sparks( position, 2 );
		}( );
	}( );
}

void misc_t::on_attach( )
{
	Discord_Initialize( xs( "1057805586487574578" ), NULL, 1, xs( "730" ) );
}

void misc_t::on_release( )
{
	Discord_ClearPresence( );
	Discord_Shutdown( );
}