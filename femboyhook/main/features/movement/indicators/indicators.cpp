#include "indicators.h"
#include "../../particle_system/particle_effects.h"
#include "../../prediction/prediction.h"
#include "../movement.h"

const int max_speed         = 365;
const int max_stamina       = 40;
const int graph_height      = 150;
const int graph_items       = 220;
const int graph_width       = 200;
const int graph_compression = 2;

void indicators_t::on_create_move( )
{
	if ( !interfaces.m_engine->is_in_game( ) || !globals.m_local || !globals.m_local->is_alive( ) )
		return;

	const auto speed   = std::floor( std::min( 10000, ( int )prediction_system.pre.velocity_len ) );
	const auto stamina = std::floor( std::min( 100.f, ( float )prediction_system.pre.stamina_len ) );

	this->speed_history.push_back( { ( int )speed, movement.movement_data.edgebug.should_indicate_success, movement.movement_data.successful_jb,
	                                 movement.movement_data.pixelsurf.should_indicate_success,
	                                 movement.movement_data.should_indicate_edgejump_success, movement.movement_data.should_indicate_minijump_success,
	                                 movement.movement_data.should_indicate_longjump_success,
	                                 movement.movement_data.should_indicate_stamina_hop_success,
	                                 movement.movement_data.should_indicate_ladderbug_success, movement.movement_data.should_indicate_fireman_success

	} );

	this->stamina_history.push_back( stamina );

	if ( this->speed_history.size( ) > graph_items ) {
		this->speed_history.erase( this->speed_history.begin( ) );
		this->speed_history.pop_back( );
	}

	if ( this->stamina_history.size( ) > graph_items ) {
		this->stamina_history.erase( this->stamina_history.begin( ) );
		this->stamina_history.pop_back( );
	}
}

void indicators_t::on_end_scene( )
{
	if ( !interfaces.m_engine->is_in_game( ) || !globals.m_local || !globals.m_local->is_alive( ) )
		return;

	/* stamina graph */
	[ & ]( ) {
		if ( !variables.visuals.indicators.stamina_graph )
			return;

		const auto draw_list   = ImGui::GetBackgroundDrawList( );
		const auto screen_size = c_vector_2d( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );

		const int x = screen_size.m_x / 2;

		int index        = -1;
		int last_stamina = -1;

		for ( const auto& stamina : this->stamina_history ) {
			index++;
			if ( index != 0 ) {
				int cur_x  = ( index / ( float )graph_items ) * graph_width + x - ( graph_width / 2 );
				int last_x = ( ( index - 1 ) / ( float )graph_items ) * graph_width + x - ( graph_width / 2 );

				int cur_y = sqrt( ( ( ( stamina / ( float )max_stamina ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) *
				                  ( ( ( stamina / ( float )max_stamina ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) );
				int last_y =
					sqrt( ( ( ( last_stamina / ( float )max_stamina ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) *
				          ( ( ( last_stamina / ( float )max_stamina ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) );

				const auto config_color = variables.visuals.indicators.stamina_graph_color;
				c_color color =
					( index <= graph_items - 25 )
						?
						/*left side*/ config_color.adjust_alpha( ( std::clamp( index, 1, 25 ) * 10 ) / 255.f )
						:
						/*right side*/ config_color.adjust_alpha( ( ( 25 - std::clamp( index - ( graph_items - 25 ), 1, 25 ) ) * 10 ) / 255.f );

				render.line( draw_list, c_vector_2d( cur_x, cur_y ), c_vector_2d( last_x, last_y ), color );
			}
			last_stamina = stamina;
		}
	}( );

	/* velocity graph */
	[ & ]( ) {
		if ( !variables.visuals.indicators.velocity_graph )
			return;

		const auto draw_list   = ImGui::GetBackgroundDrawList( );
		const auto screen_size = c_vector_2d( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );

		const int x = screen_size.m_x / 2;

		int index = -1;
		velocity_data_t last_tick;

		for ( const auto& current : this->speed_history ) {
			index++;
			if ( index != 0 ) {
				int cur_x  = ( index / ( float )graph_items ) * graph_width + x - ( graph_width / 2 );
				int last_x = ( ( index - 1 ) / ( float )graph_items ) * graph_width + x - ( graph_width / 2 );

				int cur_y =
					sqrt( ( ( ( current.velocity / ( float )max_speed ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) *
				          ( ( ( current.velocity / ( float )max_speed ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) );
				int last_y = sqrt(
					( ( ( last_tick.velocity / ( float )max_speed ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) *
					( ( ( last_tick.velocity / ( float )max_speed ) * graph_height / ( int )graph_compression ) - ( screen_size.m_y / 1.4f ) ) );

				const auto config_color = variables.visuals.indicators.velocity_graph_color;
				c_color color =
					( index <= graph_items - 25 )
						?
						/*left side*/ config_color.adjust_alpha( ( std::clamp( index, 1, 25 ) * 10 ) / 255.f )
						:
						/*right side*/ config_color.adjust_alpha( ( ( 25 - std::clamp( index - ( graph_items - 25 ), 1, 25 ) ) * 10 ) / 255.f );

				render.line( draw_list, c_vector_2d( cur_x, cur_y ), c_vector_2d( last_x, last_y ), color );
				if ( variables.visuals.indicators.include_on_graph ) {
					if ( current.edgebugged && variables.visuals.indicators.chat_logs & ( 1 << 0 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "eb" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.jumpbugged && variables.visuals.indicators.chat_logs & ( 1 << 1 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "jb" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.pixelsurfed && variables.visuals.indicators.chat_logs & ( 1 << 2 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "ps" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.edgejumped && variables.visuals.indicators.chat_logs & ( 1 << 3 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "ej" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.longjump && variables.visuals.indicators.chat_logs & ( 1 << 3 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "lj" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.minijumped && variables.visuals.indicators.chat_logs & ( 1 << 4 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "mj" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.ladderglidded && variables.visuals.indicators.chat_logs & ( 1 << 5 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "lg" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.staminahopped && variables.visuals.indicators.chat_logs & ( 1 << 6 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "sh" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

					if ( current.firemaned && variables.visuals.indicators.chat_logs & ( 1 << 7 ) )
						render.text( draw_list, render.m_flags_font, c_vector_2d( cur_x + 2, cur_y - 10 ), xs( "af" ), color,
						             c_color( 0, 0, 0 ).adjust_alpha( color.m_a / 255.f ),
						             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );
				}
			}
			last_tick = current;
		}
	}( );

	/* velocity & stamina indicator */
	[ & ]( ) {
		if ( !variables.visuals.indicators.velocity && !variables.visuals.indicators.stamina )
			return;

		auto velocity_str = std::vformat( xs( "{}" ), std::make_format_args( prediction_system.post.velocity_len ) );
		auto stamina_str  = std::vformat( xs( "{:.1f}" ), std::make_format_args( prediction_system.post.stamina_len ) );

		static float pre_speed, pre_stamina, last_set;
		static bool on_ground;

		if ( variables.visuals.indicators.previous_speeds ) {
			if ( !( globals.m_local->flags( ) & fl_on_ground ) && on_ground ) {
				pre_speed   = prediction_system.post.velocity_len;
				pre_stamina = prediction_system.post.stamina_len;
				last_set    = memory.m_globals->m_current_time + 3.f;
			}

			on_ground = globals.m_local->flags( ) & fl_on_ground;

			if ( last_set > memory.m_globals->m_current_time ) {
				velocity_str += std::vformat( xs( " ({:.0f})" ), std::make_format_args( pre_speed ) );
				stamina_str += std::vformat( xs( " ({:.1f})" ), std::make_format_args( pre_stamina ) );
			}
		}

		const auto draw_list   = ImGui::GetBackgroundDrawList( );
		const auto screen_size = c_vector_2d( ImGui::GetIO( ).DisplaySize.x, ImGui::GetIO( ).DisplaySize.y );

		const auto velocity_str_size = render.measure_text( render.m_indicator_fonts[ variables.visuals.indicators.font ], velocity_str.data( ) );
		const auto velocity_position =
			c_vector_2d( screen_size.m_x / 2 - velocity_str_size.m_x / 2, screen_size.m_y / 1.4f + +variables.visuals.indicators.offset );

		const auto stamina_str_size = render.measure_text( render.m_indicator_fonts[ variables.visuals.indicators.font ], stamina_str.data( ) );
		const auto stamina_position = c_vector_2d( screen_size.m_x / 2 - stamina_str_size.m_x / 2,
		                                           screen_size.m_y / 1.4f + velocity_str_size.m_y + 2.f + +variables.visuals.indicators.offset );

		const auto velocity_adjustment = std::clamp( prediction_system.pre.velocity_len / 325.f, 0.f, 1.f );
		const auto stamina_adjustment  = std::clamp( prediction_system.pre.stamina_len / 35.f, 0.f, 1.f );

		// velocity
		if ( variables.visuals.indicators.velocity )
			render.text( draw_list, render.m_indicator_fonts[ variables.visuals.indicators.font ], velocity_position, velocity_str,
			             c_color::blend( c_color( 255, 255, 255, 0 ), variables.visuals.indicators.velocity_color, velocity_adjustment ),
			             c_color( 0, 0, 0 ).adjust_alpha( velocity_adjustment ),
			             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

		// stamina
		if ( variables.visuals.indicators.stamina )
			render.text( draw_list, render.m_indicator_fonts[ variables.visuals.indicators.font ], stamina_position, stamina_str,
			             c_color::blend( c_color( 255, 255, 255, 0 ), variables.visuals.indicators.stamina_color, stamina_adjustment ),
			             c_color( 0, 0, 0 ).adjust_alpha( stamina_adjustment ),
			             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );
	}( );

	// redacted.
	//particle_effects.handle_simulated_effects( );

	/* indicators */
	[ & ]( ) {
		static std::map< uintptr_t, float > indicator_hits;
		static std::map< uintptr_t, float > indicator_states;
		int offset           = 0;
		const auto draw_list = ImGui::GetBackgroundDrawList( );
		auto push_indicator  = [ & ]( const std::string& label, bool should_show, bool& hit_detection ) -> void {
            const auto screen_size = ImGui::GetIO( ).DisplaySize;

            auto hit_anim = indicator_hits.find( std::hash< std::string >( )( label ) );
			if ( hit_anim == indicator_hits.end( ) ) {
                indicator_hits.emplace( std::hash< std::string >( )( label ), 0.f );
				hit_anim = indicator_hits.find( std::hash< std::string >( )( label ) );
            }

			 auto state_anim = indicator_states.find( std::hash< std::string >( )( label ) );
			if ( state_anim == indicator_states.end( ) ) {
				 indicator_states.emplace( std::hash< std::string >( )( label ), 0.f );
				state_anim = indicator_states.find( std::hash< std::string >( )( label ) );
			}

            hit_anim->second -= 0.01f * memory.m_globals->m_frame_time;
			state_anim->second =
				ImClamp( state_anim->second + ( 3.f * ( 1.f / 0.3f ) * ImGui::GetIO( ).DeltaTime * ( should_show ? 1.f : -1.f ) ), 0.f, 1.f );

            const auto text_size = render.measure_text( render.m_indicator_fonts[ variables.visuals.indicators.font ], label.data( ) );
            const auto position  = c_vector_2d( screen_size.x / 2 - text_size.m_x / 2,
			                                     screen_size.y / 1.4f + ( text_size.m_y * 2 ) + offset + variables.visuals.indicators.offset - (text_size.m_y * (1.f - state_anim->second)) );

            if ( state_anim->second > 0.1f || hit_anim->second > 0.1f ) {
                render.text( draw_list, render.m_indicator_fonts[ variables.visuals.indicators.font ], position, label,
				             c_color( 255, 255, 255 ).adjust_alpha( state_anim->second ), c_color( 0, 0, 0 ).adjust_alpha( state_anim->second ),
                             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );
                render.text( draw_list, render.m_indicator_fonts[ variables.visuals.indicators.font ], position, label,
				             variables.visuals.indicators.keybinds_color.adjust_alpha( hit_anim->second ),
				             c_color( 0, 0, 0 ).adjust_alpha( hit_anim->second ),
                             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );
            }

            if ( hit_detection && should_show ) {
				hit_anim->second = 1.f;
                /*particle_effects.simulate_explosion( ImVec2( position.m_x, position.m_y ) + ImVec2( text_size.m_x / 2, text_size.m_y / 2 ),
				                                      ImColor( variables.visuals.indicators.keybinds_color.m_r,
				                                               variables.visuals.indicators.keybinds_color.m_g,
				                                               variables.visuals.indicators.keybinds_color.m_b ),
				                                      70, 1.f, 0.8f, 20 );*/

                if ( variables.visuals.indicators.chat_logs & ( 1 << 0 ) && label == xs( "eb" ) ) {
                    this->indicator_time = 2.f;

                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_eb" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 1 ) && label == xs( "jb" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_jb" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 2 ) && label == xs( "ps" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_ps" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 3 ) && label == xs( "ej" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_ej" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 4 ) && label == xs( "lj" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_lj" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 5 ) && label == xs( "mj" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_mj" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 6 ) && label == xs( "lg" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_lg" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 7 ) && label == xs( "sh" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_sh" ) );
                }

                if ( variables.visuals.indicators.chat_logs & ( 1 << 8 ) && label == xs( "af" ) ) {
                    interfaces.m_hud_chat->chat_printf( xs( "#fbh#_print_af" ) );
                }

                hit_detection = false;
            }

            if ( state_anim->second > 0.1f || hit_anim->second > 0.1f )
                offset += (text_size.m_y * state_anim->second);
		};

		const auto should_show_jb =
			variables.movement.jumpbug && variables.movement.jumpbug_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 0 );
		push_indicator( xs( "jb" ), should_show_jb, movement.movement_data.successful_jb );

		const auto should_show_mj =
			variables.movement.minijump && variables.movement.minijump_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 3 );
		push_indicator( xs( "mj" ), should_show_mj, movement.movement_data.should_indicate_minijump_success );

		const auto should_show_ej =
			variables.movement.edgejump && variables.movement.edgejump_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 4 );
		push_indicator( xs( "ej" ), should_show_ej, movement.movement_data.should_indicate_edgejump_success );

		const auto should_show_lj =
			variables.movement.longjump && variables.movement.longjump_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 5 );
		push_indicator( xs( "lj" ), should_show_lj, movement.movement_data.should_indicate_longjump_success );

		const auto should_show_ps =
			variables.movement.pixelsurf && variables.movement.pixelsurf_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 2 );
		push_indicator( xs( "ps" ), should_show_ps, movement.movement_data.pixelsurf.should_indicate_success );

		const auto should_show_eb =
			variables.movement.edgebug && variables.movement.edgebug_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 1 );
		push_indicator( xs( "eb" ), should_show_eb, movement.movement_data.edgebug.should_indicate_success );

		const auto should_show_lg =
			variables.movement.ladderbug && variables.movement.ladderbug_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 7 );
		push_indicator( xs( "lg" ), should_show_lg, movement.movement_data.should_indicate_ladderbug_success );

		const auto should_show_sh = variables.movement.stamina_hop && variables.movement.stamina_hop_key.get_key_state( ) &&
		                            variables.visuals.indicators.keybinds & ( 1 << 5 );
		push_indicator( xs( "sh" ), should_show_sh, movement.movement_data.should_indicate_stamina_hop_success );

		const auto should_show_af =
			variables.movement.fireman && variables.movement.fireman_key.get_key_state( ) && variables.visuals.indicators.keybinds & ( 1 << 8 );
		push_indicator( xs( "af" ), should_show_af, movement.movement_data.should_indicate_fireman_success );
	}( );

	/* key states */
	[ & ]( ) {
		if ( !( variables.visuals.indicators.keybinds & ( 1 << 9 ) ) )
			return;

		static bool did_press_w, did_press_a, did_press_d, did_press_s, did_press_jump, did_press_crouch;

		const auto screen_size = ImGui::GetIO( ).DisplaySize;
		const auto draw_list   = ImGui::GetBackgroundDrawList( );
		const auto font        = render.m_indicator_fonts[ variables.visuals.indicators.font ];

		const auto full_size = render.measure_text( font, xs( "wasd" ) );
		const auto w_size = render.measure_text( font, xs( "w" ) );
		const auto a_size = render.measure_text( font, xs( "a" ) );
		const auto s_size = render.measure_text( font, xs( "s" ) );
		const auto d_size = render.measure_text( font, xs( "d" ) );

		const auto w_pos = c_vector_2d( screen_size.x / 2 - (full_size.m_x/2), screen_size.y / 1.6f );
		const auto a_pos = c_vector_2d( screen_size.x / 2 - (full_size.m_x/2) + w_size.m_x, screen_size.y / 1.6f );
		const auto s_pos = c_vector_2d( screen_size.x / 2 - (full_size.m_x/2) + w_size.m_x + a_size.m_x, screen_size.y / 1.6f );
		const auto d_pos = c_vector_2d( screen_size.x / 2 - (full_size.m_x/2) + w_size.m_x + a_size.m_x + s_size.m_x, screen_size.y / 1.6f );

		if ( globals.m_cmd->m_buttons & in_forward && !did_press_w ) {
			/*particle_effects.simulate_explosion( ImVec2( w_pos.m_x, w_pos.m_y ) + ImVec2( w_size.m_x / 2, w_size.m_y / 2 ),
			                                     ImColor( variables.visuals.indicators.keybinds_color.m_r,
			                                              variables.visuals.indicators.keybinds_color.m_g,
			                                              variables.visuals.indicators.keybinds_color.m_b ),
			                                     70, 1.f, 0.4f, 15 );*/
			did_press_w = true;
		} else if ( !( globals.m_cmd->m_buttons & in_forward ) )
			did_press_w = false;

		if ( globals.m_cmd->m_buttons & in_move_left && !did_press_a ) {
		/*	particle_effects.simulate_explosion( ImVec2( a_pos.m_x, a_pos.m_y ) + ImVec2( a_size.m_x / 2, a_size.m_y / 2 ),
			                                     ImColor( variables.visuals.indicators.keybinds_color.m_r,
			                                              variables.visuals.indicators.keybinds_color.m_g,
			                                              variables.visuals.indicators.keybinds_color.m_b ),
			                                     70, 1.f, 0.4f, 15 );*/
			did_press_a = true;
		} else if ( !( globals.m_cmd->m_buttons & in_move_left ) )
			did_press_a = false;

		if ( globals.m_cmd->m_buttons & in_move_right && !did_press_d ) {
			/*particle_effects.simulate_explosion( ImVec2( d_pos.m_x, d_pos.m_y ) + ImVec2( d_size.m_x / 2, d_size.m_y / 2 ),
			                                     ImColor( variables.visuals.indicators.keybinds_color.m_r,
			                                              variables.visuals.indicators.keybinds_color.m_g,
			                                              variables.visuals.indicators.keybinds_color.m_b ),
			                                     70, 1.f, 0.4f, 15 );*/
			did_press_d = true;
		} else if ( !( globals.m_cmd->m_buttons & in_move_right ) )
			did_press_d = false;

		if ( globals.m_cmd->m_buttons & in_back && !did_press_s ) {
			/*particle_effects.simulate_explosion( ImVec2( s_pos.m_x, s_pos.m_y ) + ImVec2( s_size.m_x / 2, s_size.m_y / 2 ),
			                                     ImColor( variables.visuals.indicators.keybinds_color.m_r,
			                                              variables.visuals.indicators.keybinds_color.m_g,
			                                              variables.visuals.indicators.keybinds_color.m_b ),
			                                     70, 1.f, 0.4f, 15 );*/
			did_press_s = true;
		} else if ( !( globals.m_cmd->m_buttons & in_back ) )
			did_press_s = false;

		render.text( draw_list, font, w_pos, xs( "w" ),
		             did_press_w ? variables.visuals.indicators.keybinds_color : c_color( 255, 255, 255 ), c_color( 0, 0, 0 ),
		             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

		render.text( draw_list, font, a_pos, xs( "a" ),
		             did_press_a ? variables.visuals.indicators.keybinds_color : c_color( 255, 255, 255 ), c_color( 0, 0, 0 ),
		             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

		render.text( draw_list, font, s_pos, xs( "s" ),
		             did_press_s ? variables.visuals.indicators.keybinds_color : c_color( 255, 255, 255 ), c_color( 0, 0, 0 ),
		             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );

		render.text( draw_list, font, d_pos, xs( "d" ),
		             did_press_d ? variables.visuals.indicators.keybinds_color : c_color( 255, 255, 255 ), c_color( 0, 0, 0 ),
		             variables.visuals.indicators.font_shadow ? text_render_flag_dropshadow : text_render_flag_none );
	}( );
}