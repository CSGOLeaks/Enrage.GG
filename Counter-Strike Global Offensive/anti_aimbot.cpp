#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "math.hpp"
#include "displacement.hpp"
#include "anti_aimbot.hpp"
#include "prediction.hpp"
#include <algorithm>
#include <iostream>
#include "menu.hpp"
#include "movement.hpp"
#include "usercmd.hpp"
#include "autowall.hpp"
#include "resolver.hpp"
#include "rage_aimbot.hpp"

float c_antiaimbot::get_max_desync_delta(C_BasePlayer* ent) //autistic but w/e
{
	auto animstate = ent->get_animation_state();
	auto speedfraction = max(0.f, min(animstate->m_speed_as_portion_of_walk_top_speed, 1.f));
	auto speedfactor = max(0.f, min(animstate->m_speed_as_portion_of_crouch_top_speed, 1.f));

	auto lol = ((animstate->m_walk_run_transition * -0.30000001f) - 0.19999999f) * speedfraction + 1.f;

	if (animstate->m_anim_duck_amount > 0.0f)
		lol += ((animstate->m_anim_duck_amount * speedfactor) * (0.5f - lol));

	return (animstate->m_aim_yaw_max * lol);
}

bool IsVisible(Vector start, Vector end, C_BasePlayer* skip, C_BasePlayer* ent)
{
	trace_t trace;
	//util_trace_line(start, end, MASK_SHOT_HULL, filter, &trace);
	Ray_t ray;
	ray.Init(start, end);

	if (skip)
	{
		CTraceFilter filter;
		filter.pSkip = skip;
		csgo.m_engine_trace()->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);
	}
	else 
	{
		CTraceFilterWorldOnly filter;
		csgo.m_engine_trace()->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);
	}

	return trace.m_pEnt == ent;
}

void c_antiaimbot::get_targets()
{
	players.clear();

	C_BasePlayer* target = nullptr;

	QAngle original_viewangles;
	csgo.m_engine()->GetViewAngles(original_viewangles);

	float lowest_distance = 99999.f;
	for (auto i = 1; i < 64; i++)
	{
		origins[i - 1] = Vector::Zero;
		auto player = csgo.m_entity_list()->GetClientEntity(i);

		if (!player || player->IsDead() || player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || player == ctx.m_local())
			continue;

		const auto idx = player->entindex() - 1;

		const auto& curlog = &feature::lagcomp->records[idx];

		origins[i - 1] = player->m_vecAbsOrigin();
		player->GetWorldSpaceCenter(origins[i-1]);

		if (!player->IsDormant() && curlog->records_count > 0)// && abs(player->m_flSimulationTime() - TICKS_TO_TIME(csgo.m_globals()->tickcount)) > 5.f)
			curlog->last_scan_time = csgo.m_client_state()->m_clockdrift_manager.m_nServerTick;

		if (std::abs(csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - curlog->last_scan_time) >= 20)
			continue;
		
		if (IsVisible(ctx.m_local()->GetEyePosition(), origins[i - 1], ctx.m_local(), player))
			players.push_back(player);
		
		QAngle angle = Math::CalcAngle(ctx.m_local()->GetEyePosition(), origins[i - 1]);
		QAngle delta = angle - original_viewangles;
		delta.Normalize();
		delta.z = 0.f;

		float dist = sqrt(delta.x * delta.x + delta.y * delta.y);

		if (dist < lowest_distance)
		{
			target = player;
			lowest_distance = dist;
		}
	}

	if (!target || *(void**)target == nullptr)
		return;

	players.push_back(target);
}

void c_antiaimbot::run_at_target(float& yaw)
{
	auto GetFOV = [](const QAngle& view_angles, const Vector& start, const Vector& end) -> float {
		Vector dir, fw;

		// get direction and normalize.
		dir = (end - start).Normalized();

		// get the forward direction vector of the view angles.
		Math::AngleVectors(view_angles, &fw);

		// get the angle between the view angles forward directional vector and the target location.
		return max(RAD2DEG(std::acos(fw.Dot(dir))), 0.f);
	};

	if (!ctx.m_settings.anti_aim_at_target[0] && !ctx.m_settings.anti_aim_at_target[1])
		return;

	const auto mode = (ctx.m_settings.anti_aim_at_target[0] && ctx.m_settings.anti_aim_at_target[1] ? 3 : (ctx.m_settings.anti_aim_at_target[0] ? 1 : 2));

	Vector best_origin = Vector::Zero;
	float best_distance = 9999.f;
	float best_fov = 9999.f;
	int best_idx = -1;
	bool best_dormant = false;

	QAngle viewangles;
	csgo.m_engine()->GetViewAngles(viewangles);

	for (auto i = 1; i < 64; i++)
	{
		if (origins[i-1].IsZero())
			continue;

		auto player = csgo.m_entity_list()->GetClientEntity(i);

		if (!player || player->IsDead() || player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || player == ctx.m_local())
			continue;

		const auto idx = player->entindex() - 1;

		const auto& curlog = &feature::lagcomp->records[idx];

		if (abs(csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - curlog->last_scan_time) >= 20)
			continue;

		//Vector forward;
		//Math::AngleVectors(Math::CalcAngle(ctx.m_eye_position, origins[i-1]), &forward);

		const auto dist = (origins[i-1] - ctx.m_local()->GetEyePosition()).LengthSquared();
		Vector wsc = origins[i-1];
		/*const*/ auto fov = GetFOV(viewangles, ctx.m_local()->GetEyePosition(), wsc);

		switch (mode)
		{
		case 1:
			if (fov < best_fov)
			{
				best_origin = origins[i - 1];
				best_distance = dist;
				best_dormant = !player->IsDormant();
				best_fov = fov;
				best_idx = i;
			}
			break;
		case 2:
			if (dist < best_distance)
			{
				best_origin = origins[i - 1];
				best_distance = dist;
				best_dormant = !player->IsDormant();
				best_fov = fov;
				best_idx = i;
			}
			break;
		case 3:
			if (dist <= (best_distance*0.9f) && fov < best_fov)
			{
				best_origin = origins[i - 1];
				best_dormant = !player->IsDormant();
				best_distance = dist;
				best_fov = fov;
				best_idx=i;
			}
			break;
		}
	}

	if (feature::ragebot->m_target != nullptr && !feature::ragebot->m_target->IsDormant() && !feature::ragebot->m_target->IsDead() && feature::ragebot->m_target->entindex() != best_idx) {
		best_origin = feature::ragebot->m_target->m_vecOrigin() + feature::ragebot->m_target->m_vecVelocity() * csgo.m_globals()->interval_per_tick;
	}

	if (best_origin.IsZero())
		return;

	const auto angle = Math::CalcAngle(ctx.m_local()->GetEyePosition(), best_origin);
	yaw = Math::normalize_angle(angle.y);
}

void c_antiaimbot::auto_direction() {
	// constants.
	static constexpr float STEP{ 4.f };
	static constexpr float RANGE{ 32.f };

	if (!ctx.m_local() || ctx.m_local()->IsDead())
		return;

	AutoTarget_t target = AutoTarget_t{ 180.f - 1.f, nullptr };

	// iterate players.
	for (int i{ 0 }; i <= csgo.m_globals()->maxClients; ++i) {
		C_BasePlayer* player = csgo.m_entity_list()->GetClientEntity(i);

		if (!player || player->IsDead() || player->entindex() < 0 || player->entindex() > 64 || player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || player == ctx.m_local())
			continue;

		const auto idx = player->entindex() - 1;

		auto* curlog = &feature::lagcomp->records[idx];

		if (!curlog || abs(csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - curlog->last_scan_time) >= 20)
			continue;

		auto absorg = player->m_vecAbsOrigin();
		player->GetWorldSpaceCenter(absorg);

		// get best target based on fov.
		float fov = Math::GetFov(Engine::Movement::Instance()->m_qRealAngles, Math::CalcAngle(ctx.m_local()->GetEyePosition(), absorg));

		if (fov < target.fov) {
			target.fov = fov;
			target.player = player;
		}
	}

	if (!target.player || !target.player->IsPlayer()) {
		// we have a timeout.
		/*if (m_auto_last > 0.f && m_auto_time > 2.5f && csgo.m_globals()->curtime < (m_auto_last + 2.5f))
			return;*/

		// set angle to backwards.
		m_auto = Math::normalize_angle(Engine::Movement::Instance()->m_qRealAngles.y - 179.f);
		m_auto_dist = -1.f;
		return;
	}

	// construct vector of angles to test.
	std::vector<AdaptiveAngle> angles = {};
	angles.emplace_back(Engine::Movement::Instance()->m_qRealAngles.y - 179.f);
	angles.emplace_back(Engine::Movement::Instance()->m_qRealAngles.y + 90.f);
	angles.emplace_back(Engine::Movement::Instance()->m_qRealAngles.y - 90.f);

	// see if we got any valid result.
	// if this is false the path was not obstructed with anything.
	bool valid = false;

	const auto start = target.player->Weapon_ShootPosition();

	// iterate vector of angles.
	for (auto i = 0; i < (int)angles.size(); ++i) {

		auto it = &angles.at(i);

		// compute the 'rough' estimation of where our head will be.
		Vector end{ ctx.m_local()->GetEyePosition().x + std::cos(DEG2RAD(it->m_yaw)) * RANGE,
					ctx.m_local()->GetEyePosition().y + std::sin(DEG2RAD(it->m_yaw)) * RANGE,
					ctx.m_local()->GetEyePosition().z };

		// draw a line for debugging purposes.
		//g_csgo.m_debug_overlay->AddLineOverlay( start, end, 255, 0, 0, true, 0.1f );

		// compute the direction.
		auto dir = end - start;
		float len = dir.Normalize();

		// should never happen.
		if (len <= 0.f)
			continue;

		// step thru the total distance, 4 units per step.
		for (float i{ 0.f }; i < len; i += STEP) {
			// get the current step position.
			const auto point = start + (dir * i);

			// get the contents at this point.
			const int contents = csgo.m_engine_trace()->GetPointContents(point, MASK_SHOT_HULL);

			// contains nothing that can stop a bullet.
			if (!(contents & MASK_SHOT_HULL))
				continue;

			float mult = 1.f;

			// over 50% of the total length, prioritize this shit.
			if (i > (len * 0.5f))
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if (i > (len * 0.75f))
				mult = 1.25f;

			// over 90% of the total length, prioritize this shit.
			if (i > (len * 0.9f))
				mult = 2.f;

			// append 'penetrated distance'.
			it->m_dist += (STEP * mult);

			// mark that we found anything.
			valid = true;
		}
	}

	if (!valid || angles.empty()) {
		// set angle to backwards.
		m_auto = Math::normalize_angle(Engine::Movement::Instance()->m_qRealAngles.y - 179.f);
		m_auto_dist = -1.f;
		return;
	}

	// put the most distance at the front of the container.
	std::sort(angles.begin(), angles.end(),
		[](const AdaptiveAngle& a, const AdaptiveAngle& b) {
			return a.m_dist > b.m_dist;
		});

	// the best angle should be at the front now.
	AdaptiveAngle* best = &angles.front();

	// check if we are not doing a useless change.
	if (best->m_dist != m_auto_dist) {
		// set yaw to the best result.
		m_auto = Math::normalize_angle(best->m_yaw);
		m_auto_dist = best->m_dist;
		m_auto_last = csgo.m_globals()->curtime;
	}
}

void c_antiaimbot::change_angles(CUserCmd* cmd, bool* send_packet)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

	auto animstate = ctx.m_local()->get_animation_state();

	get_targets();

	const auto at_target = ctx.m_settings.anti_aim_at_target && (ctx.m_settings.anti_aim_at_target[2] && ctx.side == -1 || !ctx.m_settings.anti_aim_at_target[2]);

	if (at_target)
		run_at_target(cmd->viewangles.y);

	auto_direction();

	max_delta = get_max_desync_delta(ctx.m_local());

	auto lol = RandInt(0, 1);

	if (ctx.m_settings.anti_aim_typex == 1)
		cmd->viewangles.x = (lol == 1 ? 179.0f : 178.975261f);
	else if (ctx.m_settings.anti_aim_typex == 2)
		cmd->viewangles.x = (lol == 1 ? -179.0f : -178.975261f);
	else if (ctx.m_settings.anti_aim_typex == 3)
		cmd->viewangles.x = 0.0f;

	//int aa_mode			= is_moving ? cheat::Cvars.anti_aim_m_yaw.GetValue()		 : cheat::Cvars.anti_aim_s_yaw.GetValue();
	int aa_switch			= /*is_moving ? ctx.m_settings.anti_aim_jittering_type : */ctx.m_settings.anti_aim_jittering_type; // you know
	int aa_switchspeed		= /*is_moving ? ctx.m_settings.anti_aim_jittering_speed : */ctx.m_settings.anti_aim_jittering_speed;
	int aa_switchang		= /*is_moving ? ctx.m_settings.anti_aim_jittering : */ctx.m_settings.anti_aim_jittering;

	//float left_max_limit = ctx.m_settings.anti_aim_fake_left_limit;
	float f_max_limit = ctx.m_settings.anti_aim_fake_limit;

	int aa_type = ctx.m_settings.anti_aim_typey;
	int aa_ftype = ctx.m_settings.anti_aim_typeyfake;

	if (ctx.side != -1)
	{
		if (!ctx.side)
			cmd->viewangles.y += 90.f;
		else if (ctx.side == 1)
			cmd->viewangles.y -= 90.f;
		else if (ctx.side == 2)
			cmd->viewangles.y += 178.f;
	}
	else {
		if (aa_type == 1)
			cmd->viewangles.y += 178.f;
		else if (aa_type == 2) // zero
			cmd->viewangles += 0;
		else if (aa_type == 3)
			cmd->viewangles.y = ctx.m_local()->m_flLowerBodyYawTarget() + 90.f;
		else if (aa_type == 4)
			cmd->viewangles.y += Math::normalize_angle(csgo.m_globals()->curtime * 250.f);
	}

	const auto prev_side = ctx.fside;

	//static auto yaw = 0.f;

	if (ctx.m_settings.anti_aim_freestanding_fake_type > 0 && *send_packet) {
		if (m_auto_dist != -1) {
			ctx.fside = Math::AngleDiff(Engine::Movement::Instance()->m_qRealAngles.y, m_auto) < 0 ? -1 : 1;
			if (ctx.m_settings.anti_aim_freestanding_fake_type == 1)
				ctx.fside *= -1;
			if (ctx.m_settings.anti_aim_typeyfake != 2)
				ctx.fside *= -1;
		}
		//cmd->viewangles.y = yaw;
	}

	if (ctx.allow_freestanding) {
		if ((m_auto_dist != -1 && at_target) || !at_target)
			cmd->viewangles.y = m_auto;
	}

	//static float old_yaw = 0.f;

	const auto v81 = !ctx.m_local()->m_bIsScoped();
	float v90;

	if (v81)
		v90 = ctx.latest_weapon_data->max_speed * 0.25f;
	else
		v90 = ctx.latest_weapon_data->max_speed_alt * 0.25f;

	static float previous_hp = ctx.m_local()->m_iHealth();

	//static int old_shots = 0;
	const auto can_switch_side = *send_packet && ((ctx.m_settings.anti_aim_automatic_side[2] && ctx.m_local()->m_vecVelocity().Length2D() > v90)
		|| ctx.m_settings.anti_aim_automatic_side[1] && (ctx.exploit_allowed && ctx.has_exploit_toggled && ctx.main_exploit != 0));

	previous_hp = ctx.m_local()->m_iHealth();
	
	//if (old_shots != ctx.m_local()->m_iShotsFired())
	//	old_shots = ctx.m_local()->m_iShotsFired();

	//static int old_side = ctx.fside;
	//static bool did_twist = false;
	static bool force_choke = false;

	//auto curtime = csgo.m_globals()->curtime;

	const auto move_speed = sqrtf((cmd->forwardmove * cmd->forwardmove) + (cmd->sidemove * cmd->sidemove));
	const auto vel_speed = sqrtf((ctx.m_local()->m_vecVelocity().y * ctx.m_local()->m_vecVelocity().y) + (ctx.m_local()->m_vecVelocity().x * ctx.m_local()->m_vecVelocity().x));

	//if (ctx.m_settings.anti_aim_typelby == 2) {
	//	//if (ctx.m_settings.aimbot_position_adjustment_old) {
	//	//	//if (updated_lby())
	//	//	//{
	//	//
	//	//	if (((m_next_lby_update_time - curtime) <= 0.02f || m_will_lby_update) && !send_packet)
	//	//	{
	//	//		switch (ctx.m_settings.anti_aim_typelby)
	//	//		{
	//	//		case 1:
	//	//			if (m_will_lby_update)
	//	//				cmd->viewangles.y = ctx.m_local()->m_angEyeAngles().y + 180.f;
	//	//			break;
	//	//		case 2:
	//	//			if (!m_will_lby_update)
	//	//				cmd->viewangles.y = ctx.m_local()->m_angEyeAngles().y + Math::normalize_angle(180.f * (ctx.fside == 0 ? 1.f : -1.f));
	//	//			//else
	//	//			//	cmd->viewangles.y += Math::normalize_angle(120.f * (ctx.fside == 0 ? -1.f : 1.f));
	//	//			break;
	//	//		}
	//	//
	//	//		/*switch (ctx.m_settings.anti_aim_typelby)
	//	//		{
	//	//		case 1:
	//	//			cmd->viewangles.y += 180.f;
	//	//			break;
	//	//		case 2:
	//	//			cmd->viewangles.y += ctx.fside > 0 ? 120.f : -120.f;
	//	//			break;
	//	//		}*/
	//	//		cmd->viewangles.y = Math::normalize_angle(cmd->viewangles.y);
	//	//		cmd->viewangles.Clamp();
	//	//		force_choke = true;
	//	//		send_packet = false;
	//	//
	//	//		return;
	//	//	}
	//	//}
	//	//else {
	//		if (csgo.m_client_state()->m_iChockedCommands == 0 && ctx.m_local()->m_vecVelocity().Length2D() < 6.f)
	//		{
	//			cmd->viewangles.y = Math::normalize_angle(ctx.m_local()->m_angEyeAngles().y + (180.f * ctx.fside)/*+ (ctx.fside ? 180.f : -180.f)*/);
	//			*send_packet = false;

	//			lby_expected = cmd->viewangles.y;
	//			cmd->viewangles.Clamp();
	//			force_choke = true;

	//			if (!feature::anti_aim->skip_fakelag_this_tick) {
	//				if (cmd->command_number & 1)
	//					cmd->forwardmove -= (ctx.m_local()->m_vecViewOffset().z < 64.f ? 4.941177f : 1.01f);
	//				else
	//					cmd->forwardmove += (ctx.m_local()->m_vecViewOffset().z < 64.f ? 4.941177f : 1.01f);
	//			}

	//			return;
	//		}
	//	//}
	//}
	/*else 
	{
		if (TICKS_TO_TIME(ctx.m_local()->m_nTickBase()) > feature::anti_aim->lby_timer && csgo.m_client_state()->m_iChockedCommands < 14 && ctx.m_settings.anti_aim_typelby == 0)
		{
			cmd->viewangles.y = Math::normalize_angle(ctx.m_local()->m_angEyeAngles().y - (180.f - ctx.fside));
			send_packet = false;
			force_choke = true;

			cmd->viewangles.Clamp();

			return;
		}
	}*/

	if (force_choke)
	{
		*send_packet = false;
		force_choke = false;
	}

	if (can_switch_side)
		ctx.fside *= -1;

	const auto new_max_delta = max_delta * (f_max_limit / 60.f);
	//if (ctx.changed_fake_side)
	//	old_side *= -1;

	//if (is_moving && ctx.m_settings.anti_aim_automatic_side[1]) {
	//	if (abs(Math::AngleDiff(ctx.angles[ANGLE_FAKE], ctx.angles[ANGLE_REAL])) <= 45.f/*just a quick test value*/&& ctx.m_settings.anti_aim_typeyfake == 2 && !ctx.start_switching && prev_side == ctx.fside) {
	//		if (send_packet)
	//			ctx.fside *= /*(cmd->command_number % 2 == 0 ? 1 : */-1/*)*/;

	//		did_twist = true;
	//	}
	//	else {
	//		if (!did_twist) {
	//			old_side = ctx.fside;
	//		}
	//		else
	//		{
	//			ctx.fside = old_side;
	//			did_twist = false;
	//		}
	//	}
	//}

	//if (ctx.m_settings.anti_aim_automatic_side[4] && ctx.m_local()->m_fFlags() & FL_ONGROUND)
	//{
	//	if (speed < 0.1f)
	//	{
	//		auto delta = Math::AngleDiff(ctx.m_local()->m_angEyeAngles().y, ctx.angles[ANGLE_FAKE]);

	//		if (ctx.m_local()->get_animation_layer(3).m_flWeight == 0.0f && ctx.m_local()->get_animation_layer(6).m_flWeight <= 0.01f && ctx.m_local()->get_animation_layer(3).m_flCycle == 0.0f)
	//			ctx.fside = (delta <= 0.f ? 1 : -1);
	//	}
	//	else if (int(ctx.m_local()->get_animation_layer(12).m_flWeight * 1000.f) == 0)
	//	{
	//		//2 = -1; 3 = 1; 1 = fake;
	//		if (int(ctx.m_local()->get_animation_layer(6).m_flWeight * 1000.f) >= int(ctx.m_local()->get_animation_layer(6).m_flWeight * 1000.f))
	//		{
	//			float delta1 = abs(ctx.m_local()->get_animation_layer(6).m_flPlaybackRate - ctx.local_layers[2][6].m_flPlaybackRate);
	//			float delta2 = abs(ctx.m_local()->get_animation_layer(6).m_flPlaybackRate - ctx.local_layers[0][6].m_flPlaybackRate);
	//			float delta3 = abs(ctx.m_local()->get_animation_layer(6).m_flPlaybackRate - ctx.local_layers[1][6].m_flPlaybackRate);

	//			if (delta1 < delta3 || delta2 <= delta3 || int(delta3 * 1000.0f) != 0) {
	//				if (delta1 >= delta2 && delta3 > delta2 && int(delta2 * 1000.0f) == 0)
	//					ctx.fside = -1;
	//			}
	//			else
	//				ctx.fside = 1;
	//		}
	//	}
	//}
	
	RandomSeed(cmd->command_number & 255);

	/*static float last_random_body_lean_mult = 0.f;
	static float last_random_body_lean_time = 0.f;

	if (ctx.m_settings.anti_aim_fake_jittering) {
		if (fabs(last_random_body_lean_time - csgo.m_globals()->realtime) > (0.5f - (ctx.m_settings.anti_aim_fake_jittering_speed * 0.05f))) {
			last_random_body_lean_mult = RandomFloat(ctx.m_settings.anti_aim_fake_jittering_min, ctx.m_settings.anti_aim_fake_jittering_max) * 0.01f;
			last_random_body_lean_time = csgo.m_globals()->realtime;
		}

		max_delta *= last_random_body_lean_mult;
	}*/

	/*if (ctx.m_settings.anti_aim_fake_jittering && ctx.m_settings.anti_aim_typeyfake == 2)
	{
		if (ctx.m_local()->m_vecVelocity().Length2D() < 40) {
			if (ctx.changed_fake_side)
				old_side *= -1;

			ctx.fside *= -1;
			did_twist = true;
		}
		else
		{
			if (!did_twist) {
				old_side = ctx.fside;
			}
			else
			{
				ctx.fside = old_side;
				did_twist = false;
			}
		}
	}*/

	if (aa_switch > 0 && (*send_packet))
	{
		static float last_switch_ang = 0.f;
		static float last_switch_time = 0.f;
		static bool lmao = false;

		if (aa_switch == 1)
		{
			//cmd->viewangles.y += last_switch_ang < 0.f ? (abs(aa_switchang) / 2.f) : -(abs(aa_switchang) / 2.f);

			if (/*fabs(last_switch_time - csgo.m_globals()->realtime) > (1.f - (aa_switchspeed * 0.01f)) && */*send_packet) {
				auto lol = abs(aa_switchang);
				lmao = !lmao;
				//RandomSeed(cmd->command_number & 255);
				last_switch_ang = (lmao ? (-lol / 2) : (lol / 2));
				last_switch_time = csgo.m_globals()->realtime;
			}

		}
		else if (aa_switch == 2)
		{
			cmd->viewangles.y -= (fabs(aa_switchang) / 2.f);

			if (fabs(last_switch_time - csgo.m_globals()->realtime) > 0.0015f && *send_packet)
			{
				if (fabs(last_switch_ang) < fabs(aa_switchang))
					last_switch_ang += copysign(aa_switchspeed * 0.1f, aa_switchang);
				else
					last_switch_ang = 0.f;

				last_switch_time = csgo.m_globals()->realtime;
			}
		}

		cmd->viewangles.y = Math::normalize_angle(Math::normalize_angle(cmd->viewangles.y) + last_switch_ang);
	}

	if (aa_ftype == 3)
	{
		auto should_switch = 
			ctx.m_local()->m_vecVelocity().Length2D() > 2 && 
			(!(int(ctx.m_local()->animation_layer(12).m_flWeight * 1000.f) || ctx.m_local()->animation_layer(7).m_flWeight == 1.f)
			|| abs(ctx.m_local()->m_vecVelocity().Length2D() - Engine::Prediction::Instance()->GetVelocity().Length2D()) <= 5.f);

		if (*send_packet && should_switch)
			ctx.fside *= -1;
	}


	static auto is_inverted = false;

	if (fabs(ctx.fside) != 1 && csgo.m_client_state()->m_iChockedCommands == 1)
	{
		is_inverted = (ctx.fside > 0);
		ctx.fside /= 2;
	}
	else
		is_inverted = (ctx.fside > 0);

	cmd->viewangles.y = Math::normalize_angle(cmd->viewangles.y);

	last_real_angle = cmd->viewangles;

	//const auto dsy = ctx.m_settings.anti_aim_fake_static_real ? send_packet : !send_packet;

	auto inv_body_lean = new_max_delta;
	auto view_yaw = last_real_angle.y;
	float leaned_yaw = 0.f;
	float abs_lean = 0.f;
	float j = 0.f;
	float k = 0.f;

	float v8, chto_blyad, i;
	bool v12;

	if (aa_ftype == 1)
	{
		if (!ctx.m_settings.aimbot_position_adjustment_old)
		{
			if (is_inverted)
				last_real_angle.y -= new_max_delta / 2.f;
			else
				last_real_angle.y += new_max_delta / 2.f;
		}

		//if (ctx.m_settings.aimbot_position_adjustment_old)
		//	last_real_angle.y = Math::normalize_angle(last_real_angle.y - 180.f);

		if (*send_packet)// || !send_packet && ctx.m_settings.aimbot_position_adjustment_old)
		{
			if (is_inverted)
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y + new_max_delta);
			else
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y - new_max_delta);

			//if (ctx.m_settings.aimbot_position_adjustment_old)
			//	cmd->viewangles.y = Math::normalize_angle(last_real_angle.y - 180.f + RandomFloat(-30.f, 30.f));
		}
		else //if (!ctx.m_settings.aimbot_position_adjustment_old)
		{
			if (is_inverted)
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y - new_max_delta);
			else
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y + new_max_delta);
		}
	}
	else if (aa_ftype == 2)
	{
		static bool invert_jitter = false;

		if (invert_jitter) {
			cmd->viewangles.y = last_real_angle.y + 180.0f;
			last_real_angle.y = Math::normalize_angle(cmd->viewangles.y);
		}

		if (*send_packet) {
			invert_jitter = !invert_jitter;

			if (!invert_jitter)
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y + (new_max_delta * 0.5f) * ctx.fside);
			else
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y - (new_max_delta * 0.5f) * ctx.fside);
		}
		else /*if (!flick_lby)*/ {
			if (invert_jitter)
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y - (new_max_delta + 5.0f) * ctx.fside);
			else
				cmd->viewangles.y = Math::normalize_angle(last_real_angle.y + (new_max_delta + 5.0f) * ctx.fside);
		}
		/*else {
			if (invert_jitter)
				cmd->viewangles.y = (last_real_angle.y + 90.0f * ctx.fside);
			else
				cmd->viewangles.y = (last_real_angle.y - 90.0f * ctx.fside);
		}*/


	}
	else if (aa_ftype == 3)
	{

		if (!is_inverted)
			inv_body_lean *= -1.f;

		// current abs rotation
		for (leaned_yaw = inv_body_lean + last_real_angle.y; leaned_yaw > 180.0; leaned_yaw = leaned_yaw - 360.0)
			;
		for (; leaned_yaw < -180.0; leaned_yaw = leaned_yaw + 360.0)
			;
		v8 = ctx.angles[ANGLE_REAL];
		if (v8 > 180.0)
		{
			do
				v8 = v8 - 360.0;
			while (v8 > 180.0);
		}
		for (; v8 < -180.0; v8 = v8 + 360.0)
			;
		for (abs_lean = v8 - leaned_yaw; abs_lean > 180.0; abs_lean = abs_lean - 360.0)
			;
		for (; abs_lean < -180.0; abs_lean = abs_lean + 360.0)
			;
		chto_blyad = *(float*)(uintptr_t(csgo.m_globals()) + 0x20) * 102.0f;
		// max body yaw * yaw modifier
		if (abs(abs_lean) <= chto_blyad && new_max_delta < max_delta)
			goto LABEL_57;
		if (abs(abs_lean) <= (180.0f - (max_delta + chto_blyad)))
		{
			if (abs_lean <= 0.0)
				view_yaw = max_delta + leaned_yaw;
			else
				view_yaw = leaned_yaw - max_delta;
		}
		else
		{
			view_yaw = leaned_yaw;
		}
		v12 = 1;
		i = ctx.angles[ANGLE_REAL];
		if (new_max_delta >= max_delta)
		{
			for (j = last_real_angle.y; j > 180.0; j = j - 360.0)
				;
			for (; j < -180.0; j = j + 360.0)
				;
			for (; i > 180.0; i = i - 360.0)
				;
			for (; i < -180.0; i = i + 360.0)
				;
			for (k = i - j; k > 180.0; k = k - 360.0)
				;
			for (; k < -180.0; k = k + 360.0)
				;
			if (is_inverted)
			{
				if (k < new_max_delta || k >= 179.0)
					goto LABEL_46;
			}
			else if ((new_max_delta * -1.f) < k || k <= -179.0f)
			{
				goto LABEL_50;
			}
			v12 = 0;
			if (*send_packet)
				goto LABEL_59;
		LABEL_46:
			if (is_inverted)
			{
				if (k > 0.0 && k < 179.0)
					view_yaw = last_real_angle.y + 120.0f;
				goto LABEL_53;
			}
		LABEL_50:
			if (k < 0.0 && k > -179.0)
				view_yaw = last_real_angle.y - 120.0f;
		LABEL_53:
			if (!v12)
				goto LABEL_57;
		}
	LABEL_57:
		if (!*send_packet && csgo.m_client_state()->m_iChockedCommands < 14u)
		{
			//view_yaw = last_real_angle.y;
			goto LABEL_63;
		}
	LABEL_59:
		view_yaw = last_real_angle.y;
	LABEL_63:

		cmd->viewangles.y = Math::normalize_angle(view_yaw);
	}

	//if (ctx.m_settings.security_safety_mode != 0)
		cmd->viewangles.Clamp();

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}

bool c_antiaimbot::peek_fake_lag(CUserCmd* cmd, bool* send_packet)
{
	auto choked = 15;

	if (!m_weapon())
		return false;

	const auto weapon_info = ctx.latest_weapon_data;

	if (!weapon_info)
		return false;

	auto simulated_origin = ctx.m_local()->m_vecOrigin();
	auto move_per_tick = ctx.m_local()->m_vecVelocity() * csgo.m_globals()->interval_per_tick;

	int when_started_to_penetrate = 0;

	Vector vDuckHullMin = csgo.m_movement()->GetPlayerMins(true);
	Vector vStandHullMin = csgo.m_movement()->GetPlayerMins(false);

	float fMore = (vDuckHullMin.z - vStandHullMin.z);

	Vector vecDuckViewOffset = csgo.m_movement()->GetPlayerViewOffset(true);
	Vector vecStandViewOffset = csgo.m_movement()->GetPlayerViewOffset(false);
	float duckFraction = min(1.f, ctx.m_local()->m_flDuckAmount() + 0.06f);

	float tempz = ((vecDuckViewOffset.z - fMore) * duckFraction) +
		(vecStandViewOffset.z * (1 - duckFraction));

	/*
	csgo.m_engine()->GetViewAngles(ang);
	*/

	Vector direction;
	Math::AngleVectors(Engine::Movement::Instance()->m_qRealAngles, &direction);
	auto max_range = weapon_info->range * 2;
	auto dmg = (float)weapon_info->damage;
	CTraceFilter filter;
	filter.pSkip = ctx.m_local();
	CGameTrace enterTrace;


	for (int i = 0; i < choked; i++) {
		simulated_origin += move_per_tick;

		Vector start = simulated_origin + Vector(0,0, tempz);
		Vector end = start + (direction * max_range);
		auto currentDistance = 0.f;

		feature::autowall->TraceLine(start, end, MASK_SHOT | CONTENTS_GRATE, ctx.m_local(), &enterTrace);

		if (enterTrace.fraction == 1.0f)
			dmg = 0.f;
		else
			//calculate the damage based on the distance the bullet traveled.
			currentDistance += enterTrace.fraction * max_range;

		//Let's make our damage drops off the further away the bullet is.
		dmg *= pow(weapon_info->range_modifier, (currentDistance / 500.f));

		auto enterSurfaceData = csgo.m_phys_props()->GetSurfaceData(enterTrace.surface.surfaceProps);
		float enterSurfPenetrationModifier = enterSurfaceData->game.penetrationmodifier;

		if (currentDistance > 3000.0 && weapon_info->penetration > 0.f || enterSurfPenetrationModifier < 0.1f)
			dmg = -1.f;

		if (enterTrace.m_pEnt != nullptr)
		{
			//This looks gay as fuck if we put it into 1 long line of code.
			bool canDoDamage = (enterTrace.hitgroup - 1) <= 7;
			bool isPlayer = (enterTrace.m_pEnt->GetClientClass() && enterTrace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CCSPlayer);
			//bool isEnemy = (ctx.m_local()->m_iTeamNum() != ((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum());
			bool onTeam = (((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum() == 2 || ((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum() == 3);

			//TODO: Team check config
			if (canDoDamage && isPlayer && onTeam)
				feature::autowall->ScaleDamage(enterTrace, weapon_info, dmg);

			if (!canDoDamage && isPlayer)
				dmg = -1.f;
		}

		auto penetrate_count = 4;

		ctx.force_low_quality_autowalling = true;
		if (!feature::autowall->HandleBulletPenetration(ctx.m_local(), weapon_info, enterTrace, start, direction, penetrate_count, dmg, weapon_info->penetration, 0.f, true))
			dmg = -1.f;
		ctx.force_low_quality_autowalling = false;

		if (penetrate_count <= 0)
			dmg = -1.f;

		auto can_penetrate = dmg > 0.f;

		if (can_penetrate)
			when_started_to_penetrate = i;

		//csgo.m_debug_overlay()->AddBoxOverlay(start, Vector(-3, -3, -3), Vector(3, 3, 3), Vector(0, 0, 0), can_penetrate ? 255 : 0, !can_penetrate ? 255 : 0, 0, 255, csgo.m_globals()->interval_per_tick * 2.f);

		/*auto distance = feature::anti_aim->sent_data.m_vecOrigin.DistanceSquared(simulated_origin);
		if (distance > 4096.0f)
			return true;*/
	}

	if (when_started_to_penetrate == 4) {
		*send_packet = true;
		return true;
	}
	else {
		*send_packet = false;
		return true;
	}
}

void c_antiaimbot::fake_lag(CUserCmd* cmd, bool* send_packet)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
	if (!cmd || cmd == nullptr || ctx.m_local() == nullptr)
		return;

	unchocking = false;

	bool lag = false; static float lag_timer = 0.f; static float old_delta = 0.f;

	bool force_nade_choke = false;

	static bool is_throwing = false;
	if (m_weapon() != nullptr && ctx.latest_weapon_data != nullptr && m_weapon()->IsGrenade() && m_weapon()->IsBeingThrowed())
	{
		if (!(cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2) && throw_nade || is_throwing) {
			is_throwing = true;
			enable_delay = csgo.m_globals()->realtime + csgo.m_globals()->interval_per_tick;
		}
		else
		{
			//maximal_choke = max(maximal_choke, 10);
			force_nade_choke = true;
			lag = true;

			throw_nade = true;
			is_throwing = false;
		}
	}
	else {
		throw_nade = false;
		is_throwing = false;
	}

	if (ctx.fakeducking/* || skip_fakelag_this_tick*/)
		return;

	const auto chocked_ticks = csgo.m_client_state()->m_iChockedCommands;

	const auto new_velo = (animation_speed > 7.f ? animation_speed : 0.f);

	RandomSeed(cmd->random_seed & 255);

	if (ctx.m_settings.anti_aim_enabled || !ctx.m_settings.fake_lag_enabled) {
		lag = (cmd->command_number % 2) == 0;
	}

	const auto origin_delta = sent_data.m_vecOrigin.IsZero() ? 0.f : sent_data.m_vecOrigin.Distance(ctx.m_local()->m_vecOrigin());
	auto choke_value = (int)ctx.m_settings.fake_lag_value;

	auto v6 = min(1, ctx.m_settings.fake_lag_variance) * 3;

	if (v6 > 0 && csgo.m_client_state()->m_iChockedCommands != 15)
		choke_value -= RandomInt(0, (v6 / 10));// v6 * (2 * (csgo.m_globals()->tickcount & 1) - 1);

	//if (ctx.m_settings.fake_lag_type == 0 && choke_value > 10 && RandomInt(0, 1) == 1)
	//	choke_value -= RandomInt(1, 3);

	static bool byte_3CF29400 = false;
	static int tick;
	static bool something;

	static int dword_3CF29404 = 0;
	auto v54 = cmd->command_number % 31;
	auto v55 = 8 * (cmd->command_number % 31);


	auto units_per_second = ctx.m_local()->m_vecVelocity().Length2D() * csgo.m_globals()->interval_per_tick;

	if (!ctx.m_settings.fake_lag_type)
		lag = Math::clamp(ceil(64.0f / units_per_second), 1, 16) < choke_value;
	else if (ctx.m_settings.fake_lag_type == 1)
		lag = (chocked_ticks <= choke_value); //ye kind of randomizer
	else if (ctx.m_settings.fake_lag_type == 2)
	{
		if ((cmd->command_number % max(1, ctx.m_settings.fake_lag_value)) > (choke_value / 2)) //ye kind of randomizer
			lag = (chocked_ticks < (choke_value / 2));
		else
			lag = (chocked_ticks < choke_value);
	}

	static bool ducked = false;
	const auto currently_ducked = cmd->buttons & IN_DUCK && !ctx.fakeducking;
	bool is_onground = ctx.m_local()->m_fFlags() & FL_ONGROUND;
	//static float prev_playback = ctx.m_local()->get_animation_layer(6).m_flPlaybackRate;

	if (ctx.m_settings.fake_lag_special)
	{
		const auto pre_prediction_speed = Engine::Prediction::Instance()->GetVelocity().Length2D();
		const auto post_prediction_speed = ctx.m_local()->m_vecVelocity().Length2D();

		if (is_onground && pre_prediction_speed > post_prediction_speed && post_prediction_speed > 2.f
			|| is_onground && ctx.m_local()->get_animation_layers_count() > 6 && max(pre_prediction_speed, post_prediction_speed) > 5 && ctx.m_local()->get_animation_layer(6).m_flPlaybackRate < 0.1f
			|| ctx.can_aimbot && !m_weapon()->can_shoot()
			|| (cmd->buttons & IN_JUMP || !(Engine::Prediction::Instance()->GetFlags() & FL_ONGROUND && ctx.m_local()->m_fFlags() & FL_ONGROUND))
			/*|| currently_ducked != ducked && !currently_ducked && ctx.m_local()->m_flDuckAmount() > 0.f*/)
			lag_timer = csgo.m_globals()->realtime + TICKS_TO_TIME(17);

		//const auto new_delta = max_delta;

		//if (Engine::Prediction::Instance()->GetVelocity().Length() > feature::anti_aim->animation_speed && Engine::Prediction::Instance()->GetVelocity().Length() > 6.f)
		//	lag_timer = csgo.m_globals()->realtime + 0.5f;
		//prev_playback = ctx.m_local()->get_animation_layer(6).m_flPlaybackRate;
		//old_delta = new_delta;
	}

	//if (ctx.m_settings.fake_lag_between_shots && !m_weapon()->can_shoot())
	//	lag_timer = csgo.m_globals()->realtime + 0.5f;

	auto maximal_choke = ctx.m_local()->m_vecVelocity().Length() < 5 ? 1 : max(1, choke_value);

	if (lag_timer >= csgo.m_globals()->realtime) {
		lag = true;
		maximal_choke = max(choke_value, max(1, ctx.m_settings.fake_lag_value));
	}

	if (ctx.m_settings.fake_lag_peek && ctx.m_local()->m_fFlags() & FL_ONGROUND && lag_timer <= csgo.m_globals()->realtime && m_weapon() && ctx.m_local()->m_vecVelocity().Length2D() >= Engine::Prediction::Instance()->GetVelocity().Length2D() && peek_fake_lag(cmd, send_packet))
	{
		if (*send_packet)
		{
			lag_timer = csgo.m_globals()->realtime + TICKS_TO_TIME(17);
			lag = !*send_packet;
		}
		maximal_choke = 14;
	}

	if (csgo.m_engine()->IsVoiceRecording()/* || cheat::features::music.m_playing*/)
		maximal_choke = min(choke_value, 3);

	if (ctx.has_exploit_toggled && ctx.main_exploit > 0 && ctx.exploit_allowed && ctx.ticks_allowed > 4 || ctx.allow_shooting > cmd->command_number
		|| enable_delay > csgo.m_globals()->realtime)
		maximal_choke = min(choke_value, 1);

	//auto v32 = csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - ctx.m_local()->entindex() % csgo.m_globals()->nTimestampRandomizeWindow;
	//auto some_tick = v32 - v32 % csgo.m_globals()->nTimestampNetworkingBase;

	int entityMod = ctx.m_local()->entindex() % 32;
	//int nBaseTick = 100 * (((ctx.current_tickcount + 8) - nEntityMod) / 100);

	//if (TIME_TO_TICKS(ctx.m_local()->m_flOldSimulationTime()) < some_tick
	//	&& TIME_TO_TICKS(ctx.m_local()->m_flSimulationTime()) == some_tick)
	//{
	//	//lag_timer = csgo.m_globals()->realtime + TICKS_TO_TIME(17);

	//	//if (csgo.m_client_state()->m_iChockedCommands <= min(16, choke_value))
	//		maximal_choke = min(maximal_choke, 1);
	//}
	//if ((cmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK)) != (Engine::Prediction::Instance()->prev_buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK)));
	//	maximal_choke = min(choke_value, 1);

	maximal_choke = max(maximal_choke, 1);

	ctx.accurate_max_previous_chocked_amt = (!lag ? csgo.m_client_state()->m_iChockedCommands : maximal_choke);

	if (chocked_ticks > maximal_choke) {
		*send_packet = true;
		last_chocked_amount = chocked_ticks;
	}
	else
		*send_packet = !lag;

	if (ctx.m_settings.fake_lag_lag_compensation) {
		auto tick_count = ctx.current_tickcount + 8 - entityMod;

		auto nBaseTick = 100 * (tick_count / 100);
		if (nBaseTick <= 100 * ((tick_count - 1) / 100))
		{
			auto v26 = (cmd->tick_count) - entityMod;
			if (100 * ((v26 + 1) / 100) <= 100 * (v26 / 100))
			{
				if (100
					* (((ctx.current_tickcount + 8)
						- csgo.m_client_state()->m_iChockedCommands
						- entityMod
						+ 4)
						/ 100) <= nBaseTick)
				{
					//
				}
				else
					*send_packet = 0;
			}
			else
				*send_packet = 1;
		}
	}

	unchocking = chocked_ticks == (maximal_choke - 1);

	//if (m_weapon() && last_unchoke_time <= m_weapon()->m_flLastShotTime() && (cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2) && m_weapon()->can_shoot())
	//	send_packet = true;
	//if (force_nade_choke)

	/*if (m_weapon() && ctx.m_settings.aimbot_psilent_aim) {
		auto ticks = TIME_TO_TICKS(csgo.m_globals()->curtime - m_weapon()->m_flLastShotTime());
		
		if (ticks > 0 && ticks < 3) {
			send_packet = true;
			unchocking = true;
		}
	}*/

	if (currently_ducked != ducked && !currently_ducked && ctx.m_local()->m_flDuckAmount() <= 0.2f || currently_ducked)
		ducked = cmd->buttons & IN_DUCK && !ctx.fakeducking;

	////if (ctx.m_settings.aimbot_nasa_psilent && !ctx.fakeducking && (!ctx.m_settings.aimbot_tickbase_exploit || !ctx.exploit_allowed))
	////{
	///*	if (ctx.send_next_tick & PACKET_CHOKE) {
	//		send_packet = false;
	//		ctx.send_next_tick = PACKET_SEND;
	//	}
	//	else */if (ctx.send_next_tick & PACKET_SEND)
	//	{
	//		send_packet = true;
	//		ctx.send_next_tick = PACKET_NONE;
	//		unchocking = true;
	//	}
	//}

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}

void c_antiaimbot::fake_lagv2(CUserCmd* cmd, bool* send_packet)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
	if (!cmd || cmd == nullptr || ctx.m_local() == nullptr)
		return;

	unchocking = false;

	bool lag = false; static float lag_timer = 0.f; static float old_delta = 0.f;

	bool force_nade_choke = false;

	static bool is_throwing = false;
	if (m_weapon() != nullptr && ctx.latest_weapon_data != nullptr && m_weapon()->IsGrenade() && m_weapon()->IsBeingThrowed())
	{
		if (!(cmd->buttons & IN_ATTACK || cmd->buttons & IN_ATTACK2) && throw_nade || is_throwing) {
			is_throwing = true;
			enable_delay = csgo.m_globals()->realtime + csgo.m_globals()->interval_per_tick;
		}
		else
		{
			//maximal_choke = max(maximal_choke, 10);
			force_nade_choke = true;
			lag = true;

			throw_nade = true;
			is_throwing = false;
		}
	}
	else {
		throw_nade = false;
		is_throwing = false;
	}

	if (ctx.fakeducking)
		return;

	const auto chocked_ticks = csgo.m_client_state()->m_iChockedCommands;
	const auto new_velo = (animation_speed > 2.f ? animation_speed : 0.f);

	RandomSeed(cmd->random_seed & 255);

	//const auto origin_delta = sent_data.m_vecOrigin.IsZero() ? 0.f : sent_data.m_vecOrigin.Distance(ctx.m_local()->m_vecOrigin());
	auto choke_value = (int)ctx.m_settings.fake_lag_value;

	auto v6 = RandomInt(0, choke_value / 4);

	if (choke_value > 2 && chocked_ticks != 15)
		choke_value -= v6;// v6 * (2 * (csgo.m_globals()->tickcount & 1) - 1);

	if (new_velo < 2.f)
		choke_value = 2;

	auto v7 = 0;
	auto v8 = 2;
	auto v9 = new_velo * csgo.m_globals()->interval_per_tick;

	switch (ctx.m_settings.fake_lag_type)
	{
	case 0:
		if (chocked_ticks < choke_value)
			lag = 1;
		else
			lag = false;
		break;
	case 1:
		while (float(v7 * v9) <= 68.0f)
		{
			if (float((v8 - 1) * v9) > 68.0f)
			{
				++v7;
				break;
			}
			if (float(v8 * v9) > 68.0f)
			{
				v7 += 2;
				break;
			}
			if (float(float(v8 + 1) * v9) > 68.0f)
			{
				v7 += 3;
				break;
			}
			if (float(float(v8 + 2) * v9) > 68.0f)
			{
				v7 += 4;
				break;
			}
			v8 += 5;
			v7 += 5;
			if (v8 > 16)
				break;
		}
		if (chocked_ticks < choke_value)
			lag = 1;
		else
			lag = false;
		break;
	default:
		break;
	}

	if (!ctx.m_settings.fake_lag_enabled)
		lag = false;

	if (lag)
	{
		if (csgo.m_engine()->IsVoiceRecording() && chocked_ticks > 3)
			lag = false;

		if (chocked_ticks > 0 && (ctx.has_exploit_toggled && ctx.main_exploit > 0 && ctx.exploit_allowed && ctx.ticks_allowed > 4 || ctx.allow_shooting > cmd->command_number
			|| enable_delay > csgo.m_globals()->realtime))
			lag = false;
	}
	else
	{
		if (ctx.m_settings.anti_aim_enabled && !ctx.m_settings.fake_lag_enabled)
			lag = (cmd->command_number % 2) == 0;
	}

	if (lag)
		*send_packet = false;

	//if (currently_ducked != ducked && !currently_ducked && ctx.m_local()->m_flDuckAmount() <= 0.2f || currently_ducked)
	//	ducked = cmd->buttons & IN_DUCK && !ctx.fakeducking;

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}


void c_antiaimbot::work(CUserCmd* cmd, bool* send_packet)
{
	if (cmd == nullptr || ctx.m_local() == nullptr)
		return;

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

	/*auto pressed_move_key = (cmd->buttons & IN_MOVELEFT
		|| cmd->buttons & IN_MOVERIGHT
		|| cmd->buttons & IN_BACK
		|| cmd->buttons & IN_FORWARD);*/

	if (ctx.m_settings.anti_aim_enabled && m_weapon()->m_reloadState() == 0/*!(cmd->buttons & (IN_ATTACK | IN_ATTACK2 | IN_MOVERIGHT | IN_MOVELEFT | IN_USE | IN_BACK | IN_FORWARD | IN_JUMP))*/
		&& sqrtf((cmd->forwardmove * cmd->forwardmove) + (cmd->sidemove * cmd->sidemove)) < 0.1f)
	{
		if (sqrtf((ctx.m_local()->m_vecVelocity().y * ctx.m_local()->m_vecVelocity().y) + (ctx.m_local()->m_vecVelocity().x * ctx.m_local()->m_vecVelocity().x)) < 0.1f)
		{
			auto v12 = 1.01f;/* : -1.01f;*/

			if (ctx.m_local()->m_bDucking() || ctx.m_local()->m_fFlags() & 2)
				v12 = v12 / (((ctx.m_local()->m_flDuckAmount() * 0.34f) + 1.0f) - ctx.m_local()->m_flDuckAmount());

			if (!(cmd->command_number & 1))
				v12 *= -1;

			cmd->forwardmove = v12;
		}
	}

	//if (/*!pressed_move_key &&*/ move_speed <= 0.1f && vel_speed <= 0.1f 
	//	&& ctx.m_local()->m_fFlags() & FL_ONGROUND && Engine::Prediction::Instance()->GetFlags() & FL_ONGROUND 
	//	&& (ctx.m_settings.anti_aim_typelby == 1 || ctx.m_settings.anti_aim_typelby == 0 && ctx.main_exploit > 0 && ctx.has_exploit_toggled && ctx.exploit_allowed))
	//{
	//	const auto duck_amnt = ctx.m_local()->m_flDuckAmount();
	//	float duck_amnt2;
	//	const auto in_duck_button = (cmd->buttons & IN_DUCK);

	//	if (in_duck_button || ctx.fakeducking)
	//		duck_amnt2 = fminf(1.0f, ((csgzo.m_globals()->interval_per_tick * 0.8f) * ctx.m_local()->m_flDuckSpeed()) + duck_amnt);
	//	else
	//		duck_amnt2 = fmaxf(0.0f, duck_amnt - (fmaxf(1.5f, ctx.m_local()->m_flDuckSpeed()) * csgo.m_globals()->interval_per_tick));

	//	if (!(cmd->command_number & 1))
	//		cmd->forwardmove = -(ctx.m_local()->m_vecViewOffset().z < 64.f ? 3.f * duck_amnt2 : 1.01f);
	//	else
	//		cmd->forwardmove = (ctx.m_local()->m_vecViewOffset().z < 64.f ? 3.f * duck_amnt2 : 1.01f);

	//	/*static float old_forwardmove = 1.01f;

	//	auto newforwardmove = old_forwardmove;
	//	auto duck_amnt = ctx.m_local()->m_flDuckAmount();
	//	float duck_amnt2 = 0.f;
	//	auto in_duck_button = (cmd->buttons & IN_DUCK);

	//	if (in_duck_button)
	//		duck_amnt2 = fminf(1.0f, ((csgo.m_globals()->interval_per_tick * 0.8f) * ctx.m_local()->m_flDuckSpeed()) + duck_amnt);
	//	else
	//		duck_amnt2 = fmaxf(0.0f, duck_amnt - (fmaxf(1.5f, ctx.m_local()->m_flDuckSpeed()) * csgo.m_globals()->interval_per_tick));

	//	if (in_duck_button || ctx.m_local()->m_bDucked() || ctx.m_local()->m_fFlags() & 2)
	//		newforwardmove = newforwardmove / (((duck_amnt2 * 0.34f) + 1.0f) - duck_amnt2);

	//	old_forwardmove *= -1.f;

	//	auto old_move = cmd->forwardmove;

	//	if (old_move != 0.0f)
	//	{
	//		if (old_move >= 0.0f)
	//			newforwardmove = newforwardmove + old_move;
	//		else
	//			newforwardmove = old_move - newforwardmove;
	//	}

	//	cmd->forwardmove = newforwardmove;*/
	//}

	if (ctx.fside == 0)
		previous_side = ctx.fside = 1;

	ctx.changed_fake_side = false;

	const bool can_show = !(enable_delay > csgo.m_globals()->realtime || !m_weapon() || !ctx.m_settings.anti_aim_enabled || ctx.m_local()->m_MoveType() == 9 || ctx.m_local()->m_MoveType() == 8 || (cmd->buttons & IN_USE && !(m_weapon()->m_iItemDefinitionIndex() == WEAPON_C4 || ctx.m_local()->m_bIsDefusing())));

	/*if (ctx.get_key_press(ctx.m_settings.anti_aim_fake_switch.key)) {
		ctx.fside *= -1;

		previous_side = ctx.fside;

		if (can_show) {
			ctx.changed_fake_side = true;
			ctx.active_keybinds[7].mode = 4;
		}
	}

	if (ctx.get_key_press(ctx.m_settings.anti_aim_yaw_left_switch.key)){
		ctx.side = (ctx.side == 0 ? -1 : 0);
		
		if (can_show)
			ctx.active_keybinds[10].mode = 4;
	}

	if (ctx.get_key_press(ctx.m_settings.anti_aim_yaw_right_switch.key)){
		ctx.side = (ctx.side == 1 ? -1 : 1);

		if (can_show)
			ctx.active_keybinds[9].mode = 4;
	}

	if (ctx.get_key_press(ctx.m_settings.anti_aim_yaw_backward_switch.key)) {
		ctx.side = (ctx.side == 2 ? -1 : 2);

		if (can_show)
			ctx.active_keybinds[8].mode = 4;
	}*/

	if (!can_show || ctx.onshot_aa_cmd == cmd->command_number/*ctx.onshot_desync == 2 && m_weapon()->can_shoot() && ctx.do_autostop*/) return;

	if (cmd->buttons & IN_USE)
	{
		static bool force_choke = false;

		if (m_weapon()->m_iItemDefinitionIndex() == WEAPON_C4 || ctx.m_local()->m_bIsDefusing()) 
		{
			if (TICKS_TO_TIME(ctx.m_local()->m_nTickBase()) > feature::anti_aim->lby_timer && csgo.m_client_state()->m_iChockedCommands == 0)
			{
				cmd->viewangles.y = Math::normalize_angle(ctx.m_local()->m_angEyeAngles().y - (180.f - ctx.fside));
				*send_packet = false;
				force_choke = true;

				cmd->viewangles.Clamp();

				return;
			}

			if (force_choke)
			{
				*send_packet = false;
				force_choke = false;
			}

			return;
		}
		else 		//legit aimbot on USE key can be there.
			return;
	}

	//#TODO: planting & defusing anti-aim to be less hittable

	change_angles(cmd, send_packet);

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}