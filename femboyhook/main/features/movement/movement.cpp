#include "movement.h"

#include "../prediction/prediction.h"

void movement_t::on_attach( ) { }

void movement_t::on_release( ) { }

bool movement_t::has_invalid_movetypes( )
{
	return ( prediction_system.post.movetype == move_type_ladder || prediction_system.post.movetype == move_type_noclip );
}

bool movement_t::has_invalid_predicted_movetypes( )
{
	return ( prediction_system.predicted.movetype == move_type_ladder || prediction_system.predicted.movetype == move_type_noclip );
}

float movement_t::get_magic_value( )
{
	const auto sv_gravity = interfaces.m_console->get_convar( xs( "sv_gravity" ) )->get_float( );

	return -sv_gravity * memory.m_globals->m_interval_per_tick * 0.5f;
}

void movement_t::on_pre_create_move( )
{
	/* bunny hop */
	[ & ]( ) {
		if ( !variables.movement.bunny_hop )
			return;

		/* TODO ~ check for sv_autobunnyhopping 1 */

		if ( variables.movement.jumpbug && variables.movement.jumpbug_key.get_key_state( ) )
			return;

		if ( globals.m_local->move_type( ) == move_type_ladder || globals.m_local->move_type( ) == move_type_noclip )
			return;

		if ( !( globals.m_local->flags( ) & e_entity_flags::fl_on_ground ) )
			globals.m_cmd->m_buttons &= ~e_command_buttons::in_jump;
	}( );

	/* infinite duck */
	[ & ]( ) {
		if ( !variables.movement.infinite_duck )
			return;

		globals.m_cmd->m_buttons |= e_command_buttons::in_bullrush;
	}( );
}

void movement_t::on_post_create_move( )
{
	/* edgejump */
	[ & ]( ) {
		if ( !variables.movement.edgejump || !variables.movement.edgejump_key.get_key_state( ) ) {
			this->movement_data.should_indicate_edgejump_success = false;
			return;
		}

		const auto pre_flags  = prediction_system.pre.flags;
		const auto post_flags = globals.m_local->flags( );

		if ( ( pre_flags & fl_on_ground ) && !( post_flags & fl_on_ground ) ) {
			// we were holding the jump key while holding edgejump
			const auto did_previously_jump = ( globals.m_cmd->m_buttons & in_jump );

			// jump
			globals.m_cmd->m_buttons |= in_jump;

			interfaces.m_engine->execute_command( xs( "-forward" ) );

			// if we didn't jump before jumping we've automatically jumped meaning we've edgejumped correctly.
			if ( !did_previously_jump ) {
				this->movement_data.should_indicate_edgejump_success = true;
			}
		}
	}( );

	/* minijump */
	[ & ]( ) {
		if ( !variables.movement.minijump || !variables.movement.minijump_key.get_key_state( ) ) {
			this->movement_data.should_indicate_minijump_success = false;
			return;
		}

		const auto pre_flags = prediction_system.pre.flags;

		if ( ( pre_flags & fl_on_ground ) && !( globals.m_local->flags( ) & fl_on_ground ) ) {
			globals.m_cmd->m_buttons |= in_duck;
			this->movement_data.should_indicate_minijump_success = true;
		}
	}( );

	/* jumpbug detection */
	[ & ]( ) {
		const auto pre_flags = prediction_system.pre.flags;

		if ( !globals.m_local->is_alive( ) || pre_flags & fl_on_ground || this->has_invalid_movetypes( ) ||
		     this->has_invalid_predicted_movetypes( ) || globals.m_local->observer_target( ) == 1 ) {
			this->movement_data.successful_jb = false;
			return;
		} else if ( ( globals.m_local->velocity( ).m_z > prediction_system.pre.fall_velocity ) && !( pre_flags & fl_on_ground ) &&
		            !( globals.m_local->flags( ) & fl_on_ground ) && !this->movement_data.edgebug.should_hit && variables.movement.jumpbug &&
		            variables.movement.jumpbug_key.get_key_state( ) && !( globals.m_local->velocity( ).m_z == -7.f ) &&
		            !( globals.m_local->velocity( ).m_z == this->get_magic_value( ) ) ) { //-6.25f
			this->movement_data.successful_jb = true;
		} else {
			this->movement_data.successful_jb = false;
		}
	}( );

	/* jumpbug */
	[ & ]( ) {
		if ( !variables.movement.jumpbug || !variables.movement.jumpbug_key.get_key_state( ) )
			return;

		if ( this->has_invalid_movetypes( ) )
			return;

		const auto pre_flags = prediction_system.pre.flags;
		if ( !( pre_flags & fl_on_ground ) && globals.m_local->flags( ) & fl_on_ground ) {
			globals.m_cmd->m_buttons |= in_duck;
			globals.m_cmd->m_buttons &= ~in_jump;
		}
	}( );

	/* edgebug */
	[ & ]( ) {
		if ( !variables.movement.edgebug || !variables.movement.edgebug_key.get_key_state( ) ) {
			return;
		}

		if ( this->movement_data.edgebug.should_hit || this->movement_data.pixelsurf.surfing ) {
			return;
		}

		if ( this->has_invalid_movetypes( ) || !globals.m_local->in_air( ) || !globals.m_local->falling( ) ) {
			return;
		}

		for ( int scenarios = 0; scenarios < 4; scenarios++ ) {
			if ( this->movement_data.edgebug.should_hit )
				break;

			interfaces.m_prediction->repredict( );

			int flags         = globals.m_local->flags( );
			c_vector velocity = globals.m_local->velocity( );

			for ( int i = 0; i < variables.movement.edgebug_ticks; i++ ) {
				c_user_cmd* scenario       = new c_user_cmd( *globals.m_cmd );
				const auto player_scenario = *&globals.m_local;

				scenario->m_side_move    = 0.f;
				scenario->m_forward_move = 0.f;

				if ( ( scenarios % 2 ) )
					scenario->m_buttons |= in_duck;
				else
					scenario->m_buttons &= ~in_duck;

				if ( flags & fl_on_ground ) {
					delete scenario;
					break;
				}

				const auto was_never_on_ground = !( globals.m_local->flags( ) & fl_on_ground ) && !( flags & fl_on_ground );
				const auto velocity_is_ideal = globals.m_local->velocity( ).m_z == this->get_magic_value( );
				const auto was_going_up_last_tick = velocity.m_z >= 0.f;
				const auto is_xy_velocity_ideal =
					globals.m_local->velocity( ).length_2d( ) > 5.f && globals.m_local->velocity( ).length_2d( ) - velocity.length_2d( ) < 5.f;

				this->movement_data.edgebug.predicted_position = player_scenario->origin( );
				if ( prediction_system.simulate( player_scenario, scenario ) ) {
					if ( was_never_on_ground && velocity_is_ideal && is_xy_velocity_ideal && !was_going_up_last_tick &&
					     globals.m_local->velocity( ).m_z < 19.f ) {
						this->movement_data.edgebug.tick_count  = memory.m_globals->m_tick_count + i;
						this->movement_data.edgebug.should_duck = ( scenarios % 2 );
						this->movement_data.edgebug.current_combo++;
						this->movement_data.edgebug.should_hit = true;

					}
				} else if ( ( flags & fl_on_ground ) || player_scenario->move_type( ) == move_type_ladder ) {
					delete scenario, player_scenario;
					break;
				}

				delete scenario, player_scenario;

				velocity = globals.m_local->velocity( );
				flags    = globals.m_local->flags( );
			}
			prediction_system.end_simulate( globals.m_local );
		}
	}( );

	/* edgebug lock */
	[ & ]( ) {
		if ( !variables.movement.edgebug || !variables.movement.edgebug_key.get_key_state( ) )
			this->movement_data.edgebug.should_indicate_success = false;

		if ( this->movement_data.edgebug.should_hit ) {
			interfaces.m_prediction->repredict( );
			if ( globals.m_local->flags( ) & fl_on_ground ||
			     this->movement_data.edgebug.tick_count < globals.m_cmd->m_tick_count && !this->movement_data.edgebug.should_indicate_success ) {
				this->movement_data.edgebug.should_indicate_success = true;
				if ( variables.visuals.indicators.edgebug_hitsound )
					interfaces.m_engine->execute_command( xs( "playvol resource/warning.wav 0.5" ) );

				this->movement_data.edgebug.should_hit = false;
				return;
			}

			globals.m_cmd->m_side_move    = 0.f;
			globals.m_cmd->m_forward_move = 0.f;

			if ( this->movement_data.edgebug.should_duck ) {
				globals.m_cmd->m_buttons |= in_duck;
			} else {
				globals.m_cmd->m_buttons &= ~in_duck;
			}

			return;
		}
	}( );

	/* edgebug combo */
	[ & ]( ) {
		const auto pre_flags  = prediction_system.pre.flags;
		const auto pred_flags = prediction_system.predicted.flags;
		const auto post_flags = prediction_system.post.flags;
		if ( pre_flags & fl_on_ground || pred_flags & fl_on_ground || post_flags & fl_on_ground || !variables.movement.edgebug || !variables.movement.edgebug_key.get_key_state() )
			this->movement_data.edgebug.current_combo = 0;
	}( );

	/* blockbot */
	[ & ]( ) {
		if ( !variables.movement.blockbot || !variables.movement.blockbot_key.get_key_state( ) )
			return;

		if ( globals.m_local->move_type( ) == move_type_ladder || globals.m_local->move_type( ) == move_type_noclip )
			return;

		float best_dist = std::numeric_limits<float>::max();
		int best_target = -1;
		for ( int i = 1; i < memory.m_globals->m_max_clients; i++ ) {
			auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( i ) );
			if ( !entity || entity->is_dormant( ) || !entity->is_alive( ) || entity == globals.m_local )
				continue;

			float dist = globals.m_local->origin( ).dist_to( entity->origin( ) );
			if ( dist < best_dist ) {
				best_dist   = dist;
				best_target = i;
			}
		}

		auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( best_target ) );
		if ( !entity )
			return;

		const float speed  = entity->velocity( ).length( );
		const auto forward = entity->origin( ) - globals.m_local->origin( );

		// head
		if ( entity->get_bone_position( 6 ).m_z < globals.m_local->origin( ).m_z &&
		     globals.m_local->origin( ).dist_to( entity->origin( ) ) < 100.f ) {
			globals.m_cmd->m_forward_move = ( ( sin( math.deg_to_rad( globals.m_cmd->m_view_point.m_y ) ) * forward.m_y ) +
			                                  ( cos( math.deg_to_rad( globals.m_cmd->m_view_point.m_y ) ) * forward.m_x ) ) *
			                                speed;
			globals.m_cmd->m_side_move = ( ( cos( math.deg_to_rad( globals.m_cmd->m_view_point.m_y ) ) * -forward.m_y ) +
			                               ( sin( math.deg_to_rad( globals.m_cmd->m_view_point.m_y ) ) * forward.m_x ) ) *
			                             speed;

		} else {
			auto yaw_delta = ( atan2( forward.m_y, forward.m_x ) * 180.0f / 3.14159265359f ) - globals.m_cmd->m_view_point.m_y;
			if ( yaw_delta > 180 ) {
				yaw_delta -= 360;
			} else if ( yaw_delta < -180 ) {
				yaw_delta += 360;
			}
			if ( yaw_delta > 0.25 ) {
				globals.m_cmd->m_side_move = -speed;
			} else if ( yaw_delta < -0.25 ) {
				globals.m_cmd->m_side_move = speed;
			}
		}
	}( );

	// redacted.
	/* auto align */
	[ & ]( ) {

	}( );

	/* pixelsurf */
	[ & ]( ) {
		if ( !variables.movement.pixelsurf || !variables.movement.pixelsurf_key.get_key_state( ) )
			return;

		if ( this->movement_data.pixelsurf.surfing || this->movement_data.edgebug.should_hit )
			return;

		if ( variables.movement.jumpbug && variables.movement.jumpbug_key.get_key_state( ) )
			return;

		if ( variables.movement.edgebug && variables.movement.edgebug_key.get_key_state( ) )
			return;

		if ( this->has_invalid_movetypes( ) || !globals.m_local->in_air( ) )
			return;

		for ( int scenarios = 0; scenarios < 2; scenarios++ ) {
			if ( this->movement_data.pixelsurf.surfing )
				break;

			interfaces.m_prediction->repredict( );

			int flags         = globals.m_local->flags( );
			c_vector velocity = globals.m_local->velocity( );

			for ( int i = 0; i < variables.movement.pixelsurf_ticks; i++ ) {
				c_user_cmd* scenario = new c_user_cmd( *globals.m_cmd );

				if ( scenarios == 0 ) {
					scenario->m_buttons |= in_duck;
				} else {
					scenario->m_buttons &= ~in_duck;
				}

				if ( flags & fl_on_ground ) {
					delete scenario;
					break;
				}

				if ( prediction_system.simulate( globals.m_local, scenario ) ) {
					if ( velocity.m_z == -6.25f && globals.m_local->velocity( ).m_z == -6.25f ) {
						this->movement_data.pixelsurf.tick         = globals.m_cmd->m_tick_count + i;
						this->movement_data.pixelsurf.view_point   = globals.m_cmd->m_view_point;
						this->movement_data.pixelsurf.forward_move = globals.m_cmd->m_forward_move;
						this->movement_data.pixelsurf.side_move    = globals.m_cmd->m_side_move;
						this->movement_data.pixelsurf.user_cmd     = scenario;
						this->movement_data.pixelsurf.should_duck  = ( scenarios == 0 );
						this->movement_data.pixelsurf.surfing      = true;
						delete scenario;
						break;
					}
				}

				delete scenario;

				flags    = globals.m_local->flags( );
				velocity = globals.m_local->velocity( );
			}
			prediction_system.end_simulate( globals.m_local );
		}
	}( );

	/* pixelsurf lock */
	[ & ]( ) {
		if ( !variables.movement.pixelsurf || !variables.movement.pixelsurf_key.get_key_state( ) )
			this->movement_data.pixelsurf.should_indicate_success = false;

		if ( this->movement_data.pixelsurf.surfing ) {
			interfaces.m_prediction->repredict( );
			if ( !this->movement_data.pixelsurf.predicted ) {
				if ( this->movement_data.pixelsurf.tick < globals.m_cmd->m_tick_count ) {
					this->movement_data.pixelsurf.should_indicate_success = true;
					if ( variables.visuals.indicators.pixelsurf_hitsound )
						interfaces.m_engine->execute_command( xs( "playvol resource/warning.wav 0.5" ) );
					this->movement_data.pixelsurf.predicted = true;
				}

				// globals.m_cmd->m_buttons      = this->movement_data.pixelsurf.user_cmd->m_buttons;
				globals.m_cmd->m_forward_move = this->movement_data.pixelsurf.user_cmd->m_forward_move;
				globals.m_cmd->m_side_move    = this->movement_data.pixelsurf.user_cmd->m_side_move;
				globals.m_cmd->m_up_move      = this->movement_data.pixelsurf.user_cmd->m_up_move;
				globals.m_cmd->m_view_point   = this->movement_data.pixelsurf.user_cmd->m_view_point;
			}

			if ( this->movement_data.pixelsurf.should_duck ) {
				globals.m_cmd->m_buttons |= in_duck;
			} else {
				globals.m_cmd->m_buttons &= ~in_duck;
			}

			if ( variables.movement.pixelsurf_free_look ) {
				globals.m_cmd->m_view_point = this->movement_data.pixelsurf.view_point;

				if ( globals.m_local->velocity( ).length_2d( ) <= 1.f ) {
					this->movement_data.pixelsurf.surfing = false;
				}
			}

			if ( variables.movement.pixelsurf_hold_needed_keys ) {
				globals.m_cmd->m_forward_move = this->movement_data.pixelsurf.forward_move;
				globals.m_cmd->m_side_move    = this->movement_data.pixelsurf.side_move;
			}

			if ( variables.visuals.indicators.pixelsurf_line ) {
				static float last_update_time = -1.f;
				if ( memory.m_globals->m_current_time - last_update_time >= 0.01f ) {
					this->movement_data.pixelsurf.location.push_back(
						{ variables.visuals.indicators.pixelsurf_line_duration, memory.m_globals->m_current_time, globals.m_local->origin( ) } );
					last_update_time = memory.m_globals->m_current_time + 0.01f;
				}
			} else
				this->movement_data.pixelsurf.location.clear( );

			if ( this->movement_data.pixelsurf.predicted && globals.m_local->velocity( ).m_z != this->get_magic_value( ) ) {
				this->movement_data.pixelsurf.surfing   = false;
				this->movement_data.pixelsurf.predicted = false;
			}
		}
	}( );

	/* stamina hop */
	[ & ]( ) {
		if ( !variables.movement.stamina_hop || !variables.movement.stamina_hop_key.get_key_state( ) ) {
			this->movement_data.should_indicate_stamina_hop_success = false;
			return;
		}

		if ( this->has_invalid_movetypes( ) ) {
			this->movement_data.should_indicate_stamina_hop_success = false;
			return;
		}

		static int tick_count;
		static bool set = false;
		if ( globals.m_local->flags( ) & fl_on_ground && !set ) {
			tick_count = memory.m_globals->m_tick_count;
			set        = true;
		}

		if ( memory.m_globals->m_tick_count - tick_count > variables.movement.stamina_hop_ticks && globals.m_local->flags( ) & fl_on_ground ) {
			globals.m_cmd->m_buttons |= in_jump;
			set                                                     = false;
			this->movement_data.should_indicate_stamina_hop_success = true;
		}
	}( );

	/* ladderbug */
	[ & ]( ) {
		if ( !variables.movement.ladderbug || !variables.movement.ladderbug_key.get_key_state( ) ) {
			this->movement_data.should_indicate_ladderbug_success = false;
			return;
		}

		if ( globals.m_local->move_type( ) == move_type_noclip ) {
			this->movement_data.should_indicate_ladderbug_success = false;
			return;
		}

		int move_type        = globals.m_local->move_type( );
		c_user_cmd* scenario = new c_user_cmd( *globals.m_cmd );
		if ( prediction_system.simulate( globals.m_local, scenario ) ) {
			prediction_system.end_simulate( globals.m_local );
		}

		if ( move_type != move_type_ladder && globals.m_local->move_type( ) == move_type_ladder ) {
			globals.m_cmd->m_buttons |= in_duck;
			globals.m_cmd->m_buttons |= in_jump;
			this->movement_data.should_indicate_ladderbug_success = true;
		}

		delete scenario;
	}( );

	/* longjump */
	[ & ]( ) {
		if ( !variables.movement.longjump || !variables.movement.longjump_key.get_key_state( ) ) {
			this->movement_data.should_indicate_longjump_success = false;
			return;
		}

		if ( this->has_invalid_movetypes( ) ) {
			this->movement_data.should_indicate_longjump_success = false;
			return;
		}

		static int last_tick = 0;
		if ( globals.m_local->flags( ) & fl_on_ground ) {
			last_tick = globals.m_cmd->m_tick_count;
		} else if ( globals.m_cmd->m_tick_count - last_tick <= variables.movement.longjump_wait_time ) {
			// we were crouching previously anyways, no need to false detect
			if ( !( globals.m_cmd->m_buttons & in_duck ) )
				this->movement_data.should_indicate_longjump_success = true;

			globals.m_cmd->m_buttons |= in_duck;
		}
	}( );

	// redacted.
	/* fire man */
	[ & ]( ) {
	
	}( );
}

void movement_t::on_end_scene( )
{
	// !in game

	/* */
	[ & ]( ) {

	}( );

	// in game

	if ( !interfaces.m_engine->is_in_game( ) )
		return;

	/* predicted edgebug position */
	[ & ]( ) {
		if ( !variables.movement.edgebug || !variables.movement.edgebug_key.get_key_state( ) ) {
			return;
		}

		render.circle_filled_3d( this->movement_data.edgebug.predicted_position, 35.f,
		                         this->movement_data.edgebug.should_hit ? c_color( 125, 255, 125 ) : c_color( 255, 255, 255 ) );
	}( );

	/* pixelsurf line */
	[ & ]( ) {
		if ( !variables.visuals.indicators.pixelsurf_line )
			return;

		auto it        = this->movement_data.pixelsurf.location.begin( );
		c_vector last_origin;
		while ( it != this->movement_data.pixelsurf.location.end( ) ) {
			const auto alpha = ( ( it->current_time + it->die_time ) - memory.m_globals->m_current_time );

			const auto color = variables.visuals.indicators.pixelsurf_line_color.adjust_alpha( alpha );
			
			ImVec2 last_scr  = { };
			ImVec2 scr       = { };

			if ( render.world_to_screen( it->origin, scr ) && render.world_to_screen( last_origin, last_scr ) ) {
				if ( last_origin == c_vector( 0, 0, 0 ) )
					render.line( ImGui::GetBackgroundDrawList( ), c_vector_2d( scr.x, scr.y ), c_vector_2d( scr.x + 1, scr.y + 1 ), color, 2.f );
				else
					render.line( ImGui::GetBackgroundDrawList( ), c_vector_2d( last_scr.x, last_scr.y ), c_vector_2d( scr.x, scr.y ), color, 2.f );
			}

			last_origin = it->origin;

			if ( alpha <= 0.f )
				it = this->movement_data.pixelsurf.location.erase( it );
			else
				++it;
		}
	}( );
}

void movement_t::on_override_mouse_input( float* x, float* y )
{
	/* mouse strafe limiter */
	[ & ]( ) {
		if ( !variables.movement.mouse_strafe_limiter || !variables.movement.mouse_strafe_limiter_key.get_key_state( ) )
			return;

		if ( this->has_invalid_movetypes( ) )
			return;

		if ( !globals.m_local->in_air( ) )
			return;

		const auto config_value = variables.movement.mouse_strafe_limiter_value;
		*x                      = std::clamp< float >( *x, -config_value, config_value );
		if ( variables.movement.mouse_strafe_limiter_affect_pitch )
			*y = std::clamp< float >( *y, -config_value, config_value );
	}( );
}