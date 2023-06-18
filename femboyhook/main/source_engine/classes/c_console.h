#pragma once
#include <cstdint>
#define FCVAR_HIDDEN          ( 1 << 4 ) // Hidden. Doesn't appear in find or
#define FCVAR_UNREGISTERED    ( 1 << 0 ) // If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY ( 1 << 1 ) // Hidden in released products. Flag is removed
class c_app_system;

typedef void* ( *create_interface_fn )( const char* pName, int* pReturnCode );

class c_app_system
{
public:
	virtual bool connect( create_interface_fn factory )                               = 0;
	virtual void disconnect( )                                                        = 0;
	virtual void* query_interface( const char* pInterfaceName )                       = 0;
	virtual int init( )                                                               = 0;
	virtual void shutdown( )                                                          = 0;
	virtual const void* get_dependencies( )                                           = 0;
	virtual int get_tier( )                                                           = 0;
	virtual void reconnect( create_interface_fn factory, const char* pInterfaceName ) = 0;
	virtual void unknown( )                                                           = 0;
};

class c_convar;
class c_con_command;
class c_con_command_base
{
public:
	void* m_base;
	c_con_command_base* next;
	bool registered;
	const char* name;
	const char* help_string;
	int flags;
	c_con_command_base* bases;
	void* accessor;
};

typedef int cvar_dll_indentifier_t;

class i_console_display_func
{
public:
	virtual void color_print( const uint8_t* clr, const char* mesasge ) = 0;
	virtual void print( const char* message )                          = 0;
	virtual void d_print( const char* pMessage )                         = 0;
};

struct _color {
	unsigned char r, g, b, a;
};

using fn_change_callback_t = void(*)(c_convar* var, const char* old_value, float f_old_value);

class c_console : public c_app_system
{
public:
	virtual cvar_dll_indentifier_t allocate_dll_indentifier( )                                         = 0;
	virtual void register_con_command( c_con_command_base* pCommandBase )                                = 0;
	virtual void unregister_con_command( c_con_command_base* pCommandBase )                            = 0;
	virtual void unregister_con_commands( cvar_dll_indentifier_t id )                                  = 0;
	virtual const char* get_command_line_value( const char* pVariableName )                            = 0;
	virtual c_con_command_base* find_command_base( const char* name )                                    = 0;
	virtual const c_con_command_base* find_command_base( const char* name ) const                        = 0;
	virtual c_convar* get_convar( const char* var_name )                                                 = 0;
	virtual const c_convar* get_convar( const char* var_name ) const                                     = 0;
	virtual c_con_command* find_command( const char* name )                                              = 0;
	virtual const c_con_command* find_command( const char* name ) const                                  = 0;
	virtual void install_global_change_callback( fn_change_callback_t callback )                       = 0;
	virtual void remove_global_change_callback( fn_change_callback_t callback )                        = 0;
	virtual void call_global_change_callbacks( c_convar* var, const char* pOldString, float flOldValue ) = 0;
	virtual void install_console_display_func( i_console_display_func* pDisplayFunc )                  = 0;
	virtual void remove_console_display_func( i_console_display_func* pDisplayFunc )                   = 0;
	virtual void console_color_printf( const _color& clr, const char* pFormat, ... ) const             = 0;
	virtual void console_printf( const char* pFormat, ... ) const                                      = 0;
	virtual void donsole_dprintf( const char* pFormat, ... ) const                                     = 0;
	virtual void rever_flagged_convars( int nFlag )                                                    = 0;
};