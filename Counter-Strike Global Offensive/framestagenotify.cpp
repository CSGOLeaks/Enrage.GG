#include "hooked.hpp"
#include "prediction.hpp"
#include "lag_comp.hpp"
#include "chams.hpp"
#include "weapon.hpp"
#include "anti_aimbot.hpp"
#include "displacement.hpp"
#include "bell.hpp"
#include "screenshot_sound.hpp"
#include "music_player.hpp"
#include "movement.hpp"
#include "autowall.hpp"
#include "resolver.hpp"
#include "sound_parser.hpp"
#include "weather_controller.hpp"
#include <fstream>
#include "prop_manager.hpp"

using CNETMsg_File_constructor_fn = void(__thiscall*)(void*);
using CNETMsg_File_destructor_fn = void(__thiscall*)(void*);
using CNETMsg_File_proto_fn = void(__thiscall*)(void*, void*);

template<typename t> 
t follow_rel32(uintptr_t address, size_t offset) {

	if (!address)
		return t{};

	auto offsetAddr = address + offset;
	auto relative = *(uint32_t*)offsetAddr;
	if (!relative)
		return t{};

	return (t)(offsetAddr + relative + sizeof(uint32_t));
}

const char* models_to_change[] = {
("models/player/custom_player/legacy/tm_phoenix.mdl"),
("models/player/custom_player/legacy/ctm_sas.mdl"),
("models/player/custom_player/legacy/tm_balkan_variantj.mdl"),
("models/player/custom_player/legacy/tm_balkan_variantg.mdl"),
("models/player/custom_player/legacy/tm_balkan_varianti.mdl"),
("models/player/custom_player/legacy/tm_balkan_variantf.mdl"),
("models/player/custom_player/legacy/ctm_st6_varianti.mdl"),
("models/player/custom_player/legacy/ctm_st6_variantm.mdl"),
("models/player/custom_player/legacy/ctm_st6_variantg.mdl"),
("models/player/custom_player/legacy/ctm_st6_variante.mdl"),
("models/player/custom_player/legacy/ctm_st6_variantk.mdl"),
("models/player/custom_player/legacy/tm_balkan_varianth.mdl"),
("models/player/custom_player/legacy/ctm_fbi_varianth.mdl"),
("models/player/custom_player/legacy/ctm_fbi_variantg.mdl"),
("models/player/custom_player/legacy/ctm_fbi_variantf.mdl"),
("models/player/custom_player/legacy/tm_phoenix_variantg.mdl"),
("models/player/custom_player/legacy/tm_phoenix_variantf.mdl"),
("models/player/custom_player/legacy/tm_phoenix_varianth.mdl"),
("models/player/custom_player/legacy/tm_leet_variantf.mdl"),
("models/player/custom_player/legacy/tm_leet_varianti.mdl"),
("models/player/custom_player/legacy/tm_leet_varianth.mdl"),
("models/player/custom_player/legacy/tm_leet_variantg.mdl"),
("models/player/custom_player/legacy/ctm_fbi_variantb.mdl"),
("models/player/custom_player/legacy/ctm_sas_variantf.mdl"),
("models/player/custom_player/legacy/tm_anarchist.mdl"),
("models/player/custom_player/legacy/tm_anarchist_varianta.mdl"),
("models/player/custom_player/legacy/tm_anarchist_variantb.mdl"),
("models/player/custom_player/legacy/tm_anarchist_variantc.mdl"),
("models/player/custom_player/legacy/tm_anarchist_variantd.mdl"),
("models/player/custom_player/legacy/tm_pirate.mdl"),
("models/player/custom_player/legacy/tm_pirate_varianta.mdl"),
("models/player/custom_player/legacy/tm_pirate_variantb.mdl"),
("models/player/custom_player/legacy/tm_pirate_variantc.mdl"),
("models/player/custom_player/legacy/tm_pirate_variantd.mdl"),
("models/player/custom_player/legacy/tm_professional.mdl"),
("models/player/custom_player/legacy/tm_professional_var1.mdl"),
("models/player/custom_player/legacy/tm_professional_var2.mdl"),
("models/player/custom_player/legacy/tm_professional_var3.mdl"),
("models/player/custom_player/legacy/tm_professional_var4.mdl"),
("models/player/custom_player/legacy/tm_separatist.mdl"),
("models/player/custom_player/legacy/tm_separatist_varianta.mdl"),
("models/player/custom_player/legacy/tm_separatist_variantb.mdl"),
("models/player/custom_player/legacy/tm_separatist_variantc.mdl"),
("models/player/custom_player/legacy/tm_separatist_variantd.mdl"),
("models/player/custom_player/legacy/ctm_gign.mdl"),
("models/player/custom_player/legacy/ctm_gign_varianta.mdl"),
("models/player/custom_player/legacy/ctm_gign_variantb.mdl"),
("models/player/custom_player/legacy/ctm_gign_variantc.mdl"),
("models/player/custom_player/legacy/ctm_gign_variantd.mdl"),
("models/player/custom_player/legacy/ctm_gsg9.mdl"),
("models/player/custom_player/legacy/ctm_gsg9_varianta.mdl"),
("models/player/custom_player/legacy/ctm_gsg9_variantb.mdl"),
("models/player/custom_player/legacy/ctm_gsg9_variantc.mdl"),
("models/player/custom_player/legacy/ctm_gsg9_variantd.mdl"),
("models/player/custom_player/legacy/ctm_idf.mdl"),
("models/player/custom_player/legacy/ctm_idf_variantb.mdl"),
("models/player/custom_player/legacy/ctm_idf_variantc.mdl"),
("models/player/custom_player/legacy/ctm_idf_variantd.mdl"),
("models/player/custom_player/legacy/ctm_idf_variante.mdl"),
("models/player/custom_player/legacy/ctm_idf_variantf.mdl"),
("models/player/custom_player/legacy/ctm_swat.mdl"),
("models/player/custom_player/legacy/ctm_swat_varianta.mdl"),
("models/player/custom_player/legacy/ctm_swat_variantb.mdl"),
("models/player/custom_player/legacy/ctm_swat_variantc.mdl"),
("models/player/custom_player/legacy/ctm_swat_variantd.mdl"),
("models/player/custom_player/legacy/ctm_sas_varianta.mdl"),
("models/player/custom_player/legacy/ctm_sas_variantb.mdl"),
("models/player/custom_player/legacy/ctm_sas_variantc.mdl"),
("models/player/custom_player/legacy/ctm_sas_variantd.mdl"),
("models/player/custom_player/legacy/ctm_st6.mdl"),
("models/player/custom_player/legacy/ctm_st6_varianta.mdl"),
("models/player/custom_player/legacy/ctm_st6_variantb.mdl"),
("models/player/custom_player/legacy/ctm_st6_variantc.mdl"),
("models/player/custom_player/legacy/ctm_st6_variantd.mdl"),
("models/player/custom_player/legacy/tm_balkan_variante.mdl"),
("models/player/custom_player/legacy/tm_balkan_varianta.mdl"),
("models/player/custom_player/legacy/tm_balkan_variantb.mdl"),
("models/player/custom_player/legacy/tm_balkan_variantc.mdl"),
("models/player/custom_player/legacy/tm_balkan_variantd.mdl"),
("models/player/custom_player/legacy/tm_jumpsuit_varianta.mdl"),
("models/player/custom_player/legacy/tm_jumpsuit_variantb.mdl"),
("models/player/custom_player/legacy/tm_jumpsuit_variantc.mdl"),
("models/player/custom_player/legacy/tm_phoenix_heavy.mdl"),
("models/player/custom_player/legacy/ctm_heavy.mdl"),
("models/player/custom_player/legacy/tm_leet_varianta.mdl"),
("models/player/custom_player/legacy/tm_leet_variantb.mdl"),
("models/player/custom_player/legacy/tm_leet_variantc.mdl"),
("models/player/custom_player/legacy/tm_leet_variantd.mdl"),
("models/player/custom_player/legacy/tm_leet_variante.mdl"),
("models/player/custom_player/legacy/tm_phoenix.mdl"),
("models/player/custom_player/legacy/tm_phoenix_varianta.mdl"),
("models/player/custom_player/legacy/tm_phoenix_variantb.mdl"),
("models/player/custom_player/legacy/tm_phoenix_variantc.mdl"),
("models/player/custom_player/legacy/tm_phoenix_variantd.mdl"),
("models/player/custom_player/legacy/ctm_fbi.mdl"),
("models/player/custom_player/legacy/ctm_fbi_varianta.mdl"),
("models/player/custom_player/legacy/ctm_fbi_variantc.mdl"),
("models/player/custom_player/legacy/ctm_fbi_variantd.mdl"),
("models/player/custom_player/legacy/ctm_fbi_variante.mdl"),
("models/player/custom_player/legacy/ctm_sas.mdl")
};

IMaterial* smoke1 = nullptr;
IMaterial* smoke2 = nullptr;
IMaterial* smoke3 = nullptr;
IMaterial* smoke4 = nullptr;

namespace Hooked
{
	struct clientanimating_t {
		C_BaseAnimating* pAnimating;
		unsigned int	flags;
		clientanimating_t(C_BaseAnimating* _pAnim, unsigned int _flags) : pAnimating(_pAnim), flags(_flags) { }
	};

	tl::optional<knife_t&> get_knife_by_id(uint16_t id)
	{
		for (auto& knife : parser::knifes.list) {
			if (knife.id == id) {
				return knife;
			}
		}

		return tl::nullopt;
	}

	auto get_skin_by_paint_kit = [](int wpn, int id) -> int
	{
		for (auto i = 0; i < parser::weapons.list[wpn].skins.list.size(); i++) {
			auto& skin = parser::weapons.list[wpn].skins.list[i];

			if (skin.id == id) {
				return i;
			}
		}

		return 0;
	};

	void __fastcall FrameStageNotify(void* ecx, void* edx, ClientFrameStage_t stage)
	{
		auto clantag_changer = []() -> void
		{
			static float oldTime = -1.f;

			auto setclantag = [](const char* tag, const char* lol) -> void
			{
				typedef void(__fastcall* SetClanTagFn)(const char*, const char*);
				static auto set_clan_tag = reinterpret_cast<SetClanTagFn>(Engine::Displacement::Signatures[c_signatures::SET_CLAN_TAG]);

				if (!Engine::Displacement::Signatures[c_signatures::INIT_KEY_VALUES])
					return;

				set_clan_tag(tag, lol);
			};

			/*auto Marquee = [](std::string& clantag) -> void
			{
				std::string temp = clantag;
				clantag.erase(0, 1);
				clantag += temp[0];
			};*/

			static int v22 = 0;
			const auto v15 = (1.0f / csgo.m_globals()->interval_per_tick);
			/*static int tick = ctx.current_tickcount;
			if (v16 && ctx.current_tickcount != tick)
			{
				v17 = v16->GetAvgLatency(0);
				v14 = TIME_TO_TICKS(v17);
				tick = ctx.current_tickcount;
			}*/

			const auto v21 = static_cast<int>((csgo.m_globals()->tickcount + TIME_TO_TICKS(ctx.latency[FLOW_OUTGOING])) / (v15 / 2)) % 17;

			if (!ctx.m_settings.visuals_clantag) {
				if (oldTime > -1.f) {
					setclantag(sxor(""), sxor("enrage.gg"));
					oldTime = -1.f;
				}

				return;
			}

			if (csgo.m_engine()->IsInGame() && csgo.m_client_state() && csgo.m_client_state()->m_iChockedCommands <= 0) {

				if (v21 != v22)
				{
					oldTime = csgo.m_globals()->realtime;
					v22 = v21;
					//Marquee(cur_clantag);
					switch (v21)
					{
					case 0: setclantag(sxor(" e.gg "), sxor("D.z")); break;
					case 1: setclantag(sxor(" en.gg "), sxor("d.l")); break;
					case 2: setclantag(sxor(" enr.gg "), sxor("d.k")); break;
					case 3: setclantag(sxor(" enra.gg "), sxor("a.j")); break;
					case 4: setclantag(sxor(" enrag.gg "), sxor("d.h")); break;
					case 10: setclantag(sxor(" enrage.gg "), sxor("s.g")); break;
					case 11: setclantag(sxor(" nrage.gg "), sxor("f.f")); break;
					case 12: setclantag(sxor(" rage.gg "), sxor("g.s")); break;
					case 13: setclantag(sxor(" age.gg "), sxor("h.d")); break;
					case 14: setclantag(sxor(" ge.gg "), sxor("j.z")); break;
					case 15: setclantag(sxor(" e.gg "), sxor("k.a")); break;
					case 16: setclantag(sxor(" "), sxor("h.f")); break;
					default: break;
					}
				}
			}
		};

		auto extend_fakelag_packets = []() -> void
		{
			static bool noob = false;

			if (noob)
				return;

			if (!noob) {
				static DWORD lol = Engine::Displacement::Signatures[c_signatures::COMMANDS_LIMIT] + 0xBC + 1;
				DWORD old;

				VirtualProtect((LPVOID)lol, 1, PAGE_READWRITE, &old);
				*(int*)lol = 62;
				VirtualProtect((LPVOID)lol, 1, old, &old);

				noob = true;
			}
		};

		auto translate_id = [](int wpn) -> int
		{
			for (auto i = 0; i < parser::weapons.list.size(); i++) {
				auto& skin = parser::weapons.list[i];

				if (skin.id == wpn) {
					return i;
				}
			}

			return 0;
		};

		auto change_skins = [translate_id](int stage)->void
		{
			if (!ctx.m_settings.skinchanger_enabled) {

				if (ctx.m_local() && !ctx.m_local()->IsDead())
					ctx.original_model_index = ctx.m_local()->m_nModelIndex();
				else
					ctx.original_model_index = -1;

				return;
			}
			if (!csgo.m_engine()->IsInGame() || parser::knifes.list.size() <= 1)
				return;
			if (!ctx.m_local())
				return;
			if (ctx.m_local()->m_iHealth() <= 0)
				return;

			if (ctx.m_settings.skins_player_model) {
				const int iTeam = ctx.m_local()->m_iTeamNum();

				if (iTeam == 2 && ctx.m_settings.skins_player_model_type_t > 0) {
					const auto TeroristModelIndex = csgo.m_model_info()->GetModelIndex(models_to_change[max(0, ctx.m_settings.skins_player_model_type_t)]);

					if (TeroristModelIndex > 0) {
						if (ctx.m_local()->m_nModelIndex() != TeroristModelIndex)
							ctx.original_model_index = ctx.m_local()->m_nModelIndex();
						ctx.m_local()->set_model_index(TeroristModelIndex);
					}
					else
						ctx.original_model_index = ctx.m_local()->m_nModelIndex();
				}
				else if (iTeam == 3 && ctx.m_settings.skins_player_model_type_ct > 0) {
					const auto CTeroristModelIndex = csgo.m_model_info()->GetModelIndex(models_to_change[max(0, ctx.m_settings.skins_player_model_type_ct)]);

					if (CTeroristModelIndex > 0) {
						if (ctx.m_local()->m_nModelIndex() != CTeroristModelIndex)
							ctx.original_model_index = ctx.m_local()->m_nModelIndex();
						ctx.m_local()->set_model_index(CTeroristModelIndex);
					}
					else
						ctx.original_model_index = ctx.m_local()->m_nModelIndex();
				}
			}
			else
				ctx.original_model_index = ctx.m_local()->m_nModelIndex();
			//ctx.updating_skins = false;

			player_info local_info;
			if (!csgo.m_engine()->GetPlayerInfo(csgo.m_engine()->GetLocalPlayer(), &local_info))
				return;

			auto* const weapons = ctx.m_local()->m_hMyWeapons();
			if (!weapons) return;

			C_WeaponCSBaseGun* i_weapon = m_weapon();

			if (!i_weapon) return;

			C_BasePlayer* viewmodel = csgo.m_entity_list()->GetClientEntityFromHandle(
				reinterpret_cast<CBaseHandle>(ctx.m_local()->m_hViewModel()));
			if (!viewmodel) return;

			C_BasePlayer* worldmodel = csgo.m_entity_list()->GetClientEntityFromHandle(i_weapon->m_hWeaponWorldModel());
			if (!worldmodel) return;

			//apply_gloves(local_info.xuidlow);

			if (!parser::knifes.list.empty() && i_weapon->is_knife())
			{
				const auto new_knife_id = parser::knifes.list[ctx.m_settings.skinchanger_knife].id;

				auto knife = get_knife_by_id(new_knife_id);

				if (knife.has_value())
				{
					viewmodel->set_model_index(knife->model_player);
					worldmodel->set_model_index(knife->model_world);
				}
			}

			bool lol = false;

			auto cknife = translate_id(parser::knifes.list[ctx.m_settings.skinchanger_knife].id);

			for (int i = 0; weapons[i] != 0xFFFFFFFF; i++)
			{
				if (!weapons[i])
					continue;

				auto weapon = reinterpret_cast<C_WeaponCSBaseGun*>(csgo.m_entity_list()->GetClientEntityFromHandle(weapons[i]));

				if (!weapon)
					continue;

				//auto& m_iItemIDHigh = weapon->m_iItemIDHigh();
				//auto& m_iItemDefinitionIndex = weapon->m_iItemDefinitionIndex();
				auto& m_nFallbackPaintKit = weapon->m_nFallbackPaintKit();
				//auto& m_iEntityQuality = weapon->m_iEntityQuality();
				//auto& m_iAccountID = weapon->m_iAccountID();
				auto& m_OriginalOwnerXuidLow = weapon->m_OriginalOwnerXuidLow();
				auto& m_OriginalOwnerXuidHigh = weapon->m_OriginalOwnerXuidHigh();
				//auto& m_szCustomName = weapon->m_szCustomName();

				weapon->m_iItemIDHigh() = -1;

				weapon->m_iAccountID() = local_info.xuidlow;

				if (local_info.xuidhigh != m_OriginalOwnerXuidHigh ||
					local_info.xuidlow != m_OriginalOwnerXuidLow)
					continue; // not OUR weapon

				if (!parser::knifes.list.empty() && weapon->is_knife())
				{
					const auto new_knife_id = parser::knifes.list[ctx.m_settings.skinchanger_knife].id;

					auto knife = get_knife_by_id(new_knife_id);

					if (knife.has_value())
					{
						if (static_cast<int>(weapon->m_iItemDefinitionIndex()) != static_cast<int>(new_knife_id)) {
							weapon->m_iItemDefinitionIndex() = static_cast<int>(new_knife_id);
							weapon->m_iEntityQuality() = 3;
						}

						if (knife->model_player == -1 || knife->model_world == -1) {
							knife->model_world = csgo.m_model_info()->GetModelIndex(knife->model_world_path.c_str());
							knife->model_player = csgo.m_model_info()->GetModelIndex(knife->model_player_path.c_str());
						}

						ctx.knife_model_index = knife->model_player;
						weapon->set_model_index(knife->model_player);
						weapon->m_iWorldModelIndex() = knife->model_player + 1;
					}
				}
				else
					continue;

				if (parser::weapons.list.size() <= 1 || parser::weapons.list[cknife].skins.list.size() <= 1)
					break;

				if (ctx.m_settings.chams_misc[1] || ctx.m_settings.skinchanger_knife_skin < 0 || ctx.m_settings.skinchanger_knife_skin >= parser::weapons.list[cknife].skins.list.size())
				{
					m_nFallbackPaintKit = 0;
					continue;
				}

				const auto skin = parser::weapons.list[cknife].skins.list[ctx.m_settings.skinchanger_knife_skin].id;
				if (skin != m_nFallbackPaintKit)
				{
					m_nFallbackPaintKit = skin;
					//m_nFallbackSeed = item.Seed;
					//m_nFallbackStatTrak = item.StatTrak;
					//if (item.StatTrak >= 0) m_iEntityQuality = 9;
					//if (item.Name) strcpy(m_szCustomName, item.Name);
					lol = true;
				}



				//weapon->m_nFallbackStatTrak() = cheat::settings.stattrak[cheat::main::local()->get_weapon()->m_iItemDefinitionIndex()];
			}

			if (lol && ctx.update_hud_weapons)
			{
				//ctx.updating_skins = true;

				auto ClearHudWeaponIcon = reinterpret_cast<std::int32_t(__thiscall*)(void*, std::int32_t)>(ctx.update_hud_weapons);

				if (ClearHudWeaponIcon) {
					auto dw_hud_weapon_selection = feature::find_hud_element<DWORD>(sxor("CCSGO_HudWeaponSelection"));
					if (dw_hud_weapon_selection)
					{
						auto hud_weapons = reinterpret_cast<int*>(dw_hud_weapon_selection - 0xA0);
						if (hud_weapons && *hud_weapons)
						{
							auto hud_weapons_count = reinterpret_cast<std::int32_t*>(std::uintptr_t(hud_weapons) + 0x80);

							if (hud_weapons_count) {
								for (int i = 0; i < *hud_weapons_count; i++)
									i = ClearHudWeaponIcon(hud_weapons, i);
							}
							//hud_weapons = 0;
						}
					}
				}
			}
		};

		using Fn = void(__thiscall*)(void*, ClientFrameStage_t);

		//if (stage != FRAME_START)
			ctx.last_frame_stage = stage;

		extend_fakelag_packets();

		const auto is_valid = csgo.m_engine()->IsInGame() && ctx.m_local() && !ctx.m_local()->IsDead();

		if (is_valid) {
			if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
				change_skins(stage);
		}
		else
			ctx.latest_weapon_data = nullptr;

		static bool once = false;

		if (!once && ctx.init_finished)
		{
			//#ifdef AUTH
			//			erase_fn(Source::Create);
			//#endif // AUTH

			if (parser::knifes.list.size() > 1) {
				for (auto i = 0; i < parser::knifes.list.size(); i++)
				{
					auto skins = parser::knifes.list[i];
					std::string p = std::string(skins.translated_name.begin(), skins.translated_name.end());
					ctx.knifes.push_back(p);
				}


				//if (GetAsyncKeyState(VK_SHIFT)) {
				//	ctx.boost_fps = true;
				//}

				auto cknife = translate_id(parser::knifes.list[ctx.m_settings.skinchanger_knife].id);
				ctx.skins.clear();
				for (auto i = 0; i < parser::weapons.list[cknife].skins.list.size(); i++)
				{
					auto skins = parser::weapons.list[cknife].skins.list.data()[i];
					std::string p = std::string(skins.translated_name.begin(), skins.translated_name.end());
					ctx.skins.push_back(p);
				}
			}

			std::ofstream bell_stream(sxor("csgo\\sound\\bell.wav"), std::ios::binary);

			for (int i = 0; i < 42276; i++)
			{
				bell_stream << bell[i];
			}

			bell_stream.close();

			std::ofstream voice_input_stream(sxor("csgo\\sound\\voice_input.wav"), std::ios::binary);

			for (int i = 0; i < 29526; i++)
			{
				voice_input_stream << screenshot[i];
			}

			voice_input_stream.close();

			/*CRC32_t crc;

				CRC32_Init(&crc);
				CRC32_ProcessBuffer(&crc, &screenshot, sizeof(unsigned char) * 29526);
				CRC32_Final(&crc);*/

			once = true;
		}

		bool was_in_tp = false;
		//bool filled = false;

		if (stage == FRAME_RENDER_START && is_valid) {
			was_in_tp = csgo.m_input()->is_in_tp();

			if (was_in_tp) {
				ctx.m_local()->UpdateVisibilityAllEntities();
			}

			if (ctx.m_settings.visuals_bloom_enabled) {
				static auto m_bUseCustomAutoExposureMin = Engine::PropManager::Instance().GetOffset(sxor("DT_EnvTonemapController"), sxor("m_bUseCustomAutoExposureMin"));
				static auto m_bUseCustomAutoExposureMax = Engine::PropManager::Instance().GetOffset(sxor("DT_EnvTonemapController"), sxor("m_bUseCustomAutoExposureMax"));
				static auto m_bUseCustomBloomScale = Engine::PropManager::Instance().GetOffset(sxor("DT_EnvTonemapController"), sxor("m_bUseCustomBloomScale"));

				static auto m_flCustomAutoExposureMin = Engine::PropManager::Instance().GetOffset(sxor("DT_EnvTonemapController"), sxor("m_flCustomAutoExposureMin"));
				static auto m_flCustomAutoExposureMax = Engine::PropManager::Instance().GetOffset(sxor("DT_EnvTonemapController"), sxor("m_flCustomAutoExposureMax"));

				static auto m_flCustomBloomScale = Engine::PropManager::Instance().GetOffset(sxor("DT_EnvTonemapController"), sxor("m_flCustomBloomScale"));

				for (auto i = 64; i < csgo.m_entity_list()->GetHighestEntityIndex(); i++) {
					auto entity = csgo.m_entity_list()->GetClientEntity(i);
					if (!entity)
						continue;

					auto client_class = entity->GetClientClass();

					if (strcmp(client_class->m_pNetworkName, "CEnvTonemapController")) {
						continue;
					}

					*(bool*)(uintptr_t(entity) + m_bUseCustomAutoExposureMin) = true;
					*(bool*)(uintptr_t(entity) + m_bUseCustomAutoExposureMax) = true;
					*(bool*)(uintptr_t(entity) + m_bUseCustomBloomScale) = true;

					*(float*)(uintptr_t(entity) + m_flCustomAutoExposureMin) = ctx.m_settings.visuals_bloom_exposure / 10.0f;
					*(float*)(uintptr_t(entity) + m_flCustomAutoExposureMax) = ctx.m_settings.visuals_bloom_exposure / 10.0f;
					*(float*)(uintptr_t(entity) + m_flCustomBloomScale) = ctx.m_settings.visuals_bloom_scale / 10.0f;

				}
			}

			if (ctx.is_charging)
				csgo.m_globals()->interpolation_amount = 0.f;

			if (csgo.m_engine()->IsInGame())
				feature::lagcomp->interpolate(stage);

			csgo.m_input()->m_fCameraInThirdPerson = false;

			ctx.in_tp = ctx.get_key_press(ctx.m_settings.visuals_tp_key);

			ctx.active_keybinds[3].mode = 0;

			if (ctx.in_tp && ctx.m_settings.visuals_tp_key.mode != 3)
				ctx.active_keybinds[3].mode = ctx.m_settings.visuals_tp_key.mode;
		}

		if (csgo.m_engine()->IsInGame()) {
			if (csgo.m_client_state()->m_iDeltaTick > 0 && stage == FRAME_RENDER_START)
				clantag_changer();
		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{
			if (ctx.m_local() != nullptr)
			{
				if (ctx.m_settings.visuals_no_flash)
					ctx.m_local()->m_flFlashDuration() = 0;
			}
#if 0
			if (GetAsyncKeyState(VK_RETURN))
			{
				g_WeatherController.StopAllWeather();
				for (int i = 0; i <= Interfaces::ClientEntList->GetHighestEntityIndex(); ++i)
				{
					CBaseEntity* bleh = Interfaces::ClientEntList->GetBaseEntity(i);
					if (bleh && bleh->GetClientClass() && bleh->GetClientClass()->m_ClassID == ClassID::_CPrecipitation)
					{
						int modelindex = bleh->GetModelIndex();
						bleh->GetClientNetworkable()->Release();
					}
				}
			}
			else
			{
				int i = 1;
				if (!g_WeatherController.HasSpawnedWeatherOfType(CWeatherController::CClient_Precipitation::PRECIPITATION_TYPE_PARTICLESNOW))
				{
					if (GetAsyncKeyState(VK_F1))
						g_WeatherController.CreateWeather(CWeatherController::CClient_Precipitation::PRECIPITATION_TYPE_PARTICLESNOW);
				}
				if (!g_WeatherController.HasSpawnedWeatherOfType(CWeatherController::CClient_Precipitation::PRECIPITATION_TYPE_PARTICLERAIN))
				{
					if (GetAsyncKeyState(VK_F2))
						g_WeatherController.CreateWeather(CWeatherController::CClient_Precipitation::PRECIPITATION_TYPE_PARTICLERAIN);
				}
				g_WeatherController.UpdateAllWeather();
			}
#endif
		}


		//static float networkedCycle = 0.0f;
		//static float animationTime = 0.f;

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START && is_valid && csgo.m_input() && ctx.m_local()->m_hViewModel()) {
			auto viewmodel = csgo.m_entity_list()->GetClientEntityFromHandle((CBaseHandle)ctx.m_local()->m_hViewModel());

			// m_flAnimTime : 0x260
			if (viewmodel) {
				// onetap.su
				if (Engine::Prediction::Instance()->m_nSequence == ((C_BaseViewModel*)viewmodel)->m_nSequence()
					&& Engine::Prediction::Instance()->m_hWeapon == ((C_BaseViewModel*)viewmodel)->get_viewmodel_weapon()
					&& Engine::Prediction::Instance()->m_nAnimationParity == ((C_BaseViewModel*)viewmodel)->m_nAnimationParity()) {
					viewmodel->m_flCycle() = Engine::Prediction::Instance()->networkedCycle;
					((C_BaseViewModel*)viewmodel)->m_flModelAnimTime() = Engine::Prediction::Instance()->animationTime;
					ctx.update_cycle = CYCLE_NONE;
				}
			}
		}


		feature::resolver->approve_shots(stage);

		feature::resolver->update_missed_shots(stage);

		vmt.m_client->VCall<Fn>(Index::IBaseClientDLL::FrameStageNotify)(ecx, stage);

		//ctx.setup_bones = false;

		if (csgo.m_engine()->IsInGame())
			feature::lagcomp->store_records(stage);

		if (ctx.current_tickcount % 2 == 0) {
			if (csgo.m_engine()->IsInGame() && ctx.m_local()) {

				if (ctx.hud_death_notice == nullptr || ctx.update_hud_weapons == nullptr || csgo.m_player_resource() == nullptr || csgo.m_player_resource()->this_ptr == nullptr)
				{
					for (auto i = 0; i < parser::knifes.list.size(); i++)
					{
						auto knife = &parser::knifes.list[i];

						knife->model_world = csgo.m_model_info()->GetModelIndex(knife->model_world_path.c_str());
						knife->model_player = csgo.m_model_info()->GetModelIndex(knife->model_player_path.c_str());
					}

					csgo.m_player_resource.set(C_PlayerResource::get());

					if (csgo.m_game_rules() == nullptr)
						csgo.m_game_rules.set((**reinterpret_cast<CCSGameRules***>(Engine::Displacement::Signatures[c_signatures::GAMERULES] + 0x1)));
					if (ctx.hud_death_notice == nullptr)
						ctx.hud_death_notice = feature::find_hud_element<CCSGO_HudDeathNotice*>(sxor("CCSGO_HudDeathNotice"));
					if (ctx.update_hud_weapons == nullptr)
						ctx.update_hud_weapons = (void*)Engine::Displacement::Signatures[c_signatures::UPDATEHUDWEAPONS];
				}
			}
			else
			{
				if (csgo.m_game_rules() != nullptr) {
					ctx.hud_death_notice = nullptr;
					ctx.update_hud_weapons = nullptr;
					ctx.fake_state.m_player = 0;
					ctx.shots_fired.fill(0);
					ctx.shots_total.fill(0);
					//ctx.command_numbers.clear();
					vmt.m_net_channel.reset();
					vmt.m_clientstate.reset();
					csgo.m_game_rules.set(nullptr);
					csgo.m_player_resource.set(nullptr);
				}
			}

			if (stage == FRAME_RENDER_START && is_valid && m_weapon() && ctx.latest_weapon_data) {

				//auto &weapon_info = ctx.latest_weapon_data;

				ctx.autowall_crosshair = -1;

				if ((int)ctx.latest_weapon_data > 0x1000 && m_weapon()->IsGun() && ctx.m_settings.visuals_autowall_crosshair)
				{
					Vector start = ctx.m_eye_position;
					Vector direction;
					auto ang = Engine::Movement::Instance()->m_qRealAngles;
					csgo.m_engine()->GetViewAngles(ang);
					Math::AngleVectors(ang, &direction);

					auto max_range = ctx.latest_weapon_data->range * 2;
					auto dmg = (float)ctx.latest_weapon_data->damage;

					Vector end = start + (direction * max_range);
					float currentDistance = 0;

					CGameTrace enterTrace;

					CTraceFilter filter;
					filter.pSkip = ctx.m_local();

					feature::autowall->TraceLine(start, end, MASK_SHOT | CONTENTS_GRATE, ctx.m_local(), &enterTrace);

					if (enterTrace.fraction == 1)
						dmg = -1;
					else
						//calculate the damage based on the distance the bullet traveled.
						currentDistance += enterTrace.fraction * max_range;

					//Let's make our damage drops off the further away the bullet is.
					dmg *= pow(ctx.latest_weapon_data->range_modifier, currentDistance / 500);

					auto enterSurfaceData = csgo.m_phys_props()->GetSurfaceData(enterTrace.surface.surfaceProps);
					float enterSurfPenetrationModifier = enterSurfaceData->game.penetrationmodifier;

					if (currentDistance > 3000 && ctx.latest_weapon_data->penetration > 0 || enterSurfPenetrationModifier <= 0)
						dmg = -1;

					if (enterTrace.m_pEnt != nullptr)
					{
						//This looks gay as fuck if we put it into 1 long line of code.
						bool canDoDamage = (enterTrace.hitgroup - 1) <= 7;
						bool isPlayer = (enterTrace.m_pEnt->GetClientClass() && enterTrace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CCSPlayer);
						//bool isEnemy = (ctx.m_local()->m_iTeamNum() != ((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum());
						bool onTeam = (((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum() == 2 || ((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum() == 3);

						//TODO: Team check config
						if (canDoDamage && isPlayer && onTeam)
							feature::autowall->ScaleDamage(enterTrace, ctx.latest_weapon_data, dmg);

						if (!canDoDamage && isPlayer)
							dmg = -1;
					}

					auto penetrate_count = 4;

					ctx.force_low_quality_autowalling = true;

					if (!feature::autowall->HandleBulletPenetration(ctx.m_local(), ctx.latest_weapon_data, enterTrace, start, direction, penetrate_count, dmg, ctx.latest_weapon_data->penetration, 0.f, true))
						dmg = -1;

					ctx.force_low_quality_autowalling = false;

					if (penetrate_count <= 0)
						dmg = -1;

					ctx.autowall_crosshair = (dmg > ctx.m_settings.aimbot_min_damage) ? 2 : (dmg > 0 ? 1 : 0);
				}
			}
		}

		/*if (is_valid)
		{
			auto viewmodel = csgo.m_entity_list()->GetClientEntityFromHandle((CBaseHandle)ctx.m_local()->m_hViewModel());

			if (viewmodel)
			{
				cycle = viewmodel->m_flCycle();
				anim_time = viewmodel->m_flAnimTime();
			}
		}*/

		//if (stage == FRAME_RENDER_START && is_valid) {
		//
		//	if (ctx.m_settings.visuals_no_recoil && filled && !was_in_tp) {
		//		ctx.m_local()->m_aimPunchAngle() = aim;
		//		ctx.m_local()->m_viewPunchAngle() = view;
		//	}
		//	//Engine::Prediction::Instance()->DoInterpolateVars();
		//}

		if (is_valid && ctx.current_tickcount % 2 == 0) {
			static float local_spawntime = ctx.m_local()->m_flSpawnTime();

			if (local_spawntime != ctx.m_local()->m_flSpawnTime())
			{
				if (ctx.m_settings.misc_preserve_killfeed) {

					ctx.hud_death_notice = feature::find_hud_element<CCSGO_HudDeathNotice*>(sxor("CCSGO_HudDeathNotice"));
					ctx.update_hud_weapons = reinterpret_cast<void*>(Engine::Displacement::Signatures[c_signatures::UPDATEHUDWEAPONS]);

					if (ctx.hud_death_notice) {
						int* death_notices = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(ctx.hud_death_notice) -
							20);

						if (death_notices) {
							auto ClearDeathNotices = reinterpret_cast<void(__thiscall*)(DWORD)>(Engine::Displacement::Signatures[c_signatures::CLEARDEATHNOTICES]);

							if (ClearDeathNotices)
								ClearDeathNotices(reinterpret_cast<DWORD>(ctx.hud_death_notice) - 20);
						}
					}
				}

				local_spawntime = ctx.m_local()->m_flSpawnTime();
			}
		}

		if (stage == FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
			if (!smoke1)
				smoke1 = csgo.m_material_system()->FindMaterial(sxor("particle/vistasmokev1/vistasmokev1_fire"), sxor("Other textures"));

			if (!smoke2)
				smoke2 = csgo.m_material_system()->FindMaterial(sxor("particle/vistasmokev1/vistasmokev1_smokegrenade"), sxor("Other textures"));

			if (!smoke3)
				smoke3 = csgo.m_material_system()->FindMaterial(sxor("particle/vistasmokev1/vistasmokev1_emods"), sxor("Other textures"));

			if (!smoke4)
				smoke4 = csgo.m_material_system()->FindMaterial(sxor("particle/vistasmokev1/vistasmokev1_emods_impactdust"), sxor("Other textures"));

			if (ctx.m_settings.visuals_no_smoke) {
				if (!smoke1->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke1->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

				if (!smoke2->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke2->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

				if (!smoke3->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke3->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

				if (!smoke4->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke4->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
			}

			else {
				if (smoke1->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke1->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

				if (smoke2->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke2->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

				if (smoke3->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke3->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

				if (smoke4->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
					smoke4->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
			}
		}

		//if (stage == FRAME_RENDER_START && is_valid && csgo.m_input()->m_fCameraInThirdPerson)
		//	ctx.m_local()->DrawServerHitboxes();

		//auto& prediction = Engine::Prediction::Instance();
		//prediction->OnFrameStageNotify(stage);
	}

}