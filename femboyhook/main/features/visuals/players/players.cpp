#include "players.h"
#include "../../../includes.h"
#include "../../cache/cache.h"

#include <utility>

void players_t::on_paint_traverse( )
{
	if ( !render.m_initialised )
		return;

	const float delta_time = ( 1.f / 0.5f ) * memory.m_globals->m_frame_time;

	for ( int i = 0; i < interfaces.m_client_entity_list->get_highest_entity_index( ); i++ ) {
		const auto entity = reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity( i ) );
		if ( !entity )
			continue;

		if ( !entity->is_player( ) )
			continue;

		if ( entity->team( ) == globals.m_local->team( ) && !variables.players.draw_on_team )
			continue;

		const int index = entity->index( );
		entity->is_dormant( ) || !entity->is_alive( ) ? this->player_alpha[ index ] -= delta_time : this->player_alpha[ index ] += delta_time;

		this->player_alpha[ index ] = std::clamp( this->player_alpha[ index ], 0.f, 1.f );

		if ( this->player_alpha[ index ] < 0.1f )
			continue;

		bbox_t box = { };
		if ( !get_bbox( entity, &box ) )
			continue;

		this->m_player_data[ index ].m_box_data = &box;

		player_info_t player_information = { };
		if ( !interfaces.m_engine->get_player_information( index, &player_information ) )
			continue;

		// retarted ass bounds check XDDD
		if ( box.m_left > 2000 || box.m_top > 2000 || box.m_width > 2000 || box.m_height > 2000 || box.m_left < -2000 || box.m_top < -2000 ||
		     box.m_width < -2000 || box.m_height < -2000 )
			continue;

		/* player name */
		[ & ]( ) {
			if ( !variables.players.name )
				return;

			std::string player_name = player_information.m_name;

			if ( player_name.length( ) > 24U )
				player_name = player_name.substr( 0U, 24U ).append( xs( "..." ) );

			const auto font_size = render.m_segoe_ui->FontSize;
			const auto text_size = render.m_segoe_ui->CalcTextSizeA( font_size, FLT_MAX, 0.0f, player_name.c_str( ) );

			render.m_draw_data.emplace_back(
				e_draw_type::draw_type_text,
				std::make_any< text_draw_object_t >( render.m_segoe_ui,
			                                         c_vector_2d( box.m_left + ( box.m_width / 2 ) - text_size.x / 2,
			                                                      ( box.m_top - font_size - 5 ) - ( this->player_alpha[ index ] * -4.f ) ),
			                                         player_name, variables.players.name_color.adjust_alpha( this->player_alpha[ index ] ),
			                                         c_color( 0, 0, 0, int( this->player_alpha[ index ] * 255 ) ),
			                                         e_text_render_flags::text_render_flag_dropshadow ) );
		}( );

		/* player weapon */
		[ & ]( ) {
			const auto active_weapon = entity->get_active_weapon( );
			if ( active_weapon )
				this->m_player_data[ index ].m_active_weapon = active_weapon;

			if ( !this->m_player_data[ index ].m_active_weapon )
				return;

			const auto weapon_data =
				memory.m_weapon_system->get_weapon_data( this->m_player_data[ index ].m_active_weapon->item_definition_index( ) );
			if ( weapon_data && this->m_player_data[ index ].m_active_weapon ) {
				this->m_player_data[ index ].m_weapon_data = weapon_data;
			}

			if ( !this->m_player_data[ index ].m_weapon_data )
				return;

			/* name */
			[ & ]( ) {
				if ( !variables.players.weapon_name )
					return;

				const auto localized_name = interfaces.m_localize->find( this->m_player_data[ index ].m_weapon_data->m_hud_name );

				const std::wstring w = localized_name;
				if ( w.empty( ) )
					return;

				const std::string converted_name( w.begin( ), w.end( ) );
				if ( converted_name.empty( ) )
					return;

				render.m_draw_data.emplace_back(
					e_draw_type::draw_type_text,
					std::make_any< text_draw_object_t >(
						render.m_flags_font,
						c_vector_2d(
							box.m_left + ( box.m_width / 2 ) -
								render.m_flags_font->CalcTextSizeA( render.m_flags_font->FontSize, FLT_MAX, 0.0f, converted_name.c_str( ) ).x / 2,
							box.m_bottom + 2 ),
						converted_name, variables.players.weapon_name_color.adjust_alpha( this->player_alpha[ index ] ),
						c_color( 0, 0, 0, int( this->player_alpha[ index ] * 255 ) ), e_text_render_flags::text_render_flag_dropshadow ) );
			}( );

			/* icon */
			[ & ]( ) {
				if ( !variables.players.weapon_icon )
					return;

				if ( !this->m_player_data[ index ].m_weapon_data )
					return;

				const auto weapon_name    = this->m_player_data[ index ].m_weapon_data->m_weapon_name;
				const auto weapon_texture = utilities.get_weapon_texture( weapon_name, 2.f );

				if ( weapon_texture ) {
					D3DSURFACE_DESC surface_desc;
					weapon_texture->GetLevelDesc( 0, &surface_desc );

					const c_vector_2d size     = { static_cast< float >( surface_desc.Width ) * 0.25f,
						                           static_cast< float >( surface_desc.Height ) * 0.25f };
					const c_vector_2d position = { box.m_left + box.m_width / 2.0f - size.m_x / 2.0f,
						                           box.m_top + box.m_height + 2.0f + ( variables.players.weapon_name ? 10.f : 0.f ) };

					render.m_draw_data.emplace_back(
						e_draw_type::draw_type_texture,
						std::make_any< texture_draw_object_t >( position, size,
					                                            variables.players.weapon_icon_color.adjust_alpha( this->player_alpha[ index ] ),
					                                            ( ImTextureID )weapon_texture ) );
				}
			}( );
		}( );

		/* player bounding box */
		[ & ]( ) {
			if ( !variables.players.bounding_box )
				return;

			if ( variables.players.filled_bounding_box ) {
				render.m_draw_data.emplace_back(
					e_draw_type::draw_type_gradient,
					std::make_any< grad_rect_draw_object_t >(
						c_vector_2d( box.m_left + 1, box.m_top + 1 ), c_vector_2d( box.m_width - 2, box.m_height - 2 ),
						variables.players.filled_bounding_box_color_top.adjust_alpha(
							( variables.players.filled_bounding_box_color_top.m_a / 255.f ) * this->player_alpha[ index ] ),
						variables.players.filled_bounding_box_color_bottom.adjust_alpha(
							( variables.players.filled_bounding_box_color_bottom.m_a / 255.f ) * this->player_alpha[ index ] ),
						false ) );
			}

			switch ( variables.players.bounding_box_type ) {
			case 0:
				/* full box */
				[ & ]( ) {
					render.m_draw_data.emplace_back(
						e_draw_type::draw_type_rect,
						std::make_any< rect_draw_object_t >( c_vector_2d( box.m_left, box.m_top ), c_vector_2d( box.m_width, box.m_height ),
					                                         variables.players.bounding_box_color.adjust_alpha( this->player_alpha[ index ] ), 0.0f,
					                                         0, 1.0f, e_rect_render_flags::rect_render_flag_full_outline,
					                                         c_color( 0, 0, 0, this->player_alpha[ index ] ) ) );
				}( );
				break;
			case 1:
				/* corner box */
				[ & ]( ) {
					/* TODO ~ float ~ fix this up (not very neat) */
					auto corner_box = [ & ]( float x1, float y1, float x2, float y2, const c_color& color ) {
						int w = x2 - x1;
						int h = y2 - y1;

						int iw = w / 3;
						int ih = h / 5;

						const c_vector_2d line_draw_positions[ 16 ] = { c_vector_2d( x1, y1 ),
							                                            c_vector_2d( x1 + iw, y1 ),
							                                            c_vector_2d( x1 + w - iw, y1 ),
							                                            c_vector_2d( x1 + w, y1 ),
							                                            c_vector_2d( x1, y1 ),
							                                            c_vector_2d( x1, y1 + ih ),
							                                            c_vector_2d( x1 + w - 1, y1 ),
							                                            c_vector_2d( x1 + w - 1, y1 + ih ),
							                                            c_vector_2d( x1, y1 + h ),
							                                            c_vector_2d( x1 + iw, y1 + h ),
							                                            c_vector_2d( x1 + w - iw, y1 + h ),
							                                            c_vector_2d( x1 + w, y1 + h ),
							                                            c_vector_2d( x1, y1 + h - ih ),
							                                            c_vector_2d( x1, y1 + h ),
							                                            c_vector_2d( x1 + w - 1, y1 + h - ih ),
							                                            c_vector_2d( x1 + w - 1, y1 + h ) };

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 0 ], line_draw_positions[ 1 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 2 ], line_draw_positions[ 3 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 4 ], line_draw_positions[ 5 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 6 ], line_draw_positions[ 7 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 8 ], line_draw_positions[ 9 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 10 ], line_draw_positions[ 11 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 12 ], line_draw_positions[ 13 ], color, 1.f ) );

						render.m_draw_data.emplace_back(
							e_draw_type::draw_type_line,
							std::make_any< line_draw_object_t >( line_draw_positions[ 14 ], line_draw_positions[ 15 ], color, 1.f ) );
					};

					corner_box( box.m_left, box.m_top, box.m_right, box.m_bottom,
					            variables.players.bounding_box_color.adjust_alpha( this->player_alpha[ index ] ) );

					/* inner outline */
					corner_box( box.m_left + 1, box.m_top + 1, box.m_right - 1, box.m_bottom - 1,
					            c_color( 0, 0, 0, int( this->player_alpha[ index ] * 255 ) ) );

					/* outer outline */
					corner_box( box.m_left - 1, box.m_top - 1, box.m_right + 1, box.m_bottom + 1,
					            c_color( 0, 0, 0, int( this->player_alpha[ index ] * 255 ) ) );
				}( );
				break;
			}
		}( );

		/* player healthbar */
		[ & ]( ) {
			if ( !variables.players.health_bar )
				return;

			const auto health = entity->health( );

			const auto back_position = c_vector_2d( box.m_left - 6, box.m_top - 1 );
			const auto back_size     = c_vector_2d( 4, box.m_height + 2 );

			const auto bar_size = std::clamp( health * box.m_height / 100, 0.f, box.m_height + 1 );
			const auto size     = c_vector_2d( 2, bar_size );
			const auto position = c_vector_2d( box.m_left - 5, box.m_top + ( box.m_height - bar_size ) );

			render.m_draw_data.emplace_back( e_draw_type::draw_type_rect_filled,
			                                 std::make_any< rect_filled_draw_object_t >(
												 back_position, back_size, c_color( 0, 0, 0 ).adjust_alpha( this->player_alpha[ index ] ) ) );

			if ( variables.players.gradient_health_bar ) {
				render.m_draw_data.emplace_back(
					e_draw_type::draw_type_gradient,
					std::make_any< grad_rect_draw_object_t >(
						position, size, variables.players.gradient_health_bar_color_top.adjust_alpha( this->player_alpha[ index ] ),
						variables.players.gradient_health_bar_color_bottom.adjust_alpha( this->player_alpha[ index ] ), false ) );
			} else {
				render.m_draw_data.emplace_back(
					e_draw_type::draw_type_rect_filled,
					std::make_any< rect_filled_draw_object_t >( position, size,
				                                                variables.players.health_bar_color.adjust_alpha( this->player_alpha[ index ] ) ) );
			}
		}( );

		/* player viewangles */
		[ & ]( ) {
			if ( !variables.players.viewangles )
				return;

			c_vector src;
			c_vector pos = entity->get_bone_position( 8 );
			math.angle_vectors( entity->abs_eye_angles( ), &src );
			src *= variables.players.viewangles_distance;
			c_vector dst = pos + src;

			ImVec2 start, end;
			if ( !render.world_to_screen( pos, start ) || !render.world_to_screen( dst, end ) )
				return;

			render.m_draw_data.emplace_back(
				e_draw_type::draw_type_line,
				std::make_any< line_draw_object_t >( c_vector_2d( start.x, start.y ), c_vector_2d( end.x, end.y ),
			                                         variables.players.viewangles_color.adjust_alpha( this->player_alpha[ index ] ), 2.f ) );
		}( );

		/* player skeleton */
		[ & ]( ) {
			if ( !variables.players.skeleton )
				return;

			auto studio_hdr = interfaces.m_model_info->get_studio_model( entity->get_model( ) );
			if ( !studio_hdr )
				return;

			// rip fps lolz
			if ( variables.players.skeleton_outline ) {
				for ( int i = 0; i < studio_hdr->bones_count; i++ ) {
					auto bone = studio_hdr->get_bone( i );
					if ( !bone )
						return;

					if ( bone && ( bone->flags & 0x00000100 ) && ( bone->parent != -1 ) ) {
						auto child  = entity->get_bone_position( i );
						auto parent = entity->get_bone_position( bone->parent );

						ImVec2 start, end;
						if ( render.world_to_screen( parent, start ) && render.world_to_screen( child, end ) ) {
							render.m_draw_data.emplace_back(
								e_draw_type::draw_type_line,
								std::make_any< line_draw_object_t >( c_vector_2d( start.x + 1, start.y + 1 ), c_vector_2d( end.x + 1, end.y + 1 ),
							                                         c_color( 0, 0, 0 ).adjust_alpha( this->player_alpha[ index ] ), 2.f ) );
						}
					}
				}
			}

			for ( int i = 0; i < studio_hdr->bones_count; i++ ) {
				auto bone = studio_hdr->get_bone( i );
				if ( !bone )
					return;

				if ( bone && ( bone->flags & 0x00000100 ) && ( bone->parent != -1 ) ) {
					auto child  = entity->get_bone_position( i );
					auto parent = entity->get_bone_position( bone->parent );

					ImVec2 start, end;
					if ( render.world_to_screen( parent, start ) && render.world_to_screen( child, end ) ) {
						render.m_draw_data.emplace_back( e_draw_type::draw_type_line,
						                                 std::make_any< line_draw_object_t >(
															 c_vector_2d( start.x, start.y ), c_vector_2d( end.x, end.y ),
															 variables.players.skeleton_color.adjust_alpha( this->player_alpha[ index ] ), 2.f ) );
					}
				}
			}
		}( );

		/* player flags */ // TODO: add more flags xoxo
		[ & ]( ) {
			std::vector< std::string > flags;

			const auto menu_flags = variables.players.flags;
			if ( menu_flags & ( 1 << 0 ) && entity->health( ) <= 1 )
				flags.push_back( xs( "FLASH KILL" ) );

			if ( menu_flags & ( 1 << 1 ) && entity->has_helmet( ) )
				flags.push_back( xs( "HK" ) );
			else if ( menu_flags & ( 1 << 1 ) && entity->armor_value( ) > 0 )
				flags.push_back( xs( "K" ) );

			if ( menu_flags & ( 1 << 2 ) && entity->flash_duration( ) > 0.0f )
				flags.push_back( xs("FLASHED") );

			// TODO: add a function to c_base_entity that returns if they have a bomb or not
			if ( menu_flags & ( 1 << 3 ) ) {
				// bomb
			}

			if ( menu_flags & ( 1 << 3 ) && entity->has_defuser( ) )
				flags.push_back( xs( "KIT" ) );

			if ( menu_flags & ( 1 << 4 ) && entity->is_scoped( ) )
				flags.push_back( xs( "ZOOM" ) );

			int offset = 0;
			for ( auto flag : flags ) {
				render.m_draw_data.emplace_back(
					e_draw_type::draw_type_text,
					std::make_any< text_draw_object_t >( render.m_flags_font, c_vector_2d( box.m_left + box.m_width + 2, box.m_top + offset ), flag,
				                                         c_color( 255, 255, 255 ).adjust_alpha( this->player_alpha[ index ] ),
				                                         c_color( 0, 0, 0, int( this->player_alpha[ index ] * 255 ) ),
				                                         e_text_render_flags::text_render_flag_dropshadow ) );

				offset += 10;
			}
		}( );
	}
	//cache.enumerate( e_cache_types::cache_type_player, [ & ]( c_base_entity* entity ) {
	//	
	//} );
}

bool players_t::get_bbox( c_base_entity* entity, bbox_t* box ) const
{
	if ( !entity ) {
		box = this->m_player_data[ entity->index( ) ].m_box_data;
		return false;
	}

	if ( entity->life_state( ) != 0 ) {
		box = this->m_player_data[ entity->index( ) ].m_box_data;
		return false;
	}

	if ( entity->is_dormant( ) )
		return false;

	c_collideable* collideable = entity->collideable( );

	if ( !collideable )
		return false;

	c_vector min = collideable->mins( );
	c_vector max = collideable->maxs( );

	std::array< c_vector, 8U > vector = { c_vector( min.m_x, min.m_y, min.m_z ), c_vector( min.m_x, max.m_y, min.m_z ),
		                                  c_vector( max.m_x, max.m_y, min.m_z ), c_vector( max.m_x, min.m_y, min.m_z ),
		                                  c_vector( max.m_x, max.m_y, max.m_z ), c_vector( min.m_x, max.m_y, max.m_z ),
		                                  c_vector( min.m_x, min.m_y, max.m_z ), c_vector( max.m_x, min.m_y, max.m_z ) };

	if ( !vector.data( ) )
		return false;

	const matrix3x4_t& matTransformed = entity->coordinate_frame( );

	float left   = std::numeric_limits< float >::max( );
	float top    = std::numeric_limits< float >::max( );
	float right  = std::numeric_limits< float >::lowest( );
	float bottom = std::numeric_limits< float >::lowest( );

	std::array< ImVec2, 8U > screen_points = { };
	for ( std::size_t i = 0U; i < 8U; i++ ) {
		if ( !render.world_to_screen( math.vector_transform( vector[ i ], matTransformed ), screen_points[ i ] ) )
			return false;

		/*
		 * transformed points navigation:
		 * [N] [back/front][left/right][bottom/top]
		 *	0 - frb
		 *	1 - flb
		 *	2 - blb
		 *	3 - brb
		 *	4 - frt
		 *	5 - blt
		 *	6 - brt
		 *	7 - flt
		 */
		left   = std::min( left, screen_points[ i ].x );
		top    = std::min( top, screen_points[ i ].y );
		right  = std::max( right, screen_points[ i ].x );
		bottom = std::max( bottom, screen_points[ i ].y );
	}

	// set calculated box
	box->m_left   = static_cast< int >( left );
	box->m_top    = static_cast< int >( top );
	box->m_right  = static_cast< int >( right );
	box->m_bottom = static_cast< int >( bottom );
	box->m_width  = static_cast< int >( right - left );
	box->m_height = static_cast< int >( bottom - top );

	return true;
}

void players_t::on_post_screen_effects( )
{
	if ( !interfaces.m_engine->is_in_game( ) || !globals.m_local || !globals.m_local->is_alive( ) )
		return;

	for ( int i = 0; i < interfaces.m_glow_manager->size; i++ ) {
		auto glow_obj = &interfaces.m_glow_manager->objects[ i ];

		if ( glow_obj->unused( ) || !glow_obj->entity || glow_obj->entity->is_dormant( ) || !glow_obj->entity->is_player( ) )
			continue;

		if ( glow_obj->entity->team( ) == globals.m_local->team( ) && !variables.players.draw_on_team )
			continue;

		if ( variables.players.glow.visible ) {
			glow_obj->set( variables.players.glow.visible_color );
		}
	}
}