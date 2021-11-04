#include "prediction.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "source.hpp"
#include "displacement.hpp"
#include "weapon.hpp"
#include "movement.hpp"
#include "anti_aimbot.hpp"
#include "visuals.hpp"
#include "misc.hpp"

void copy_cmd(CUserCmd* cmd1, CUserCmd* v2)
{
	if (v2 != cmd1)
	{
		v2->command_number = cmd1->command_number;
		v2->tick_count = cmd1->tick_count;
		v2->viewangles.x = cmd1->viewangles.x;
		v2->viewangles.y = cmd1->viewangles.y;
		v2->viewangles.z = cmd1->viewangles.z;
		v2->aimdirection.x = cmd1->aimdirection.x;
		v2->aimdirection.y = cmd1->aimdirection.y;
		v2->aimdirection.z = cmd1->aimdirection.z;
		v2->forwardmove = cmd1->forwardmove;
		v2->sidemove = cmd1->sidemove;
		v2->upmove = cmd1->upmove;
		v2->buttons = cmd1->buttons;
		v2->impulse = cmd1->impulse;
		v2->weaponselect = cmd1->weaponselect;
		v2->weaponsubtype = cmd1->weaponsubtype;
		v2->random_seed = cmd1->random_seed;
		v2->mousedx = cmd1->mousedx;
		v2->mousedy = cmd1->mousedy;
		v2->hasbeenpredicted = cmd1->hasbeenpredicted;
		v2->headangles.x = cmd1->headangles.x;
		v2->headangles.y = cmd1->headangles.y;
		v2->headangles.z = cmd1->headangles.z;
		v2->headoffset.x = cmd1->headoffset.x;
		v2->headoffset.y = cmd1->headoffset.y;
		v2->headoffset.z = cmd1->headoffset.z;
	}
}

bool did_predict = false;
float itime = 0.f;
int new_tickbase = 0;
int last_tickbase = 0;

#define VEC_VIEW			csgo.m_game_rules()->GetViewVectors()->m_vView
#define VEC_HULL_MIN		csgo.m_game_rules()->GetViewVectors()->m_vHullMin
#define VEC_HULL_MAX		csgo.m_game_rules()->GetViewVectors()->m_vHullMax

#define VEC_DUCK_HULL_MIN	csgo.m_game_rules()->GetViewVectors()->m_vDuckHullMin
#define VEC_DUCK_HULL_MAX	csgo.m_game_rules()->GetViewVectors()->m_vDuckHullMax
#define VEC_DUCK_VIEW		csgo.m_game_rules()->GetViewVectors()->m_vDuckView

namespace Engine
{
	void Prediction::calculate_desync_delta()
	{
		auto animstate = ctx.m_local()->get_animation_state();

		feature::anti_aim->min_delta = animstate->m_aim_yaw_min;
		feature::anti_aim->max_delta = animstate->m_aim_yaw_max;

		float max_speed = 260.f;

		if (ctx.latest_weapon_data)
			max_speed = fmaxf(.001f, ctx.latest_weapon_data->max_speed);

		const auto current_origin = ctx.m_local()->m_vecOrigin();
		const auto fraction = 1.0f / csgo.m_globals()->interval_per_tick;
		auto calculated_velocity = (current_origin - Engine::Prediction::Instance()->m_vecOrigin) * fraction;
	
		feature::anti_aim->animation_velocity = calculated_velocity;

		feature::anti_aim->animation_speed = feature::anti_aim->animation_velocity.Length();

		if (feature::anti_aim->animation_speed >= 260.f)
			feature::anti_aim->animation_speed = 260.f;


		//feature::anti_aim->feet_speed_stand = feature::anti_aim->animation_speed / (max_speed * 0.52f);
		//feature::anti_aim->feet_speed_ducked = feature::anti_aim->animation_speed / (max_speed * 0.34f);

		// narrow the available aim matrix width as speed increases
		//float flAimMatrixWidthRange = Math::Lerp(Math::clamp(feature::anti_aim->feet_speed_stand, 0, 1), 1.0f, Math::Lerp(feature::anti_aim->stop_to_full_running_fraction, 0.8f, 0.5f));

		//if (ctx.m_local()->m_flDuckAmount() > 0)
		//	flAimMatrixWidthRange = Math::Lerp(ctx.m_local()->m_flDuckAmount() * Math::clamp(feature::anti_aim->feet_speed_ducked, 0, 1), flAimMatrixWidthRange, 0.5f);

		auto v19 = (1.923077f / max_speed) * feature::anti_aim->animation_speed;
		auto v21 = (2.9411764f / max_speed) * feature::anti_aim->animation_speed;
		auto v20 = 1.0f;
		if (v19 <= 1.0f)
		{
			if (v19 < 0.0f)
				v19 = 0.0f;
		}
		else
		{
			v19 = 1.0f;
		}
		auto v22 = 1.0f - (((animstate->m_walk_to_run_transition_state * 0.30000001f) + 0.2f) * v19);
		auto v23 = animstate->m_anim_duck_amount;
		if (v23 > 0.0f)
		{
			if (v21 <= 1.0f)
			{
				if (v21 >= 0.0f)
					v20 = (2.9411764f / max_speed) * feature::anti_aim->animation_speed;
				else
					v20 = 0.0f;
			}
			v22 = v22 + ((v20 * v23) * (0.5f - v22));
		}

		feature::anti_aim->max_delta *= v22;
		feature::anti_aim->min_delta *= v22;

		if (calculated_velocity.Length() < 5)
			feature::anti_aim->animation_speed = 0.f;

		/*if (feature::anti_aim->stop_to_full_running_fraction > 0 && feature::anti_aim->stop_to_full_running_fraction < 1)
		{
			const auto interval = csgo.m_globals()->interval_per_tick + csgo.m_globals()->interval_per_tick;

			if (feature::anti_aim->is_standing)
				feature::anti_aim->stop_to_full_running_fraction -= interval;
			else
				feature::anti_aim->stop_to_full_running_fraction += interval;

			feature::anti_aim->stop_to_full_running_fraction = Math::clamp(feature::anti_aim->stop_to_full_running_fraction, 0, 1);
		}

		if (feature::anti_aim->animation_speed > (260.0f * 0.52f) && feature::anti_aim->is_standing)
		{
			feature::anti_aim->is_standing = false;
			feature::anti_aim->stop_to_full_running_fraction = fmaxf(0.01f, feature::anti_aim->stop_to_full_running_fraction);
		}

		if (feature::anti_aim->animation_speed < (260.0f * 0.52f) && !feature::anti_aim->is_standing)
		{
			feature::anti_aim->is_standing = true;
			feature::anti_aim->stop_to_full_running_fraction = fminf(0.99f, feature::anti_aim->stop_to_full_running_fraction);
		}

		if (ctx.m_local() && ctx.m_local()->get_animation_state() != nullptr)
		{
			ctx.m_local()->get_animation_state()->m_walk_run_transition = feature::anti_aim->stop_to_full_running_fraction;
			ctx.m_local()->get_animation_state()->m_walk_to_run_transition_state = feature::anti_aim->is_standing;
		}*/
	}

	void Prediction::SetupMovement(CUserCmd* cmd)
	{
		if (!ctx.m_local() || ctx.m_local()->IsDead())
			return;

	//	restore_data.is_filled = false;

		//const auto data = &Engine::Prediction::Instance()->m_Data[(cmd->command_number - 1) % 150];

		//if (data && data->tickbase == ctx.m_local()->m_nTickBase() && (cmd->command_number - 1) == data->command_number/*|| data->tick_count >= FLT_MAX*//* && data->command_number >= INT_MAX*/)
		//	Engine::Prediction::Instance()->FixNetvarCompression(cmd->command_number - 1);

		/*if (ctx.last_frame_stage == 4 && ctx.last_netvars_update_tick && (cmd->command_number - ctx.last_netvars_update_tick - 2) <= 148)
		{
			csgo.m_prediction()->Update(csgo.m_client_state()->m_iDeltaTick, csgo.m_client_state()->m_iDeltaTick > 0, csgo.m_client_state()->m_iLastCommandAck,
						csgo.m_client_state()->m_iLastOutgoingCommand + csgo.m_client_state()->m_iChockedCommands);
		}*/

		//if (ctx.last_velocity_modifier < 1.f/* && ctx.m_local()->m_vecVelocity().Length2D() >= (ctx.m_local()->get_player_max_speed() * 0.1f)*/)
		//{
		//	csgo.m_prediction()->m_bPreviousAckHadErrors = 1;
		//	csgo.m_prediction()->m_nCommandsPredicted = 0;
		//}

		//if ((ctx.last_time_command_arrived + 1) == csgo.m_client_state()->m_iCommandAck)//ctx.last_netvars_update_tick == cmd->command_number)

		//CMoveData move = { }; // or u can grab it from SetupMove hook as aimware does
		//memset(&move_data, 0, sizeof(CMoveData));
		//static auto spawntime = 0.f;

		//CopyMoveData(&move_data, &newest_move_data);

		/*if (spawntime != ctx.m_local()->m_flSpawnTime())
		{
			memset(&move_data, 0, sizeof(CMoveData));
			spawntime = ctx.m_local()->m_flSpawnTime();
		}*/
		/*ctx.m_local()->SetCurrentCommand(cmd);
		C_BaseEntity::SetPredictionRandomSeed(cmd);
		C_BaseEntity::SetPredictionPlayer(ctx.m_local());*/

		//CopyMoveData(&move_data_clear, &move_data);
	}

	void Prediction::PrePrediction(CUserCmd* cmd)
	{
		if (!sv_footsteps)
			sv_footsteps = csgo.m_engine_cvars()->FindVar(sxor("sv_footsteps"));

		if (!sv_min_jump_landing_sound)
			sv_min_jump_landing_sound = csgo.m_engine_cvars()->FindVar(sxor("sv_min_jump_landing_sound"));

		//if (!cl_pred_optimize)
		//	cl_pred_optimize = csgo.m_engine_cvars()->FindVar(sxor("cl_pred_optimize"));

		if (!ctx.m_local() || ctx.m_local()->IsDead())
			return;

		//bRandomSeed = *(int*)(Engine::Displacement::Signatures[c_signatures::PREDRANDOMSEED]);
		//bUserCmd = *(uint32_t*)(uintptr_t(ctx.m_local()) + Engine::Displacement::C_BasePlayer::m_pCurrentCommand);
		//bPredCmd = *(CUserCmd*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::C_BasePlayer::m_pCurrentCommand - 0xB0));
		//bPredPlayer = *(C_BasePlayer**)(Engine::Displacement::Signatures[c_signatures::PREDPLAYER]);

		//*(BYTE*)((DWORD)csgo.m_prediction() + 0x18) = m_nServerCommandsAcknowledged;
		//*(BYTE*)((DWORD)csgo.m_prediction() + 8) = m_bInPrediction;

		/*cl_pred_optimize->m_nFlags &= ~FCVAR_HIDDEN;
		cl_pred_optimize->SetValue(0);*/

		bFirstCommandPredicted = *(bool*)(uintptr_t(csgo.m_prediction()) + 0x18);
		m_bInPrediction = csgo.m_prediction()->m_in_prediction;
		m_flCurrentTime = csgo.m_globals()->curtime;
		m_flFrameTime = csgo.m_globals()->frametime;

		//if (ctx.last_velocity_modifier_update_tick == csgo.m_client_state()->m_iLastCommandAck) {
		//	//csgo.m_prediction()->force_repredict();
		//	csgo.m_prediction()->m_nCommandsPredicted = 0;
		//	csgo.m_prediction()->m_previous_startframe = -1;
		//}

		/*if (ctx.last_frame_stage == FRAME_NET_UPDATE_END && ctx.last_netvars_update_tick && (cmd->command_number - ctx.last_netvars_update_tick) <= 150)
			csgo.m_prediction()->Update(csgo.m_client_state()->m_iDeltaTick, csgo.m_client_state()->m_iDeltaTick > 0, csgo.m_client_state()->m_iLastCommandAck,
				csgo.m_client_state()->m_iLastOutgoingCommand + csgo.m_client_state()->m_iChockedCommands);*/

		pred_error_time = *(float*)(uintptr_t(ctx.m_local()) + 0x35A4);

		m_fFlags = ctx.m_local()->m_fFlags();
		m_vecVelocity = ctx.m_local()->m_vecVelocity();
		m_flDuckAmount = ctx.m_local()->m_flDuckAmount();
		m_vecOrigin = ctx.m_local()->m_vecOrigin();
		m_MoveType = ctx.m_local()->m_MoveType();
		sv_footsteps_backup = *(float*)(uintptr_t(sv_footsteps) + 0x2C);
		sv_min_jump_landing_sound_backup = *(float*)(uintptr_t(sv_min_jump_landing_sound) + 0x2C);
		//auto lmao = (DWORD*)(uintptr_t(csgo.m_movement()) + 8);
		//something = *csgo.m_movement()->mv;

		csgo.m_globals()->curtime = (float)ctx.m_local()->m_nTickBase() * csgo.m_globals()->interval_per_tick;
		csgo.m_globals()->frametime = (ctx.m_local()->m_fFlags() & 0x40) ? 0.f : csgo.m_globals()->interval_per_tick;

		//restore_data.Setup(ctx.m_local());

		//const auto data = &Engine::Prediction::Instance()->m_Data[(csgo.m_client_state()->m_iCommandAck - 1) % 150];

		//if (data && data->tickbase == ctx.m_local()->m_nTickBase()/* || (ctx.last_usercmd->command_number - 1) == data->command_number*//* && data->command_number >= INT_MAX*/)
		//{
		//	Engine::Prediction::Instance()->FixNetvarCompression(csgo.m_client_state()->m_iCommandAck - 1);
		//}
	}

	float elapsed_revolver_time;

	using MD5_PseudoRandom_t = uint32_t(__thiscall*)(uint32_t);


	void Prediction::Predict(CUserCmd* cmd)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

		if (!cmd)
			return;

		if (!ctx.m_local() || ctx.m_local()->IsDead())
			return;

		m_pWeapon = (C_WeaponCSBaseGun*)(csgo.m_entity_list()->GetClientEntityFromHandle(ctx.m_local()->m_hActiveWeapon()));

		if (!m_pWeapon)
			return;

		//if (cmd->buttons == previous_predicted_cmd.buttons
		//	&& cmd->forwardmove == previous_predicted_cmd.forwardmove
		//	&& cmd->command_number == previous_predicted_cmd.command_number
		//	&& cmd->sidemove == previous_predicted_cmd.sidemove
		//	&& cmd->viewangles == previous_predicted_cmd.viewangles
		//	&& cmd->weaponselect == previous_predicted_cmd.weaponselect)
		//	return;

		/*if (csgo.m_client_state()->m_iDeltaTick > 0)
			csgo.m_prediction()->Update(csgo.m_client_state()->m_iDeltaTick, csgo.m_client_state()->m_iDeltaTick > 0, csgo.m_client_state()->m_iLastCommandAck,
				csgo.m_client_state()->m_iLastOutgoingCommand + csgo.m_client_state()->m_iChockedCommands);*/

		cmd->random_seed = reinterpret_cast<MD5_PseudoRandom_t>(Engine::Displacement::Signatures[c_signatures::MD5_PSEUDORANDOM])(cmd->command_number) & 0x7fffffff;

		csgo.m_globals()->curtime = TICKS_TO_TIME(ctx.m_local()->m_nTickBase());

		ctx.is_predicting = true;
		did_predict = true;
		float value = 0.0f;

		if (sv_footsteps)
			*(float*)(uintptr_t(sv_footsteps) + 0x2C) = /**(uint32_t*) & (value) ^ */(uint32_t)sv_footsteps ^ uint32_t(value);
		
		if (sv_min_jump_landing_sound)
			*(float*)(uintptr_t(sv_min_jump_landing_sound) + 0x2C) = (uint32_t)sv_min_jump_landing_sound ^ 0x7F7FFFFF;
	
		*(BYTE*)(uintptr_t(csgo.m_prediction()) + 0x18) = 0;
		csgo.m_prediction()->m_in_prediction = true;

		const auto velmod = ctx.m_local()->m_flVelocityModifier();

		ctx.m_local()->SetCurrentCommand(cmd);
		C_BaseEntity::SetPredictionRandomSeed(cmd);
		C_BaseEntity::SetPredictionPlayer(ctx.m_local());

		csgo.m_move_helper()->SetHost(ctx.m_local());
		csgo.m_prediction()->SetupMove(ctx.m_local(), cmd, csgo.m_move_helper(), &move_data);
		/*move_data.m_nButtons = cmd->buttons;
		move_data.m_nImpulseCommand = (unsigned __int8)cmd->impulse;
		move_data.m_flForwardMove = cmd->forwardmove;
		move_data.m_flSideMove = cmd->sidemove;
		move_data.m_flUpMove = cmd->upmove;
		move_data.m_vecAngles.x = cmd->viewangles.x;
		move_data.m_vecAngles.y = cmd->viewangles.y;
		move_data.m_vecAngles.z = cmd->viewangles.z;
		move_data.m_vecViewAngles.x = cmd->viewangles.x;
		move_data.m_vecViewAngles.y = cmd->viewangles.y;
		move_data.m_vecViewAngles.z = cmd->viewangles.z;*/
		auto v9 = ctx.m_local()->m_nTickBase();
		csgo.m_movement()->ProcessMovement(ctx.m_local(), &move_data);
		ctx.m_local()->m_nTickBase() = v9;
		csgo.m_prediction()->FinishMove(ctx.m_local(), cmd, &move_data);
		csgo.m_move_helper()->SetHost(nullptr);

		*(bool*)(uintptr_t(csgo.m_prediction()) + 0x18) = bFirstCommandPredicted;
		csgo.m_prediction()->m_in_prediction = m_bInPrediction;

		C_BaseEntity::SetPredictionRandomSeed(nullptr);
		C_BaseEntity::SetPredictionPlayer(nullptr);
		ctx.m_local()->SetCurrentCommand(0);
		ctx.m_local()->m_flVelocityModifier() = velmod;

		calculate_desync_delta();

		m_flSpread = FLT_MAX;
		m_flInaccuracy = FLT_MAX;
		m_flCalculatedInaccuracy = 0.f;

		if (m_weapon()) {
			m_weapon()->UpdateAccuracyPenalty();

			m_flSpread = m_weapon()->GetSpread();
			m_flInaccuracy = m_weapon()->GetInaccuracy();

			auto is_special_weapon = (m_weapon()->m_iItemDefinitionIndex() == 9
				|| m_weapon()->m_iItemDefinitionIndex() == 11
				|| m_weapon()->m_iItemDefinitionIndex() == 38
				|| m_weapon()->m_iItemDefinitionIndex() == 40);

			auto pweapon_info = ctx.latest_weapon_data;

			if (ctx.m_local()->m_fFlags() & FL_DUCKING)
			{
				if (is_special_weapon)
					m_flCalculatedInaccuracy = pweapon_info->flInaccuracyCrouchAlt;
				else
					m_flCalculatedInaccuracy = pweapon_info->flInaccuracyCrouch;
			}
			else if (is_special_weapon)
			{
				m_flCalculatedInaccuracy = pweapon_info->flInaccuracyStandAlt;
			}
			else
			{
				m_flCalculatedInaccuracy = pweapon_info->flInaccuracyStand;
			}
		}

		m_vecPredVelocity = ctx.m_local()->m_vecVelocity();
		m_vecPrePredVelocity = m_vecVelocity;
		prev_buttons = cmd->buttons;

		if (ctx.m_local()->get_animation_state()) {
			const auto absang = ctx.m_local()->get_abs_angles();

			const auto oldposeparam = *(float*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48));

			auto angles = QAngle(0.f, ctx.m_local()->get_animation_state()->m_abs_yaw, 0);
			ctx.m_local()->set_abs_angles(angles);

			auto eye_pitch = Math::normalize_angle(ctx.cmd_original_angles.x);

			if (eye_pitch > 180.f)
				eye_pitch = eye_pitch - 360.f;

			eye_pitch = Math::clamp(eye_pitch, -90, 90);
			*(float*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = Math::clamp((eye_pitch + 90.f) / 180.f, 0.0f, 1.0f);

			ctx.m_local()->force_bone_rebuild();

			const auto absorg = ctx.m_local()->get_abs_origin();
			ctx.m_local()->set_abs_origin(ctx.m_local()->m_vecOrigin());
			ctx.m_local()->SetupBonesEx(0x100);
			ctx.m_local()->set_abs_origin(absorg);

			ctx.m_local()->force_bone_cache();

			ctx.m_eye_position = ctx.m_local()->GetEyePosition(); //call weapon_shootpos

			*(float*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = oldposeparam;
			ctx.m_local()->set_abs_angles(absang);
			//}
		}

		prev_cmd_command_num = cmd->command_number;
		if (sv_footsteps)
		*(float*)(uintptr_t(sv_footsteps) + 0x2C) = sv_footsteps_backup;
		ctx.is_predicting = false;

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void Prediction::End()
	{
		//csgo.m_movement()->Reset();

		csgo.m_globals()->curtime = m_flCurrentTime;
		csgo.m_globals()->frametime = m_flFrameTime;
		//memcpy(&move_data_clear, &move_data, sizeof(CMoveData));
		//this->restore_data.is_filled = false;

		did_predict = false;
	}

	int Prediction::GetFlags()
	{
		return m_fFlags;
	}
	
	int Prediction::GetMoveType()
	{
		return m_MoveType;
	}

	Vector Prediction::GetVelocity()
	{
		return m_vecVelocity;
	}

	float Prediction::GetDuckAmount()
	{
		return m_flDuckAmount;
	}
	
	float Prediction::GetSpread()
	{
		return m_flSpread;
	}

	float Prediction::GetInaccuracy()
	{
		return m_flInaccuracy;
	}

	void Prediction::FixNetvarCompression(int time)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

		PlayerData* data = &m_Data[time % 150];

		if (data == nullptr || !data->is_filled || data->command_number != time || (time - data->command_number) > 150)
			return;

		const auto aim_punch_vel_diff = data->m_aimPunchAngleVel - ctx.m_local()->m_aimPunchAngleVel();
		const auto aim_punch_diff = data->m_aimPunchAngle - ctx.m_local()->m_aimPunchAngle();
		const auto viewpunch_diff = data->m_viewPunchAngle.x - ctx.m_local()->m_viewPunchAngle().x;
		const auto velocity_diff = data->m_vecVelocity - ctx.m_local()->m_vecVelocity();
		const auto origin_diff = data->m_vecOrigin - ctx.m_local()->m_vecOrigin();

		if (std::abs(aim_punch_diff.x) <= 0.03125f && std::abs(aim_punch_diff.y) <= 0.03125f && std::abs(aim_punch_diff.z) <= 0.03125f)
			ctx.m_local()->m_aimPunchAngle() = data->m_aimPunchAngle;

		if (std::abs(aim_punch_vel_diff.x) <= 0.03125f && std::abs(aim_punch_vel_diff.y) <= 0.03125f && std::abs(aim_punch_vel_diff.z) <= 0.03125f)
			ctx.m_local()->m_aimPunchAngleVel() = data->m_aimPunchAngleVel;

		if (std::abs(ctx.m_local()->m_vecViewOffset().z - data->m_vecViewOffset.z) <= 0.25f)
			ctx.m_local()->m_vecViewOffset().z = data->m_vecViewOffset.z;

		if (std::abs(viewpunch_diff) <= 0.03125f)
			ctx.m_local()->m_viewPunchAngle().x = data->m_viewPunchAngle.x;

		if (abs(ctx.m_local()->m_flDuckAmount() - data->m_flDuckAmount) <= 0.03125f)
			ctx.m_local()->m_flDuckAmount() = data->m_flDuckAmount;
		
		//if (abs(origin_diff.x) <= 0.03125f && abs(origin_diff.y) <= 0.03125f && abs(origin_diff.z) <= 0.03125f) {
		//	ctx.m_local()->m_vecOrigin() = data->m_vecOrigin;
			//ctx.m_local()->set_abs_origin(data->m_vecOrigin);
		//}
		
		if (std::abs(velocity_diff.x) <= 0.03125f && std::abs(velocity_diff.y) <= 0.03125f && std::abs(velocity_diff.z) <= 0.03125f)
			ctx.m_local()->m_vecVelocity() = data->m_vecVelocity;

		if (abs(ctx.m_local()->m_flThirdpersonRecoil() - data->m_flThirdpersonRecoil) <= 0.03125f)
			ctx.m_local()->m_flThirdpersonRecoil() = data->m_flThirdpersonRecoil;
		
		if (abs(ctx.m_local()->m_flDuckSpeed() - data->m_flDuckSpeed) <= 0.03125f)
			ctx.m_local()->m_flDuckSpeed() = data->m_flDuckSpeed;
		
		if (abs(ctx.m_local()->m_flFallVelocity() - data->m_flFallVelocity) <= 0.03125f)
			ctx.m_local()->m_flFallVelocity() = data->m_flFallVelocity;

		if (std::abs(ctx.m_local()->m_flVelocityModifier() - data->m_flVelocityModifier) <= 0.00625f)
			ctx.m_local()->m_flVelocityModifier() = data->m_flVelocityModifier;

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void Prediction::detect_prediction_error(PlayerData* m_data, int m_tick)
	{
		if (m_data 
			&& ctx.m_local() 
			&& m_data->command_number == m_tick)
		{
			if (m_data->is_filled && m_data->tick_count <= (ctx.current_tickcount + 8))
			{
				/*if (ctx.m_local()->m_flThirdpersonRecoil() >= (0.0f - std::numeric_limits<float>::epsilon()))
					ctx.m_local()->m_flThirdpersonRecoil() = 0.0f;*/
	
				auto viewPunch_delta = std::fabsf(ctx.m_local()->m_viewPunchAngle().x - m_data->m_viewPunchAngle.x);

				if (viewPunch_delta <= 0.03125f)
					ctx.m_local()->m_viewPunchAngle().x = m_data->m_viewPunchAngle.x;
				//else
				//	m_data->m_flThirdpersonRecoil = ctx.m_local()->m_flThirdpersonRecoil();

				int v34 = 1;
				int v35 = 1;
				int repredict = 0;
				int v37 = 1;

				if (std::abs(ctx.m_local()->m_aimPunchAngle().x - m_data->m_aimPunchAngle.x) > 0.03125f
					|| std::abs(ctx.m_local()->m_aimPunchAngle().y - m_data->m_aimPunchAngle.y) > 0.03125f
					|| std::abs(ctx.m_local()->m_aimPunchAngle().z - m_data->m_aimPunchAngle.z) > 0.03125f)
				{
					v34 = 0;
				}

				if (v34)
					ctx.m_local()->m_aimPunchAngle() = m_data->m_aimPunchAngle;
				else {
					m_data->m_aimPunchAngle = ctx.m_local()->m_aimPunchAngle();
					repredict = 1;
				}
	
				if (std::abs(ctx.m_local()->m_aimPunchAngleVel().x - m_data->m_aimPunchAngleVel.x) > 0.03125f
					|| std::abs(ctx.m_local()->m_aimPunchAngleVel().y - m_data->m_aimPunchAngleVel.y) > 0.03125f
					|| std::abs(ctx.m_local()->m_aimPunchAngleVel().z - m_data->m_aimPunchAngleVel.z) > 0.03125f)
				{
					v35 = 0;
				}

				if (v35)
					ctx.m_local()->m_aimPunchAngleVel() = m_data->m_aimPunchAngleVel;
				else {
					m_data->m_aimPunchAngleVel = ctx.m_local()->m_aimPunchAngleVel();
					repredict = 1;
				}

				auto v28 = std::abs(ctx.m_local()->m_vecViewOffset().z - m_data->m_vecViewOffset.z);
				if (v28 > 0.25f)
				{
					m_data->m_vecViewOffset.z = ctx.m_local()->m_vecViewOffset().z;
					repredict = 1;
				}
				else
					ctx.m_local()->m_vecViewOffset().z = m_data->m_vecViewOffset.z;

				if (std::abs(ctx.m_local()->m_vecVelocity().x - m_data->m_vecVelocity.x) > 0.03125f
					|| std::abs(ctx.m_local()->m_vecVelocity().y - m_data->m_vecVelocity.y) > 0.03125f
					|| std::abs(ctx.m_local()->m_vecVelocity().z - m_data->m_vecVelocity.z) > 0.03125f)
				{
					v37 = 0;
				}

				if (v37)
					ctx.m_local()->m_vecVelocity() = m_data->m_vecVelocity;
				else {
					m_data->m_vecVelocity = ctx.m_local()->m_vecVelocity();
					repredict = 1;
				}

				auto v29 = std::abs(m_data->m_flVelocityModifier - ctx.m_local()->m_flVelocityModifier());
				if (v29 > 0.00625f)
				{
					m_data->m_flVelocityModifier = ctx.m_local()->m_flVelocityModifier();
					//repredict = 1;
				}
				else
					ctx.m_local()->m_flVelocityModifier() = m_data->m_flVelocityModifier;
				//auto v30 = ctx.m_local()->m_vecOrigin();
				//bool v3 = false;

				/*if ((v30 - m_data->m_vecOrigin).LengthSquared() < 1.0f)
					v3 = v36;
				else
				{
					m_data->m_vecOrigin = v30;
					v3 = 1;
				}*/

				if ((ctx.m_local()->m_vecOrigin() - m_data->m_vecOrigin).LengthSquared() >= 1.f)
				{
					m_data->m_vecOrigin = ctx.m_local()->m_vecOrigin();
					repredict = 1;
				}
				//else
				//	ctx.m_local()->m_vecOrigin() = m_data->m_vecOrigin;

				if (repredict/* || m_data->m_nTickBase != ctx.m_local()->m_nTickBase()*/) {
					csgo.m_prediction()->force_repredict();
					//_events.push_back({ "prediction error detected." });
				}
			}
		}
	}

	void RestoreData::Setup(C_BasePlayer* player) {

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

		if (!ctx.m_local() || ctx.m_local()->IsDead())
			return;

		m_aimPunchAngle = player->m_aimPunchAngle();
		m_aimPunchAngleVel = player->m_aimPunchAngleVel();
		m_viewPunchAngle = player->m_viewPunchAngle();

		m_vecViewOffset = player->m_vecViewOffset();
		m_vecBaseVelocity = player->m_vecBaseVelocity();
		m_vecVelocity = player->m_vecVelocity();
		m_vecOrigin = player->m_vecOrigin();
		m_vecPreviouslyPredictedOrigin = *(Vector*)(uintptr_t(player) + 0x35A8);
		m_vecNetworkOrigin = *(Vector*)(uintptr_t(player) + 0x138);
		//m_flMaxspeed = *(float*)(uintptr_t(player) + 0x3248);
		//m_flWaterJumpTime = *(float*)(uintptr_t(player) + 0x320C);
		m_flFallVelocity = player->m_flFallVelocity();
		m_flVelocityModifier = player->m_flVelocityModifier();
		//m_nNextThinkTick = *(int*)(uintptr_t(ctx.m_local()) + 0xFC);
		//m_vecLocalOrigin = *(Vector*)(uintptr_t(ctx.m_local()) + 0xAC);
		//m_local_view_angles = ctx.m_local()->get_render_angles();
		m_flDuckAmount = player->m_flDuckAmount();
		m_flDuckSpeed = player->m_flDuckSpeed();
		m_iEFlags = player->m_iEFlags();

		m_surfaceFriction = player->m_surfaceFriction();
		m_flTimeLastTouchedGround = player->m_flTimeLastTouchedGround();

		m_hGroundEntity = player->m_hGroundEntity();
		m_RefEHandle = player->GetRefEHandle();
		m_nMoveType = player->m_MoveType();
		//m_iMoveState = player->m_iMoveState();
		m_MoveCollide = player->m_MoveCollide();
		m_nFlags = player->m_fFlags();
		m_nTickBase = player->m_nTickBase();

		auto weapon = player->get_weapon();
		if (weapon) {
			m_fAccuracyPenalty = weapon->m_fAccuracyPenalty();
			m_flRecoilIndex = weapon->m_flRecoilIndex();
			//m_Activity = weapon->m_Activity();
			//m_flLastShotTime = weapon->m_flLastShotTime();
		}

		is_filled = true;

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void RestoreData::Apply(C_BasePlayer* player) {

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		if (!is_filled)
			return;

		if (!ctx.m_local() || ctx.m_local()->IsDead())
			return;

		player->m_aimPunchAngle() = m_aimPunchAngle;
		player->m_aimPunchAngleVel() = m_aimPunchAngleVel;
		player->m_viewPunchAngle() = m_viewPunchAngle;

		player->m_vecViewOffset() = m_vecViewOffset;
		player->m_vecBaseVelocity() = m_vecBaseVelocity;
		player->m_vecVelocity() = m_vecVelocity;
		player->m_iEFlags() = m_iEFlags;
		//ctx.m_local()->get_render_angles() = m_local_view_angles;
		//player->m_flTimeLastTouchedGround() = m_flTimeLastTouchedGround;
		//*(int*)(uintptr_t(ctx.m_local()) + 0xFC) = m_nNextThinkTick;
		//*(float*)(uintptr_t(ctx.m_local()) + 0x3248) = m_flMaxspeed;
		//player->m_vecAbsVelocity() = m_vecAbsVelocity;
		//*(Vector*)(uintptr_t(player) + 0x138) = m_vecNetworkOrigin;
		//*(float*)(uintptr_t(player) + 0x320C) = m_flWaterJumpTime;
		//*(Vector*)(uintptr_t(player) + 0xAC) = m_vecLocalOrigin;
		*(Vector*)(uintptr_t(player) + 0x35A8) = m_vecPreviouslyPredictedOrigin;
		player->m_vecOrigin() = m_vecOrigin;

		player->m_flFallVelocity() = m_flFallVelocity;
		player->m_flVelocityModifier() = m_flVelocityModifier;
		player->m_flDuckAmount() = m_flDuckAmount;
		player->m_flDuckSpeed() = m_flDuckSpeed;

		player->m_surfaceFriction() = m_surfaceFriction;

		player->m_hGroundEntity() = m_hGroundEntity;
		//player->SetRefEHandle(m_RefEHandle);

		player->m_MoveType() = m_nMoveType;
		//player->m_iMoveState() = m_iMoveState;
		//player->m_MoveCollide() = m_MoveCollide;
		player->m_fFlags() = m_nFlags;
		player->m_nTickBase() = m_nTickBase;
		//player->m_bResumeZoom() = this->m_nTickBase;

		auto weapon = player->get_weapon();
		if (weapon) {
			weapon->m_fAccuracyPenalty() = m_fAccuracyPenalty;
			weapon->m_flRecoilIndex() = m_flRecoilIndex;
			//weapon->m_Activity() = m_Activity;
			//weapon->m_flLastShotTime() = m_flLastShotTime;
		}

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	//void Prediction::OnRunCommand(C_BasePlayer* player)
	//{
	//	auto local = ctx.m_local();
	//
	//	if (!local || local != player /*|| ctx.is_predicting*/)
	//		return;
	//
	//	auto data = &m_Data[player->m_nTickBase() % 150];
	//
	//	data->m_aimPunchAngle = local->m_aimPunchAngle();
	//	data->m_vecViewOffset = local->m_vecViewOffset();
	//	data->m_vecVelocity = local->m_vecVelocity();
	//	data->m_vecOrigin = local->m_vecOrigin();
	//	data->m_aimPunchAngleVel = local->m_aimPunchAngleVel();
	//
	//	data->m_flDuckAmount = local->m_flDuckAmount();
	//	/*data->m_flDuckSpeed = local->m_flDuckSpeed();
	//
	//	data->m_viewPunchAngle = local->m_viewPunchAngle();
	//	data->m_vecBaseVelocity = local->m_vecBaseVelocity();
	//	data->m_vecOrigin = local->m_vecOrigin();
	//
	//	data->m_flFallVelocity = local->m_flFallVelocity();
	//	data->m_flVelocityModifier = local->m_flVelocityModifier();
	//	data->m_hGroundEntity = local->m_hGroundEntity();
	//	data->m_nMoveType = local->m_MoveType();
	//	data->m_nFlags = local->m_fFlags();
	//
	//	if (auto wpn = m_weapon(); wpn != nullptr)
	//	{
	//		data->m_fAccuracyPenalty = wpn->m_fAccuracyPenalty();
	//		data->m_flRecoilIndex = wpn->m_flRecoilIndex();
	//	}*/
	//	data->tickbase = local->m_nTickBase();
	//
	//	//last_time_filled = csgo.m_client_state()->m_iCommandAck;
	//
	//	data->is_filled = true;
	//}
}