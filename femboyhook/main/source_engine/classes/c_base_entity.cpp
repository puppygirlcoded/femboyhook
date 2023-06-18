#include "c_base_entity.h"
#include "../../hashing/strings/xorstr.h"
#include "../../memory/memory.h"
#include "../enumerations/e_item_definition_index.h"
#include "../interfaces/interfaces.h"
#include "../math/math.h"
#include "c_weapon_data.h"

c_base_entity* entity_handle_t::get( ) const
{
	if ( !this || this->handle == 0xFFFFFFFF )
		return nullptr;

	return reinterpret_cast<c_base_entity*>(interfaces.m_client_entity_list->get_client_entity_from_handle( this->handle ));
}

bool entity_handle_t::operator==( const entity_handle_t& other ) const
{
	return handle == other.handle;
}

bool entity_handle_t::operator==( c_base_entity* entity ) const
{
	return get( ) == entity;
}

bool entity_handle_t::operator!=( const entity_handle_t& other ) const
{
	return handle != other.handle;
}

bool entity_handle_t::operator!=( c_base_entity* entity ) const
{
	return get( ) != entity;
}

c_base_entity* c_base_entity::get_active_weapon( )
{
	return reinterpret_cast< c_base_entity* >( interfaces.m_client_entity_list->get_client_entity_from_handle( this->active_weapon_handle( ) ) );
}

c_vector c_base_entity::get_bone_position( int idx )
{
	matrix3x4_t matrices[ 256 ];
	if ( !this->setup_bones( matrices, 256, 0x100, memory.m_globals->m_current_time ) ) {
		return { 0.f, 0.f, 0.f };
	}

	return { matrices[ idx ][ 0 ][ 3 ], matrices[ idx ][ 1 ][ 3 ], matrices[ idx ][ 2 ][ 3 ] };
}

c_vector c_base_entity::get_hitbox_position( const int idx )
{
	if ( idx == -1 ) {
		return { 0.f, 0.f, 0.f };
	}

	matrix3x4_t matrices[ 128 ];

	if ( !this->setup_bones( matrices, 128, 0x100, memory.m_globals->m_current_time ) ) {
		return { 0.f, 0.f, 0.f };
	}

	const auto model = this->get_model( );
	if ( !model ) {
		return { 0.f, 0.f, 0.f };
	}

	const auto studio_hdr = interfaces.m_model_info->get_studio_model( model );
	if ( !studio_hdr ) {
		return { 0.f, 0.f, 0.f };
	}

	const auto bbox = studio_hdr->get_hitbox( idx, 0 );
	if ( !bbox ) {
		return { 0.f, 0.f, 0.f };
	}

	const matrix3x4_t& matrix = matrices[ bbox->bone ];
	const float modifier      = bbox->radius != -1.0f ? bbox->radius : 0.0f;

	const auto min = math.vector_transform( bbox->mins - modifier, matrix );
	const auto max = math.vector_transform( bbox->maxs + modifier, matrix );

	return ( min + max ) * 0.5f;
}

int c_base_entity::get_weapon_type( )
{
	if ( !this ) {
		return -1;
	}

	if ( !this->item_definition_index( ) ) {
		return -1;
	}

	switch ( this->item_definition_index( ) ) {
	case WEAPON_DEAGLE:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_ELITE:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_FIVESEVEN:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_GLOCK:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_AK47:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_AUG:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_AWP:
		return WEAPON_TYPE_SNIPER;
	case WEAPON_FAMAS:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_G3SG1:
		return WEAPON_TYPE_SNIPER;
	case WEAPON_GALILAR:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_M249:
		return WEAPON_TYPE_MG;
	case WEAPON_M4A1:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_MAC10:
		return WEAPON_TYPE_SMG;
	case WEAPON_P90:
		return WEAPON_TYPE_SMG;
	case WEAPON_UMP45:
		return WEAPON_TYPE_SMG;
	case WEAPON_XM1014:
		return WEAPON_TYPE_SHOTGUN;
	case WEAPON_BIZON:
		return WEAPON_TYPE_SMG;
	case WEAPON_MAG7:
		return WEAPON_TYPE_SHOTGUN;
	case WEAPON_NEGEV:
		return WEAPON_TYPE_MG;
	case WEAPON_SAWEDOFF:
		return WEAPON_TYPE_SHOTGUN;
	case WEAPON_TEC9:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_TASER:
		return WEAPON_TYPE_TASER;
	case WEAPON_HKP2000:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_MP7:
		return WEAPON_TYPE_SMG;
	case WEAPON_MP9:
		return WEAPON_TYPE_SMG;
	case WEAPON_NOVA:
		return WEAPON_TYPE_SHOTGUN;
	case WEAPON_P250:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_SCAR20:
		return WEAPON_TYPE_SNIPER;
	case WEAPON_SG556:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_SSG08:
		return WEAPON_TYPE_SNIPER;
	case WEAPON_KNIFE:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_FLASHBANG:
		return WEAPON_TYPE_GRENADE;
	case WEAPON_HEGRENADE:
		return WEAPON_TYPE_GRENADE;
	case WEAPON_SMOKEGRENADE:
		return WEAPON_TYPE_GRENADE;
	case WEAPON_MOLOTOV:
		return WEAPON_TYPE_GRENADE;
	case WEAPON_DECOY:
		return WEAPON_TYPE_GRENADE;
	case WEAPON_INCGRENADE:
		return WEAPON_TYPE_GRENADE;
	case WEAPON_C4:
		return WEAPON_TYPE_INVALID;
	case WEAPON_KNIFE_T:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_M4A1_SILENCER:
		return WEAPON_TYPE_RIFLE;
	case WEAPON_USP_SILENCER:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_CZ75A:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_REVOLVER:
		return WEAPON_TYPE_PISTOL;
	case WEAPON_KNIFE_BAYONET:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_CSS:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_FLIP:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_GUT:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_KARAMBIT:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_M9_BAYONET:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_TACTICAL:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_FALCHION:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_BUTTERFLY:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_PUSH:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_CORD:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_CANIS:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_URSUS:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_OUTDOOR:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_STILETTO:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_WIDOWMAKER:
		return WEAPON_TYPE_KNIFE;
	case WEAPON_KNIFE_SKELETON:
		return WEAPON_TYPE_KNIFE;
	default:
		return WEAPON_TYPE_INVALID;
	}

	return false;
}

bool c_base_entity::can_see_player( c_base_entity* player, const c_vector& pos )
{
	trace_t tr;
	ray_t ray;
	trace_filter filter;
	filter.skip = this;

	auto start = get_eye_pos( );
	auto dir   = ( pos - start ).normalized( );

	ray.initialize( start, pos );
	interfaces.m_trace->run( ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr );

	return tr.entity == player || tr.fraction > 0.97f;
}

void c_base_entity::set_abs_origin( c_vector new_origin )
{
	using original_fn = void( __thiscall* )( void*, const c_vector& );
	static original_fn set_position_fn =
		( original_fn )( ( DWORD )memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8" ) ) );
	set_position_fn( this, new_origin );
}

bool c_base_entity::physics_run_think(int idx) {
	using PhysicsRunThinkFn      = bool( __thiscall* )( void*, int );
	static auto oPhysicsRunThink = reinterpret_cast< PhysicsRunThinkFn >(
		memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 83 EC 10 53 56 57 8B F9 8B 87" ) ) );
	return oPhysicsRunThink( this, idx );
}