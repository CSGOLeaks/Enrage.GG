#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "math.hpp"
#include "displacement.hpp"
#include "resolver.hpp"
#include "anti_aimbot.hpp"
#include <unordered_map>
#include <algorithm>
#include "menu.hpp"

#include <thread>
#include "usercmd.hpp"
#include "movement.hpp"

//CUserCmd* c_usercmd::find_latest_non_sent_command()
//{
//	auto cmd_from_slot = csgo.m_input()->GetUserCmd(feature::usercmd->command_numbers.at(2).command_number);
//	return cmd_from_slot;
//}

//void cmd_autostop(CUserCmd* cmd, c_cmd data)
//{
//	static auto accel = csgo.m_engine_cvars()->FindVar("sv_accelerate");
//
//	if (!ctx.m_settings.aimbot_autostop || !m_weapon())
//		return;
//
//	static bool was_onground = ctx.m_local()->m_fFlags() & FL_ONGROUND;
//
//	auto local_weapon = m_weapon();
//
//	if (ctx.do_autostop && local_weapon && local_weapon->GetCSWeaponData() && was_onground && ctx.m_local()->m_fFlags() & FL_ONGROUND)
//	{
//		auto speed = ((cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove));
//		auto lol = sqrt(speed);
//
//		auto velocity = data.velocity;
//		float maxspeed = 30.f;
//
//		if (!ctx.m_local()->m_bIsScoped())
//			maxspeed = *(float*)(uintptr_t(local_weapon->GetCSWeaponData()) + 0x130);
//		else
//			maxspeed = *(float*)(uintptr_t(local_weapon->GetCSWeaponData()) + 0x134);//local_weapon->GetCSWeaponData()->max_speed;
//
//		maxspeed *= 0.33f;
//
//		float max_accelspeed = accel->GetFloat() * csgo.m_globals()->interval_per_tick * maxspeed * ctx.m_local()->m_surfaceFriction();
//		const auto chocked_ticks = abs(ctx.last_sent_tick - csgo.m_globals()->tickcount);
//
//		if (ctx.m_settings.aimbot_autostop_options[0])
//		{
//			cmd->buttons |= IN_SPEED;
//
//			if (velocity.Length2D() > maxspeed)
//			{
//				//	cmd->buttons |= IN_WALK;
//				//Engine::Movement::Instance()->quick_stop(cmd);
//
//				if ((maxspeed + 1.0f) <= velocity.Length2D())
//				{
//					cmd->forwardmove = 0.0f;
//					cmd->sidemove = 0.0f;
//				}
//				else
//				{
//					cmd->sidemove = (maxspeed * (cmd->sidemove / lol));
//					cmd->forwardmove = (maxspeed * (cmd->forwardmove / lol));
//				}
//			}
//		}
//		else
//		{
//			//cmd->buttons |= IN_SPEED;
//			Engine::Movement::Instance()->Quick_stop(cmd);
//		}
//	}
//}

void c_usercmd::run_fixes(CUserCmd* cmd)
{
	//if (cmd == nullptr
	//	/*|| feature::usercmd->last_cmd.command_number <= 0*/
	//	|| !m_weapon()
	//	|| m_weapon()->m_iItemDefinitionIndex() == 64
	//	|| cmd->command_number != ctx.m_ragebot_shot_nr
	//	|| csgo.m_client_state()->m_iChockedCommands <= 1)
		return;

	static bool meme = false;
	meme = !meme;

	for (auto i = 1; i < max(2, (csgo.m_client_state()->m_iChockedCommands-1)); i++) {
		auto& cmdinfo = feature::usercmd->cmd_info[(cmd->command_number - i) % 150];

		auto ccmd = csgo.m_input()->GetUserCmd(cmdinfo.command_number);

		if (ccmd == nullptr || cmdinfo.command_number != ccmd->command_number)
			return;

		auto vcmd = csgo.m_input()->GetVerifiedUserCmd(cmdinfo.command_number);

		if (vcmd == nullptr || ccmd->buttons & IN_ATTACK)
			return;

		auto side = (ctx.fside ? -120.f : 120.f);

		auto angle = (float)-ctx.fside;//Math::AngleDiff(ctx.angles[ANGLE_REAL], ctx.angles[ANGLE_FAKE]);

		switch (ctx.m_settings.anti_aim_typeyfake_shot)
		{
		case 0:
			return;
			break;
		case 1:
		{
			side = copysignf(120.f, angle);
			break;
		}
		case 2:
			side = copysignf(120.f, -angle);
			break;
		case 3:
		{
			side = copysignf(120.f, angle) * (meme ? 1.f : -1.f);
			break;
		}
		}

		ccmd->viewangles.y = Math::normalize_angle(ccmd->viewangles.y + side);
		ccmd->viewangles.Clamp();

		ccmd->forwardmove = cmdinfo.move_data.x;// Engine::Movement::Instance()->old_movement.x;
		ccmd->sidemove = cmdinfo.move_data.y;// Engine::Movement::Instance()->old_movement.y;

		Engine::Movement::Instance()->FixMove(ccmd, cmdinfo.original_angles);

		//csgo.m_prediction()->Update(csgo.m_client_state()->m_iDeltaTick, csgo.m_client_state()->m_iDeltaTick > 0, csgo.m_client_state()->m_iLastCommandAck,
		//	csgo.m_client_state()->m_iLastOutgoingCommand + csgo.m_client_state()->m_iChockedCommands);

		//csgo.m_prediction()->Update(csgo.m_client_state()->m_iDeltaTick, csgo.m_client_state()->m_iDeltaTick > 0, csgo.m_client_state()->m_iLastCommandAck,
		//		ccmd->command_number);

		vcmd->m_cmd = *ccmd;
		vcmd->m_crc = ccmd->GetChecksum();
	}

	//csgo.m_prediction()->force_repredict();
}