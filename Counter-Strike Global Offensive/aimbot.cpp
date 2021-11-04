#include "sdk.hpp"
#include "aimbot.hpp"
#include "source.hpp"
#include "weapon.hpp"
#include "misc.hpp"
#include "autowall.hpp"
#include "player.hpp"

constexpr int getWeaponIndex(int weaponId) noexcept
{
	switch (weaponId) {
	default: return 0;

	case WEAPON_GLOCK: return 1;
	case WEAPON_HKP2000: return 2;
	case WEAPON_USP_SILENCER: return 3;
	case WEAPON_ELITE: return 4;
	case WEAPON_P250: return 5;
	case WEAPON_TEC9: return 6;
	case WEAPON_FIVESEVEN: return 7;
	case WEAPON_CZ75A: return 8;
	case WEAPON_DEAGLE: return 9;
	case WEAPON_REVOLVER: return 10;

	case WEAPON_NOVA: return 11;
	case WEAPON_XM1014: return 12;
	case WEAPON_SAWEDOFF: return 13;
	case WEAPON_MAG7: return 14;
	case WEAPON_M249: return 15;
	case WEAPON_NEGEV: return 16;

	case WEAPON_MAC10: return 17;
	case WEAPON_MP9: return 18;
	case WEAPON_MP7: return 19;
	case WEAPON_MP5SD: return 20;
	case WEAPON_UMP45: return 21;
	case WEAPON_P90: return 22;
	case WEAPON_BIZON: return 23;

	case WEAPON_GALILAR: return 24;
	case WEAPON_FAMAS: return 25;
	case WEAPON_AK47: return 26;
	case WEAPON_M4A1: return 27;
	case WEAPON_M4A1_SILENCER: return 28;
	case WEAPON_SSG08: return 29;
	case WEAPON_SG556: return 30;
	case WEAPON_AUG: return 31;
	case WEAPON_AWP: return 32;
	case WEAPON_G3SG1: return 33;
	case WEAPON_SCAR20: return 34;

	case WEAPON_TASER: return 39;
	}
}

constexpr int getWeaponClass(int weaponId) noexcept
{
	switch (weaponId) {
	default: return 0;

	case WEAPON_GLOCK:
	case WEAPON_HKP2000:
	case WEAPON_USP_SILENCER:
	case WEAPON_ELITE:
	case WEAPON_P250:
	case WEAPON_TEC9:
	case WEAPON_FIVESEVEN:
	case WEAPON_CZ75A:
	case WEAPON_DEAGLE:
	case WEAPON_REVOLVER: return 35;

	case WEAPON_NOVA:
	case WEAPON_XM1014:
	case WEAPON_SAWEDOFF:
	case WEAPON_MAG7:
	case WEAPON_M249:
	case WEAPON_NEGEV: return 36;

	case WEAPON_MAC10:
	case WEAPON_MP9:
	case WEAPON_MP7:
	case WEAPON_MP5SD:
	case WEAPON_UMP45:
	case WEAPON_P90:
	case WEAPON_BIZON: return 37;

	case WEAPON_GALILAR:
	case WEAPON_FAMAS:
	case WEAPON_AK47:
	case WEAPON_M4A1:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_SSG08:
	case WEAPON_SG556:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_G3SG1:
	case WEAPON_SCAR20: return 38;
	}
}

QAngle CalculateRelativeAngle(const Vector& source, const Vector& destination, QAngle view_angles) noexcept
{
	QAngle angles = Math::CalcAngle(source, destination);

	return (angles - view_angles).Normalized();
}

auto kill_delay_time = 0;
void c_legitaimbot::kill_delay(IGameEvent* event)
{
	player_info po;
	if (!ctx.m_local() || ctx.m_local()->IsDead() || csgo.m_engine()->GetPlayerInfo(ctx.m_local()->entindex(), &po))
		return;

	if (event->GetInt("attacker") != po.userid || event->GetInt("userid") == po.userid)
		return;

	auto* const active_weapon = ctx.m_local()->get_weapon();
	if (!active_weapon || !active_weapon->m_iClip1())
		return;

	if (ctx.m_local()->m_iShotsFired() > 0 && !ctx.latest_weapon_data->bFullAuto)
		return;

	auto weapon_index = getWeaponIndex(active_weapon->m_iItemDefinitionIndex());
	if (!weapon_index)
		return;

	const auto weapon_class = getWeaponClass(active_weapon->m_iItemDefinitionIndex());
	if (!ctx.m_settings.aimbot[weapon_index].enabled)
		weapon_index = weapon_class;

	if (!ctx.m_settings.aimbot[weapon_index].enabled)
		weapon_index = 0;

	kill_delay_time = csgo.m_globals()->realtime + ctx.m_settings.aimbot[weapon_index].killdelay;
}

auto pressed = false;
auto last_pressed = false;

bool goes_thru_smoke(const Vector& start, const Vector& end)
{
	typedef bool(__cdecl* GoesThroughSmoke)(Vector, Vector);

	static auto linegoesthrusmoke = Memory::Scan(sxor("client.dll"), "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0");

	if (!linegoesthrusmoke)
		return false;

	static GoesThroughSmoke goesthru_fn = (GoesThroughSmoke)(linegoesthrusmoke);

	return goesthru_fn(start, end);
}

void c_legitaimbot::run(CUserCmd* cmd)
{
	const auto is_visible = [&](C_BasePlayer* m_player, Vector start, Vector end) -> bool
	{
		if (!m_player) return false;
		CGameTrace tr;
		Ray_t ray;
		static CTraceFilter traceFilter;
		traceFilter.pSkip = ctx.m_local();

		ray.Init(start, end);

		csgo.m_engine_trace()->TraceRay(ray, 0x4600400B, &traceFilter, &tr);

		return (tr.m_pEnt == m_player || tr.fraction >= 0.99f);
	};

	if (GetAsyncKeyState(VK_LBUTTON) & 1)
	{
		if (!pressed)
		{
			pressed = true;
			last_pressed = true;
		}
	}
	else
	{
		pressed = false;
	}

	if (!ctx.m_local() || ctx.m_local()->m_flNextAttack() > csgo.m_globals()->curtime || kill_delay_time > csgo.m_globals()->realtime || ctx.m_local()->m_bIsDefusing() ||
		ctx.m_local()->m_bWaitForNoAttack())
		return;

	auto active_weapon = m_weapon();
	if (!active_weapon || !active_weapon->m_iClip1())
		return;

	if (ctx.m_local()->m_iShotsFired() > 0 && !ctx.latest_weapon_data->bFullAuto)
		return;

	auto weapon_index = getWeaponIndex(active_weapon->m_iItemDefinitionIndex());
	if (!weapon_index)
		return;

	const auto weapon_class = getWeaponClass(active_weapon->m_iItemDefinitionIndex());
	if (!ctx.m_settings.aimbot[weapon_index].enabled)
		weapon_index = weapon_class;

	if (!ctx.m_settings.aimbot[weapon_index].enabled)
		weapon_index = 0;

	if (!ctx.m_settings.aimbot[weapon_index].betweenShots && active_weapon->m_flNextPrimaryAttack() > csgo.m_globals()->curtime)
		return;

	if (!ctx.m_settings.aimbot[weapon_index].ignoreFlash && ctx.m_local()->m_flFlashMaxAlpha() > 100.f)
		return;

	if (ctx.m_settings.aimbot[weapon_index].onKey)
	{
		if (!ctx.m_settings.aimbot[weapon_index].keyMode)
		{
			if (!GetAsyncKeyState(ctx.m_settings.aimbot[weapon_index].key))
				return;
		}
		else
		{
			static auto toggle = true;
			if (GetAsyncKeyState(ctx.m_settings.aimbot[weapon_index].key) & 1)
				toggle = !toggle;
			if (!toggle)
				return;
		}
	}

	if (ctx.m_settings.aimbot[weapon_index].enabled 
		&& (cmd->buttons & IN_ATTACK 
			|| ctx.m_settings.aimbot[weapon_index].autoShot 
			|| ctx.m_settings.aimbot[weapon_index].aimlock) 
		&& active_weapon->GetInaccuracy() <= ctx.m_settings.aimbot[weapon_index].maxAimInaccuracy)
	{
		if (ctx.m_settings.aimbot[weapon_index].scopedOnly && active_weapon->IsSniper() && !ctx.m_local()->m_bIsScoped())
			return;

		auto best_fov = ctx.m_settings.aimbot[weapon_index].fov;
		Vector best_target{};
		const auto local_player_eye_position = ctx.m_local()->GetEyePosition();

		auto need_rcs = ctx.latest_weapon_data->flRecoilMagnitude < 35.0f && ctx.latest_weapon_data->flRecoveryTimeStand > ctx.latest_weapon_data->flCycleTime;

		const auto aim_punch = ctx.m_settings.aimbot[weapon_index].rcsfov >= best_fov && need_rcs ? ctx.m_local()->m_aimPunchAngle() : QAngle{};

		for (auto i = 1; i <= csgo.m_engine()->GetMaxClients(); i++)
		{
			auto entity = csgo.m_entity_list()->GetClientEntity(i);
			if (!entity 
				|| entity == ctx.m_local()
				|| entity->IsDormant() 
				|| entity->IsDead()
				|| entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum() && !ctx.m_settings.aimbot[weapon_index].friendlyFire 
				|| entity->m_bGunGameImmunity())
				continue;

			for (auto bone : { 8, 4, 3, 7, 6, 5 })
			{
				auto bone_position = entity->get_bone_pos(
					ctx.m_settings.aimbot[weapon_index].bone > 1 ? 10 - ctx.m_settings.aimbot[weapon_index].bone : bone);
				const auto angle = CalculateRelativeAngle(local_player_eye_position, bone_position,
					cmd->viewangles + aim_punch);

				const auto fov = std::hypot(angle.x, angle.y);
				if (fov > best_fov)
					continue;

				if (!ctx.m_settings.aimbot[weapon_index].ignoreSmoke 
					&& goes_thru_smoke(local_player_eye_position, bone_position))
					continue;

				auto mdmg = ctx.m_settings.aimbot[weapon_index].killshot ? entity->m_iHealth() : ctx.m_settings.aimbot[weapon_index].minDamage;

				if (!is_visible(entity, local_player_eye_position, bone_position) 
					&& (ctx.m_settings.aimbot[weapon_index].visibleOnly 
					|| feature::autowall->CanHit(local_player_eye_position, bone_position, ctx.m_local(), entity, 0) >= mdmg))
					continue;

				if (fov < best_fov)
				{
					best_fov = fov;
					best_target = bone_position;
				}
				if (ctx.m_settings.aimbot[weapon_index].bone)
					break;
			}
		}

		if (best_target.IsValid() 
			&& !best_target.IsZero())
		{
			static auto last_angles{ cmd->viewangles };
			static int last_command{};

			const auto can_use_silent = ctx.m_settings.aimbot[weapon_index].silent && best_fov <= ctx.m_settings.aimbot[weapon_index].silentfov && !(ctx.m_local()->m_iShotsFired() > 1);

			auto angle = CalculateRelativeAngle(local_player_eye_position, best_target, cmd->viewangles + aim_punch);
			auto clamped{ false };

			if (std::abs(angle.x) > 255.0f
				|| std::abs(angle.y) > 255.0f)
			{
				angle.x = Math::clamp(angle.x, -255.0f, 255.0f);
				angle.y = Math::clamp(angle.y, -255.0f, 255.0f);
				clamped = true;
			}

			if (last_command < cmd->command_number 
				&& !last_angles.IsZero() 
				&& can_use_silent)
				cmd->viewangles = Math::CalcAngle(local_player_eye_position, best_target);

			angle /= ctx.m_settings.aimbot[weapon_index].smooth;

			cmd->viewangles.y += angle.y;
			cmd->viewangles.x += angle.x;

			if (!can_use_silent)
				csgo.m_engine()->SetViewAngles(cmd->viewangles);

			if (ctx.m_settings.aimbot[weapon_index].autoScope 
				&& active_weapon->m_flNextPrimaryAttack() <= csgo.m_globals()->curtime 
				&& active_weapon->IsSniper() 
				&& !ctx.m_local()->m_bIsScoped())
				cmd->buttons |= IN_ATTACK2;

			if (ctx.m_settings.aimbot[weapon_index].autoShot 
				&& active_weapon->m_flNextPrimaryAttack() <= csgo.m_globals()->curtime 
				&& !clamped && active_weapon->GetInaccuracy() <= ctx.m_settings.aimbot[weapon_index].maxShotInaccuracy)
				cmd->buttons |= IN_ATTACK;

			if (clamped)
				cmd->buttons &= ~IN_ATTACK;

			if (clamped || ctx.m_settings.aimbot[weapon_index].smooth > 1.0f) 
				last_angles = cmd->viewangles;
			else 
				last_angles = QAngle();

			static auto max_time = .0f;
			static auto can_reset_auto_delay_time = true;

			if (ctx.m_settings.aimbot[weapon_index].autodelay && !can_use_silent && !ctx.m_local()->m_iShotsFired())
			{
				static const auto fov = .05f;

				const auto id = ctx.m_local()->m_iCrosshairID();

				if (id)
				{
					auto entity = csgo.m_entity_list()->GetClientEntity(id);
					if (entity->GetClientClass()->m_ClassID == CCSPlayer)
					{
						max_time = 0;
						can_reset_auto_delay_time = false;
					}
				}

				if (best_fov > fov && can_reset_auto_delay_time)
				{
					max_time = csgo.m_globals()->curtime + best_fov / ctx.m_settings.aimbot[weapon_index].smooth;
					can_reset_auto_delay_time = false;
				}

				if (!id && max_time > csgo.m_globals()->curtime)
				{
					cmd->buttons &= ~IN_ATTACK;
					csgo.m_engine()->SetViewAngles(cmd->viewangles);
				}
			}

			last_command = cmd->command_number;
		}
	}
}
