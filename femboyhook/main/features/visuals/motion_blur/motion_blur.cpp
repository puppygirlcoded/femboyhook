#include "motion_blur.h"

// void draw_screen_effect( c_material* material )
//{
//	// Get screen size
//	int w, h;
//	interfaces.m_engine->get_screen_size( w, h );
//
//	static auto fn = memory.m_modules[e_module_names::client].find_pattern( xs( "55 8B EC 83 E4 ? 83 EC ? 53 56 57 8D 44 24 ? 89 4C 24 ?" ) );
//	_asm {
//         push h
//         push w
//         push 0
//         xor edx, edx
//         mov ecx, material
//         call fn
//         add esp, 12
//	}
// }
template< typename T >
static auto rel_to_a( const T& address ) noexcept
{
	return address + 4 + *reinterpret_cast< int* >( address );
}
void draw_screen_effect( c_material* material )
{
	int w, h;
	interfaces.m_engine->get_screen_size( w, h );

	static auto fn = rel_to_a( memory.m_modules[ e_module_names::client ].find_pattern( xs( "E8 ? ? ? ? 83 C4 0C 8D 4D F8" ) ) + 1 );
	__asm {                                                                                                                            
        __asm push h                                                                                                                       
        __asm push w                                                                                                                       
        __asm push 0                                                                                                                       
        __asm xor edx, edx                                                                                                                 
        __asm mov ecx, material                                                                                                            
        __asm call fn                                                                                                                      
        __asm add esp, 12 }
}

static float motion_blur_values[ 4 ]          = { 0.f, 0.f, 0.f, 0.f };
static float motion_blur_viewport_values[ 4 ] = { 0.f, 0.f, 1.f, 1.f };
void motion_blur_t::simulate( c_view_setup view_setup )
{
	if ( !variables.visuals.motion_blur.enabled || !interfaces.m_engine->is_in_game( ) || !globals.m_local || !globals.m_local->is_alive( ) )
		return;

	const auto get_full_frame_buffer_texture = [ & ]( const int texture_index ) -> c_texture* {
		using fn = c_texture*( __thiscall* )( int );
		static auto _func =
			reinterpret_cast< fn >( memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 81 EC ? ? ? ? 56 8B F1 83 3C" ) ) );

		return _func( texture_index );
	};

	const auto update_screen_effect_texture = [ & ]( const int texture_index, const float x, const float y, const float z, const float w,
	                                                 const bool dest_fullscreen, c_vector_4d* actual_rect ) -> void {
		c_vector_4d src_rect{ x, y, z, w };

		const auto render_context = interfaces.m_material_system->get_render_context( );
		c_texture* texture        = get_full_frame_buffer_texture( texture_index );
		int src_width, src_height;
		render_context->get_render_target_dimensions( &src_width, &src_height );
		int dest_width  = texture->get_actual_width( );
		int dest_height = texture->get_actual_height( );

		c_vector_4d dest_rect = src_rect;
		if ( !dest_fullscreen && ( src_width > dest_width || src_height > dest_height ) ) {
			int scale_x   = dest_width / src_width;
			int scale_y   = dest_height / src_height;
			dest_rect.m_x = src_rect.m_x * scale_x;
			dest_rect.m_y = src_rect.m_y * scale_y;
			dest_rect.w   = src_rect.w * scale_x;
			dest_rect.m_z = src_rect.m_z * scale_y;
			dest_rect.m_x = std::clamp( dest_rect.m_x, 0.f, ( float )dest_width );
			dest_rect.m_y = std::clamp( dest_rect.m_y, 0.f, ( float )dest_height );
			dest_rect.w   = std::clamp( dest_rect.w, 0.f, dest_width - dest_rect.m_x );
			dest_rect.m_z = std::clamp( dest_rect.m_z, 0.f, dest_height - dest_rect.m_y );
		}

		render_context->copy_render_target_to_texture_ex( texture, 0, &src_rect, dest_fullscreen ? NULL : &dest_rect );
		render_context->set_frame_buffer_copy_texture( texture, texture_index );

		if ( actual_rect ) {
			actual_rect->m_x = dest_rect.m_x;
			actual_rect->m_y = dest_rect.m_y;
			actual_rect->w   = dest_rect.w;
			actual_rect->m_z = dest_rect.m_z;
		}
	};

	const auto find_required_materials = [ & ]( c_material* material ) -> void {
		if ( !material )
			return;

		auto material_valid = false;
		auto blur_internal  = material->find_var( xs( "$MotionBlurInternal" ), &material_valid, false );
		if ( !material_valid ) {
			return;
		}

		auto vp_blur_internal = material->find_var( xs( "$MotionBlurViewportInternal" ), &material_valid, false );
		if ( !material_valid ) {
			return;
		}

		blur_internal->set_vector_component( motion_blur_values );
		vp_blur_internal->set_vector_component( motion_blur_viewport_values );
	};

	constexpr auto dot_product = []( const c_vector& a, const c_vector& b ) { return ( a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z ); };

	auto vector_angle_matrix = []( const c_vector& angles, matrix3x4_t& matrix ) {
		float sr{ }, sp{ }, sy{ }, cr{ }, cp{ }, cy{ };

		math.sin_cos( math.deg_to_rad( angles.m_y ), &sy, &cy );
		math.sin_cos( math.deg_to_rad( angles.m_x ), &sp, &cp );
		math.sin_cos( math.deg_to_rad( angles.m_z ), &sr, &cr );

		matrix[ 0 ][ 0 ] = cp * cy;
		matrix[ 1 ][ 0 ] = cp * sy;
		matrix[ 2 ][ 0 ] = -sp;

		matrix[ 0 ][ 1 ] = sr * sp * cy + cr * -sy;
		matrix[ 1 ][ 1 ] = sr * sp * sy + cr * cy;
		matrix[ 2 ][ 1 ] = sr * cp;
		matrix[ 0 ][ 2 ] = ( cr * sp * cy + -sr * -sy );
		matrix[ 1 ][ 2 ] = ( cr * sp * sy + -sr * cy );
		matrix[ 2 ][ 2 ] = cr * cp;

		matrix[ 0 ][ 3 ] = 0.0f;
		matrix[ 1 ][ 3 ] = 0.0f;
		matrix[ 2 ][ 3 ] = 0.0f;
	};

	auto x = view_setup.x;
	auto y = view_setup.y;
	auto w = view_setup.width;
	auto h = view_setup.height;

	auto rotation_intensity = variables.visuals.motion_blur.rotation_intensity;
	auto blur_strength      = variables.visuals.motion_blur.blur_strength;
	auto falling_min        = variables.visuals.motion_blur.falling_min;
	auto falling_max        = variables.visuals.motion_blur.falling_max;

	static motion_blur_history_t history{ };
	matrix3x4_t current_basis_vectors = { };
	vector_angle_matrix( view_setup.angles, current_basis_vectors );
	c_vector current_side_vec( current_basis_vectors[ 0 ][ 1 ], current_basis_vectors[ 1 ][ 1 ], current_basis_vectors[ 2 ][ 1 ] );
	c_vector current_forward_vec( current_basis_vectors[ 0 ][ 0 ], current_basis_vectors[ 1 ][ 0 ], current_basis_vectors[ 2 ][ 0 ] );
	c_vector normalized_angles = view_setup.angles.normalized1( );
	c_vector position_change   = history.previous_position - view_setup.origin;

	if ( true ) {
		auto horizontal_fov = view_setup.fov;
		auto vertical_fov   = ( view_setup.aspect_ratio <= 0.0f ) ? ( view_setup.fov ) : ( view_setup.fov / view_setup.aspect_ratio );

		auto view_dot_motion = dot_product( current_forward_vec, position_change );

		auto yaw_diff_original = history.previous_yaw - normalized_angles.m_y;
		if ( ( ( history.previous_yaw - normalized_angles.m_y > 180.0f ) || ( history.previous_yaw - normalized_angles.m_y < -180.0f ) ) &&
		     ( ( history.previous_yaw + normalized_angles.m_y > -180.0f ) && ( history.previous_yaw + normalized_angles.m_y < 180.0f ) ) )
			yaw_diff_original = history.previous_yaw + normalized_angles.m_y;

		auto side_dot_motion   = dot_product( current_side_vec, position_change );
		auto yaw_diff_adjusted = yaw_diff_original + ( side_dot_motion / 3.0f );

		if ( yaw_diff_original < 0.0f )
			yaw_diff_adjusted = std::clamp( yaw_diff_adjusted, yaw_diff_original, 0.0f );
		else
			yaw_diff_adjusted = std::clamp( yaw_diff_adjusted, 0.0f, yaw_diff_original );

		float undampened_yaw    = yaw_diff_adjusted / horizontal_fov;
		motion_blur_values[ 0 ] = undampened_yaw * ( 1.0f - ( fabs( normalized_angles.m_x ) / 90.0f ) );

		float pitch_compensate_mask = 1.0f - ( ( 1.0f - fabs( current_forward_vec[ 2 ] ) ) * ( 1.0f - fabs( current_forward_vec[ 2 ] ) ) );
		float pitch_diff_original   = history.previous_pitch - normalized_angles.m_x;
		float pitch_diff_adjusted   = pitch_diff_original;

		if ( normalized_angles.m_x > 0.0f )
			pitch_diff_adjusted = pitch_diff_original - ( ( view_dot_motion / 2.0f ) * pitch_compensate_mask );
		else
			pitch_diff_adjusted = pitch_diff_original + ( ( view_dot_motion / 2.0f ) * pitch_compensate_mask );

		if ( pitch_diff_original < 0.0f )
			pitch_diff_adjusted = std::clamp( pitch_diff_adjusted, pitch_diff_original, 0.0f );
		else
			pitch_diff_adjusted = std::clamp( pitch_diff_adjusted, 0.0f, pitch_diff_original );

		motion_blur_values[ 1 ] = pitch_diff_adjusted / vertical_fov;

		motion_blur_values[ 3 ] = undampened_yaw;
		motion_blur_values[ 3 ] *=
			( fabs( normalized_angles.m_x ) / 90.0f ) * ( fabs( normalized_angles.m_x ) / 90.0f ) * ( fabs( normalized_angles.m_x ) / 90.0f );

		if ( ( memory.m_globals->m_real_time - history.last_time_update ) > 0.0f )
			motion_blur_values[ 2 ] /= ( memory.m_globals->m_real_time - history.last_time_update ) * 30.0f;
		else
			motion_blur_values[ 2 ] = 0.0f;

		motion_blur_values[ 2 ] = std::clamp( ( fabs( motion_blur_values[ 2 ] ) - falling_min ) / ( falling_max - falling_min ), 0.0f, 1.0f ) *
		                          ( motion_blur_values[ 2 ] >= 0.0f ? 1.0f : -1.0f );
		motion_blur_values[ 2 ] /= 30.0f;

		motion_blur_values[ 0 ] *= rotation_intensity * blur_strength;
		motion_blur_values[ 1 ] *= rotation_intensity * blur_strength;
		motion_blur_values[ 2 ] *= rotation_intensity * blur_strength;
		motion_blur_values[ 3 ] *= rotation_intensity * blur_strength;

		history.previous_position            = view_setup.origin;
		history.previous_frame_basis_vectors = current_basis_vectors;
		history.previous_pitch               = normalized_angles.m_x;
		history.previous_yaw                 = normalized_angles.m_y;
		history.last_time_update             = memory.m_globals->m_real_time;

		if ( true ) {
			auto render_ctx = interfaces.m_material_system->get_render_context( );
			auto full_frame = interfaces.m_material_system->find_texture( xs( "_rt_FullFrameFB" ), xs( "RenderTargets" ) );

			auto src_width  = full_frame->get_actual_width( );
			auto src_height = full_frame->get_actual_height( );
			int vport_width{ }, vport_height{ }, dummy{ };
			render_ctx->get_viewport( &dummy, &dummy, &vport_width, &vport_height );
			update_screen_effect_texture( 0, ( float )x, ( float )y, ( float )w, ( float )h, false, NULL );

			auto mat_motion_blur = interfaces.m_material_system->find_material( xs( "dev/motion_blur" ), xs( "Other textures" ) );

			find_required_materials( mat_motion_blur );

			if ( mat_motion_blur && src_width > 0 && src_height > 0 ) {
				draw_screen_effect( mat_motion_blur );
			}
		}
	}
}
