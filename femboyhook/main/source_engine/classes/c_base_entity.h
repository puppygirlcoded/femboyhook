#pragma once
#include "../netvars/netvars.h"
#include "../structs/matrix_t.h"
#include "c_model_info.h"
#include "c_vector.h"

#include "../../hashing/strings/xorstr.h"
#include "c_client_class.h"
#include "c_weapon_data.h"

enum precipitation_type_t {
	PRECIPITATION_TYPE_RAIN = 0,
	PRECIPITATION_TYPE_SNOW,
	PRECIPITATION_TYPE_ASH,
	PRECIPITATION_TYPE_SNOWFALL,
	PRECIPITATION_TYPE_PARTICLERAIN,
	PRECIPITATION_TYPE_PARTICLEASH,
	PRECIPITATION_TYPE_PARTICLERAINSTORM,
	PRECIPITATION_TYPE_PARTICLESNOW,
	NUM_PRECIPITATION_TYPES
};
// How many bits to use to encode an edict.
#define MAX_EDICT_BITS 11 // # of bits needed to represent max edicts
// Max # of edicts in a level
#define MAX_EDICTS ( 1 << MAX_EDICT_BITS )
template< typename T >
constexpr T nigger( void* class_base, int index )
{
	return ( *( T** )class_base )[ index ];
}

#define MEMBER_FUNC_ARGS( ... )                                                                                                                      \
	( this, __VA_ARGS__ );                                                                                                                           \
	}
#define DECLARE_VFUNC( i, f, s )                                                                                                                     \
	auto f                                                                                                                                           \
	{                                                                                                                                                \
		return nigger< s >( this, i ) MEMBER_FUNC_ARGS
//
// class c_client_networkable
//{
// private:
//	enum e_indexes {
//		_get_client_class = 8,
//		_dormant          = 9,
//		_index            = 10
//	};
//
// public:
//	/*int index( )
//	{
//		return this->in
//		using fn = int( __thiscall* )( c_client_networkable* );
//		return ( *( fn** )this )[ this->e_indexes::_index ]( this );
//	}*/
//
//	/*void release( )
//	{
//		using fn = void( __thiscall* )( void* );
//		return ( *( fn** )this )[ 1 ]( this );
//	}
//
//	bool dormant( )
//	{
//		using fn = bool( __thiscall* )( c_client_networkable* );
//		return ( *( fn** )this )[ this->e_indexes::_dormant ]( this );
//	}*/
//
//	/*c_client_class* get_client_class( )
//	{
//		using fn = c_client_class*( __thiscall* )( c_client_networkable* );
//		return ( *( fn** )this )[ this->e_indexes::_get_client_class ]( this );
//	}*/
//	/*DECLARE_VFUNC( 4, on_pre_data_changed( int type ), void( __thiscall* )( void*, int ) )( type );
//	DECLARE_VFUNC( 5, on_data_changed( int type ), void( __thiscall* )( void*, int ) )( type );
//	DECLARE_VFUNC( 6, pre_data_update( int type ), void( __thiscall* )( void*, int ) )( type );
//	DECLARE_VFUNC( 7, post_data_update( int type ), void( __thiscall* )( void*, int ) )( type );*/
//	// void on_pre_data_changed( int type )
//	//{
//	//	using fn = void( __thiscall* )( void*, int );
//	//	return ( *( fn** )this )[ 4 ]( this, type );
//	// }
//	//
//	// DECLARE_VFUNC( 6, pre_data_update( int type ), void( __thiscall* )( void*, int ) )( type );
//	//
//	// void on_data_changed( int type )
//	//{
//	//	using fn = void( __thiscall* )( void*, int );
//	//	return ( *( fn** )this )[ 5 ]( this, type );
//	// }
//	//
//	// void post_data_update( int type )
//	//{
//	//	using fn = void( __thiscall* )( void*, int );
//	//	return ( *( fn** )this )[ 7 ]( this, type );
//	// }
//};

class c_collideable
{
private:
	enum e_indexes {
		_mins = 1,
		_maxs = 2
	};

public:
	c_vector& mins( )
	{
		using fn = c_vector&( __thiscall* )( void* );
		return ( *( fn** )this )[ this->e_indexes::_mins ]( this );
	}

	c_vector& maxs( )
	{
		using fn = c_vector&( __thiscall* )( void* );
		return ( *( fn** )this )[ this->e_indexes::_maxs ]( this );
	}
};

// i am incredibly lazy
template< typename I >
__forceinline I v( void* iface, uint32_t index )
{
	return ( I )( ( *( uint32_t** )iface )[ index ] );
}

class c_user_cmd;

// shitty mess
// TODO: clean this mf up :sob:
class CBaseHandle;

class IHandleEntity
{
public:
	virtual ~IHandleEntity( ) { }
	virtual void SetRefEHandle( const CBaseHandle& handle ) = 0;
	virtual const CBaseHandle& GetRefEHandle( ) const       = 0;
};
class ICollideable;
class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class entity_t;
class IClientThinkable;
class IClientAlphaProperty;
class cclientunknown;
class bf_write;
class bf_read;
class c_client_networkable
{
public:
	virtual cclientunknown* get_client_unknown( )             = 0;
	virtual void release( )                                   = 0;
	virtual c_client_class* get_client_class( )               = 0;
	virtual void notify_should_transmit( int state )          = 0;
	virtual void on_pre_data_changed( int update_type )       = 0;
	virtual void on_data_changed( int update_type )           = 0;
	virtual void pre_data_update( int update_type )           = 0;
	virtual void post_data_update( int update_type )          = 0;
	virtual void __unkn( void )                               = 0;
	virtual bool is_dormant( void )                           = 0;
	virtual int index( void ) const                           = 0;
	virtual void receive_message( int classID, bf_read& msg ) = 0;
	virtual void* get_data_table_base_ptr( )                  = 0;
	virtual void set_destroyed_on_recreate_entities( void )   = 0;
};

class c_client_unknown : IHandleEntity
{
public:
	virtual ICollideable* GetCollideable( )             = 0;
	virtual c_client_networkable* get_networkable( ) = 0;
	virtual IClientRenderable* GetClientRenderable( )   = 0;
	virtual IClientEntity* GetIClientEntity( )          = 0;
	virtual entity_t* GetBaseEntity( )                  = 0;
	virtual IClientThinkable* GetClientThinkable( )     = 0;
	// virtual IClientModelRenderable*  GetClientModelRenderable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty( ) = 0;
};

class c_client_renderable
{
public:
	virtual c_client_unknown* GetIClientUnknown( )  = 0;
	virtual c_vector const& GetRenderOrigin( void ) = 0;
	virtual c_vector const& GetRenderAngles( void ) = 0;
	virtual bool ShouldDraw( void )                 = 0;
	virtual int GetRenderFlags( void )              = 0; // ERENDERFLAGS_xxx
	virtual void Unused( void ) const { }
	virtual unsigned short GetShadowHandle( ) const                                                                                     = 0;
	virtual unsigned short& RenderHandle( )                                                                                             = 0;
	virtual const c_model* GetModel( ) const                                                                                            = 0;
	virtual int DrawModel( int flags, const int /*RenderableInstance_t*/& instance )                                                    = 0;
	virtual int GetBody( )                                                                                                              = 0;
	virtual void GetColorModulation( float* color )                                                                                     = 0;
	virtual bool LODTest( )                                                                                                             = 0;
	virtual bool SetupBones( matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime )                             = 0;
	virtual void SetupWeights( const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights ) = 0;
	virtual void DoAnimationEvents( void )                                                                                              = 0;
	virtual void* GetPVSNotifyInterface( )                                                                                              = 0;
	virtual void GetRenderBounds( c_vector& mins, c_vector& maxs )                                                                      = 0;
	virtual void GetRenderBoundsWorldspace( c_vector& mins, c_vector& maxs )                                                            = 0;
	virtual void GetShadowRenderBounds( c_vector& mins, c_vector& maxs, int /*ShadowType_t*/ shadowType )                               = 0;
	virtual bool ShouldReceiveProjectedTextures( int flags )                                                                            = 0;
	virtual bool GetShadowCastDistance( float* pDist, int /*ShadowType_t*/ shadowType ) const                                           = 0;
	virtual bool GetShadowCastDirection( c_vector* pDirection, int /*ShadowType_t*/ shadowType ) const                                  = 0;
	virtual bool IsShadowDirty( )                                                                                                       = 0;
	virtual void MarkShadowDirty( bool bDirty )                                                                                         = 0;
	virtual IClientRenderable* GetShadowParent( )                                                                                       = 0;
	virtual IClientRenderable* FirstShadowChild( )                                                                                      = 0;
	virtual IClientRenderable* NextShadowPeer( )                                                                                        = 0;
	virtual int /*ShadowType_t*/ ShadowCastType( )                                                                                      = 0;
	virtual void CreateModelInstance( )                                                                                                 = 0;
	virtual unsigned short GetModelInstance( )                                                                                          = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform( )                                                                            = 0;
	virtual int LookupAttachment( const char* pAttachmentName )                                                                         = 0;
	virtual bool GetAttachment( int number, c_vector& origin, c_vector& angles )                                                        = 0;
	virtual bool GetAttachment( int number, matrix3x4_t& matrix )                                                                       = 0;
	virtual float* GetRenderClipPlane( void )                                                                                           = 0;
	virtual int GetSkin( )                                                                                                              = 0;
	virtual void OnThreadedDrawSetup( )                                                                                                 = 0;
	virtual bool UsesFlexDelayedWeights( )                                                                                              = 0;
	virtual void RecordToolMessage( )                                                                                                   = 0;
	virtual bool ShouldDrawForSplitScreenUser( int nSlot )                                                                              = 0;
	virtual uint8_t OverrideAlphaModulation( uint8_t nAlpha )                                                                           = 0;
	virtual uint8_t OverrideShadowAlphaModulation( uint8_t nAlpha )                                                                     = 0;
};

class c_client_entity : public c_client_unknown, public c_client_renderable, public c_client_networkable
{
public:
	virtual void release( void )                          = 0;
	virtual const c_vector& get_abs_origin( void ) const  = 0;
	virtual const c_vector& get_abs_anglesn( void ) const = 0;
};

struct entity_handle_t {
	uintptr_t handle;

	constexpr entity_handle_t( ) : handle( 0xFFFFFFFF ) { }

	constexpr entity_handle_t( unsigned long handle ) : handle( handle ) { }

	class c_base_entity* get( ) const;

	bool operator==( const entity_handle_t& other ) const;
	bool operator==( class c_base_entity* entity ) const;

	bool operator!=( const entity_handle_t& other ) const;
	bool operator!=( class c_base_entity* entity ) const;
};

class c_base_entity : public c_client_entity
{
private:
	enum e_indexes {
		_collideable         = 3,
		_renderable          = 4,
		_get_model           = 8,
		_setup_bones         = 13,
		_data_desc_map       = 15,
		_prediction_desc_map = 17,
		_is_player           = 158
	};

public:
	data_map_t* data_desc_map( )
	{
		using fn = data_map_t*( __thiscall* )( c_base_entity* );
		return ( *( fn** )this )[ this->e_indexes::_data_desc_map ]( this );
	}

	data_map_t* prediction_desc_map( )
	{
		if ( !this )
			return { };
		using fn = data_map_t*( __thiscall* )( c_base_entity* );
		return ( *( fn** )this )[ this->e_indexes::_prediction_desc_map ]( this );
	}

	c_client_networkable* networkable( )
	{
	    if ( !this )
	        return { };
	    return reinterpret_cast< c_client_networkable* >( uintptr_t( this ) + 0x8 );
	}

	void* renderable( )
	{
		return reinterpret_cast< void* >( uintptr_t( this ) + 0x4 );
	}

	bool in_air( )
	{
		return !(this->flags( ) & 1);
	}

	c_collideable* collideable( )
	{
		if ( !this )
			return { };
		using fn = c_collideable*( __thiscall* )( void* );
		return ( *( fn** )this )[ 3 ]( this );
	}

	// CUtlVector< matrix3x4_t >& get_cached_bone_data( )
	//{
	//	return *( CUtlVector< matrix3x4_t >* )( ( uintptr_t )this + 0x2914 );
	// }

	bool is_player( )
	{
		using fn = bool( __thiscall* )( c_base_entity* );
		return ( *( fn** )this )[ this->e_indexes::_is_player ]( this );
	}

	bool is_alive( )
	{
		if ( !this )
			return false;
		return ( this->life_state( ) == 0 /* e_life_states::life_state_alive */ );
	}

	c_vector get_hitbox_position( const int idx );

	bool setup_bones( matrix3x4_t* out, int max_bones, int mask, int time )
	{
		if ( !this )
			return false;

		using fn = bool( __thiscall* )( void*, matrix3x4_t*, int, int, float );
		return ( *( fn** )renderable( ) )[ this->e_indexes::_setup_bones ]( renderable( ), out, max_bones, mask, time );
	}

	c_model* get_model( )
	{
		using fn = c_model*( __thiscall* )( void* );
		return ( *( fn** )renderable( ) )[ this->e_indexes::_get_model ]( renderable( ) );
	}

	void get_eye_pos_rel( c_vector& vec )
	{
		using fn = void( __thiscall* )( void*, c_vector& );
		return ( *( fn** )this )[ 169 ]( this, vec );
	}

	c_vector get_eye_pos( bool should_correct = true )
	{
		c_vector vec_pos = { };
		get_eye_pos_rel( vec_pos );
		return vec_pos;

		// if (is_used_new_animation_state() && should_correct) {
		//
		// }
	}

	c_user_cmd** current_cmd( )
	{
		return reinterpret_cast< c_user_cmd** >( std::uintptr_t( this ) + 0x3348 );
	}
	c_user_cmd& last_cmd( )
	{
		return *reinterpret_cast< c_user_cmd* >( std::uintptr_t( this ) + 0x3288 );
	}

	bool physics_run_think( int index );

	void pre_think( )
	{
		// return v< void( __thiscall* )( void* ) >( this, 318 )( this );
		using fn = void( __thiscall* )( void* );
		return ( *( fn** )this )[ 318 ]( this );
	}
	void think( )
	{
		// return v< void( __thiscall* )( void* ) >( this, 139 )( this );
		using fn = void( __thiscall* )( void* );
		return ( *( fn** )this )[ 139 ]( this );
	}
	void post_think( )
	{
		// return v< void( __thiscall* )( void* ) >( this, 319 )( this );
		using fn = void( __thiscall* )( void* );
		return ( *( fn** )this )[ 319 ]( this );
	}

	void update_colision_bounds( )
	{
		using original_fn = void( __thiscall* )( void* );
		( *( original_fn** )this )[ 340 ]( this );
	}

	float& fall_velocity( )
	{
		return *( float* )( this + 0x3024 );
	}

	int32_t& get_sequence( )
	{
		return *( int32_t* )( this + 0x28C0 );
	}

	void set_sequence( int index )
	{
		if ( !this )
			return;
		v< void( __thiscall* )( decltype( this ), int ) >( this, 220 )( this, index );
	}

	bool falling() {
		return this->velocity( ).m_z < 0.f;
	}

	void studio_frame_advance( )
	{
		v< void( __thiscall* )( decltype( this ) ) >( this, 220 )( this );
	}

	bool is_visible( c_base_entity* local, const c_vector& src, const c_vector& dst );

	bool can_see_player( c_base_entity* player, const c_vector& pos );

	int get_weapon_type( );

	void set_abs_origin( c_vector new_origin );

	c_base_entity* get_active_weapon( );

	c_vector get_bone_position( int idx );

	add_offset( std::uint32_t, ent_index, 0x100 );

	/* CBasePlayer */
	add_variable( int, health, "CBasePlayer->m_iHealth" );
	add_variable( int, life_state, "CBasePlayer->m_lifeState" );
	add_variable( int, flags, "CBasePlayer->m_fFlags" );
	add_variable( int, tick_base, "CBasePlayer->m_nTickBase" );
	add_variable( float, duck_amount, "CBasePlayer->m_flDuckAmount" );
	add_variable( bool, ducked, "CBasePlayer->m_bDucked" );
	add_variable( c_vector, velocity, "CBasePlayer->m_vecVelocity[0]" );
	add_variable( c_vector, stamina, "CCSPlayer->m_flStamina" );
	add_variable( c_vector, view_offset, "CBasePlayer->m_vecViewOffset[0]" );
	add_variable( c_vector, aim_punch_angle, "CBasePlayer->m_aimPunchAngle" );
	add_variable( unsigned long, observer_target, "CBasePlayer->m_hObserverTarget" );
	add_variable( int, model_index, "CBasePlayer->m_nModelIndex" );
	add_variable( bool, is_spotted, "CBasePlayer->m_bSpotted" );
	add_variable( int, view_model_handle, "CBasePlayer->m_hViewModel[0]" );

	/* CBaseEntity */
	add_variable( int, team, "CBaseEntity->m_iTeamNum" );
	add_variable( c_vector, origin, "CBaseEntity->m_vecOrigin" );
	add_variable( float, simulation_time, "CBaseEntity->m_flSimulationTime" );

	/* CCSPlayer */
	add_variable_offset( int, is_used_new_animation_state, "CCSPlayer->m_flLastExoJumpTime", 0x8 );
	add_variable( float, flash_duration, "CCSPlayer->m_flFlashDuration" );
	add_variable( int, shots_fired, "CCSPlayer->m_iShotsFired" );

	/* DT_CSPlayer */
	add_variable( c_vector, abs_eye_angles, "DT_CSPlayer->m_angEyeAngles" );
	add_variable( bool, has_helmet, "DT_CSPlayer->m_bHasHelmet" );
	add_variable( int, armor_value, "DT_CSPlayer->m_ArmorValue" );
	add_variable( bool, has_defuser, "DT_CSPlayer->m_bHasDefuser" );
	add_variable( bool, is_scoped, "DT_CSPlayer->m_bIsScoped" );

	/* DT_BasePlayer */

	/* CBaseCombatCharacter */
	add_variable( unsigned int, active_weapon_handle, "CBaseCombatCharacter->m_hActiveWeapon" );
	entity_handle_t* my_weapons() {
		return ( entity_handle_t* )( ( uintptr_t )this + 0x2E08 );
	}
	/* CBaseCombatWeapon */
	add_variable( float, next_primary_attack, "CBaseCombatWeapon->m_flNextPrimaryAttack" );
	add_variable( float, next_secondary_attack, "CBaseCombatWeapon->m_flNextSecondaryAttack" );
	add_variable( int, ammo, "CBaseCombatWeapon->m_iClip1" );
	add_variable( int, ammo_reserve, "CBaseCombatWeapon->m_iPrimaryReserveAmmoCount" );

	/* CBaseAttributableItem */
	add_variable( short, item_definition_index, "CBaseAttributableItem->m_iItemDefinitionIndex" );

	/* datamap variables */
	add_datafield( int, move_type, this->prediction_desc_map( ), "m_MoveType" );
	add_datafield( const matrix3x4_t, coordinate_frame, this->data_desc_map( ), "m_rgflCoordinateFrame" );

	/* DT_CollisionProperty */
	add_variable( c_vector, mins, "DT_CollisionProperty->m_vecMins" );
	add_variable( c_vector, maxs, "DT_CollisionProperty->m_vecMaxs" );

	/* CEnvTonemapController */
	add_variable( bool, use_custom_bloom_scale, "CEnvTonemapController->m_bUseCustomBloomScale" );
	add_variable( float, custom_bloom_scale, "CEnvTonemapController->m_flCustomBloomScale" );
	add_variable( bool, use_custom_auto_exposure_min, "CEnvTonemapController->m_bUseCustomAutoExposureMin" );
	add_variable( float, custom_auto_exposure_min, "CEnvTonemapController->m_flCustomAutoExposureMin" );
	add_variable( bool, use_custom_auto_exposure_max, "CEnvTonemapController->m_bUseCustomAutoExposureMax" );
	add_variable( float, custom_auto_exposure_max, "CEnvTonemapController->m_flCustomAutoExposureMax" );
};

class c_precipitation_entity : public c_base_entity
{
public:
	add_variable( int, precipitation_type, "DT_Precipitation->m_nPrecipType" );
};

class c_weapon : public c_base_entity
{
public:
	//NETVAR( "DT_BaseCombatWeapon", "m_flNextPrimaryAttack", next_primary_attack, float )
	//NETVAR( "DT_BaseCombatWeapon", "m_flNextSecondaryAttack", next_secondary_attack, float )
	//NETVAR( "DT_BaseCombatWeapon", "m_iClip1", clip1_count, int )
	//NETVAR( "DT_BaseCombatWeapon", "m_iClip2", clip2_count, int )
	//NETVAR( "DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount", primary_reserve_ammo_acount, int )
	//NETVAR( "DT_WeaponCSBase", "m_flRecoilIndex", recoil_index, float )
	//NETVAR( "DT_WeaponCSBaseGun", "m_zoomLevel", zoom_level, float )
	//NETVAR( "DT_BaseAttributableItem", "m_iItemDefinitionIndex", item_definition_index, short )
	//NETVAR( "DT_BaseCombatWeapon", "m_iEntityQuality", entity_quality, int )

	float inaccuracy( )
	{
		using original_fn = float( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 482 ]( this );
	}

	float get_spread( )
	{
		using original_fn = float( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 452 ]( this );
	}
	float get_inaccuracy( )
	{
		using original_fn = float( __thiscall* )( void* );
		return ( *( original_fn** )this )[ 482 ]( this );
	}
	void update_accuracy_penalty( )
	{
		using original_fn = void( __thiscall* )( void* );
		( *( original_fn** )this )[ 483 ]( this );
	}
};

class c_economy_item : public c_base_entity
{
public:
	add_variable( entity_handle_t, world_model_handle, "CBaseCombatWeapon->m_hWeaponWorldModel" );
};

class c_viewmodel {
public:
	add_variable( int, model_index, "CBaseViewModel->m_nModelIndex" );
	add_variable( int, viewmodel_index, "CBaseViewModel->m_nViewModelIndex" );
	add_variable( int, owner, "CBaseViewModel->m_hOwner" );
};