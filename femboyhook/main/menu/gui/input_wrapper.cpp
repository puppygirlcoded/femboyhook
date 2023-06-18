#include <imgui.h>
#include <imgui_impl_win32.h>
#include <vector>
#include <windows.h>

#include "input_wrapper.h"

static bool is_initialised            = false;

bool utils::input_wrapper::is_ready( )
{
	return is_initialised;
}

bool utils::input_wrapper::setup( )
{
	if ( is_initialised )
		return false;

	is_initialised = true;

	return true;
}

bool utils::input_wrapper::undo( )
{
	if ( !is_initialised )
		return false;

	is_initialised = false;

	enable_raw_input( true );


	return true;
}

void utils::input_wrapper::update( const sdk::vector2f& raw_pos )
{
	
}

sdk::vector2f utils::input_wrapper::get_raw_mouse_pos( )
{
	return { };
}

void utils::input_wrapper::enable_raw_input( bool enabled )
{
	// if (enabled) {
	//     if (raw_input_devices_count > 0)
	//         RegisterRawInputDevices(raw_input_devices.data(), raw_input_devices_count, sizeof(RAWINPUTDEVICE));
	// }
	// else {
	//     GetRegisteredRawInputDevices(nullptr, &raw_input_devices_count, sizeof(RAWINPUTDEVICE));
	//
	//     raw_input_devices.resize(raw_input_devices_count);
	//
	//     GetRegisteredRawInputDevices(raw_input_devices.data(), &raw_input_devices_count, sizeof(RAWINPUTDEVICE));
	//
	//     for (auto i = 0u; i < raw_input_devices_count; i++) {
	//         auto device = raw_input_devices[i];
	//
	//         device.dwFlags = RIDEV_REMOVE;
	//         device.hwndTarget = nullptr;
	//
	//         RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));
	//     }
	// }
}

void utils::input_wrapper::enable_cursor( bool state )
{
	ImGui::GetIO( ).MouseDrawCursor = state;
}

void utils::input_wrapper::set_cursor( int cursor )
{
	ImGui::SetMouseCursor( cursor );
}

float utils::input_wrapper::get_mouse_wheel( )
{
	return ImGui::GetIO( ).MouseWheel;
}

float utils::input_wrapper::get_avg_frame_time( )
{
	return 1000.0f / ImGui::GetIO( ).Framerate;
}

float utils::input_wrapper::get_last_frame_time( )
{
	return ImGui::GetIO( ).DeltaTime;
}

sdk::vector2 utils::input_wrapper::get_mouse_pos( )
{
	const auto& io = ImGui::GetIO( );

	return { ( int )io.MousePos.x, ( int )io.MousePos.y };
}

sdk::vector2 utils::input_wrapper::get_mouse_delta( )
{
	const auto& io = ImGui::GetIO( );

	return { ( int )io.MouseDelta.x, ( int )io.MouseDelta.y };
}

bool utils::input_wrapper::is_hovering( const vector2& min, const vector2& max )
{
	const auto mouse_pos = get_mouse_pos( );

	return mouse_pos.x >= min.x && mouse_pos.y >= min.y && mouse_pos.x <= max.x && mouse_pos.y <= max.y;
}

bool utils::input_wrapper::is_mouse_clicked( int key, bool repeat )
{
	return ImGui::IsMouseClicked( key, repeat );
}

bool utils::input_wrapper::is_mouse_double_clicked( int key )
{
	return ImGui::IsMouseDoubleClicked( key );
}

bool utils::input_wrapper::is_mouse_down( int key )
{
	return ImGui::IsMouseDown( key );
}

bool utils::input_wrapper::is_mouse_released( int key )
{
	return ImGui::IsMouseReleased( key );
}
int utils::input_wrapper::vk_to_mouse( int vk )
{
	switch ( vk ) {
	case VK_LBUTTON:
		return MOUSE_LEFT;
	case VK_RBUTTON:
		return MOUSE_RIGHT;
	case VK_MBUTTON:
		return MOUSE_MIDDLE;
	case VK_XBUTTON1:
		return MOUSE_SIDE1;
	case VK_XBUTTON2:
		return MOUSE_SIDE2;
	}

	return 0;
}
bool utils::input_wrapper::is_key_pressed( int key, bool repeat )
{
	if ( key > VK_XBUTTON2 ) {
		return ImGui::IsKeyPressed( ( ImGuiKey )key, repeat );
	} else {
		return ImGui::IsMouseClicked( vk_to_mouse( key ), repeat );
	}
}

bool utils::input_wrapper::is_key_down( int key )
{
	if ( key > VK_XBUTTON2 ) {
		return ImGui::IsKeyDown( ( ImGuiKey )key );
	} else {
		return ImGui::IsMouseDown( vk_to_mouse( key ) );
	}
}

bool utils::input_wrapper::is_key_released( int key )
{
	if ( key > VK_XBUTTON2 ) {
		return ImGui::IsKeyReleased((ImGuiKey) key );
	} else {
		return ImGui::IsMouseReleased( vk_to_mouse( key ) );
	}
}