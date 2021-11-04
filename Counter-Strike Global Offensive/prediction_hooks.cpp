#include "hooked.hpp"
#include "prediction.hpp"
#include "weapon.hpp"
#include <map>
#include <intrin.h>
#include "visuals.hpp"
#include "anti_aimbot.hpp"

//void FixViewmodel(CUserCmd* cmd, bool restore)
//{
//	if (ctx.m_local()->IsDead() || ctx.m_local()->m_hViewModel() <= 0)
//		return;
//
//	static float cycleBackup = 0.0f;
//	static bool weaponAnimation = false;
//
//	auto viewmodel = csgo.m_entity_list()->GetClientEntityFromHandle((CBaseHandle)ctx.m_local()->m_hViewModel());
//
//	if (viewmodel) {
//		if (restore) {
//			weaponAnimation = cmd->weaponselect > 0 || cmd->buttons & (IN_ATTACK2 | IN_ATTACK);
//			cycleBackup = viewmodel->m_flCycle();//*(float*)(uintptr_t(viewModel) + 0xA14);
//		}
//		else if (weaponAnimation && ctx.update_cycle == 0) {
//			if (viewmodel->m_flCycle() == 0.0f && cycleBackup > 0.0f)
//				ctx.update_cycle |= CYCLE_PRE_UPDATE;
//		}
//	}
//}

// runcommand 
//float VelocityFix(int command_number, bool before) {
//	float velModifier = ctx.m_local()->m_flVelocityModifier();
//
//	if (ctx.last_velocity_modifier_tick == -1 || ctx.m_local()->m_vecVelocity().Length2D() < (ctx.m_local()->get_player_max_speed() * 0.1f))
//		return ctx.m_local()->m_flVelocityModifier();
//
//	auto delta = command_number - ctx.last_velocity_modifier_tick - 1;
//
//	if (!before)
//		delta = command_number - ctx.last_velocity_modifier_tick;
//
//	const auto vel_mod_backup = ctx.last_velocity_modifier;
//
//	if (delta < 0 || vel_mod_backup == 1.0f) {
//		velModifier = ctx.m_local()->m_flVelocityModifier();
//	}
//	else if (delta) {
//		auto v6 = ((csgo.m_globals()->interval_per_tick * 0.4f) * float(delta)) + vel_mod_backup;
//
//		if (v6 <= 1.0f) {
//			if (v6 >= 0.0f)
//				velModifier = v6;
//			else
//				velModifier = 0.0f;
//		}
//		else
//			velModifier = 1.0f;
//	}
//	else
//		velModifier = ctx.last_velocity_modifier;
//
//	return velModifier;
//}

//float recalculate_velocity_modifier()
//{
//	if (ctx.m_local()->m_fFlags() & FL_ONGROUND)
//	{
//		float velocitymodifier = ctx.m_local()->m_flVelocityModifier();
//		if (velocitymodifier < 1.0f)
//		{
//			float newvelocitymodifier = velocitymodifier + (csgo.m_globals()->interval_per_tick * 0.4f);
//
//			if (newvelocitymodifier >= 0.0f)
//				newvelocitymodifier = fminf(newvelocitymodifier, 1.0f);
//
//			if (velocitymodifier != newvelocitymodifier)
//				ctx.m_local()->m_flVelocityModifier() = velocitymodifier;
//		}
//	}
//}

int old_tickbase=0;
int last_shift_amount = 0;
int tickbase_switch = 0;
//float curtime_backup = 0.f;

namespace Hooked
{
	void __fastcall ProcessMovement(void* ecx, void* edx, C_BasePlayer* basePlayer, CMoveData* moveData)
	{
		using Fn = void(__thiscall*)(void*, C_BasePlayer*, CMoveData*);
		static auto ofc = vmt.m_movement->VCall<Fn>(1);
		//if (ctx.is_predicting)
			moveData->m_bGameCodeMovedPlayer = false;
		ofc(ecx, basePlayer, moveData);	
	}

	void __fastcall RunCommand(void* ecx, void* edx, C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* moveHelper)
	{

		/*static float tickbase_records[150] = {};
		static bool in_attack[150] = {};
		static bool can_shoot_check[150] = {};*/

		using Fn = void(__thiscall*)(void*, C_BasePlayer*, CUserCmd*, IMoveHelper*);

		//static auto PhysicsSimulate = (void*)Memory::Scan("client.dll", "56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21");

		//float m_flVelocityModifier = FLT_MAX;
		//static int old_tickbase = 0;
		//static auto tickbase_backup = 0;
		//static auto curtime_backup = 0.f;

		/*if (ctx.m_local() != nullptr && player == ctx.m_local())*/
		//const auto m_flVelocityModifier = ctx.m_local()->m_flVelocityModifier();

		//if (ctx.m_local() != nullptr && player == ctx.m_local() && ctx.fix_velocity_modifier && ucmd->command_number == (csgo.m_client_state()->m_iLastCommandAck + 1))
		//	ctx.m_local()->m_flVelocityModifier() = ctx.old_velocity_modifier;

		const auto is_valid = player != nullptr && player == ctx.m_local() && !ctx.m_local()->IsDead();

		if (csgo.m_move_helper() != moveHelper)
			csgo.m_move_helper.set(moveHelper);

		if (!is_valid) {
			vmt.m_prediction->VCall<Fn>(Index::IPrediction::RunCommand)(ecx, player, ucmd, moveHelper);
			return;
		}

		ctx.fix_modify_eye_pos = true;
		ctx.fix_runcommand = !ucmd->hasbeenpredicted;
		auto& prediction = Engine::Prediction::Instance();

		//if (ucmd->tick_count == -1337)
		//{
		//	ucmd->tick_count = ctx.current_tickcount + 1 + int(1 / csgo.m_globals()->interval_per_tick);//TIME_TO_TICKS(player->m_flSimulationTime() + csgo.m_globals()->interval_per_tick);
		//	//const auto tickbase = ctx.m_local()->m_nTickBase();
		//	ctx.m_local()->AdjustPlayerTimeBase(min(16, ctx.host_frameticks() + csgo.m_client_state()->m_iChockedCommands - ctx.accurate_max_previous_chocked_amt - 1 + int(1 / csgo.m_globals()->interval_per_tick)));
		//	vmt.m_prediction->VCall<Fn>(Index::IPrediction::RunCommand)(ecx, player, ucmd, moveHelper);
		//	return;
		//}

		//if(ctx.last_usercmd && ctx.last_usercmd->command_number > 10/*
		//	&& csgo.m_input()->GetUserCmd(ctx.last_usercmd->command_number - 10)->command_number == ctx.last_usercmd->command_number - 10*/)
		//{
		//	auto last_move_data = *(CMoveData**)(csgo.m_movement() + 8);
		//	if (last_move_data)
		//	{
		//		auto v3 = last_move_data->m_nPlayerHandle & 0xFFF;
		//		if (v3 == csgo.m_engine()->GetLocalPlayer())
		//		{
		//			auto v4 = last_move_data->m_nOldButtons;
		//			//static auto m_nButtons = ctx.m_local()->FindInDataMap(ctx.m_local()->GetPredDescMap(), "m_nButtons") + 0x138;
		//			if (v4 & IN_BULLRUSH)
		//				last_move_data->m_nButtons = v4;
		//		}
		//	}
		//}

		ctx.last_predicted_command = ucmd;

		if (ucmd->tick_count >= (ctx.current_tickcount + ctx.tickrate + 8)/* || ctx.speed_hack > 0*/) {
			ucmd->hasbeenpredicted = true;
			//auto serverTickcount = ctx.fakeducking ? ctx.fakeduck_will_choke : ctx.current_tickcount/* + 2*/;
			//const auto outgoing = ctx.latency[FLOW_OUTGOING];

			//serverTickcount += (outgoing / csgo.m_globals()->interval_per_tick/* + incoming*/) + 3;
			//player->m_nFinalPredictedTick() = serverTickcount;
			//ctx.m_local()->set_abs_origin(ctx.m_local()->m_vecOrigin());

			//if (!csgo.m_prediction()->m_engine_paused && csgo.m_globals()->frametime > 0)
			//	++ctx.m_local()->m_nTickBase();

			/*auto* ndata = &Engine::Prediction::Instance()->m_Data[ucmd->command_number % 150];

			if (!ndata->is_filled) {
				ndata->m_nTickBase = player->m_nTickBase();
				ndata->command_number = ucmd->command_number;
				ndata->m_aimPunchAngle = player->m_aimPunchAngle();
				ndata->m_aimPunchAngleVel = player->m_aimPunchAngleVel();
				ndata->m_viewPunchAngle = player->m_viewPunchAngle();
				ndata->m_vecViewOffset = player->m_vecViewOffset();
				ndata->m_vecViewOffset.z = fminf(fmaxf(ndata->m_vecViewOffset.z, 46.0f), 64.0f);
				ndata->m_vecVelocity = player->m_vecVelocity();
				ndata->m_flFallVelocity = player->m_flFallVelocity();
				ndata->m_flDuckAmount = player->m_flDuckAmount();
				ndata->m_flVelocityModifier = player->m_flVelocityModifier();
				ndata->tick_count = ucmd->tick_count;
				ctx.last_netvars_update_tick = ucmd->command_number;
				ndata->is_filled = true;
			}*/

			return;
		}

		//if (ucmd->buttons & IN_ATTACK && ucmd->command_number == ctx.m_ragebot_shot_nr)
		//	ucmd->random_seed = 25;

		static auto last_spawntime = 0.f;
		if (ctx.m_settings.misc_autobuy_enabled && last_spawntime != ctx.m_local()->m_flSpawnTime())
		{
			ctx.buy_weapons = true;
			ctx.has_scar = false;

			if (ctx.m_settings.misc_autobuy_primary == 1) {
				auto weapons = ctx.m_local()->m_hMyWeapons();

				for (int i = 0; weapons[i] != 0xFFFFFFFF; i++)
				{
					if (weapons[i] == 0) continue;

					if (i > 10)
						break;

					auto weapon = reinterpret_cast<C_WeaponCSBaseGun*>(csgo.m_entity_list()->GetClientEntityFromHandle(weapons[i]));

					if (weapon == nullptr) continue;

					if (weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1)
						ctx.has_scar = true;
				}
			}

			last_spawntime = ctx.m_local()->m_flSpawnTime();
		}

		//bool changed_tb = false;

		//auto& pred_data = Engine::Prediction::Instance()->m_Data[(ucmd->command_number - 1) % 150];

		//if (pred_data.m_nTickBase == ctx.m_local()->m_nTickBase())
		//	Engine::Prediction::Instance()->FixNetvarCompression((ucmd->command_number - 1));

		//if (ucmd->command_number == ctx.tickbase_shift_nr/* || ctx.shifted_cmd == ucmd->command_number*/) {

			//if (ctx.main_exploit != 2)
		//		ctx.m_local()->m_nTickBase() = (ctx.original_tickbase - 1 - ctx.exploit_tickbase_shift);
			//else
			//	ctx.m_local()->AdjustPlayerTimeBase(ctx.m_local()->DetermineSimulationTicks());
			//if (ctx.shifted_cmd == ucmd->command_number)
			//	ctx.m_local()->AdjustPlayerTimeBase(ctx.m_local()->DetermineSimulationTicks());
			//++ctx.m_local()->m_nTickBase();
		//	csgo.m_globals()->curtime = float(ctx.m_local()->m_nTickBase()) * csgo.m_globals()->interval_per_tick;
		
		//}
		//int otick = 0;

		//if (prediction->m_tickbase_array[ucmd->command_number % 150].tickbase_original > 0 && ctx.has_exploit_toggled && prediction->m_tickbase_array[ucmd->command_number % 150].command_num == ucmd->command_number) {
		//	const auto pre_tickbase = ctx.m_local()->m_nTickBase();
		//	ctx.m_local()->m_nTickBase() = prediction->m_tickbase_array[ucmd->command_number % 150].tickbase_original;
		//	_events.push_back({ "old tb: " + std::to_string(pre_tickbase) + " | cmd num: " + std::to_string(ucmd->command_number) + " | new tb: " + std::to_string(ctx.m_local()->m_nTickBase()) });
		//	/*tickbase_switch = ctx.m_local()->m_nTickBase();
		//	last_shift_amount = ctx.exploit_tickbase_shift;*/
		//	//tickbase_backup = ctx.m_local()->m_nTickBase();
		//	//curtime_backup = csgo.m_globals()->curtime;
		//	//changed_tb = true;
		//}
		//else
		//{
		//	if ((ctx.m_local()->m_nTickBase() - tickbase_switch) == 2) {
		//		const auto pre_tickbase = ctx.m_local()->m_nTickBase();
		//		ctx.m_local()->m_nTickBase() += last_shift_amount;
		//		last_shift_amount = 0;
		//		_events.push_back({ "POSTFIX old tb: " + std::to_string(pre_tickbase) + " | cmd num: " + std::to_string(ucmd->command_number) + " | new tb: " + std::to_string(ctx.m_local()->m_nTickBase()) });
		//	}
		//}

		//if (ucmd->command_number == ctx.tickbase_shift_nr)
		//{
		//	otick = ctx.m_local()->m_nTickBase();
		//	ctx.m_local()->m_nTickBase() = ctx.original_tickbase - ctx.exploit_tickbase_shift;
		////	//ctx.m_local()->AdjustPlayerTimeBase(ctx.m_local()->DetermineSimulationTicks() + ctx.exploit_tickbase_shift + 1);
		//	//
		//	_events.push_back({ "fixed tickbase num: " + std::to_string(ctx.m_local()->m_nTickBase()) + " | cmd num: " + std::to_string(ucmd->command_number) });
		////	ctx.did_set_shift = true;
		//}
		//else if (ctx.did_set_shift && ucmd->command_number - ctx.tickbase_shift_nr == 2)// if (cctx->cmd.command_number - ctx.tickbase_shift_nr == 1 && ctx.main_exploit == 1)
		//{
		//	//if ((ctx.m_local()->m_nTickBase() - ctx.original_tickbase <= 2) && ctx.exploit_allowed) {
		//		//ctx.m_local()->AdjustPlayerTimeBase(ctx.m_local()->DetermineSimulationTicks());
		//	ctx.m_local()->m_nTickBase() += ctx.exploit_tickbase_shift;
		//	ctx.did_set_shift = false;
		//	_events.push_back({ "corrected post shift tickbase num: " + std::to_string(ctx.m_local()->m_nTickBase()) + " | cmd num: " + std::to_string(ucmd->command_number) });
		//	//}
		//}

		//csgo.m_globals()->curtime = TICKS_TO_TIME(ctx.m_local()->m_nTickBase());

		//if (ucmd->command_number == ctx.shifted_cmd)
		//{
		//	ctx.m_local()->m_nTickBase() = ctx.original_tickbase - 13;
		//	//ctx.m_local()->AdjustPlayerTimeBase(ctx.m_local()->DetermineSimulationTicks() + ctx.exploit_tickbase_shift + 1);

		//	_events.push_back({ "fixed tickbase num: " + std::to_string(ctx.m_local()->m_nTickBase()) + " | cmd num: " + std::to_string(ucmd->command_number) });
		//}
		//else if (ucmd->command_number - ctx.shifted_cmd == 14)
		//{
		//	//if ((ctx.m_local()->m_nTickBase() - ctx.original_tickbase <= 2) && ctx.exploit_allowed) {
		//		//ctx.m_local()->AdjustPlayerTimeBase(ctx.m_local()->DetermineSimulationTicks());
		//	ctx.m_local()->m_nTickBase() += 14;
		//	_events.push_back({ "corrected post shift tickbase num: " + std::to_string(ctx.m_local()->m_nTickBase()) + " | cmd num: " + std::to_string(ucmd->command_number) });
		//	//}
		//}

		//csgo.m_globals()->curtime = float(ctx.m_local()->m_nTickBase()) * csgo.m_globals()->interval_per_tick;

		//fix_velocity_modifier(ucmd->command_number, true);

		//if (/*ctx.started_speedhack + 14 > ucmd->command_number && */ctx.started_speedhack <= ucmd->command_number && ctx.started_speedhack + 15 >= ucmd->command_number)
		//{
		//	tickbase_backup = ctx.m_local()->m_nTickBase();
		//	curtime_backup = csgo.m_globals()->curtime;

		//	ctx.m_local()->m_nTickBase() = ctx.original_tickbase;
		//	csgo.m_globals()->curtime = float(ctx.m_local()->m_nTickBase()) * csgo.m_globals()->interval_per_tick;
		//	//changed_tb = true;
		//}

		if (vmt.m_prediction)
			vmt.m_prediction->VCall<Fn>(Index::IPrediction::RunCommand)(ecx, player, ucmd, moveHelper);
		else
			return;

	/*	if (otick > 0) {
			ctx.m_local()->m_nTickBase() = otick + 1;
			csgo.m_globals()->curtime = TICKS_TO_TIME(ctx.m_local()->m_nTickBase());
		}*/

		//fix_velocity_modifier(ucmd->command_number, false);

		// fix r8 timings
		if (ctx.m_settings.aimbot_auto_revolver && ctx.m_local()->get_weapon()) {
			/*if (weapon->m_iItemDefinitionIndex() == 64) {
				if (weapon->m_Activity() == 208 && next_update_time == csgo.m_globals()->realtime && !weapon->m_weaponMode())
					weapon->m_flPostponeFireReadyTime() = csgo.m_globals()->curtime + 0.2f;
			}

			next_update_time = csgo.m_globals()->realtime + csgo.m_globals()->interval_per_tick;*/
			static int old_activity = ctx.m_local()->get_weapon()->m_Activity();
			const auto tickbase = player->m_nTickBase() - 1;
			auto activity = ctx.m_local()->get_weapon()->m_Activity();

			if (ctx.m_local()->get_weapon()->m_iItemDefinitionIndex() == 64) {

				if (old_activity != activity && ctx.m_local()->get_weapon()->m_Activity() == 208)
					old_tickbase = tickbase + 2;

				if (ctx.m_local()->get_weapon()->m_Activity() == 208 && old_tickbase == tickbase)
					ctx.m_local()->get_weapon()->m_flPostponeFireReadyTime() = TICKS_TO_TIME(tickbase) + 0.2f;
			}

			old_activity = activity;
		}

		// netvar compression fix
		/*if (!ucmd->hasbeenpredicted)
		{
			auto* ndata = &Engine::Prediction::Instance()->m_Data[ucmd->command_number % 150];

			if (!ndata->is_filled) {
				ndata->m_nTickBase = player->m_nTickBase();
				ndata->command_number = ucmd->command_number;
				ndata->m_aimPunchAngle = player->m_aimPunchAngle();
				ndata->m_aimPunchAngleVel = player->m_aimPunchAngleVel();
				ndata->m_viewPunchAngle = player->m_viewPunchAngle();
				ndata->m_vecViewOffset = player->m_vecViewOffset();
				ndata->m_vecViewOffset.z = fminf(fmaxf(ndata->m_vecViewOffset.z, 46.0f), 64.0f);
				ndata->m_vecVelocity = player->m_vecVelocity();
				ndata->m_vecOrigin = player->m_vecOrigin();
				ndata->m_flFallVelocity = player->m_flFallVelocity();
				ndata->m_flDuckAmount = player->m_flDuckAmount();
				ndata->m_flVelocityModifier = player->m_flVelocityModifier();
				ndata->tick_count = ucmd->tick_count;
				ctx.last_netvars_update_tick = ucmd->command_number;
				ndata->is_filled = true;
			}
		}*/

		ctx.fix_modify_eye_pos = false;
		ctx.fix_runcommand = false;

		// dont interrupt tickbase simulation
		//if (changed_tb) {
		//	ctx.m_local()->m_nTickBase() = tickbase_backup;
			//if (!csgo.m_prediction()->m_engine_paused && csgo.m_globals()->frametime > 0)
		//	//	++ctx.m_local()->m_nTickBase();
		//	csgo.m_globals()->curtime = TICKS_TO_TIME(ctx.m_local()->m_nTickBase());
		//}
		/*else {
			if (ucmd->hasbeenpredicted) {
				for (auto i = 0; i < 6; i++) {
					auto* ndata = &Engine::Prediction::Instance()->m_Data[(csgo.m_client_state()->m_iCommandAck - i) % 150];

					if (!ndata || !ndata->is_filled)
						continue;

					ndata->m_nTickBase--;
				}
			}
		}*/
		// last RunCommand cmd number
		ctx.command_number = ucmd->command_number;

		//if (ctx.can_store_netvars && ctx.run_cmd_got_called)
		//	Engine::Prediction::Instance()->OnRunCommand(ctx.m_local());
		//auto& prediction = Engine::Prediction::Instance();
		//prediction->OnRunCommand(player);
	}

	void __fastcall SetupMove(void* player, int ecx, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move)
	{
		using Fn = void(__thiscall*)(void*, CUserCmd*, IMoveHelper*, CMoveData*);
		static auto ofc = vmt.m_prediction->VCall<Fn>(Index::IPrediction::SetupMove);
		
		if ((&Engine::Prediction::Instance()->move_data) != move)
			memcpy(&Engine::Prediction::Instance()->move_data, move, 1380);

		ofc(player, ucmd, pHelper, move);

		//if (ctx.m_settings.aimbot_no_spread)
		//{
		//	if (ctx.m_settings.security_safety_mode == 0)
		//	{
		//		if (player == ctx.m_local())
		//		{
		//			if (ctx.m_local()->m_fFlags() & 1 && ucmd->buttons & IN_JUMP)
		//			{
		//				ctx.m_local()->m_vecVelocity()/*.x =*/= ctx.m_local()->m_vecVelocity()/*.x*/ * 1.2f;
		//				//ctx.m_local()->m_vecVelocity().y = ctx.m_local()->m_vecVelocity().y * 1.2f;
		//				
		//				*(Vector*)((DWORD)pHelper + 0x44) = ctx.m_local()->m_vecVelocity();
		//				*(Vector*)((DWORD)pHelper + 0x40) = ctx.m_local()->m_vecVelocity();
		//			}
		//		}
		//	}
		//}
	}

	bool __fastcall InPrediction(void* ecx, void* edx)
	{
		if ((DWORD)ecx < 0x10)
			return false;

		using Fn =  bool(__thiscall*)(void*);
		static auto ofc = vmt.m_prediction->VCall<Fn>(Index::IPrediction::InPrediction);

		static auto weapon_recoil_scale = csgo.m_engine_cvars()->FindVar(sxor("weapon_recoil_scale"));
		static auto view_recoil_tracking = csgo.m_engine_cvars()->FindVar(sxor("view_recoil_tracking"));


		static auto MaintainSequenceTransitions = (void*)Memory::Scan(sxor("client.dll"), sxor("84 C0 74 17 8B 87")); //C_BaseAnimating::MaintainSequenceTransitions
		static auto SetupBones_Timing = (void*)Memory::Scan(sxor("client.dll"), sxor("84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05")); //C_BaseAnimating::SetupBones
																															  /*
																															  if (GetPredictable() && Interfaces::Prediction->InPrediction())
																															  currentTime = Interfaces::Prediction->GetSavedTime();
																															  */
		auto result = ofc(ecx);

		if (ecx != nullptr) {
			if (MaintainSequenceTransitions && ctx.setup_bones && _ReturnAddress() == MaintainSequenceTransitions)
				return true;
			if (SetupBones_Timing && _ReturnAddress() == SetupBones_Timing)
				return false;
		}

		// note - dex; first 2 'test al, al' instructions in C_BasePlayer::CalcPlayerView.
		static void* CalcPlayerView_ret1 = (void*)Memory::Scan(sxor("client.dll"), sxor("84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C"));
		static void* CalcPlayerView_ret2 = (void*)Memory::Scan(sxor("client.dll"), sxor("84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06"));
		static void* CalcPlayerView_ret3 = (void*)Memory::Scan(sxor("client.dll"), sxor("84 C0 75 24 ? ? ? ? ? ? ? ? ? ? FF 50 ? ? ? ? ? ? 51 C7 ? ? ? ? ? ? ? ? ? ? ? 53 57 FF 50"));
		static void* CalcPlayerView_ret4 = (void*)Memory::Scan(sxor("client.dll"), sxor("84 C0 0F 85 ? ? ? ? 83 EC 08 8D"));

		if (csgo.m_engine()->IsInGame()) {
			// note - dex; apparently this calls 'view->DriftPitch()'.
			//             i don't know if this function is crucial for normal gameplay, if it causes issues then comment it out.
			if (_ReturnAddress() == CalcPlayerView_ret1 /*|| _ReturnAddress() == CalcPlayerView_ret3*/) {
				//if (_ReturnAddress() == CalcPlayerView_ret3)
				//	ctx.cheat_option_flags |= hook_should_return_cl_smooth;
				return true/*(_ReturnAddress() != CalcPlayerView_ret3)*/;
			}

			if (_ReturnAddress() == CalcPlayerView_ret2) {
				// at this point, angles are copied into the CalcPlayerView's eyeAngles argument.
				// (ebp) InPrediction -> (ebp) CalcPlayerView + 0xC = eyeAngles.
				if (ctx.m_settings.visuals_no_recoil && ctx.m_local() && !ctx.m_local()->IsDead())
				{
					uintptr_t stack = *(uintptr_t*)((*(uintptr_t*)((uintptr_t)_AddressOfReturnAddress() - sizeof(uintptr_t))) + 0xC);

					QAngle* angles = reinterpret_cast<QAngle*>(stack);
					//angles = stack.next().arg(0xC).to< QAngle* >();

					if (angles) {
						*angles -= ctx.m_local()->m_viewPunchAngle()
							+ (ctx.m_local()->m_aimPunchAngle() * weapon_recoil_scale->GetFloat())
							* view_recoil_tracking->GetFloat();
					}
				}

				return true;
			}
		}

		return result;
	}
}