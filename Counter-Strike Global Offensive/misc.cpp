#include "hooked.hpp"
#include "displacement.hpp"
#include "prop_manager.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include "sdk.hpp"
#include "misc.hpp"
#include "source.hpp"
#include "player.hpp"
#include "menu/setup/settings.h"
#include "visuals.hpp"

void c_misc::unlock_cvars()
{
	ICvar::Iterator iter(csgo.m_engine_cvars());

	int nUnhidden = 0;

	for (iter.SetFirst(); iter.IsValid(); iter.Next())
	{
		ConCommandBase* cmd = iter.Get();
		auto& flags = *(int*)((DWORD)cmd + 20);

		if (flags & 18) {
			nUnhidden++;
			flags &= ~18;
		}
	}

	//csgo.m_engine_cvars()->ConsolePrintf(sxor("unlocked hidden cvars.\n"), nUnhidden);
	_events.emplace_back(std::string(sxor("successfully unlocked hidden cvars.")));
}

void c_misc::unlock_cl_cvars()
{
	ICvar::Iterator iter(csgo.m_engine_cvars());

	int nUnhidden = 0;

	for (iter.SetFirst(); iter.IsValid(); iter.Next())
	{
		ConCommandBase* cmd = iter.Get();
		auto& flags = *(int*)((DWORD)cmd + 20);

		if (flags & FCVAR_CHEAT || flags & FCVAR_REPLICATED) {
			nUnhidden++;
			flags &= ~FCVAR_CHEAT;
			flags &= ~FCVAR_REPLICATED;
		}
	}

	//csgo.m_engine_cvars()->ConsolePrintf(sxor("unlocked cheat cvars.\n"), nUnhidden);
	_events.emplace_back(std::string(sxor("successfully unlocked cheat cvars.")));
}

//void c_misc::begin(CUserCmd* cmd)
//{
//
//}

void c_misc::pre_prediction(CUserCmd* cmd)
{
	if (ctx.m_settings.aimbot_anti_aim_air_duck && !((cmd->buttons & IN_DUCK)) && ctx.m_local())
	{
		if (!(cmd->buttons & IN_JUMP) || (ctx.m_local()->m_fFlags() & FL_ONGROUND) || ctx.m_local()->m_vecVelocity().z >= -140.0)
			cmd->buttons &= ~IN_DUCK;
		else
			cmd->buttons |= IN_DUCK;
	}
}

int c_misc::hitbox_to_hitgroup(int Hitbox)
{
	switch (Hitbox)
	{
	case HITBOX_HEAD:
	case HITBOX_NECK:
		return HITGROUP_HEAD;
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
	case HITBOX_THORAX:
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_RIGHT_UPPER_ARM:
		return HITGROUP_CHEST;
	case HITBOX_PELVIS:
	case HITBOX_LEFT_THIGH:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_BODY:
		return HITGROUP_STOMACH;
	case HITBOX_LEFT_CALF:
	case HITBOX_LEFT_FOOT:
		return HITGROUP_LEFTLEG;
	case HITBOX_RIGHT_CALF:
	case HITBOX_RIGHT_FOOT:
		return HITGROUP_RIGHTLEG;
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_HAND:
		return HITGROUP_LEFTARM;
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_RIGHT_HAND:
		return HITGROUP_RIGHTARM;
	default:
		return HITGROUP_STOMACH;
	}
}

int c_misc::hitgroup_to_hitbox(int hitgroup)
{
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return HITBOX_HEAD;
	case HITGROUP_CHEST:
		return HITBOX_CHEST;
	case HITGROUP_STOMACH:
		return HITBOX_BODY;
	case HITGROUP_LEFTLEG:
		return HITBOX_LEFT_FOOT;
	case HITGROUP_RIGHTLEG:
		return HITBOX_RIGHT_FOOT;
	case HITGROUP_LEFTARM:
		return HITBOX_LEFT_FOREARM;
	case HITGROUP_RIGHTARM:
		return HITBOX_RIGHT_FOREARM;
	default:
		return -1;
	}
}

void c_misc::end(CUserCmd* cmd)
{
	float v2; // xmm2_4
	int v3; // eax
	float v4; // xmm1_4

	if (ctx.m_local() == nullptr || ctx.m_local()->m_MoveType() == MOVETYPE_LADDER)
		return;

	v2 = cmd->sidemove;
	v3 = cmd->buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);
	v4 = cmd->forwardmove;

	auto leg_type = ctx.m_settings.anti_aim_leg_movement;

	if (leg_type == 3) {
		leg_type = (cmd->command_number % 2 == 1/* && ctx.m_local()->m_vecVelocity().Length2D() > 100.f*/) ? 2 : 1;
		if (cmd->command_number % 3 == 0)
			leg_type = 0;
	}

	if (leg_type == 2)
	{
		if (v4 <= 0.0f)
		{
			if (v4 < 0.0f)
				v3 |= IN_BACK;
		}
		else
		{
			v3 |= IN_FORWARD;
		}
		if (v2 > 0.0f)
			goto LABEL_15;
		if (v2 >= 0.0f)
			goto LABEL_18;
		goto LABEL_17;
	}
	if (leg_type != 1)
		goto LABEL_18;
	if (v4 <= 0.0f)
	{
		if (v4 < 0.0f)
			v3 |= IN_FORWARD;
	}
	else
	{
		v3 |= IN_BACK;
	}
	if (v2 > 0.0)
	{
	LABEL_17:
		v3 |= IN_MOVELEFT;
		goto LABEL_18;
	}
	if (v2 < 0.0)
		LABEL_15:
	v3 |= IN_MOVERIGHT;
LABEL_18:
	cmd->buttons = v3;

	//cmd->buttons &= ~IN_MOVERIGHT;
	//cmd->buttons &= ~IN_MOVELEFT;
	//cmd->buttons &= ~IN_FORWARD;
	//cmd->buttons &= ~IN_BACK;

	//if (cmd->forwardmove > 0.f)
	//	cmd->buttons |= IN_FORWARD;
	//else if (cmd->forwardmove < 0.f)
	//	cmd->buttons |= IN_BACK;

	//if (cmd->sidemove > 0.f)
	//	cmd->buttons |= IN_MOVERIGHT;
	//else if (cmd->sidemove < 0.f)
	//	cmd->buttons |= IN_MOVELEFT;
}

bool c_misc::save_cfg()
{
	CreateDirectoryA(sxor("C:\\enrage csgo"), NULL);

	std::string file_path = sxor("C:\\enrage csgo\\") + std::to_string(g_settings.cur_cfg+1) + sxor(".bin");

	_events.emplace_back(std::string(sxor("successfully saved cfg ") + std::to_string(g_settings.cur_cfg + 1) + sxor(".bin")));

	std::fstream file(file_path, std::ios::out | std::ios::in | std::ios::trunc);
	file.close();

	file.open(file_path, std::ios::out | std::ios::in);
	if (!file.is_open())
	{
		file.close();
		return false;
	}

	const size_t settings_size = sizeof(c_variables);
	for (int i = 0; i < settings_size; i++)
	{
		byte current_byte = *reinterpret_cast<byte*>(uintptr_t(&ctx.m_settings) + i);
		for (int x = 0; x < 8; x++)
		{
			file << (int)((current_byte >> x) & 1);
		}
	}

	file.close();

	return true;
}

bool c_misc::load_cfg()
{
	CreateDirectoryA(sxor("C:\\enrage csgo"), NULL);

	std::string file_path = sxor("C:\\enrage csgo\\") + std::to_string(g_settings.cur_cfg+1) + sxor(".bin");

	_events.emplace_back(std::string(sxor("successfully loaded cfg ") + std::to_string(g_settings.cur_cfg + 1) + sxor(".bin")));

	std::fstream file;
	file.open(file_path, std::ios::out | std::ios::in);
	if (!file.is_open())
	{
		file.close();
		return false;
	}

	std::string line;
	while (file)
	{
		std::getline(file, line);

		const size_t settings_size = sizeof(c_variables);
		if (line.size() > (settings_size * 8))
		{
			file.close();
			return false;
		}
		for (int i = 0; i < settings_size; i++)
		{
			byte current_byte = *reinterpret_cast<byte*>(uintptr_t(&ctx.m_settings) + i);
			for (int x = 0; x < 8; x++)
			{
				if (line.size() < (i * 8) + x || line[(i * 8) + x] == '1')
					current_byte |= 1 << x;
				else
					current_byte &= ~(1 << x);
			}
			*reinterpret_cast<byte*>(uintptr_t(&ctx.m_settings) + i) = current_byte;
		}
	}

	file.close();

	return true;
}

int c_misc::get_client_num()
{
	DWORD dwFirstAddr = Memory::Scan(sxor("client.dll"), sxor("C7 05 ? ? ? ? ? ? ? ? A3 ? ? ? ? 85 C9 74 0A"));

	dwFirstAddr -= (DWORD)GetModuleHandleA(sxor("client.dll"));
	dwFirstAddr += 2;

	if (dwFirstAddr)
	{
		DWORD dwSecondAddr = (DWORD)GetModuleHandleA(sxor("client.dll")) + dwFirstAddr;

		DWORD dwReturnAddr = 0;

		while (!dwReturnAddr) { dwReturnAddr = *(DWORD*)dwSecondAddr; Sleep(200); }

		return **(int**)dwSecondAddr;
	}
	return 0;
}

std::string c_misc::get_steam_name()
{
	DWORD dwFirstAddr = Memory::Scan(sxor("matchmaking.dll"), sxor("6A 6C 0F 57 C0")) + 24;
	DWORD dwSecondAddr = *(DWORD*)dwFirstAddr + 4;

	char name[33];
	for (auto i = 0; i < 33; i++)
		name[i] = *(char*)(*(DWORD*)dwSecondAddr + 24 + i);

	return std::string(name);
}

__int64 c_misc::get_friend_id(const char* pszAuthID)
{
	if (!pszAuthID)
		return 0;

	int iServer = 0;
	int iAuthID = 0;

	char szAuthID[64];
	strcpy_s(szAuthID, 63, pszAuthID);

	char* szTmp = strtok(szAuthID, ":");
	while (szTmp = strtok(NULL, ":"))
	{
		char* szTmp2 = strtok(NULL, ":");
		if (szTmp2)
		{
			iServer = atoi(szTmp);
			iAuthID = atoi(szTmp2);
		}
	}

	if (iAuthID == 0)
		return 0;

	__int64 i64friendID = (__int64)iAuthID * 2;

	//Friend ID's with even numbers are the 0 auth server.
	//Friend ID's with odd numbers are the 1 auth server.
	i64friendID += 76561197960265728 + iServer;

	return i64friendID;
}

std::string c_misc::get_steam_id()
{
	double flUniqueID = (double)get_client_num() / 2.f;

	if (floor(flUniqueID) == flUniqueID) //tseloe 4islo
		return std::string(sxor("STEAM_1:0:")) + std::to_string((int)flUniqueID);
	else
		return std::string(sxor("STEAM_1:1:")) + std::to_string((int)floor(flUniqueID));
}

std::string c_misc::get_user_data()
{
	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);//convertSteamID

	return std::string(sxor("2k17.club [") + std::string(ctx.data->username) + sxor("] successfully injected.") + '\n' + sxor("His STEAMID is: ") + get_steam_id() + '\n' + sxor("His STEAM nickname is: ") + get_steam_name() + '\n' + sxor("Community ID: ") + std::to_string(get_friend_id(get_steam_id().c_str())) + '\n' + '\n' + std::string(sxor("https://steamcommunity.com/profiles/") + std::to_string(get_friend_id(get_steam_id().c_str()))) + '\n' + '\n' + std::string(sxor("local time: ")) + asctime(timeinfo));
}