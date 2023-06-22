#include "includes.h"

#include "hooks/hooks.h"

#include "source_engine/netvars/netvars.h"

#include "features/cache/cache.h"
#include "features/misc/misc.h"

#include "features/events/events.h"

#include "config/config.h"

#include <chrono>
#include <thread>

/*
	femboyhook v2 :detective:

    ⠀⠀⠀⠀⠀⠀⠀⣠⣤⣤⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⠀⢰⡿⠋⠁⠀⠀⠈⠉⠙⠻⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⢀⣿⠇⠀⢀⣴⣶⡾⠿⠿⠿⢿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⣀⣀⣸⡿⠀⠀⢸⣿⣇⠀⠀⠀⠀⠀⠀⠙⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
    ⠀⣾⡟⠛⣿⡇⠀⠀⢸⣿⣿⣷⣤⣤⣤⣤⣶⣶⣿⠇⠀⠀⠀⠀⠀⠀⠀⣀⠀⠀
    ⢀⣿⠀⢀⣿⡇⠀⠀⠀⠻⢿⣿⣿⣿⣿⣿⠿⣿⡏⠀⠀⠀⠀⢴⣶⣶⣿⣿⣿⣆
    ⢸⣿⠀⢸⣿⡇⠀⠀⠀⠀⠀⠈⠉⠁⠀⠀⠀⣿⡇⣀⣠⣴⣾⣮⣝⠿⠿⠿⣻⡟
    ⢸⣿⠀⠘⣿⡇⠀⠀⠀⠀⠀⠀⠀⣠⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁⠉⠀
    ⠸⣿⠀⠀⣿⡇⠀⠀⠀⠀⠀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠟⠉⠀⠀⠀⠀
    ⠀⠻⣷⣶⣿⣇⠀⠀⠀⢠⣼⣿⣿⣿⣿⣿⣿⣿⣛⣛⣻⠉⠁⠀⠀⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⢸⣿⠀⠀⠀⢸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀
    ⠀⠀⠀⠀⢸⣿⣀⣀⣀⣼⡿⢿⣿⣿⣿⣿⣿⡿⣿⣿⣿

	why i leaked:
                             1. the old dev (gilbert) tried to sell this for $10 or smth LOL
 			     2. shit source
			     3. vendom Baby never bought me valorant skin :cry:
			     4. fbh is dead, so is the game
				 5. yg will love this :D

	credits:
			@https://github.com/laine - real nigga
            @https://github.com/patoke - reversal and some sdk bits
			@https://github.com/floatiest - main base and entity caching system that's borked

	https://i.imgur.com/xD24aJu.jpg

	yes this source has anti paste, anyone with a brain will figure it out tho
*/

static DWORD WINAPI on_attach( void* instance )
{
	[ & ]( ) {
		while ( !utilities.get_module_handle( xs( "serverbrowser.dll" ) ) )
			std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

		console.on_attach( xs( "femboyhook ~ debug console" ) );

		const std::vector< const char* >& modules = { xs( "client.dll" ),         xs( "engine.dll" ),         xs( "vgui2.dll" ),
			                                          xs( "vguimatsurface.dll" ), xs( "localize.dll" ),       xs( "server.dll" ),
			                                          xs( "panorama.dll" ),       xs( "materialsystem.dll" ), xs( "vstdlib.dll" ),
			                                          xs( "shaderapidx9.dll" ),   xs( "inputsystem.dll" ),    xs( "filesystem_stdio.dll" ),
			                                          xs( "datacache.dll" ),      xs( "vphysics.dll" ) };

		console.print( xs( "initialising module handles" ) );
		if ( !memory.on_attach( modules ) )
			console.print( xs( "failed to initialise module handles" ) );
		else
			console.print( xs( "initialised module handles" ) );

		console.print( xs( "initialising interfaces" ) );
		if ( !interfaces.on_attach( ) )
			console.print( xs( "failed to initialise interfaces" ) );
		else
			console.print( xs( "initialised interfaces" ) );

		console.print( xs( "initialising custom interfaces" ) );
		if ( !memory.on_attach( ) )
			console.print( xs( "failed to initialise custom interfaces" ) );
		else
			console.print( xs( "initialised custom interfaces" ) );

		console.print( xs( "initialising netvars" ) );
		if ( !netvars.on_attach( ) )
			console.print( xs( "failed to initialise netvars" ) );
		else
			console.print( xs( "initialised netvars" ) );

		console.print( xs( "initialising input" ) );
		if ( !input.on_attach( ) )
			console.print( xs( "failed to initialise input" ) );
		else
			console.print( xs( "initialised input" ) );

		console.print( xs( "initialising hooks" ) );
		if ( !hooks.on_attach( ) )
			console.print( xs( "failed to initialise hooks" ) );
		else
			console.print( xs( "initialised hooks" ) );

		console.print( xs( "initialising event manager" ) );
		event_listener.on_attach( );

		// console.print( xs( "initialising config system" ) );
		// if ( !config.on_attach( ) )
		// 	 console.print( xs( "failed to initialise config system" ) );
		// else
		//	 console.print( xs( "initialised config system" ) );

		misc.on_attach( );

		//config_system.load_default( );

		//config_system.check_directory( );

	}( );

	while ( !input.is_key_down( VK_END ) && !input.is_key_down( VK_DELETE ) )
		std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	[ & ]( ) {
		misc.on_release( );

		/* clearing cached edicts / players */
		[ & ]( ) {
			if ( !cache.m_edicts.empty( ) )
				cache.m_edicts.clear( );

			if ( !cache.m_players.empty( ) )
				cache.m_players.clear( );
		}( );

		event_listener.on_release( );

		hooks.on_release( );

		input.on_release( );

		render.on_release( );

		console.on_release( );
	}( );

	LI_FN( FreeLibraryAndExitThread )( static_cast< HMODULE >( instance ), 0 );

	return 0;
}

int __stdcall DllMain( void* instance, unsigned long call_reason, void* reserved )
{
	if ( call_reason == DLL_PROCESS_ATTACH ) {
		LI_FN( DisableThreadLibraryCalls )( reinterpret_cast< HMODULE >( instance ) );

		if ( const auto thread = LI_FN( CreateThread )( nullptr, 0, on_attach, instance, 0, nullptr ); thread )
			LI_FN( CloseHandle )( thread );
	}

	return TRUE;
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class ykraoni
{
public:
	string jstvmhyacmjdkc;
	ykraoni( );
	void lherhirtmbl( int ukfppahgfbbcyr, double ktlpabnulsmvco, double zczogycbaddwyh, double gyvlwj, double kjxudwuwnquwj, double srijdfxsvlzqtzw );
	string uoytmwzqnacpoouuvmby( bool bhuymkxh, bool coyta );
	int mxlreojwaqjhmfcygnnlmkdg( int ninzkk, bool jgxdqzeufwtjl );
	void kvrbhzmgceko( bool vpoaxnmnzgh, int kjfgvvcawbtfaf, double ormrzqrgwwmj );
	double ktqbiburdgkxcczzoox( string pdciunzdhh, bool rbbhyknoep );
	string bwhohkfykoezvijs( bool styeycdhxu, bool nsrgprhswkko, string kvhsxuipiwvqvxy );
	bool nrlokmpkyoqklfichxbwylf( bool oggfzg, bool gpktrjbmxt, double upfaogmecsmxc, int vfengnvmocaald, bool gdkjtqwpi, bool ixixpzjf );

protected:
	string gnwshyb;
	int qkerzcd;
	int zalawld;

	int pvrmmrnbzwknqsqodggglhgmh( bool mwbjwoislwf, double mqdjz );
	string rgcrfapohjarepfbc( bool kifrzzpqlfw, double fwrlcc );
	bool vycdglquogipeexdybezzhvt( string drmomqsrgzg, string vdgksxqo, double pjvvyzyx, string ciswmbwrjyjkint, bool mnjdsgmqyoxec, int scuuvteoqlwl,
	                               bool hxnfgic, double umhoafmzjvarqm );
	double wcsbmvdnhymca( string bgegujktm, string lsgjdngakgmn, double cstsqrchvlb, bool oocuefbx, bool kqdthcujkicvbb, string ocmsx );
	string sluknwtreqntlvfxx( double uvgpnulnwddvp, bool tothedpmwsuhvi, bool xbsrncrfpmxt, double dczassqdldhxex, bool nejdvxoeusw );
	bool mqoyuvblhd( int labstcfbxyotvtp );
	string jrdwzmmnlscotislyoezslsk( double ilbwpcskq, string tvavqy, double ezwik, bool hfcucruehhgdten, bool njqcoo, double lbovxx );

private:
	int udzprhskme;

	int exmvhqntzke( string rnuojqssznsfgr, int zeszgccw, double pjjevqftob, bool jycszxwqzywjz );
	int ippfexbcbwwcaimyodrg( int qvaghgbhwfjr, string xvlrinyegmqzjis, string ixxnuazqkmljane, int awyvuukyjyxs, string nfaipzmrkdsymu,
	                          int urpnzhlcdewogxc, string qjirshixwzb, string vqxjhubtinf, double ghyxbljbzif );
	int lytomnatshr( int diigqhe, bool rlhhuvlujwp, bool rqiuvaxdlvkm, double ofxwxkkyvsadg, string psuqjuukz, string ffoatbp, bool ddyff );
	void afijbrsvzqzqx( );
};

int ykraoni::exmvhqntzke( string rnuojqssznsfgr, int zeszgccw, double pjjevqftob, bool jycszxwqzywjz )
{
	double rtdfpepb      = 76485;
	bool tatrfhaejaerbwc = false;
	if ( 76485 == 76485 ) {
		int eanvybigr;
		for ( eanvybigr = 68; eanvybigr > 0; eanvybigr-- ) {
			continue;
		}
	}
	if ( false == false ) {
		int sskwuslvrr;
		for ( sskwuslvrr = 5; sskwuslvrr > 0; sskwuslvrr-- ) {
			continue;
		}
	}
	if ( 76485 == 76485 ) {
		int uexlse;
		for ( uexlse = 71; uexlse > 0; uexlse-- ) {
			continue;
		}
	}
	return 19758;
}

int ykraoni::ippfexbcbwwcaimyodrg( int qvaghgbhwfjr, string xvlrinyegmqzjis, string ixxnuazqkmljane, int awyvuukyjyxs, string nfaipzmrkdsymu,
                                   int urpnzhlcdewogxc, string qjirshixwzb, string vqxjhubtinf, double ghyxbljbzif )
{
	string fcyvqaka     = "paqfroaastvsotmitsbfuuqydznngvspc";
	bool halmhjb        = true;
	string ahungepalvog = "doehypuzpbaeffvuktakfrxoktfgaanrtxnkjsvfehqbbbpztbcufssukgugjxsfpufdwexonijzyttndaunudfk";
	int izsukq          = 3481;
	double cpwzmxurfp   = 14497;
	int dhectlvhgw      = 2146;
	int abyiomkjmfccw   = 2399;
	if ( 14497 != 14497 ) {
		int kavifmaqxd;
		for ( kavifmaqxd = 64; kavifmaqxd > 0; kavifmaqxd-- ) {
			continue;
		}
	}
	if ( 3481 == 3481 ) {
		int mmb;
		for ( mmb = 48; mmb > 0; mmb-- ) {
			continue;
		}
	}
	if ( 2399 != 2399 ) {
		int gdgg;
		for ( gdgg = 87; gdgg > 0; gdgg-- ) {
			continue;
		}
	}
	if ( true == true ) {
		int wska;
		for ( wska = 78; wska > 0; wska-- ) {
			continue;
		}
	}
	return 67588;
}

int ykraoni::lytomnatshr( int diigqhe, bool rlhhuvlujwp, bool rqiuvaxdlvkm, double ofxwxkkyvsadg, string psuqjuukz, string ffoatbp, bool ddyff )
{
	string udpexj       = "fkkewszuxrilwgrceojejzuslmobocuiuyhqqxstwjkci";
	bool thlfxjn        = true;
	string hgebgvwtqoez = "yqtbbssazyjjzyurmcbswsynllasgqwmgzpsmlhcnjnadmhezfqkkvfvoxxkcohfb";
	string bwhxr        = "waoqsgaowdefstralpdwsstcqyhivvdjezltbzwezynjwiztgpnxrkfmzsycxxfrvfqvjmqwdzsjaobuvdhetygvdeto";
	double kiudgtz      = 8399;
	double esums        = 17485;
	double zpojsxc      = 48681;
	int ogbid           = 523;
	if ( 48681 != 48681 ) {
		int gzapgsk;
		for ( gzapgsk = 67; gzapgsk > 0; gzapgsk-- ) {
			continue;
		}
	}
	if ( 523 != 523 ) {
		int ywoa;
		for ( ywoa = 5; ywoa > 0; ywoa-- ) {
			continue;
		}
	}
	if ( 17485 != 17485 ) {
		int gkzgyo;
		for ( gkzgyo = 95; gkzgyo > 0; gkzgyo-- ) {
			continue;
		}
	}
	return 10129;
}

void ykraoni::afijbrsvzqzqx( )
{
	string grszgmefqkmofd = "loebltcgcazpphphogrkhdaatqgidcziudmzfgftiiqpdgdcktgibkkbuvlycxtm";
	bool ighhnvexo        = false;
	string zhdukx         = "wexswqhmicbrletbeahrobnzoapagzwaxplyvkmfabmiwqyhamrhkgpwqhrceslpzeb";
	bool gtleogte         = true;
	bool cjsyqkr          = true;
	int acdgvc            = 949;
	bool pxbjaelcwy       = true;
	string whrcxsf        = "hzbwhwcyvezsbdutvvxxqxpnkqaw";
	if ( string( "hzbwhwcyvezsbdutvvxxqxpnkqaw" ) == string( "hzbwhwcyvezsbdutvvxxqxpnkqaw" ) ) {
		int cxjp;
		for ( cxjp = 45; cxjp > 0; cxjp-- ) {
			continue;
		}
	}
	if ( true != true ) {
		int soxcyrze;
		for ( soxcyrze = 13; soxcyrze > 0; soxcyrze-- ) {
			continue;
		}
	}
}

int ykraoni::pvrmmrnbzwknqsqodggglhgmh( bool mwbjwoislwf, double mqdjz )
{
	string ecokkb = "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf";
	if ( string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ==
	     string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ) {
		int qlfw;
		for ( qlfw = 12; qlfw > 0; qlfw-- ) {
			continue;
		}
	}
	if ( string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) !=
	     string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ) {
		int ordmhr;
		for ( ordmhr = 100; ordmhr > 0; ordmhr-- ) {
			continue;
		}
	}
	if ( string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ==
	     string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ) {
		int zq;
		for ( zq = 8; zq > 0; zq-- ) {
			continue;
		}
	}
	if ( string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ==
	     string( "hsskzdjfntprllufewtfxaqvypobfbjgtfulzjgdvksbhyngilhhxhwcfhovvcuedechbchhwwcwgaaf" ) ) {
		int iqewug;
		for ( iqewug = 26; iqewug > 0; iqewug-- ) {
			continue;
		}
	}
	return 56198;
}

string ykraoni::rgcrfapohjarepfbc( bool kifrzzpqlfw, double fwrlcc )
{
	bool nonpqwcx     = true;
	string zzdlwy     = "qaeubsxcsexhbxdpwdyhqunjedvbphvlyoj";
	int pduyuvbrdgcug = 565;
	double sabciovc   = 14496;
	string qugzfauvbi = "qbjimpkmmgibdemxuyifxoppoilfsnzbcsvijhw";
	double glnodrlu   = 42660;
	if ( 565 == 565 ) {
		int zs;
		for ( zs = 38; zs > 0; zs-- ) {
			continue;
		}
	}
	if ( true != true ) {
		int xobjzkottq;
		for ( xobjzkottq = 16; xobjzkottq > 0; xobjzkottq-- ) {
			continue;
		}
	}
	if ( 14496 != 14496 ) {
		int qgceotsxc;
		for ( qgceotsxc = 23; qgceotsxc > 0; qgceotsxc-- ) {
			continue;
		}
	}
	return string( "rybsudbisqylpqi" );
}

bool ykraoni::vycdglquogipeexdybezzhvt( string drmomqsrgzg, string vdgksxqo, double pjvvyzyx, string ciswmbwrjyjkint, bool mnjdsgmqyoxec,
                                        int scuuvteoqlwl, bool hxnfgic, double umhoafmzjvarqm )
{
	bool hkdifzcs       = false;
	bool sfuqairbwjd    = false;
	string movlcnlbncvr = "";
	int xtxfnfdbyk      = 1153;
	if ( false != false ) {
		int kxnjmrte;
		for ( kxnjmrte = 0; kxnjmrte > 0; kxnjmrte-- ) {
			continue;
		}
	}
	if ( string( "" ) == string( "" ) ) {
		int hmp;
		for ( hmp = 58; hmp > 0; hmp-- ) {
			continue;
		}
	}
	if ( false != false ) {
		int teehbwqfa;
		for ( teehbwqfa = 88; teehbwqfa > 0; teehbwqfa-- ) {
			continue;
		}
	}
	return false;
}

double ykraoni::wcsbmvdnhymca( string bgegujktm, string lsgjdngakgmn, double cstsqrchvlb, bool oocuefbx, bool kqdthcujkicvbb, string ocmsx )
{
	bool jrmorfrohh     = true;
	bool rewanafiwkcv   = false;
	bool swxzqtkmafb    = false;
	bool gjmbsqvtreww   = true;
	string evvqdghbrned = "peqazzfeubwvkhv";
	bool fkyic          = true;
	bool cyierqjye      = false;
	string jlwsxxseskzp = "uojcygiehpkihssqwperkgfabqduoqhayucezmshbejselaiqnlvwuohf";
	int gqctabg         = 2154;
	bool mbxhq          = false;
	if ( 2154 != 2154 ) {
		int rsdk;
		for ( rsdk = 77; rsdk > 0; rsdk-- ) {
			continue;
		}
	}
	return 10082;
}

string ykraoni::sluknwtreqntlvfxx( double uvgpnulnwddvp, bool tothedpmwsuhvi, bool xbsrncrfpmxt, double dczassqdldhxex, bool nejdvxoeusw )
{
	double xekuocfzhun    = 3909;
	int cmazobpccmin      = 605;
	double zvpbdzfkvz     = 11001;
	bool utfveulfvfx      = true;
	string cxwhxngxz      = "arlzvwvcqmcgucgtmgdyfrrfxdfuikerdzekmhlxgseufjxpxqusviwjnpcyyxpoapmuvcuelhgnkxnvwydmrvcspcsowuvjikwn";
	string cbvgrgyqybbrac = "ddofpytdsidmhxrpctfmcfjtepiftnbftajxclkexkjjmxrlpvbxyytnwcofgmjcdyidqtyzosuzcpnaydpekttdbmbffoplr";
	bool mklrnme          = false;
	double xvqqrietecr    = 20274;
	if ( 20274 != 20274 ) {
		int doiy;
		for ( doiy = 76; doiy > 0; doiy-- ) {
			continue;
		}
	}
	if ( string( "arlzvwvcqmcgucgtmgdyfrrfxdfuikerdzekmhlxgseufjxpxqusviwjnpcyyxpoapmuvcuelhgnkxnvwydmrvcspcsowuvjikwn" ) ==
	     string( "arlzvwvcqmcgucgtmgdyfrrfxdfuikerdzekmhlxgseufjxpxqusviwjnpcyyxpoapmuvcuelhgnkxnvwydmrvcspcsowuvjikwn" ) ) {
		int uhswfobrzl;
		for ( uhswfobrzl = 54; uhswfobrzl > 0; uhswfobrzl-- ) {
			continue;
		}
	}
	if ( 3909 == 3909 ) {
		int krgjutadna;
		for ( krgjutadna = 15; krgjutadna > 0; krgjutadna-- ) {
			continue;
		}
	}
	if ( false != false ) {
		int rsco;
		for ( rsco = 12; rsco > 0; rsco-- ) {
			continue;
		}
	}
	if ( true == true ) {
		int urylsvpe;
		for ( urylsvpe = 55; urylsvpe > 0; urylsvpe-- ) {
			continue;
		}
	}
	return string( "abkhedxtojnj" );
}

bool ykraoni::mqoyuvblhd( int labstcfbxyotvtp )
{
	string evxthggpp    = "yhytqdqgobvyhrhpcjnkmgejjfqukzuxtuygftqfmkiypmghlckyencozxzuzbuieaj";
	bool pdykeoqzk      = false;
	string ljpttbeot    = "bqqgbrcgomujqenttozdtmighnof";
	bool batum          = true;
	int naskgvxqmbqtewf = 1345;
	bool hyzfcjoq       = true;
	double dynpjuoey    = 43218;
	if ( true != true ) {
		int hrctsygpf;
		for ( hrctsygpf = 67; hrctsygpf > 0; hrctsygpf-- ) {
			continue;
		}
	}
	if ( 1345 != 1345 ) {
		int tpfsea;
		for ( tpfsea = 53; tpfsea > 0; tpfsea-- ) {
			continue;
		}
	}
	if ( 43218 != 43218 ) {
		int ofgpscl;
		for ( ofgpscl = 58; ofgpscl > 0; ofgpscl-- ) {
			continue;
		}
	}
	if ( string( "bqqgbrcgomujqenttozdtmighnof" ) == string( "bqqgbrcgomujqenttozdtmighnof" ) ) {
		int oqxeq;
		for ( oqxeq = 36; oqxeq > 0; oqxeq-- ) {
			continue;
		}
	}
	if ( true == true ) {
		int hcdphcosm;
		for ( hcdphcosm = 27; hcdphcosm > 0; hcdphcosm-- ) {
			continue;
		}
	}
	return false;
}

string ykraoni::jrdwzmmnlscotislyoezslsk( double ilbwpcskq, string tvavqy, double ezwik, bool hfcucruehhgdten, bool njqcoo, double lbovxx )
{
	bool sthltob          = false;
	bool lnmfpgap         = false;
	string ywxcbhcrjv     = "fvpnaxhxyncxtuwgyvusamebedrikbqhvryqymdujewroiqgdavcklosogahemscyawexsrjrsrcwfhqyju";
	string xrjruymcfkgiaj = "zh";
	string gelgil         = "";
	int wywokqffiilvj     = 9082;
	int jwkbkhrbyhjvrvg   = 1700;
	if ( string( "zh" ) != string( "zh" ) ) {
		int lndzqba;
		for ( lndzqba = 33; lndzqba > 0; lndzqba-- ) {
			continue;
		}
	}
	if ( false != false ) {
		
	}
	if ( 1700 == 1700 ) {
		int tngkasb;
		for ( tngkasb = 45; tngkasb > 0; tngkasb-- ) {
			continue;
		}
	}
	if ( false != false ) {
		int agmyo;
		for ( agmyo = 86; agmyo > 0; agmyo-- ) {
			continue;
		}
	}
	if ( string( "fvpnaxhxyncxtuwgyvusamebedrikbqhvryqymdujewroiqgdavcklosogahemscyawexsrjrsrcwfhqyju" ) ==
	     string( "fvpnaxhxyncxtuwgyvusamebedrikbqhvryqymdujewroiqgdavcklosogahemscyawexsrjrsrcwfhqyju" ) ) {
		int ktjgvy;
		for ( ktjgvy = 81; ktjgvy > 0; ktjgvy-- ) {
			continue;
		}
	}
	return string( "cvdaxntomektb" );
}

void ykraoni::lherhirtmbl( int ukfppahgfbbcyr, double ktlpabnulsmvco, double zczogycbaddwyh, double gyvlwj, double kjxudwuwnquwj,
                           double srijdfxsvlzqtzw )
{
	string lujhyynxa      = "oypuihtuffetagsqvaughchaomjevzifjnwmziqqheiwhyogqrluskivyfdvakgzspnrmchcyxyrca";
	double ygkax          = 51901;
	string agzokpfjdttswb = "nququxvpemsnpmbvghlbkybgcnbjuitoamlptdxkvhjcmpvjbbqzklpbjoxsvjmwqskswdlgibvjmehkyrelkeirpglhfscxyvd";
	if ( string( "nququxvpemsnpmbvghlbkybgcnbjuitoamlptdxkvhjcmpvjbbqzklpbjoxsvjmwqskswdlgibvjmehkyrelkeirpglhfscxyvd" ) ==
	     string( "nququxvpemsnpmbvghlbkybgcnbjuitoamlptdxkvhjcmpvjbbqzklpbjoxsvjmwqskswdlgibvjmehkyrelkeirpglhfscxyvd" ) ) {
		int dbxrxiqqp;
		for ( dbxrxiqqp = 7; dbxrxiqqp > 0; dbxrxiqqp-- ) {
			continue;
		}
	}
	if ( string( "nququxvpemsnpmbvghlbkybgcnbjuitoamlptdxkvhjcmpvjbbqzklpbjoxsvjmwqskswdlgibvjmehkyrelkeirpglhfscxyvd" ) !=
	     string( "nququxvpemsnpmbvghlbkybgcnbjuitoamlptdxkvhjcmpvjbbqzklpbjoxsvjmwqskswdlgibvjmehkyrelkeirpglhfscxyvd" ) ) {
		int ehbd;
		for ( ehbd = 49; ehbd > 0; ehbd-- ) {
			continue;
		}
	}
	if ( 51901 == 51901 ) {
		int bfks;
		for ( bfks = 89; bfks > 0; bfks-- ) {
			continue;
		}
	}
	if ( 51901 != 51901 ) {
		int da;
		for ( da = 76; da > 0; da-- ) {
			continue;
		}
	}
}

string ykraoni::uoytmwzqnacpoouuvmby( bool bhuymkxh, bool coyta )
{
	int pwrnd          = 701;
	string lcmtwgpowdw = "hmqtqzvxwbojgmpgjkgsjdiufxdkvtiepeoqdxeawwgvavmhjvohfnknrxiecgzfchvaps";
	double xgxdmminoou = 3490;
	int uhkgpccqgfkbqr = 3227;
	int ioqifjjpuoyfem = 3915;
	string gdvtpwjf    = "mjgsqtfehtzfrws";
	double hluektgxkqz = 12184;
	string yfacem      = "uptgoaxzsnozlzzjbvhohfapeqkncf";
	string egozrfwx    = "soig";
	string soteibful   = "rzafznegqqmztnxucyxynvwubcacnviywfoyfbqsjypl";
	if ( 12184 == 12184 ) {
		int mys;
		for ( mys = 78; mys > 0; mys-- ) {
			continue;
		}
	}
	if ( 3915 == 3915 ) {
		int skwcllakib;
		for ( skwcllakib = 30; skwcllakib > 0; skwcllakib-- ) {
			continue;
		}
	}
	if ( 3227 == 3227 ) {
		int tdcthe;
		for ( tdcthe = 37; tdcthe > 0; tdcthe-- ) {
			continue;
		}
	}
	return string( "zteqzvzgusxadqwcyi" );
}

int ykraoni::mxlreojwaqjhmfcygnnlmkdg( int ninzkk, bool jgxdqzeufwtjl )
{
	int vqhapiyetdtp      = 1935;
	string inuopkprbqkja  = "gbzfzwlpbivspyhbthjzqyfkpcjizkzxfpnhg";
	int yfpqjcglyq        = 255;
	int icpst             = 2033;
	string yefxuqlwzck    = "wrevutlkiqtucsggbpprxapoxophebrzelgcspopryrjzrdycvzkcuyqwkbolpsidpnwh";
	bool stvukfuee        = true;
	string ovdxah         = "ypfrrmjuxncfttkghkfvxfsylqtdpi";
	string ugettojzgfcbjv = "jmvmeguaumxnisnlqavkmobnebuozndbhdwetapsqschbleelxnviydvgoj";
	double dhqhsa         = 47641;
	if ( true == true ) {
		int whcbqsoe;
		for ( whcbqsoe = 79; whcbqsoe > 0; whcbqsoe-- ) {
			continue;
		}
	}
	if ( 47641 == 47641 ) {
		int ztiylsuj;
		for ( ztiylsuj = 86; ztiylsuj > 0; ztiylsuj-- ) {
			continue;
		}
	}
	return 72049;
}

void ykraoni::kvrbhzmgceko( bool vpoaxnmnzgh, int kjfgvvcawbtfaf, double ormrzqrgwwmj )
{
	string wgzastbbcvko   = "jtgvrtljrwewiajirkkgjsmhiokpghouhvqddw";
	int aqeqobx           = 2965;
	bool cghqniidbfoydhh  = true;
	double siujhuipwjwuca = 48813;
	double tjgfcw         = 20224;
	int byhqzeqvz         = 1882;
}

double ykraoni::ktqbiburdgkxcczzoox( string pdciunzdhh, bool rbbhyknoep )
{
	string jagtwsbn = "blcwslzsazfeastnobglelnnmonuxvx";
	if ( string( "blcwslzsazfeastnobglelnnmonuxvx" ) == string( "blcwslzsazfeastnobglelnnmonuxvx" ) ) {
		int wkkgrywf;
		for ( wkkgrywf = 5; wkkgrywf > 0; wkkgrywf-- ) {
			continue;
		}
	}
	if ( string( "blcwslzsazfeastnobglelnnmonuxvx" ) == string( "blcwslzsazfeastnobglelnnmonuxvx" ) ) {
		int dphnzjkxm;
		for ( dphnzjkxm = 19; dphnzjkxm > 0; dphnzjkxm-- ) {
			continue;
		}
	}
	if ( string( "blcwslzsazfeastnobglelnnmonuxvx" ) == string( "blcwslzsazfeastnobglelnnmonuxvx" ) ) {
		int mvbaxbge;
		for ( mvbaxbge = 16; mvbaxbge > 0; mvbaxbge-- ) {
			continue;
		}
	}
	return 94656;
}

string ykraoni::bwhohkfykoezvijs( bool styeycdhxu, bool nsrgprhswkko, string kvhsxuipiwvqvxy )
{
	string qrnwkroujs = "crvkhzlondpamawzorktgnfcnttgrtxzy";
	int faicsww       = 1005;
	string gubnmfo    = "bcsacelfomfyldaijiiupxittnyjhfcrqklucpyosobhybrtfyubelfaxyna";
	string ygylqxg    = "qvnvardwqgygfkkemsijkalekutczhuvakgnreqnvaadngtewkkfpx";
	double qttnnhjrrc = 10026;
	if ( 1005 != 1005 ) {
		int cfvy;
		for ( cfvy = 27; cfvy > 0; cfvy-- ) {
			continue;
		}
	}
	if ( 10026 == 10026 ) {
		int eey;
		for ( eey = 81; eey > 0; eey-- ) {
			continue;
		}
	}
	return string( "v" );
}

bool ykraoni::nrlokmpkyoqklfichxbwylf( bool oggfzg, bool gpktrjbmxt, double upfaogmecsmxc, int vfengnvmocaald, bool gdkjtqwpi, bool ixixpzjf )
{
	int svciarjsxmoqp   = 9512;
	int dkzaxsfzfpowv   = 879;
	bool cfeispxflsma   = false;
	double ykuvhaqqnpvw = 17868;
	if ( 879 != 879 ) {
		int nykhdnwl;
		for ( nykhdnwl = 89; nykhdnwl > 0; nykhdnwl-- ) {
			continue;
		}
	}
	if ( 879 != 879 ) {
		int ik;
		for ( ik = 11; ik > 0; ik-- ) {
			continue;
		}
	}
	if ( 879 != 879 ) {
		int rufcmfpzde;
		for ( rufcmfpzde = 15; rufcmfpzde > 0; rufcmfpzde-- ) {
			continue;
		}
	}
	if ( 879 == 879 ) {
		int klmko;
		for ( klmko = 77; klmko > 0; klmko-- ) {
			continue;
		}
	}
	return false;
}

ykraoni::ykraoni( )
{
	this->lherhirtmbl( 7277, 129, 6684, 55287, 9806, 25789 );
	this->uoytmwzqnacpoouuvmby( false, true );
	this->mxlreojwaqjhmfcygnnlmkdg( 471, true );
	this->kvrbhzmgceko( true, 1982, 21231 );
	this->ktqbiburdgkxcczzoox( string( "hxtpanwjojintpdqusvwlwmfyakti" ), false );
	this->bwhohkfykoezvijs( false, true, string( "rqdpmvozwzuwqeclxilsqoyxymfhucxdatqvbjiuuzquajiogbailjzsisfetafwjnsxgryeiftrrx" ) );
	this->nrlokmpkyoqklfichxbwylf( true, false, 59363, 4432, false, true );
	this->pvrmmrnbzwknqsqodggglhgmh( false, 12681 );
	this->rgcrfapohjarepfbc( false, 2267 );
	this->vycdglquogipeexdybezzhvt( string( "lalxnbebula" ),
	                                string( "lrcigaptxoggcucpvxkbjtzzcfjrrikeezohlhzwtdranrkchndndhmsgllpvamfhzcaqkqsgcahlfghrqbuy" ), 24323,
	                                string( "fblpwhrgptsomlnbylqbjreikdponbegibskmjcezrthpmtemyly" ), false, 216, true, 21254 );
	this->wcsbmvdnhymca( string( "cooyuhgqqqdibpzwjgnhjrlpgqwz" ), string( "ywettlnbiizpbschfzqvqbisflhqyihikwyrlrbzzggo" ), 73036, false, false,
	                     string( "emb" ) );
	this->sluknwtreqntlvfxx( 8256, false, true, 32634, false );
	this->mqoyuvblhd( 1214 );
	this->jrdwzmmnlscotislyoezslsk( 15718, string( "lpzkqskfxnjlhdakaxinfoiprnivwasuezqkhzvfcsprvbjjdqsekxwlhjdl" ), 18673, false, true, 15928 );
	this->exmvhqntzke( string( "erhzvhuewovydllblfcpacaltktmqv" ), 314, 41054, true );
	this->ippfexbcbwwcaimyodrg( 576, string( "ydttpholszkffceqygrpquajzlbsqmkkrfjtffzcipg" ),
	                            string( "kzaazmtytbypmvpmgjwztcfnvjfkotgitxrpaomtzxzoihugqriud" ), 604,
	                            string( "vtcsztewjrpfahllltcxvtmomjzjsmzeuevcwkaqhxtcuocbwemoxmbtxchfjcchwyfdkutxucwtviiectgqzsovpc" ), 2692,
	                            string( "qtvcmsvkvnzbxeezkocpokgsvtnmzpzalzarqhqmxcsylihvinhotwuxdkwghvbjhuitnzkodegqnx" ),
	                            string( "ovlofrgqqzpahxazdugqwdnupblmmbaiztxdmjcsiqdlgseucxmodsjsejxyqmvsynulxkolddizkhrilymagiyowdmcjwfq" ), 13444 );
	this->lytomnatshr( 283, false, true, 16262,
	                   string( "jbufcrgzyooaqrwyaobpppmnwhidmvareurkcrmwkxfevfuftcqdilityaykfgkhmffrtxtievzabithiibdtzalwa" ),
	                   string( "bukssfpqrydonshypmwlzknrloanbzmoxvnqdapmrmekiadctolifgoeysxvbqtwgspfpmomcclhoyswz" ), true );
	this->afijbrsvzqzqx( );
}
