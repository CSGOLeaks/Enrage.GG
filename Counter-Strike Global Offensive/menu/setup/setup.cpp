#include "../menu/menu_v2.h"
#include "settings.h"
#include "../../source.hpp"
#include "../../misc.hpp"
#include "../../music_player.hpp"
//void test( ) { printf( "pog\n" ); }

static std::vector<std::string> models_to_change = {
	sxor("none"),
	sxor("Local СT Agent"),
	sxor("Local T Agent"),
	sxor("Blackwolf | Sabre"),
	sxor("Rezan The Ready | Sabre"),
	sxor("Maximus | Sabre"),
	sxor("Dragomir | Sabre"),
	sxor("Lt. Commander Ricksaw | NSWC SEAL"),
	sxor("'Two Times' McCoy | USAF TACP"),
	sxor("Seal Team 6 Soldier | NSWC SEAL"),
	sxor("3rd Commando Company | KSK"),
	sxor("'The Doctor' Romanov | Sabre"),
	sxor("Michael Syfers  | FBI Sniper"),
	sxor("Markus Delrow | FBI HRT"),
	sxor("Operator | FBI SWAT"),
	sxor("Slingshot | Phoenix"),
	sxor("Enforcer | Phoenix"),
	sxor("Soldier | Phoenix"),
	sxor("The Elite Mr. Muhlik | Elite Crew"),
	sxor("Prof. Shahmat | Elite Crew"),
	sxor("Osiris | Elite Crew"),
	sxor("Ground Rebel  | Elite Crew"),
	sxor("Special Agent Ava | FBI"),
	sxor("B Squadron Officer | SAS"),
};


void mmenu::setup( ) {

}