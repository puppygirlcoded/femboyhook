#pragma once

class c_client_networkable;
using m_create_client_class_fn = c_client_networkable*( __cdecl* )( int, int );
using m_create_event_fn        = c_client_networkable*( __cdecl* )( );

class c_standart_recv_proxies;
struct recv_table_t;
enum class e_class_index;

class c_base_client
{
public:
	m_create_client_class_fn m_create_fn;
	m_create_event_fn m_create_event_fn;
	char* m_network_name;
	recv_table_t* m_recv_table;
	c_base_client* m_next;
	int m_class_id;
};

class c_client
{
private:
	enum e_indexes {
		_get_all_classes           = 8,
		_dispatch_user_message = 38,
		_get_standart_recv_proxies = 48
	};

public:
	c_base_client* get_all_classes( )
	{
		using fn = c_base_client*( __thiscall* )( c_client* );
		return ( *( fn** )this )[ this->e_indexes::_get_all_classes ]( this );
	}

	c_standart_recv_proxies* get_standart_recv_proxies( )
	{
		using fn = c_standart_recv_proxies*( __thiscall* )( c_client* );
		return ( *( fn** )this )[ this->e_indexes::_get_standart_recv_proxies ]( this );
	}

	bool dispatch_user_message( int msg_type, unsigned int arg1, unsigned int length, const void* data = nullptr )
	{
		using original_fn = bool( __thiscall* )( void*, int, unsigned int, unsigned int, const void* );
		return ( *( original_fn** )this )[ this->e_indexes::_dispatch_user_message ]( this, msg_type, arg1, length, data );
	}
};