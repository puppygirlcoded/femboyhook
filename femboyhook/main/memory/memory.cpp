#include "../includes.h"

bool memory_t::on_attach( const std::vector< const char* >& modules )
{
	for ( const auto& module_names : modules )
		m_modules.push_back( module_handle_info_t( module_names, utilities.get_module_handle( module_names ) ) );

	for ( const auto& module_data : m_modules ) {
		if ( !module_data.m_name ) {
			console.print( xs( "failed to find {:s}" ), module_data.m_name );
			return false;
		}

		if ( !module_data.m_value ) {
			console.print( xs( "module handle {:s} was {:p}" ), module_data.m_name, module_data.m_value );
			return false;
		}

		console.print( xs( "found {:s} @ {:p}" ), module_data.m_name, module_data.m_value );
	}

	return true;
}

bool memory_t::on_attach( )
{
	if ( ( this->m_input = *reinterpret_cast< c_input** >(
			   memory.m_modules[ e_module_names::client ].find_pattern( xs( "B9 ? ? ? ? F3 0F 11 04 24 FF 50 10" ) ) + 0x1 ) ) == nullptr )
		return false;
	else
		console.print( xs( "found CInput @ {:p}" ), reinterpret_cast< void* >( this->m_input ) );

	if ( ( this->m_key_values_system = reinterpret_cast< key_values_system_fn >(
			   utilities.get_export_address( this->m_modules[ e_module_names::vstdlib ].m_value, xs( "KeyValuesSystem" ) ) )( ) ) == nullptr )
		return false;
	else
		console.print( xs( "found KeyValuesSystem @ {:p}" ), reinterpret_cast< void* >( this->m_key_values_system ) );

	if ( ( this->m_device = **reinterpret_cast< IDirect3DDevice9*** >(
			   memory.m_modules[ e_module_names::shaderapidx9 ].find_pattern( xs( "A1 ? ? ? ? 50 8B 08 FF 51 0C" ) ) + 0x1 ) ) == nullptr )
		return false;
	else
		console.print( xs( "found IDirect3DDevice9 @ {:p}" ), reinterpret_cast< void* >( this->m_device ) );

	if ( ( this->m_client_state = **reinterpret_cast< c_client_state*** >(
			   memory.m_modules[ e_module_names::engine ].find_pattern( xs( "A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07" ) ) + 0x1 ) ) == nullptr )
		return false;
	else
		console.print( xs( "found IClientState @ {:p}" ), reinterpret_cast< void* >( this->m_client_state ) );

	if ( ( this->m_globals =
	           **reinterpret_cast< c_globals*** >( utilities.get_virtual_function< std::uintptr_t >( interfaces.m_client, 11 ) + 0xA ) ) == nullptr )
		return false;
	else
		console.print( xs( "found IGlobalsVarsBase @ {:p}" ), reinterpret_cast< void* >( this->m_globals ) );

	if ( ( this->m_weapon_system = *reinterpret_cast< c_weapon_system** >(
			   memory.m_modules[ e_module_names::client ].find_pattern( xs( "8B 35 ? ? ? ? FF 10 0F B7 C0" ) ) + 0x2 ) ) == nullptr )
		return false;
	else
		console.print( xs( "found IWeaponSystem @ {:p}" ), reinterpret_cast< void* >( this->m_weapon_system ) );

	return true;
}

std::uint8_t* module_handle_info_t::find_pattern( const char* signature )
{
	static const auto pattern_to_byte = []( const char* pattern ) {
		std::vector< int > bytes = { };
		const auto start         = const_cast< char* >( pattern );
		const auto end           = const_cast< char* >( pattern ) + std::strlen( pattern );

		for ( auto current = start; current < end; current++ ) {
			if ( *current == '?' ) {
				++current;

				if ( *current == '?' )
					++current;

				bytes.push_back( -1 );
			} else
				bytes.push_back( std::strtoul( current, &current, 16 ) );
		}

		return bytes;
	};

	const auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( this->m_value );
	const auto nt_headers = reinterpret_cast< PIMAGE_NT_HEADERS >( reinterpret_cast< std::uint8_t* >( this->m_value ) + dos_header->e_lfanew );

	const auto size_of_image = nt_headers->OptionalHeader.SizeOfImage;
	const auto pattern_bytes = pattern_to_byte( signature );
	const auto scan_bytes    = reinterpret_cast< std::uint8_t* >( this->m_value );

	const auto s = pattern_bytes.size( );
	const auto d = pattern_bytes.data( );

	for ( auto i = 0ul; i < size_of_image - s; ++i ) {
		bool found = true;

		for ( auto j = 0ul; j < s; ++j ) {
			if ( scan_bytes[ i + j ] != d[ j ] && d[ j ] != -1 ) {
				found = false;
				break;
			}
		}

		if ( found )
			return &scan_bytes[ i ];
	}

	console.print( xs( "failed to find pattern {:s} in module {:s}" ), signature, this->m_name );

	return nullptr;
}
