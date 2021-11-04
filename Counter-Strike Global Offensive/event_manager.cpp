#include "hooked.hpp"
#include "displacement.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "prediction.hpp"
#include "movement.hpp"
#include "rage_aimbot.hpp"
#include "anti_aimbot.hpp"
#include <playsoundapi.h>
#include "visuals.hpp"
#include <intrin.h>
#include "resolver.hpp"
#include "lag_comp.hpp"
#include "menu.hpp"
#include "autowall.hpp"
#include "visuals.hpp"
#include "sdk.hpp"
#include "source.hpp"
#include "music_player.hpp"
#include "aimbot.hpp"

game_events::PlayerHurtListener player_hurt_listener;
game_events::BulletImpactListener bullet_impact_listener;
game_events::PlayerDeathListener player_death_listener;
game_events::RoundEndListener round_end_listener;
game_events::RoundStartListener round_start_listener;
game_events::BombPlantListener bomb_plant_listener;
game_events::PurchaseListener item_purchase_listener;
game_events::WeaponFireListener weapon_fire_listener;

game_events::BombStartPlantListener bomb_plant_start_listener;
game_events::BombStopPlantListener bomb_plant_end_listener;
game_events::BombStartDefuseListener bomb_defuse_start_listener;
game_events::BombStopDefuseListener bomb_defuse_end_listener;

//ctx.m_settings.misc_notifications, { "bomb info", "damage", "misses", "shots debug", "purchases" });

void game_events::init()
{
	csgo.m_event_manager()->AddListener(&item_purchase_listener, sxor("item_purchase"), false);
	csgo.m_event_manager()->AddListener(&player_hurt_listener, sxor("player_hurt"), false);
	csgo.m_event_manager()->AddListener(&bullet_impact_listener, sxor("bullet_impact"), false);
	csgo.m_event_manager()->AddListener(&weapon_fire_listener, sxor("weapon_fire"), false);
	csgo.m_event_manager()->AddListener(&player_death_listener, sxor("player_death"), false);
	csgo.m_event_manager()->AddListener(&round_end_listener, sxor("round_end"), false);
	csgo.m_event_manager()->AddListener(&round_start_listener, sxor("round_start"), false);
	csgo.m_event_manager()->AddListener(&bomb_plant_listener, sxor("bomb_planted"), false);

	csgo.m_event_manager()->AddListener(&bomb_plant_start_listener, sxor("bomb_beginplant"), false);
	csgo.m_event_manager()->AddListener(&bomb_plant_end_listener, sxor("bomb_abortplant"), false);
	csgo.m_event_manager()->AddListener(&bomb_defuse_start_listener, sxor("bomb_begindefuse"), false);
	csgo.m_event_manager()->AddListener(&bomb_defuse_end_listener, sxor("bomb_abortdefuse"), false);
}

void game_events::BombStartPlantListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));

	player_info info;

	if (ctx.m_settings.misc_notifications[0] && csgo.m_engine()->GetPlayerInfo(userid, &info))
		_events.emplace_back(std::string(sxor("bomb is being planted by ") + std::string{ info.name }.substr(0, 24)));
}

void game_events::WeaponFireListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	//const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));
	feature::resolver->listener(game_event);
}

void game_events::BombStartDefuseListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));

	const auto haskit = game_event->GetBool(sxor("haskit"));

	player_info info;

	if (ctx.m_settings.misc_notifications[0] && csgo.m_engine()->GetPlayerInfo(userid, &info))
		_events.emplace_back(std::string(sxor("bomb is being defused ") + std::string(haskit ? sxor(" (using kit) ") : "") + sxor(" by ") + std::string{ info.name }.substr(0, 24)));
}

void game_events::BombStopPlantListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));

	player_info info;

	if (ctx.m_settings.misc_notifications[0] && csgo.m_engine()->GetPlayerInfo(userid, &info))
		_events.emplace_back(std::string(std::string{ info.name }.substr(0, 24) + sxor(" stopped planting the bomb")));
}

void game_events::BombStopDefuseListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));

	player_info info;

	if (ctx.m_settings.misc_notifications[0] && csgo.m_engine()->GetPlayerInfo(userid, &info))
		_events.emplace_back(std::string(std::string{ info.name }.substr(0, 24) + sxor(" stopped defusing the bomb")));
}

void game_events::BombPlantListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	//const auto &entity = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid"))));

	//const int bombsite = game_event->GetInt(sxor("site"));
	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));

	player_info info;

	if (ctx.m_settings.misc_notifications[0] &&csgo.m_engine()->GetPlayerInfo(userid, &info))
		_events.emplace_back(std::string(sxor("bomb has been planted by ") + std::string{ info.name }.substr(0, 24)));
}

std::string hitgroup_to_name(const int hitgroup) {
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return sxor("head");
	case HITGROUP_CHEST:
		return sxor("chest");
	case HITGROUP_STOMACH:
		return sxor("stomach");
	case HITGROUP_LEFTARM:
		return sxor("left arm");
	case HITGROUP_RIGHTARM:
		return sxor("right arm");
	case HITGROUP_LEFTLEG:
		return sxor("left leg");
	case HITGROUP_RIGHTLEG:
		return sxor("right leg");
	default:
		return sxor("body");
	}
}

int hitgroup_to_idx(const int hitgroup) {
	switch (hitgroup)
	{
	case HITGROUP_HEAD:
		return 0;
	case HITGROUP_CHEST:
		return HITBOX_CHEST;
	case HITGROUP_STOMACH:
		return HITBOX_BODY;
	case HITGROUP_LEFTARM:
		return HITBOX_LEFT_HAND;
	case HITGROUP_RIGHTARM:
		return HITBOX_RIGHT_HAND;
	case HITGROUP_LEFTLEG:
		return HITBOX_LEFT_FOOT;
	case HITGROUP_RIGHTLEG:
		return HITBOX_RIGHT_FOOT;
	default:
		return HITBOX_BODY;
	}
}

void game_events::PlayerHurtListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr/* || !m_weapon() || !ctx.latest_weapon_data*/)
		return;

	const auto& entity = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid"))));

	if (!entity || !entity->GetClientClass() || !entity->IsPlayer())
		return;

	auto entity_attacker = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("attacker"))));

	if (!entity_attacker || !entity_attacker->GetClientClass() || !entity_attacker->IsPlayer())
		return;

	if (entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || entity_attacker != ctx.m_local()) {
		if (entity == ctx.m_local())
		{
			player_info attacker_info;
			if (!csgo.m_engine()->GetPlayerInfo(entity_attacker->entindex(), &attacker_info))
				return;

			//hurt indicator + log
			ctx.local_damage[entity->entindex() - 1] = { game_event->GetInt(sxor("dmg_health")), csgo.m_globals()->realtime };

			if (ctx.m_settings.misc_notifications[5]) {
				const auto name = std::string(attacker_info.name);
				_events.emplace_back(std::string(sxor("Hurt by ") + name.substr(0, 24) + sxor(" in the ") + hitgroup_to_name(game_event->GetInt(sxor("hitgroup"))) + sxor(" for ") + std::to_string(game_event->GetInt(sxor("dmg_health"))) + sxor(" (") + std::to_string(game_event->GetInt(sxor("health"))) + sxor(" health remaining)")));
			}
		}

		return;
	}

	player_info player_info;
	if (!csgo.m_engine()->GetPlayerInfo(entity->entindex(), &player_info))
		return;

	/*const auto& lag_data = &feature::resolver->player_records[entity->entindex() - 1];
	const auto& log = &feature::lagcomp->records[entity->entindex() - 1];*/

	//bool no = true;

	//if (ctx.m_settings.misc_visuals_hitboxes[1] && entity->m_bone_count() > 0)
	//	feature::ragebot->visualize_hitboxes(entity, entity->m_CachedBoneData().Base(), ctx.m_settings.misc_visuals_hitboxes_color, ctx.m_settings.misc_visuals_hitboxes_time);

	//if (entity->m_bone_count() > 0)
	//	damage_indicators.emplace_back(ctx.m_eye_position, feature::ragebot->get_hitbox(entity, hitgroup_to_idx(game_event->GetInt(sxor("hitgroup"))), entity->m_CachedBoneData().Base()), game_event->GetInt(sxor("dmg_health")), game_event->GetInt(sxor("hitgroup")) == HITGROUP_HEAD);

	feature::resolver->hurt_listener(game_event);

	//if (ctx.fired_shot.size() > 0 && !ctx.fired_shot[max(int(ctx.fired_shot.size()) - 1, 0)]._hurt_called && m_weapon()->IsGun()) {

	//	for (auto it = ctx.fired_shot.begin(); it != ctx.fired_shot.end(); it++)
	//	{
	//		const auto& record = &*it;

	//		if (record->_target != entity || record->_target == nullptr || record->_target->IsDormant())
	//			continue;

	//		if (abs(record->_avg_hurt_tick - csgo.m_globals()->tickcount) <= 1)
	//		{
	//			record->_hurt_called = true;
	//			record->_final_damage = game_event->GetInt(sxor("dmg_health"));
	//			record->_final_hitgroup = game_event->GetInt(sxor("hitgroup"));
	//			record->_headshot = game_event->GetBool(sxor("headshot"));

	//			if (record->_hitgroup == record->_final_hitgroup)
	//			{
	//				if (abs(record->_predicted_damage - record->_final_damage) < 15.f)// {
	//					lag_data->has_been_resolved = true;

	//				if (game_event->GetBool(sxor("headshot")) && record->_tickrecord)
	//					lag_data->last_hurt_resolved = record->_tickrecord->resolver_index;
	//				//}

	//				if (ctx.shots_fired[entity->entindex() - 1] > 0)
	//					ctx.shots_fired[entity->entindex() - 1] -= 1;
	//			}
	//			else
	//			{
	//				//if (record._spread < 0.0015f && record._hitchance > 100.f)
	//					//if (ctx.shots_fired[entity->entindex() - 1] > 0)
	//					//	ctx.shots_fired[entity->entindex() - 1] -= 1;
	//			}

	//			//if (abs(record._final_damage - record._predicted_damage) > 10.f)
	//			//{
	//				//_events.push_back(_event(std::string("server answer has difference [" + std::to_string(record._final_hitgroup) + "]:[" + std::to_string(record._hitgroup) + "]")));
	//				//const auto hp_left = game_event->GetInt("health");

	//				//if (hp_left > 0 && record._spread < 0.0015f && record._hitchance == 100.f && record._final_hitgroup != record._hitgroup)
	//				//{
	//					//if (ctx.shots_fired[entity->entindex() - 1] > 0)
	//					//	ctx.shots_fired[entity->entindex() - 1] -= 1;
	//				//}
	//				/*else
	//				{
	//					if (ctx.shots_fired[entity->entindex() - 1] > 0)
	//						ctx.shots_fired[entity->entindex() - 1] -= 1;
	//				}*/
	//				//	}

	//			no = false;
	//			//break;
	//		}
	//	}
	//}

	if (game_event->GetBool(sxor("headshot")))
		damage_indicators.emplace_back(Vector::Zero, csgo.m_globals()->tickcount, game_event->GetInt(sxor("dmg_health")), true);

	if (ctx.m_settings.misc_notifications[1]) {
		const auto name = std::string(player_info.name);
		_events.emplace_back(std::string(sxor("Hit ") + name.substr(0, 24) + sxor(" in the ") + hitgroup_to_name(game_event->GetInt(sxor("hitgroup"))) + sxor(" for ") + std::to_string(game_event->GetInt(sxor("dmg_health"))) + sxor(" (") + std::to_string(game_event->GetInt(sxor("health"))) + sxor(" health remaining)")));
	}
	if (ctx.m_settings.misc_hitsound_type)
	{
		//if (ctx.m_settings.misc_hitsound_type == 1)
			csgo.m_surface()->PlaySound_(sxor("buttons\\arena_switch_press_02.wav"));
		//else
		//	csgo.m_surface()->PlaySound_(sxor("bell.wav"));
	}

	ctx.hurt_time = csgo.m_globals()->realtime + 0.3f;
}

float last_bullet_impact_back = 0;

void game_events::BulletImpactListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr || !ctx.latest_weapon_data || !m_weapon())
		return;

	if (!csgo.m_engine()->IsConnected())
		return;

	const auto& entity = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid"))));

	if (!entity)
		return;

	if (entity->IsDormant())
		return;

	Vector position(game_event->GetInt(sxor("x")), game_event->GetInt(sxor("y")), game_event->GetInt(sxor("z")));

	const auto islocal = entity == ctx.m_local();

	if (!islocal)
		return;

	if (ctx.m_settings.visuals_draw_local_beams)
		bullet_tracers.emplace_back(ctx.m_eye_position, position, csgo.m_globals()->curtime, ctx.flt2color(ctx.m_settings.local_beams_color), true);

	if (ctx.m_settings.visuals_draw_local_impacts)
		csgo.m_debug_overlay()->AddBoxOverlay(position, Vector(-2.f, -2.f, -2.f), Vector(2.f, 2.f, 2.f), Vector(0.f, 0.f, 0.f), 0, 0, 255, 127, 4.f);

	feature::resolver->listener(game_event);
	//if (ctx.fired_shot.size() > 0 && !ctx.fired_shot[max(int(ctx.fired_shot.size()) - 1, 0)]._hurt_called)
	//{
	//	for (auto it = ctx.fired_shot.rbegin(); it != ctx.fired_shot.rend(); ++it)
	//	{
	//		auto& record = *it;
	//
	//		//auto tick_count = csgo.m_globals()->tickcount;
	//
	//		if (record._impact_called) {
	//
	//			if (/*record._impacts_count <= record._walls_penetrated && */(last_bullet_impact_back - csgo.m_globals()->realtime) == 0.0f)
	//			{
	//				record._impact_pos = position;
	//				record._impacts.emplace_back(position);
	//				record._impacts_count++;
	//				//csgo.m_debug_overlay()->AddBoxOverlay(position, Vector(-2, -2, -2), Vector(2, 2, 2), Vector::Zero, 0, 255, 255, 255, 5.f);
	//				break;
	//			}
	//
	//			continue;
	//		}
	//
	//		//if (abs(record._avg_impact_time - tick_count) <= 1)
	//		//{
	//		record._impact_called = true;
	//		record._impact_pos = position;
	//		record._impacts.emplace_back(position);
	//		record._avg_hurt_tick = csgo.m_globals()->tickcount + 1;
	//		record._impacts_count++;
	//		break;
	//		//}
	//	}
	//
	//	last_bullet_impact_back = csgo.m_globals()->realtime;
	//}
}

void game_events::PlayerDeathListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	const auto& entity = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid"))));

	/*if (!entity || entity->m_iTeamNum() == cheat::main::local()->m_iTeamNum())
	{
		if (entity != nullptr && entity == cheat::main::local())
		{
			for (auto i = 0; i < 64; i++)
			{
				cheat::main::shots_fired[i] = 0;
				cheat::main::shots_total[i] = 0;

				auto resolve_info = &cheat::features::aaa.player_resolver_records[i];

				cheat::features::lagcomp.records[i].reset(true);

				resolve_info->leftrec.reset();
				resolve_info->rightrec.reset();
			}
		}

		return;
	}*/
	//if (game_event->GetInt("attacker") < 0 || Source::m_pEngine->GetPlayerForUserID(game_event->GetInt("attacker")) < 0)
	//	return;

	if (!entity || !entity->GetClientClass() || entity->GetClientClass()->m_ClassID != class_ids::CCSPlayer)
		return;

	feature::legitbot->kill_delay(game_event);

	if (entity == ctx.m_local())
	{
		ctx.auto_peek_spot.clear();
		ctx.m_settings.anti_aim_autopeek_key.toggled = false;
		ctx.m_settings.anti_aim_slowwalk_key.toggled = false;
		ctx.shots_fired.fill(0);
		if (ctx.m_settings.anti_aim_slowwalk_key.key > 0)
			ctx.pressed_keys[ctx.m_settings.anti_aim_slowwalk_key.key] = false;
		ctx.m_local()->m_bIsScoped() = false;
	}

	const auto& attacker = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("attacker"))));

	if (attacker == ctx.m_local()/* && !ctx.m_local()->IsDead()*/)
	{
		if (ctx.m_settings.misc_f12_kill_sound)
			feature::music_player->play(sxor("csgo\\sound\\voice_input.wav"), 0.6f);
		//if (cheat::Cvars.Visuals_misc_killtimer.GetValue() > 0.f)
		//	cheat::main::local()->m_flHealthShotBoostExpirationTime() = Source::m_pGlobalVars->curtime + cheat::Cvars.Visuals_misc_killtimer.GetValue();

		//player_info pinfo;

		//if (Source::m_pEngine->GetPlayerInfo(entity->entindex(), &pinfo)/* && !pinfo.fakeplayer*/)
		//{
		//	cheat::main::local()->get_weapon()->m_iItemIDHigh() = -1;
		//	cheat::settings.stattrak[cheat::main::local()->get_weapon()->m_iItemDefinitionIndex()]++;
		//	cheat::main::local()->get_weapon()->m_nFallbackStatTrak() = cheat::settings.stattrak[cheat::main::local()->get_weapon()->m_iItemDefinitionIndex()];
		//	cheat::main::local()->get_weapon()->post_data_update(DATA_UPDATE_CREATED);
		//	cheat::main::local()->get_weapon()->on_data_changed(DATA_UPDATE_CREATED);
		//}

		if (ctx.m_settings.skinchanger_enabled && m_weapon() && m_weapon()->is_knife())
		{
			const auto weapon = game_event->GetString(sxor("weapon"));

			char weaponmy[23] = { 'k','n','i','f','e' };

			strcpy(weaponmy, ctx.m_local()->m_iTeamNum() == 2 ? sxor("knife_t") : sxor("knife_default_ct"));

			if (strstr(weapon, sxor("knife_default_ct"))
				|| strstr(weapon, sxor("knife"))
				|| strstr(weapon, sxor("bayonet"))
				|| strstr(weapon, sxor("knife_push"))
				|| strstr(weapon, sxor("knife_butterfly"))
				|| strstr(weapon, sxor("knife_survival_bowie"))
				|| strstr(weapon, sxor("knife_falchion"))
				|| strstr(weapon, sxor("knife_tactical"))
				|| strstr(weapon, sxor("knife_m9_bayonet"))
				|| strstr(weapon, sxor("knife_karambit"))
				|| strstr(weapon, sxor("knife_gut"))
				|| strstr(weapon, sxor("knife_flip"))
				|| strstr(weapon, sxor("knife_ursus"))
				|| strstr(weapon, sxor("knife_widowmaker"))
				|| strstr(weapon, sxor("knife_stiletto"))
				|| strstr(weapon, sxor("knife_gypsy_jackknife")))
			{
				switch (parser::knifes.list[ctx.m_settings.skinchanger_knife].id)
				{
				case WEAPON_BAYONET:
					strcpy(weaponmy, sxor("bayonet"));
					break;
				case WEAPON_KNIFE_FLIP:
					strcpy(weaponmy, sxor("knife_flip"));
					break;
				case WEAPON_KNIFE_GUT:
					strcpy(weaponmy, sxor("knife_gut"));
					break;
				case WEAPON_KNIFE_KARAMBIT:
					strcpy(weaponmy, sxor("knife_karambit"));
					break;
				case WEAPON_KNIFE_M9_BAYONET:
					strcpy(weaponmy, sxor("knife_m9_bayonet"));
					break;
				case WEAPON_KNIFE_TACTICAL:
					strcpy(weaponmy, sxor("knife_tactical"));
					break;
				case WEAPON_KNIFE_FALCHION:
					strcpy(weaponmy, sxor("knife_falchion"));
					break;
				case WEAPON_KNIFE_SURVIVAL_BOWIE:
					strcpy(weaponmy, sxor("knife_survival_bowie"));
					break;
				case WEAPON_KNIFE_BUTTERFLY:
					strcpy(weaponmy, sxor("knife_butterfly"));
					break;
				case WEAPON_KNIFE_PUSH:
					strcpy(weaponmy, sxor("knife_push"));
					break;
				case WEAPON_KNIFE_URSUS:
					strcpy(weaponmy, sxor("knife_ursus"));
					break;
				case WEAPON_KNIFE_WIDOWMAKER:
					strcpy(weaponmy, sxor("knife_widowmaker"));
					break;
				case WEAPON_KNIFE_STILETTO:
					strcpy(weaponmy, sxor("knife_stiletto"));
					break;
				case WEAPON_KNIFE_GYPSY_JACKKNIFE:
					strcpy(weaponmy, sxor("knife_gypsy_jackknife"));
					break;
				default:
					strcpy(weaponmy, ctx.m_local()->m_iTeamNum() == 2 ? sxor("knife_t") : sxor("knife_default_ct"));
					break;
				}

				if (strcmp(weapon, weaponmy))
					game_event->SetString(sxor("weapon"), weaponmy);
			}
		}
	}
}


void game_events::RoundEndListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;
	
	ctx.m_corrections_data.clear();
	/*ctx.pressed_keys[(int)cheat::Cvars.RageBot_MinDmgKey.GetValue()] = false;
	ctx.pressed_keys[(int)cheat::Cvars.RageBot_Key.GetValue()] = false;
	ctx.pressed_keys[(int)cheat::Cvars.RageBot_BaimKey.GetValue()] = false;
	ctx.pressed_keys[(int)cheat::Cvars.Exploits_fakeduck.GetValue()] = false;
	ctx.pressed_keys[(int)cheat::Cvars.LegitBot_Key.GetValue()] = false;
	ctx.pressed_keys[(int)cheat::Cvars.Misc_CircleStrafer.GetValue()] = false;*/
	
	feature::lagcomp->reset();

	/*if (!ctx.m_local()->IsDead()) {

		auto weapons = ctx.m_local()->m_hMyWeapons();

		for (int i = 0; weapons[i] != 0xFFFFFFFF; i++)
		{
			if (weapons[i] == 0) continue;

			if (i > 10)
				break;

			auto weapon = (C_WeaponCSBaseGun*)csgo.m_entity_list()->GetClientEntityFromHandle(weapons[i]);

			if (weapon == nullptr) continue;

			if (weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1)
				ctx.has_scar = true;
		}
	}*/

	/*for (auto i = 0; i++; i < 64) {
		cheat::features::dormant.m_cSoundPlayers[i].reset();
	}*/

	/*auto winner = game_event->GetInt("winner");

	auto local_player = INTERFACES::ClientEntityList->GetClientEntity(INTERFACES::Engine->GetLocalPlayer());

	if (!local_player) return;

	if (winner == local_player->GetTeam())
		should_disable_aa = true;*/
}

void game_events::RoundStartListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	if (ctx.m_settings.misc_autobuy_enabled)
		ctx.buy_weapons = true;

	ctx.shots_fired.fill(0);
	ctx.shots_total.fill(0);

	if (!bullet_tracers.empty())
		bullet_tracers.clear();

	//ctx.fired_shot.clear();
	csgo.m_debug_overlay()->ClearAllOverlays();

	//memset(feature::visuals->dormant_alpha, 0, sizeof(int) * 128);
	feature::visuals->dormant_alpha.fill(0.f);

	ctx.original_tickbase = 0; 
	ctx.exploit_tickbase_shift = 0;
	ctx.m_corrections_data.clear();

	ctx.ticks_allowed = 0;

	ctx.auto_peek_spot.clear();
	ctx.m_settings.anti_aim_autopeek_key.toggled = false;
	ctx.m_settings.anti_aim_slowwalk_key.toggled = false;

	//feature::lagcomp->reset();

	/*const auto sv_skyname = csgo.m_engine_cvars()->FindVar("sv_skyname");

	static const char* skyboxes[] = {
		"cs_tibet",
		"cs_baggage_skybox_",
		"embassy",
		"italy",
		"jungle",
		"office",
		"sky_cs15_daylight01_hdr",
		"vertigoblue_hdr",
		"sky_cs15_daylight02_hdr",
		"vertigo",
		"sky_day02_05_hdr",
		"nukeblank",
		"sky_venice",
		"sky_cs15_daylight03_hdr",
		"sky_cs15_daylight04_hdr",
		"sky_csgo_cloudy01",
		"sky_csgo_night02",
		"sky_csgo_night02b",
		"sky_csgo_night_flat",
		"sky_dust",
		"vietnam",
		"amethyst",
		"sky_descent",
		"clear_night_sky",
		"otherworld",
		"cloudynight",
		"dreamyocean",
		"grimmnight",
		"sky051",
		"sky081",
		"sky091",
		"sky561",
	};

	if (sv_skyname) {
		if (sv_skyname->m_nFlags & FCVAR_CHEAT)
			sv_skyname->m_nFlags &= ~FCVAR_CHEAT;

		if (cheat::Cvars.Visuals_world_sky.GetValue())
			sv_skyname->SetValue(skyboxes[(int)cheat::Cvars.Visuals_world_sky.GetValue() - 1]);
	}*/

	const auto shit = csgo.m_engine_cvars()->FindVar(sxor("mp_freezetime"));

	if (shit)
		feature::anti_aim->enable_delay = csgo.m_globals()->realtime + shit->GetFloat();
}

void game_events::PurchaseListener::FireGameEvent(IGameEvent* game_event)
{
	if (!game_event || ctx.m_local() == nullptr)
		return;

	//const auto &entity = csgo.m_entity_list()->GetClientEntity(csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid"))));

	const auto weapon = game_event->GetString(sxor("weapon"));

	const int idx = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));
	const int team = game_event->GetInt(sxor("team"));

	player_info info;

	if (ctx.m_settings.misc_notifications[4] && csgo.m_engine()->GetPlayerInfo(idx, &info) && (!ctx.m_local() || team != ctx.m_local()->m_iTeamNum()))
		_events.emplace_back(std::string(std::string{ info.name }.substr(0, 24) + sxor(" bought ") + weapon));
}