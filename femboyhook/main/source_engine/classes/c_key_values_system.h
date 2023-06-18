#pragma once

class c_key_values_system
{
public:
	virtual void register_sizeof_key_values( int size ) = 0;

private:
	virtual void function0( ) = 0;

public:
	virtual void* alloc_key_values_memory( int size )                                                                      = 0;
	virtual void free_key_values_memory( void* memory )                                                                    = 0;
	virtual int get_symbol_for_string( const char* name, bool create = true )                                              = 0;
	virtual const char* get_string_for_symbol( int symbol )                                                                = 0;
	virtual void add_key_values_to_memory_leak_list( void* memory, int symbol_name )                                       = 0;
	virtual void remove_key_values_from_memory_leak_list( void* memory )                                                   = 0;
	virtual void set_key_values_expression_symbol( const char* name, bool b_value )                                        = 0;
	virtual bool get_key_values_expression_symbol( const char* name )                                                      = 0;
	virtual int get_symbol_for_string_case_sensitive( int& case_insensitive_symbol, const char* name, bool create = true ) = 0;
};

using key_values_system_fn = c_key_values_system*( __cdecl* )( );
