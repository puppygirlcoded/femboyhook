#include "c_color.h"

#include <imgui.h>

[[nodiscard]] unsigned int c_color::get_u32( const float alpha_multiplier ) const
{
	return ImGui::GetColorU32( this->get_vec4( alpha_multiplier ) );
}

[[nodiscard]] ImVec4 c_color::get_vec4( const float alpha_multiplier ) const
{
	return ImVec4( ( this->m_r / 255.f ), ( this->m_g / 255.f ), ( this->m_b / 255.f ), ( this->m_a / 255.f ) * alpha_multiplier );
}