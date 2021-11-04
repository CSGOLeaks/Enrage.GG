#pragma once

#include "movement.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "source.hpp"
#include "prediction.hpp"
#include "rage_aimbot.hpp"
#include "anti_aimbot.hpp"
#include "menu.hpp"

#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)


namespace Engine
{
	void Movement::Quick_stop(CUserCmd* cmd, Vector add) 
	{

		//static auto sv_friction = csgo.m_engine_cvars()->FindVar(sxor("sv_friction"));
		//static auto sv_stopspeed = csgo.m_engine_cvars()->FindVar(sxor("sv_stopspeed"));
		static auto sv_accelerate = csgo.m_engine_cvars()->FindVar(sxor("sv_accelerate"));
		static auto sv_maxspeed = csgo.m_engine_cvars()->FindVar(sxor("sv_maxspeed"));

		if (!ctx.m_local() || !ctx.m_local()->get_weapon())
			return;

		auto velocity = add.IsZero() ? ctx.m_local()->m_vecVelocity() : add;
		velocity.z = 0.f;

		float speed = velocity.Length2D();

		if (speed < 1.f)
		{
			cmd->forwardmove = cmd->sidemove = 0.f;
			return;
		}

		QAngle angle;
		Math::VectorAngles(velocity, angle);

		// fix direction by factoring in where we are looking.
		angle.y = ctx.cmd_original_angles.y - angle.y;

		// convert corrected angle back to a direction.
		Vector direction;
		Math::AngleVectors(angle, &direction);

		if (speed > 20.f) {
			auto stop = direction * -speed;

			cmd->forwardmove = stop.x;
			cmd->sidemove = stop.y;
		}
		else
		{
			cmd->forwardmove = 0;
			cmd->sidemove = 0;
		}
	}

	void Movement::limit_speed(CUserCmd* cmd, float max_speed) {
		if (max_speed <= 0.f) return;

		auto speed = (cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove);
		speed = sqrtf(speed);

		if (speed <= 0.f) return;

		//if (cmd->buttons & IN_DUCK)
		//	max_speed /= 0.34f;

		if (speed <= max_speed)
			return;

		float ratio = fminf(max_speed / speed, speed);

		cmd->forwardmove *= ratio;
		cmd->sidemove *= ratio;
		//cmd->upmove *= ratio;
	}

//	void Movement::MoveExploit() {
//#if defined(DEV) || defined(BETA_MODE) || defined(DEBUG_MODE)
//		static bool bDisallow = false;
//		static bool bSaveOrigin = true;
//
//		if (!g_Vars.misc.move_exploit) {
//			g_Vars.globals.vecExploitOrigin.Init();
//			bSaveOrigin = true;
//			return;
//		}
//
//		if (!g_Vars.misc.move_exploit_key.enabled) {
//			g_Vars.globals.vecExploitOrigin.Init();
//			bDisallow = false;
//			bSaveOrigin = true;
//			return;
//		}
//
//		if (m_pLocal->m_vecVelocity().Length2D() < m_pLocal->GetMaxSpeed() - 20.f) {
//			g_Vars.globals.vecExploitOrigin.Init();
//			g_Vars.globals.bMoveExploiting = false;
//			bSaveOrigin = true;
//			return;
//		}
//
//		if (Interfaces::pClientState->m_nChokedCommands() < 100) {
//			if (!bDisallow) {
//				*m_pSendPacket = false;
//				g_Vars.globals.bMoveExploiting = true;
//			}
//
//			if (bSaveOrigin) {
//				g_Vars.globals.vecExploitOrigin = m_pLocal->GetEyePosition() + (m_pLocal->m_vecVelocity() * 100 * Interfaces::pGlobalVars->interval_per_tick);
//				bSaveOrigin = false;
//			}
//		}
//		else {
//			*m_pSendPacket = true;
//			bSaveOrigin = true;
//			g_Vars.globals.bMoveExploiting = false;
//		}
//#endif
//	}

	void Movement::Begin(CUserCmd* cmd, bool& send_packet)
	{
		static auto accel = csgo.m_engine_cvars()->FindVar(sxor("sv_accelerate"));
		static auto maxSpeed = csgo.m_engine_cvars()->FindVar(sxor("sv_maxspeed"));
		static auto cl_forwardspeed = csgo.m_engine_cvars()->FindVar(sxor("cl_forwardspeed"));
		static auto sv_autobunnyhopping = csgo.m_engine_cvars()->FindVar(sxor("sv_autobunnyhopping"));
		float playerSurfaceFriction = ctx.m_local()->m_surfaceFriction(); // I'm a slimy boi
		//float max_accelspeed = accel->GetFloat() * csgo.m_globals()->interval_per_tick * maxSpeed->GetFloat() * playerSurfaceFriction;

		//just a test version 
		//aimware strafer

		if (!cmd)
			return;

		if (!ctx.m_local() || ctx.m_local()->IsDead())
			return;

		/*if (feature::menu->_menu_opened && !feature::menu->_keybind_toggled)
		{
			auto pressed_move_key = (ctx.pressed_keys['A']
				|| ctx.pressed_keys['D']
				|| ctx.pressed_keys['S']
				|| ctx.pressed_keys['W']
				|| ctx.pressed_keys[VK_SPACE]);

			if (pressed_move_key)
			{
				if (ctx.pressed_keys['A']) {
					cmd->buttons |= IN_MOVELEFT;
					cmd->sidemove -= 450.f;
				}
				if (ctx.pressed_keys['D']) {
					cmd->buttons |= IN_MOVERIGHT;
					cmd->sidemove += 450.f;
				}
				if (ctx.pressed_keys['S']) {
					cmd->buttons |= IN_BACK;
					cmd->forwardmove -= 450.f;
				}
				if (ctx.pressed_keys['W']) {
					cmd->buttons |= IN_FORWARD;
					cmd->forwardmove += 450.f;
				}
				if (ctx.pressed_keys[VK_SPACE])
					cmd->buttons |= IN_JUMP;
			}
		}*/

		csgo.m_engine()->GetViewAngles(m_qRealAngles);

		//static auto sidespeed = csgo.m_engine_cvars()->FindVar("cl_sidespeed");
		static auto old_yaw = cmd->viewangles.y;				//gay

		m_qAngles = ctx.cmd_original_angles;
		m_qAnglesView = ctx.cmd_original_angles;

		auto slowwalk = cmd->buttons & IN_SPEED;

		//if (slowwalk)
		//	ctx.active_keybinds[4].mode = ctx.m_settings.anti_aim_slowwalk_key.mode + 1;

		old_movement = Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove);

		const auto chocked_ticks = (cmd->command_number % 2) != 1 ? (14 - csgo.m_client_state()->m_iChockedCommands) : csgo.m_client_state()->m_iChockedCommands;// -((cmd->command_number % 3 < 1) ? 0 : min(5, cmd->command_number % 9)));

		auto move_speed = sqrtf((cmd->forwardmove * cmd->forwardmove) + (cmd->sidemove * cmd->sidemove));
		auto vel_speed = sqrtf((ctx.m_local()->m_vecVelocity().y * ctx.m_local()->m_vecVelocity().y) + (ctx.m_local()->m_vecVelocity().x * ctx.m_local()->m_vecVelocity().x));

		auto pressed_move_key = (cmd->buttons & IN_MOVELEFT
			|| cmd->buttons & IN_MOVERIGHT
			|| cmd->buttons & IN_BACK
			|| cmd->buttons & IN_FORWARD);

		if (ctx.m_settings.anti_aim_enabled && ctx.m_local()->m_fFlags() & FL_ONGROUND && !(cmd->buttons & IN_JUMP))
		{
			//if (auto state = ctx.m_local()->get_animation_state(); state != nullptr && feature::anti_aim->animation_speed > 2.f && !pressed_move_key && ctx.m_local()->m_vecViewOffset().z >= 64.f && state->on_ground && state->time_since_inair == 0.f)
			if (vel_speed > 0.1f && !ctx.do_autostop)
			{
				if (/*move_speed < 0.1f && */!pressed_move_key)
				{
					Quick_stop(cmd);
				}
				else if (!slowwalk && pressed_move_key)
				{
					limit_speed(cmd, ctx.max_weapon_speed - ((m_weapon() && m_weapon()->m_iItemDefinitionIndex() == WEAPON_AWP ? 1.2f : 1.4f) * chocked_ticks));
				}
			}
		}

		//auto& prediction = Prediction::Instance();
		//if (/*!(ctx.m_local()->m_fFlags() & FL_ONGROUND) && */ctx.pressed_keys[16] && m_weapon()->m_iItemDefinitionIndex() == weapon_ssg08 && fabs(max(cmd->sidemove, cmd->forwardmove)) < 10.f)
		//	ctx.do_autostop = true;

		auto ground = /*ctx.m_local()->m_fFlags() & FL_PARTIALGROUND || */ctx.m_local()->m_fFlags() & FL_ONGROUND;

		if (!(cmd->buttons & IN_JUMP) && ctx.previous_buttons & IN_JUMP)
		{
			auto v10 = cmd->buttons;

			if (!ground && ctx.breaks_lc)
				v10 = v10 | IN_JUMP;

			cmd->buttons = v10;
		}

		if (cmd->buttons & IN_JUMP)
		{
			if (ctx.m_settings.misc_bhop) {
				if (!ground  && sv_autobunnyhopping->GetInt() == 0)
					cmd->buttons &= ~IN_JUMP;
				else
					cmd->buttons |= IN_JUMP;
			}

			if (ctx.pressed_keys[16] && m_weapon()->m_iItemDefinitionIndex() == WEAPON_SSG08) {
				//Quick_stop(cmd);
			}
			else if (ctx.m_settings.misc_autostrafer && !ground && ctx.m_local()->m_MoveType() != MOVETYPE_LADDER)
			{
				auto get_velocity_degree = [](const float length_2d)
				{
					auto tmp = RAD2DEG(atan2(15.f, length_2d));

					return Math::clamp(tmp, 0, 90); //Math::clamp(tmp, 0.f, 90.f);
				};

				auto velocity = ctx.m_local()->m_vecVelocity();

				/*auto turn_direction_modifier = ((cmd->command_number % 2 == 0) ? 1.f : -1.f);*/
				static auto flip = false;
				auto turn_direction_modifier = (flip) ? 1.f : -1.f;
				flip = !flip;

				const auto forwardmove = cmd->forwardmove;
				const auto sidemove = cmd->sidemove;

				bool wasd = false;

				//auto forward = Vector::Zero, right = Vector::Zero;
				//Math::AngleVectors(ctx.cmd_original_angles, &forward, &right );

				if ((forwardmove != 0.0f || sidemove != 0.0f) && ctx.m_settings.misc_autostrafer_wasd)
				{
					cmd->forwardmove = 0.0f; cmd->sidemove = 0.0f;
					//auto next_velocity = ctx.m_local()->m_vecVelocity().Normalized();

					//for (int i = 0; i < 3; i++)       // Determine x and y parts of velocity
					//	next_velocity[i] = forward[i] * forwardmove + right[i] * sidemove;
					//next_velocity[2] += cmd->upmove;

					const float move_dir = RAD2DEG(atan2f(-sidemove, forwardmove));
					/*if (cmd->buttons & IN_MOVERIGHT)
						m_qAnglesView.y -= 90;
					else if (cmd->buttons & IN_MOVELEFT)
						m_qAnglesView.y += 90;
					else if (cmd->buttons & IN_BACK)
						m_qAnglesView.y += 180;*/
					//if (cmd->buttons & IN_MOVERIGHT)

					m_qAnglesView.y = Math::normalize_angle(m_qAnglesView.y + move_dir);
					wasd = true;
				}
				else if (forwardmove > 0.f)
					cmd->forwardmove = 0.f;

				auto velocity_length_2d = velocity.Length2D();

				if (velocity_length_2d <= 0.f)
					cmd->forwardmove = 450.f;

				auto strafe_angle = Math::clamp(RAD2DEG(atanf(15.f / velocity_length_2d)), 0.f, 90.f);

				auto yaw_delta = Math::normalize_angle(m_qAnglesView.y - old_yaw);
				old_yaw = m_qAnglesView.y;

				auto abs_yaw_delta = abs(yaw_delta);

				if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.f)
				{
					Vector velocity_angles;
					Math::VectorAngles(ctx.m_local()->m_vecAbsVelocity(), velocity_angles);

					const auto velocity_angle_yaw_delta = Math::normalize_angle(m_qAnglesView.y - velocity_angles.y);
					const auto velocity_degree = (get_velocity_degree(velocity_length_2d));// *(wasd ? float(0.02f * ctx.m_settings.misc_autostrafer_retrack) : 2.f);

					if (velocity_angle_yaw_delta <= velocity_degree || velocity_length_2d <= 15.f)
					{
						if ((-velocity_degree <= velocity_angle_yaw_delta || velocity_length_2d <= 15.f))
						{
							m_qAnglesView.y += (strafe_angle * turn_direction_modifier);
							cmd->sidemove = 450.f * turn_direction_modifier;
						}
						else
						{
							m_qAnglesView.y = velocity_angles.y - velocity_degree;
							cmd->sidemove = 450.f;
						}
					}
					else
					{
						m_qAnglesView.y = velocity_angles.y + velocity_degree;
						cmd->sidemove = -450.f;
					}
				}
				else if (yaw_delta > 0.0f) {
					cmd->sidemove = -450.f;
				}
				else if (yaw_delta < 0.0f) {
					cmd->sidemove = 450.f;
				}

				auto v39 = cmd->buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD);
				cmd->buttons = v39;
				//if (msc_autostrafer_mode)               // not silent
				//{
				auto v40 = cmd->sidemove;
				if (v40 != 0.0)
				{
					if (v40 <= 0.0)
						v39 |= IN_MOVELEFT;
					else
						v39 |= IN_MOVERIGHT;
					cmd->buttons = v39;
				}
				auto v41 = cmd->forwardmove;
				if (v41 != 0.0)
				{
					if (v41 <= 0.0)
						cmd->buttons = v39 | IN_BACK;
					else
						cmd->buttons = v39 | IN_FORWARD;
				}
				//}
				//cmd->viewangles.y = m_qAnglesView.y;
				//circle_yaw = m_qAnglesView.y; // fix for circle strafer stopping after auto strafing

				//cmd->viewangles.y = ctx.cmd_original_angles.y;
			}

			if (cmd->sidemove < 0.f)
				cmd->buttons |= IN_MOVELEFT;
			else
				cmd->buttons |= IN_MOVERIGHT;

			if (cmd->forwardmove < 0.f)
				cmd->buttons |= IN_BACK;
			else
				cmd->buttons |= IN_FORWARD;

		}
		else
		{
			/*if (ctx.pressed_keys['V'])
			{
				CTraceFilter filter;
				filter.pSkip = ctx.m_local();
				auto origin_end = ctx.m_local()->get_abs_origin();
				origin_end.z -= 11.0f;
				Ray_t r;
				trace_t tr;
				r.Init(ctx.m_local()->get_abs_origin(), origin_end, ctx.m_local()->OBBMins(), ctx.m_local()->OBBMaxs());
				csgo.m_engine_trace()->TraceRay(r, 0x201400B, &filter, &tr);

				auto v40 = cmd->buttons;

				if (tr.DidHit())
					cmd->buttons = v40 & ~4u | 2;
				else
					cmd->buttons = v40 & ~2u | 4;
			}*/

			auto velocity = ctx.m_local()->m_vecVelocity();

			if (slowwalk && pressed_move_key) {
				//if (cheat::Cvars.anti_aim_slow_walk_accurate.GetValue()) {
					float maxspeed = 260.f;

					if (ctx.m_local()->m_bIsScoped())
						maxspeed = m_weapon()->GetCSWeaponData()->max_speed_alt;
					else
						maxspeed = m_weapon()->GetCSWeaponData()->max_speed;

					maxspeed *= 0.5f;

					auto v22 = (velocity.x * velocity.x) + (velocity.y * velocity.y);
					auto a1 = (cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove);
					a1 = sqrt(a1);
					v22 = sqrt(v22);
					auto v23 = v22;
					auto v15 = a1;
					auto v25 = cmd->forwardmove / v15;
					auto v27 = cmd->sidemove / v15;

					if (v15 > maxspeed)
					{
						if ((maxspeed + 1.0f) <= v23)
						{
							cmd->forwardmove = 0.0f;
							cmd->sidemove = 0.0f;
						}
						else
						{
							cmd->sidemove = maxspeed * v27;
							cmd->forwardmove = maxspeed * v25;
						}
					}
			}
		}

		auto is_auto_peek_toggled = ctx.get_key_press(ctx.m_settings.anti_aim_autopeek_key);

		ctx.active_keybinds[11].mode = 0;

		static bool was_enabled = true;
		if (is_auto_peek_toggled) {
			ctx.active_keybinds[11].mode = ctx.m_settings.anti_aim_autopeek_key.mode + 1;
			if (!was_enabled) {
				ctx.auto_peek_spot = ctx.m_local()->m_vecAbsOrigin();
				if (!(ctx.m_local()->m_fFlags() & FL_ONGROUND)) {
					Ray_t ray;
					ray.Init(ctx.m_local()->m_vecOrigin(), ctx.m_local()->m_vecOrigin() - Vector(0.0f, 0.0f, 1000.0f));

					CTraceFilterWorldAndPropsOnly filter;
					CGameTrace tr;
					csgo.m_engine_trace()->TraceRay(ray, 0x46004003u, &filter, &tr);
					if (tr.fraction < 1)
						ctx.auto_peek_spot = tr.endpos + Vector(0.0f, 0.0f, 2.0f);
				}
			}

			was_enabled = !ctx.auto_peek_spot.IsZero();
		}
		else {
			ctx.auto_peek_spot.clear();
			ctx.autopeek_back = false;
			was_enabled = false;
		}

		static bool did_finish_move = true;

		if ((is_auto_peek_toggled && ctx.autopeek_back || !did_finish_move) && !ctx.auto_peek_spot.IsZero()) {
			cmd->buttons &= ~IN_JUMP;

			auto angle = Math::CalcAngle(ctx.m_local()->m_vecAbsOrigin(), ctx.auto_peek_spot);
			m_qAnglesView.y = angle.y;

			cmd->forwardmove = cl_forwardspeed->GetFloat() - (1.2f * chocked_ticks);
			cmd->sidemove = 0.0f;

			if (ctx.auto_peek_spot.DistanceSquared(ctx.m_local()->m_vecAbsOrigin()) < 10) {
				cmd->sidemove = 0;
				cmd->forwardmove = 0;
				ctx.autopeek_back = false;
				did_finish_move = true;
			}
			else if (abs(csgo.m_globals()->realtime - ctx.last_shot_time_clientside) < 0.5f)
				did_finish_move = false;
		}

		cmd->sidemove = Math::clamp(cmd->sidemove, -450.0f, 450.0f);
		cmd->forwardmove = Math::clamp(cmd->forwardmove, -450.0f, 450.0f);
	}

	void Movement::PreMovement(CUserCmd* cmd)
	{
		m_oldangle = cmd->viewangles;
		m_oldforward = cmd->forwardmove;
		m_oldsidemove = cmd->sidemove;
	}

	void Movement::FixMove(CUserCmd* cmd, const QAngle& wish_angles) {
		if (ctx.m_local() == nullptr || cmd == nullptr/*csgo.m_client_state() && ctx.last_autostop_tick == csgo.m_client_state()->m_clockdrift_manager.m_nServerTick*/)
			return;

		Vector  move, dir;
		float   delta, len;
		QAngle   move_angle;

		// roll nospread fix.
		if (!(ctx.m_local()->m_fFlags() & FL_ONGROUND) && cmd->viewangles.z != 0 && cmd->buttons & IN_ATTACK)
			cmd->sidemove = 0;

		// convert movement to vector.
		move = { cmd->forwardmove, cmd->sidemove, 0 };

		// get move length and ensure we're using a unit vector ( vector with length of 1 ).
		len = move.Normalize();

		if (!len)
			return;

		// convert move to an angle.
		Math::VectorAngles(move, move_angle);

		// calculate yaw delta.
		delta = (cmd->viewangles.y - wish_angles.y);

		// accumulate yaw delta.
		move_angle.y += delta;

		// calculate our new move direction.
		// dir = move_angle_forward * move_length
		Math::AngleVectors(move_angle, &dir);

		// scale to og movement.
		dir *= len;

		// fix ladder and noclip.
		if (ctx.m_local()->m_MoveType() == MOVETYPE_LADDER) {
			// invert directon for up and down.
			if (cmd->viewangles.x >= 45 && wish_angles.x < 45 && std::abs(delta) <= 65)
				dir.x = -dir.x;

			// write to movement.
			cmd->forwardmove = dir.x;
			cmd->sidemove = dir.y;

			// set new button flags.
			if (cmd->forwardmove > 200)
				cmd->buttons |= IN_FORWARD;

			else if (cmd->forwardmove < -200)
				cmd->buttons |= IN_BACK;

			if (cmd->sidemove > 200)
				cmd->buttons |= IN_MOVERIGHT;

			else if (cmd->sidemove < -200)
				cmd->buttons |= IN_MOVELEFT;
		}

		// we are moving normally.
		else {
			// we must do this for pitch angles that are out of bounds.
			if (cmd->viewangles.x < -90 || cmd->viewangles.x > 90)
				dir.x = -dir.x;

			// set move.
			cmd->forwardmove = dir.x;
			cmd->sidemove = dir.y;

		/*	if (Engine::Movement::Instance()->did_force) {
				
				cmd->forwardmove = Math::clamp(Engine::Movement::Instance()->forcemovement.x, -450.f, 450.f);
				cmd->sidemove = Math::clamp(Engine::Movement::Instance()->forcemovement.y, -450.f, 450.f);
				Engine::Movement::Instance()->did_force = false;
			}*/
		}

		cmd->forwardmove = Math::clamp(cmd->forwardmove, -450.f, 450.f);
		cmd->sidemove = Math::clamp(cmd->sidemove, -450.f, 450.f);
		cmd->upmove = Math::clamp(cmd->upmove, -320.f, 320.f);
	}

	void Movement::Fix_Movement(CUserCmd* cmd, QAngle original_angles)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		Vector wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

		auto viewangles = cmd->viewangles;
		auto movedata = Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		viewangles.Normalize();

		if (!(ctx.m_local()->m_fFlags() & FL_ONGROUND) && viewangles.z != 0.f)
			movedata.y = 0.f;

		Math::AngleVectors(original_angles, &wish_forward, &wish_right, &wish_up);
		Math::AngleVectors(viewangles, &cmd_forward, &cmd_right, &cmd_up);

		const auto v8 = sqrtf(wish_forward.x * wish_forward.x + wish_forward.y * wish_forward.y), 
			v10 = sqrtf(wish_right.x * wish_right.x + wish_right.y * wish_right.y), 
			v12 = sqrtf(wish_up.z * wish_up.z);

		Vector wish_forward_norm(1.0f / v8 * wish_forward.x, 1.0f / v8 * wish_forward.y, 0.f),
			wish_right_norm(1.0f / v10 * wish_right.x, 1.0f / v10 * wish_right.y, 0.f),
			wish_up_norm(0.f, 0.f, 1.0f / v12 * wish_up.z);

		const auto v14 = sqrtf(cmd_forward.x * cmd_forward.x + cmd_forward.y * cmd_forward.y), 
			v16 = sqrtf(cmd_right.x * cmd_right.x + cmd_right.y * cmd_right.y), 
			v18 = sqrtf(cmd_up.z * cmd_up.z);

		Vector cmd_forward_norm(1.0f / v14 * cmd_forward.x, 1.0f / v14 * cmd_forward.y, 1.0f / v14 * 0.0f),
			cmd_right_norm(1.0f / v16 * cmd_right.x, 1.0f / v16 * cmd_right.y, 1.0f / v16 * 0.0f),
			cmd_up_norm(0.f, 0.f, 1.0f / v18 * cmd_up.z);

		const auto v22 = wish_forward_norm.x * movedata.x, 
			v26 = wish_forward_norm.y * movedata.x, 
			v28 = wish_forward_norm.z * movedata.x, 
			v24 = wish_right_norm.x * movedata.y, 
			v23 = wish_right_norm.y * movedata.y, 
			v25 = wish_right_norm.z * movedata.y, 
			v30 = wish_up_norm.x * movedata.z, 
			v27 = wish_up_norm.z * movedata.z, 
			v29 = wish_up_norm.y * movedata.z;

		Vector correct_movement = Vector::Zero;

		correct_movement.x = cmd_forward_norm.x * v24 + cmd_forward_norm.y * v23 + cmd_forward_norm.z * v25
			+ (cmd_forward_norm.x * v22 + cmd_forward_norm.y * v26 + cmd_forward_norm.z * v28)
			+ (cmd_forward_norm.y * v30 + cmd_forward_norm.x * v29 + cmd_forward_norm.z * v27);

		correct_movement.y = cmd_right_norm.x * v24 + cmd_right_norm.y * v23 + cmd_right_norm.z * v25
			+ (cmd_right_norm.x * v22 + cmd_right_norm.y * v26 + cmd_right_norm.z * v28)
			+ (cmd_right_norm.x * v29 + cmd_right_norm.y * v30 + cmd_right_norm.z * v27);

		correct_movement.z = cmd_up_norm.x * v23 + cmd_up_norm.y * v24 + cmd_up_norm.z * v25
			+ (cmd_up_norm.x * v26 + cmd_up_norm.y * v22 + cmd_up_norm.z * v28)
			+ (cmd_up_norm.x * v30 + cmd_up_norm.y * v29 + cmd_up_norm.z * v27);

		/*if (did_force)
		{
			correct_movement.x = forcemovement.x;
			correct_movement.y = forcemovement.y;
			did_force = false;
		}*/

		correct_movement.x = Math::clamp(correct_movement.x, -450.f, 450.f);
		correct_movement.y = Math::clamp(correct_movement.y, -450.f, 450.f);
		correct_movement.z = Math::clamp(correct_movement.z, -320.f, 320.f);

		cmd->forwardmove = correct_movement.x;
		cmd->sidemove = correct_movement.y;
		cmd->upmove = correct_movement.z;
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void Movement::End(CUserCmd* cmd)
	{
		//aimware
		if (!cmd || !ctx.m_local())
			return;

		//Fix_Movement(cmd, m_qRealAngles);
		FixMove(cmd, ctx.cmd_original_angles);
	}
}