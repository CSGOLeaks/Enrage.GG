#include "sdk.hpp"
#include "source.hpp"
#include "hooked.hpp"
#include "menu.hpp"
#include "lag_comp.hpp"
#include "resolver.hpp"
#include "visuals.hpp"
#include "displacement.hpp"

void __stdcall Hooked::LockCursor()
{
	using Fn = void(__thiscall*)(void*);
	static auto ofc = vmt.m_surface->VCall<Fn>(67);

	if (feature::menu->_menu_opened) {
		csgo.m_surface()->UnlockCursor();
		return;
	}

	ofc(csgo.m_surface());
}

void __fastcall Hooked::LevelInitPreEntity(void* ecx, void* edx, const char* map) {
	static auto ofc = vmt.m_client->VCall<LevelInitPreEntity_t>(5);
	ofc(ecx, map);

	static ConVar* cl_updaterate = csgo.m_engine_cvars()->FindVar(sxor("cl_updaterate"));
	static ConVar* cl_cmdrate = csgo.m_engine_cvars()->FindVar(sxor("cl_cmdrate"));

	if (csgo.m_globals()->interval_per_tick != 0) {
		float rate{ 1.f / csgo.m_globals()->interval_per_tick };

		//static ConVar* cl_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("cl_interp_ratio"));
		//static ConVar* cl_interp = csgo.m_engine_cvars()->FindVar(sxor("cl_interp"));
		//static ConVar* crate = csgo.m_engine_cvars()->FindVar(sxor("rate"));

		//// set rates when joining a server.
		cl_updaterate->SetValue(rate);
		cl_cmdrate->SetValue(rate);
	}
	//crate->SetValue(786432);
	//if (cl_interp_ratio && cl_interp)
	//{
		//if (cl_interp->GetFloat() != 0.0f && cl_interp->GetFloat() != cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat())
		//{
	//		cl_interp_ratio->SetValue(2.0f);
	//		cl_interp->SetValue(csgo.m_globals()->interval_per_tick);
		//}
	//}

	/*static ConVar* sv_allow_third_person = csgo.m_engine_cvars()->FindVar(sxor("sv_allow_thirdperson"));
	static ConVar* sv_cheats = csgo.m_engine_cvars()->FindVar(sxor("sv_cheats"));
	static ConVar* cam_idealdist = csgo.m_engine_cvars()->FindVar(sxor("cam_idealdist"));
	static ConVar* cam_collision = csgo.m_engine_cvars()->FindVar(sxor("cam_collision"));
	static ConVar* cam_idealpitch = csgo.m_engine_cvars()->FindVar(sxor("cam_idealpitch"));
	static ConVar* cam_idealyaw = csgo.m_engine_cvars()->FindVar(sxor("cam_idealyaw"));
	static ConVar* cam_snapto = csgo.m_engine_cvars()->FindVar(sxor("cam_snapto"));
	static ConVar* cam_ideallag = csgo.m_engine_cvars()->FindVar(sxor("cam_ideallag"));

	sv_allow_third_person->m_nFlags &= ~(FCVAR_PROTECTED | FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENTCMD_CAN_EXECUTE);
	sv_allow_third_person->SetValue(1);

	cam_idealdist->m_nFlags &= ~(FCVAR_PROTECTED | FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_NOT_CONNECTED | FCVAR_SERVER_CAN_EXECUTE | FCVAR_CLIENTCMD_CAN_EXECUTE | FCVAR_ARCHIVE);
	cam_collision->m_nFlags &= ~(FCVAR_SERVER_CAN_EXECUTE | FCVAR_ARCHIVE);
	cam_collision->SetValue(1);
	cam_idealpitch->m_nFlags &= ~(FCVAR_SERVER_CAN_EXECUTE | FCVAR_ARCHIVE);
	cam_idealpitch->SetValue(0.f);
	cam_idealyaw->m_nFlags &= ~(FCVAR_SERVER_CAN_EXECUTE | FCVAR_ARCHIVE);
	cam_idealyaw->SetValue(0.f);
	cam_snapto->m_nFlags &= ~(FCVAR_SERVER_CAN_EXECUTE | FCVAR_ARCHIVE);
	cam_snapto->SetValue(0);
	cam_ideallag->m_nFlags &= ~(FCVAR_SERVER_CAN_EXECUTE | FCVAR_ARCHIVE);
	cam_ideallag->SetValue(0.f);*/

	// invoke original method.
}

void __fastcall Hooked::LevelInitPostEntity(void* ecx, void* edx) 
{
	static auto ofc = vmt.m_client->VCall<LevelInitPostEntity_t>(6);

	for (auto i = 0; i < parser::knifes.list.size(); i++)
	{
		auto knife = &parser::knifes.list[i];

		knife->model_world = csgo.m_model_info()->GetModelIndex(knife->model_world_path.c_str());
		knife->model_player = csgo.m_model_info()->GetModelIndex(knife->model_player_path.c_str());
	}

	csgo.m_player_resource.set(C_PlayerResource::get());

	ctx.m_settings.anti_aim_autopeek_key.toggled = false;

	//if (csgo.m_game_rules() == nullptr)
		csgo.m_game_rules.set((**reinterpret_cast<CCSGameRules***>(Engine::Displacement::Signatures[c_signatures::GAMERULES] + 0x1)));
	//if (ctx.hud_death_notice == nullptr)
		ctx.hud_death_notice = feature::find_hud_element<CCSGO_HudDeathNotice*>(sxor("CCSGO_HudDeathNotice"));
	//if (ctx.update_hud_weapons == nullptr)
		ctx.update_hud_weapons = (void*)Engine::Displacement::Signatures[c_signatures::UPDATEHUDWEAPONS];

	ctx.latency[FLOW_OUTGOING] = 0.f;
	ctx.latency[FLOW_INCOMING] = 0.f;
	ctx.lerp_time = 0.f;
	feature::resolver->reset();

	ctx.hud_death_notice = nullptr;
	ctx.update_hud_weapons = nullptr;
	ctx.fake_state.m_player = 0;
	ctx.shots_fired.fill(0);
	ctx.shots_total.fill(0);
	ctx.command_numbers.clear();
	vmt.m_net_channel.reset();
	vmt.m_clientstate.reset();
	csgo.m_game_rules.set(nullptr);
	csgo.m_player_resource.set(nullptr);

	// invoke original method.
	ofc(ecx);
}

void __fastcall Hooked::LevelShutdown(void* ecx, void* edx) {
	static auto ofc = vmt.m_client->VCall<LevelShutdown_t>(7);

	ctx.hud_death_notice = nullptr;
	ctx.update_hud_weapons = nullptr;
	ctx.fake_state.m_player = 0;
	ctx.shots_fired.fill(0);
	ctx.shots_total.fill(0);
	ctx.command_numbers.clear();
	vmt.m_net_channel.reset();
	vmt.m_clientstate.reset();
	csgo.m_game_rules.set(nullptr);
	csgo.m_player_resource.set(nullptr);
	ctx.m_settings.anti_aim_autopeek_key.toggled = false;

	ctx.latency[FLOW_OUTGOING] = 0.f;
	ctx.latency[FLOW_INCOMING] = 0.f;
	ctx.lerp_time = 0.f;

	feature::lagcomp->reset();
	feature::resolver->reset();

	static auto rate = csgo.m_engine_cvars()->FindVar(sxor("rate"));
	static auto cl_cmdrate = csgo.m_engine_cvars()->FindVar(sxor("cl_cmdrate"));
	static auto cl_updaterate = csgo.m_engine_cvars()->FindVar(sxor("cl_updaterate"));
	static auto cl_interp = csgo.m_engine_cvars()->FindVar(sxor("cl_interp"));
	static auto net_maxroutable = csgo.m_engine_cvars()->FindVar(sxor("net_maxroutable"));
	static auto cl_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("cl_interp_ratio"));
	static auto r_jiggle_bones = csgo.m_engine_cvars()->FindVar(sxor("r_jiggle_bones"));

	rate->SetValue(786432);
	cl_cmdrate->SetValue(128);
	cl_updaterate->SetValue(128);
	cl_interp->SetValue(0.03125f);
	net_maxroutable->SetValue(1200);
	cl_interp_ratio->SetValue(2);
	r_jiggle_bones->SetValue(0);

	// invoke original method.
	ofc(ecx);
}

//float last_deploy_time = 0.f;
//float prev_cycle = 0.f;
//CBaseHandle last_active_weapon = 0;
std::unordered_map<int, float> sounds;

int __fastcall Hooked::EmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char* pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {

	static auto ofc = vmt.m_engine_sound->VCall<EmitSound_t>(5);

	//static std::string soundname = sxor("deploy");

	//if (pSample && strlen(pSample) > 5 && ctx.m_local() && !ctx.m_local()->IsDead() && iEntIndex == ctx.m_local()->entindex())
	//{
	//	/*if (last_active_weapon != ctx.m_local()->m_hActiveWeapon() || (last_deploy_time - csgo.m_globals()->realtime) > (ctx.latency[FLOW_OUTGOING] + csgo.m_globals()->interval_per_tick))
	//		last_active_weapon = ctx.m_local()->m_hActiveWeapon();
	//	else
	//		return 0;*/

	//	if (sounds.size() > 1)
	//	{
	//		for (auto it = sounds.begin(); it != sounds.end();)
	//		{
	//			auto s = (*it).second;

	//			if (abs(csgo.m_globals()->realtime - s) >= 1.1f)
	//				it = sounds.erase(it);

	//			++it;
	//		}

	//		if (sounds.size() > 1000)
	//			sounds.clear();
	//	}

	//	auto viewmodel = csgo.m_entity_list()->GetClientEntityFromHandle(reinterpret_cast<CBaseHandle>(ctx.m_local()->m_hViewModel()));

	//	if (viewmodel)
	//	{
	//		const auto cycle = viewmodel->m_flCycle();

	//		auto pre = sounds[hash_32_fnv1a_const(pSample)];

	//		sounds[hash_32_fnv1a_const(pSample)] = csgo.m_globals()->realtime;

	//		if ((csgo.m_globals()->realtime - pre) < 1.f)
	//			return 0;

	//		////const auto pre_update_cycle = prev_cycle;

	//		////prev_cycle = cycle;

	//		////_events.push_back(std::to_string(cycle));

	//		//if (/*(pre_update_cycle - cycle) > 0.03f || */cycle > 0.025f)
	//		//	return 0;
	//	}
	//}
	//	//last_deploy_time = csgo.m_globals()->realtime;
	//}
	if (iEntIndex == ctx.m_local()->entindex() && ctx.m_local() && !ctx.m_local()->IsDead())
	{
		//weapons/revolver/revolver_prepare.wav

		auto is_r8 = pSample[1 + 0] == 'w' && pSample[1 + 7] == '/' && pSample[1 + 8] == 'r' && pSample[1 + 16] == '/' && pSample[1 + 17] == 'r' && pSample[1 + 18] == 'e' && pSample[1 + 19] == 'v' && pSample[1 + 25] == '_' && pSample[1 + 26] == 'p'&& pSample[1 + 27] == 'r'&& pSample[1 + 28] == 'e';

		if (is_r8 && ctx.m_settings.misc_no_revolver_cock_sound && ctx.m_settings.aimbot_auto_revolver)
			return 0;
		//_events.push_back({ pSample });
	}

	return ofc(_this, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);
}