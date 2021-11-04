#include "hooked.hpp"
#include "displacement.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "prediction.hpp"
#include "movement.hpp"
#include "rage_aimbot.hpp"
#include "anti_aimbot.hpp"
#include "lag_comp.hpp"
#include <intrin.h>
#include "menu.hpp"
#include "usercmd.hpp"

//void __fastcall Hooked::StartLagCompensation(void* ecx, void* edx, uintptr_t player, int lagCompensationType, const Vector* weaponPos, const QAngle* weaponAngles, float weaponRange)
//{
//	static auto ofunc = memory::detours.original(&hkStartLagCompensation);
//
//	if (pServerGlobals && player)
//	{
//		printf("Shot tick: %d | tickcount %d\n", TIME_TO_TICKS(pServerGlobals->curtime), pServerGlobals->tickcount);
//
//		auto cmd = *(CUserCmd**)(player + 0x100C);
//		if (cmd)
//		{
//			static auto engine = *Utils::PatternScan<void***>(GetModuleHandleA("server.dll"), "8B 0D ? ? ? ? 52 8B 01 8B 40 58", 2);
//			auto v26 = *(int*)(player + 0x1C);
//			if (v26)
//				v26 = (v26 - pServerGlobals->pEdicts) >> 4;
//
//			INetChannel* nci = get_vfunc<INetChannel * (__thiscall*)(void*, int)>(*engine, 22)(*engine, v26);
//			float correct = 0.0f;
//			auto latency = 0.0f;
//			if (nci)
//			{
//				// add network latency
//				latency = nci->GetLatency(FLOW_OUTGOING);
//				correct += latency;
//			}
//
//			// NOTE:  do these computations in float time, not ticks, to avoid big roundoff error accumulations in the math
//			// add view interpolation latency see C_BaseEntity::GetInterpolationAmount()
//			correct += *(float*)(player + 0xC94);
//
//			// check bounds [0,sv_maxunlag]
//			correct = std::clamp(correct, 0.0f, ConVars::sv_maxunlag->GetFloat());
//
//			// correct tick send by player
//			float flTargetTime = TICKS_TO_TIME(cmd->tick_count) - *(float*)(player + 0xC94);
//
//			// calculate difference between tick sent by player and our latency based tick
//			float deltaTime = correct - (pServerGlobals->curtime - flTargetTime);
//
//			if (fabs(deltaTime) > 0.2f)
//			{
//				// difference between cmd time and latency is too big > 200ms, use time correction based on latency
//				Utils::ConsolePrint("StartLagCompensation: delta too big (%.3f)\n", deltaTime);
//				flTargetTime = pServerGlobals->curtime - correct;
//			}
//
//			Utils::ConsolePrint("correct: %.3f | target %.3f | deltaTime %.3f | latency in ticks %d\n",
//				correct, flTargetTime, deltaTime, TIME_TO_TICKS(latency));
//		}
//	}
//
//	return ofunc(ecx, edx, player, lagCompensationType, weaponPos, weaponAngles, weaponRange);
//}