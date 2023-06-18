#pragma once

class c_file_system
{
private:
	enum e_indexes {
		_read = 0,
		_open = 2,
		_close = 3,
		_size = 7,
	};

public:
	int read(void* output, int size, void* file) {
		using fn = int( __thiscall* )( void*, void*, int, void* );
		return ( *( fn** )this )[ this->e_indexes::_read ]( this, output, size, file );
	}

	void* open(const char* file_name, const char* options, const char* path_id = nullptr) {
		using fn = void*( __thiscall* )( void*, const char*, const char*, const char* );
		return ( *( fn** )this )[ this->e_indexes::_open ]( this, file_name, options, path_id );
	}

	void close(void* file) {
		using fn = void(__thiscall*)(void*, void*);
		return ( *( fn** )this )[ this->e_indexes::_close ]( this, file );
	}

	unsigned int size(void* file) {
		using fn = unsigned int(__thiscall*)(void*, void*);
		return ( *( fn** )this )[ this->e_indexes::_size ]( this, file );
	}
};