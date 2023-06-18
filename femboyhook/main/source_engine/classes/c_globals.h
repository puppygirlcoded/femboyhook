#pragma once

class c_globals
{
public:
	float m_real_time;
	int m_frame_count;
	float m_abs_frame_time;
	float m_abs_frame_start_time;
	float m_current_time;
	float m_frame_time;
	int m_max_clients;
	int m_tick_count;
	float m_interval_per_tick;
	float m_interpolation_amount;
	int m_frame_simulation_ticks;
	int m_network_protocol;
	void* m_save_data;
	bool m_client;
	bool m_remote_client;
	int m_timestamp_networking_base;
	int m_timestamp_randomize_window;
};