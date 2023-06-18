#pragma once

enum e_send_prop_type : int {
	dpt_int = 0,
	dpt_float,
	dpt_vector,
	dpt_vector2d,
	dpt_string,
	dpt_array,
	dpt_datatable,
	dpt_int64,
	dpt_sendproptypemax
};

struct recv_prop_t;
struct data_variant_t {
	union {
		float m_float;
		long m_int;
		char* m_string;
		void* m_data;
		float m_vector[ 3 ];
		long long m_int64;
	};

	e_send_prop_type m_type;
};

class c_recv_proxy_data
{
public:
	const recv_prop_t* m_recv_prop;
	data_variant_t m_value;
	int m_element;
	int m_object_id;
};

using recv_var_proxy_fn     = void( __cdecl* )( const c_recv_proxy_data*, void*, void* );
using array_length_proxy_fn = void( __cdecl* )( void*, int, int );
using data_table_proxy_fn   = void( __cdecl* )( const recv_prop_t*, void**, void*, int );

class c_standart_recv_proxies
{
public:
	recv_var_proxy_fn m_int32_to_int8;
	recv_var_proxy_fn m_int32_to_int16;
	recv_var_proxy_fn m_int32_to_int32;
	recv_var_proxy_fn m_int64_to_int64;
	recv_var_proxy_fn m_float_to_float;
	recv_var_proxy_fn m_vector_to_vector;
};

struct recv_table_t {
	recv_prop_t* m_props;
	int m_props_count;
	void* m_decoder;
	char* m_net_table_name;
	bool m_initialized;
	bool m_in_main_list;
};

struct recv_prop_t {
	char* m_var_name;
	e_send_prop_type m_recv_type;
	int m_flags;
	int m_string_buffer_size;
	bool m_inside_array;
	const void* m_extra_data;
	recv_prop_t* m_array_prop;
	array_length_proxy_fn* m_array_length_proxy_fn;
	recv_var_proxy_fn m_proxy_fn;
	data_table_proxy_fn* m_data_table_proxy_fn;
	recv_table_t* m_data_table;
	int m_offset;
	int m_element_stride;
	int m_elements;
	const char* m_parent_array_prop_name;
};