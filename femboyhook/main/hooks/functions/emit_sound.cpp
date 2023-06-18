#include "../../features/prediction/prediction.h"
#include "../hooks.h"

void __fastcall n_detoured_functions::emit_sound( uintptr_t ecx, uintptr_t edx, void* filter, int index, int ichannel, const char* soundentry,
                                                  unsigned int soundhash, const char* sample, float flvolume, int seed, float flattenuation,
                                                  int iflags, int ipitch, const c_vector* origin, const c_vector* direction, void* utlvecorigins,
                                                  bool updatepositions, float soundtime, int speakerentity, int unk )
{
	static auto original = hooks.emit_sound.get_original< decltype( &n_detoured_functions::emit_sound ) >( );

	if ( variables.misc.helpers.auto_accept && !strcmp( soundentry, xs( "UIPanorama.popup_accept_match_beep" ) ) ) {
		static auto local_player_ready = reinterpret_cast< bool( __stdcall* )( const char* ) >(
			memory.m_modules[ e_module_names::client ].find_pattern( xs( "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12" ) ) );
		local_player_ready( xs( "" ) );
		interfaces.m_engine->execute_command( xs( "echo [fbh] auto accepted match." ) );
	}

	if ( !prediction_system.is_simulating )
		return original( ecx, edx, filter, index, ichannel, soundentry, soundhash, sample, flvolume, seed, flattenuation, iflags, ipitch, origin,
		                 direction, utlvecorigins, updatepositions, soundtime, speakerentity, unk );
}
