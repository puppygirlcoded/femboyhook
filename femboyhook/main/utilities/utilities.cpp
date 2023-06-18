#include "../includes.h"

#include "memory/pe32.h"

#include <intrin.h>

void* utilities_t::get_module_handle( const std::string_view module_name )
{
	const _PEB32* peb_data = reinterpret_cast< _PEB32* >( __readfsdword( 0x30 ) );

	if ( module_name.empty( ) )
		return peb_data->ImageBaseAddress;

	const std::wstring converted_module_name( module_name.begin( ), module_name.end( ) );

	for ( LIST_ENTRY* list_entry = peb_data->Ldr->InLoadOrderModuleList.Flink; list_entry != &peb_data->Ldr->InLoadOrderModuleList;
	      list_entry             = list_entry->Flink ) {
		const _LDR_DATA_TABLE_ENTRY* entry = CONTAINING_RECORD( list_entry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks );

		if ( entry->BaseDllName.Buffer && converted_module_name.compare( entry->BaseDllName.Buffer ) == 0 )
			return entry->DllBase;
	}

	return nullptr;
}

void* utilities_t::get_export_address( const void* module_handle, const std::string_view export_name )
{
	const std::uint8_t* address                  = static_cast< const std::uint8_t* >( module_handle );
	const IMAGE_DOS_HEADER* dos_header           = static_cast< const IMAGE_DOS_HEADER* >( module_handle );
	const IMAGE_NT_HEADERS* nt_headers           = reinterpret_cast< const IMAGE_NT_HEADERS* >( address + dos_header->e_lfanew );
	const IMAGE_OPTIONAL_HEADER* optional_header = &nt_headers->OptionalHeader;

	const std::uintptr_t export_size    = optional_header->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].Size;
	const std::uintptr_t export_address = optional_header->DataDirectory[ IMAGE_DIRECTORY_ENTRY_EXPORT ].VirtualAddress;

	if ( export_size <= 0U )
		return nullptr;

	const IMAGE_EXPORT_DIRECTORY* export_directory = reinterpret_cast< const IMAGE_EXPORT_DIRECTORY* >( address + export_address );
	const std::uintptr_t* names_rva                = reinterpret_cast< const std::uintptr_t* >( address + export_directory->AddressOfNames );
	const std::uintptr_t* functions_rva            = reinterpret_cast< const std::uintptr_t* >( address + export_directory->AddressOfFunctions );
	const std::uint16_t* name_ordinals             = reinterpret_cast< const std::uint16_t* >( address + export_directory->AddressOfNameOrdinals );

	std::uintptr_t right = export_directory->NumberOfNames;
	std::uintptr_t left  = 0;

	while ( right != left ) {
		const std::uintptr_t middle = left + ( ( right - left ) >> 1U );
		const int result            = export_name.compare( reinterpret_cast< const char* >( address + names_rva[ middle ] ) );

		if ( result == 0 )
			return const_cast< void* >( static_cast< const void* >( address + functions_rva[ name_ordinals[ middle ] ] ) );

		if ( result > 0 )
			left = middle;
		else
			right = middle;
	}

	return nullptr;
}

IDirect3DTexture9* utilities_t::load_texture_from_vpk(const char* file_path, float scale) {
	void* file = interfaces.m_filesystem->open( file_path, xs( "r" ), xs( "GAME" ) );
	if ( !file ) {
		return nullptr;
	}

	std::vector< char > buf( interfaces.m_filesystem->size( file ) );

	interfaces.m_filesystem->read( buf.data( ), buf.size( ), file );
	interfaces.m_filesystem->close( file );

	auto rasterize_vector = [ & ]( const char* m_file_path ) -> IDirect3DTexture9* {
		// [hash(file_path), texture]
		static std::map< int, IDirect3DTexture9* > rasterization_map;
		auto hash    = std::hash< std::string >( )( m_file_path );
		auto texture = rasterization_map.find( hash );
		if ( texture == rasterization_map.end( ) ) {
			rasterization_map.insert( { hash, render.rasterize_vector( buf.data( ), scale ) } );
			texture = rasterization_map.find( hash );
		}

		return texture->second;
	};

	IDirect3DTexture9* texture = rasterize_vector( file_path );
	if ( !texture ) {
		return nullptr;
	}

	return texture;
}

IDirect3DTexture9* utilities_t::get_weapon_texture( std::string weapon_name, const float scale )
{
	static std::vector< std::pair< uint32_t, IDirect3DTexture9* > > textures;

	const auto it = std::ranges::find_if(
		textures, [ & ]( const std::pair< uint32_t, IDirect3DTexture9* >& element ) { return element.first == fnv1a::hash( weapon_name.c_str( ) ); } );

	if ( it != textures.end( ) ) {
		return it->second; // texture has already been created
	}

	const std::string path =
		std::vformat( xs( "materials/panorama/images/icons/equipment/{}.svg" ), std::make_format_args( weapon_name.substr( 7 ) ) );
	IDirect3DTexture9* texture = this->load_texture_from_vpk( path.c_str( ), scale );

	if ( texture ) {
		textures.emplace_back( fnv1a::hash( weapon_name.c_str( ) ), texture );
	}

	return texture;
}