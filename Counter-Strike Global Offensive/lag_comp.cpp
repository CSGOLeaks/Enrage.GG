#include "rage_aimbot.hpp"
#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "math.hpp"
#include "displacement.hpp"
#include "lag_comp.hpp"
//#include "autowall.hpp"
#include "resolver.hpp"
//#include "game_movement.h"
#include "usercmd.hpp"
#include "anti_aimbot.hpp"
#include <unordered_map>
#include <algorithm>
#include "menu.hpp"
#include "visuals.hpp"
#include "prediction.hpp"
#include "movement.hpp"
#include "sound_parser.hpp"
#include "displacement.hpp"
#include "prop_manager.hpp"

#include <thread>

#define LAG_COMPENSATION_TICKS 32

void C_Tickrecord::store(C_BasePlayer* player, bool backup)
{
	//if (player != nullptr)
	//{
		//auto activity = player->get_sec_activity(player->get_animation_layer(1).m_nSequence);
		//auto shot_bt = ((activity >= ACT_CSGO_FIRE_PRIMARY && activity <= ACT_CSGO_FIRE_SECONDARY_OPT_2) && player->get_animation_layer(1).m_flWeight > 0.01f && player->get_animation_layer(1).m_flCycle < 0.05f) || (player->get_weapon() && player->get_weapon()->m_Activity() == 208);

		//type = RECORD_NORMAL;

		//auto priority = ((activity == ACT_CSGO_RELOAD) && player->get_animation_layer(1).m_flWeight > 0.001f && player->get_animation_layer(1).m_flCycle < 1.f);
		shot_this_tick = false;
		valid = false;
		dormant = false;

		/*if (auto wpn = player->get_weapon(); wpn != nullptr)
			shot_time = wpn->m_flLastShotTime();
		else
			shot_time = 0;*/

		//if (priority)
		//	type = RECORD_PRIORITY;

		bones_count = player->m_bone_count();
		bones_count = Math::clamp(bones_count, 0, 128);

		if (backup) {
			memcpy(matrixes, player->m_CachedBoneData().Base(), bones_count * sizeof(matrix3x4_t));
			valid = false;
			dormant = false;
			animated = true;
			exploit = false;
		}
		//memcpy(leftmatrixes, feature::resolver->player_records[player->entindex() - 1].left_mx, bones_count * sizeof(matrix3x4_t));
		//memcpy(rightmatrixes, feature::resolver->player_records[player->entindex() - 1].right_mx, bones_count * sizeof(matrix3x4_t));

		//memcpy(leftlmatrixes, feature::resolver->player_records[player->entindex() - 1].left_lmx, bones_count * sizeof(matrix3x4_t));
		//memcpy(rightlmatrixes, feature::resolver->player_records[player->entindex() - 1].right_lmx, bones_count * sizeof(matrix3x4_t));

		//left_side = feature::resolver->player_records[player->entindex() - 1].left_side;
		//right_side = feature::resolver->player_records[player->entindex() - 1].right_side;

		//resolver_index = 0;

		origin = player->m_vecOrigin();
		abs_origin = player->get_abs_origin();
		velocity = player->m_vecVelocity();
		animation_time = feature::lagcomp->get_interpolated_time();
		object_mins = player->OBBMins();
		object_maxs = player->OBBMaxs();
		eye_angles = player->m_angEyeAngles();
		abs_angles = player->get_abs_angles().y;
		entity_flags = player->m_fFlags();
		simulation_time = player->m_flSimulationTime();
		simulation_time_old = player->m_flOldSimulationTime();
		lower_body_yaw = player->m_flLowerBodyYawTarget();
		time_of_last_injury = player->m_flTimeOfLastInjury();
		velocity_modifier = player->m_flVelocityModifier();
		//anim_velocity = player->m_vecVelocity();
		ientity_flags = player->m_iEFlags();
		duck_amt = player->m_flDuckAmount();
		ground_accel_last_time = player->m_flGroundAccelLinearFracLastTime();

		if (!backup)
			head_pos = player->get_bone_pos(8);
		if (!backup)
			desync_delta = feature::anti_aim->get_max_desync_delta(player);

		thirdperson_recoil = player->m_flThirdpersonRecoil();
		stop_to_full_run_frac = player->get_animation_state() ? player->get_animation_state()->m_walk_run_transition : 0.f;

		if (auto weapon = player->get_weapon(); weapon != nullptr && weapon)
			shot_time = weapon->m_flLastShotTime();
		else
			shot_time = -1;

		lag = TIME_TO_TICKS(player->m_flSimulationTime() - player->m_flOldSimulationTime());

		// clamp it so we don't interpolate too far : )
		lag = Math::clamp(lag, 0, 31);

		time_delta = player->m_flSimulationTime() - player->m_flOldSimulationTime();

		if (*(void**)player && player->get_animation_state())
			memcpy(&animstate, player->get_animation_state(), 0x334);

		/*pose_paramaters.fill(0);
		left_poses.fill(0);
		right_poses.fill(0);*/
		fill(begin(pose_paramaters), end(pose_paramaters), 0.f);
		if (!backup) {
			fill(begin(left_poses), end(left_poses), 0.f);
			fill(begin(right_poses), end(right_poses), 0.f);
		}

		if (player->get_animation_layers_count() > 0)
			memcpy(anim_layers, player->animation_layers_ptr(), 0x38 * player->get_animation_layers_count());

		//*(&player->get_bone_accessor()->m_WritableBones + 8) = m_writable_bones;
		//readable_bones_count = player->GetBoneAccessor().m_ReadableBones;
		breaking_lc = false;

		tickcount = ctx.current_tickcount;
		simulation_time_delay = csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - TIME_TO_TICKS(player->m_flSimulationTime());

		lc_exploit = simulation_time_delay >= 12;

		if (csgo.m_client_state())
			m_tick = csgo.m_client_state()->m_clockdrift_manager.m_nServerTick;

		latency = ctx.latency[FLOW_INCOMING];

		not_desyncing = false;
		data_filled = true;
	//}
}

void C_Tickrecord::apply(C_BasePlayer* player, bool backup, bool dont_force_matrix)
{
		//auto curr = feature::resolver->player_records[entity->entindex() - 1].resolving_method;

		//if (force && curr != 0 && !entity->IsBot())
		//{
		//	std::memcpy(entity->m_CachedBoneData().Base(), /*(lower_delta ? (curr < 0 ? record_data->leftlmatrixes : record_data->rightlmatrixes) : */(curr < 0 ? record_data->leftmatrixes : record_data->rightmatrixes)/*)*/, record_data->bones_count * sizeof(matrix3x4_t));
		//	entity->set_abs_angles(QAngle(0, (curr < 0 ? record_data->left_side : record_data->right_side), 0));
		//}
		//else {

		//auto r_log = &feature::resolver->player_records[player->entindex() - 1];

		const auto need_rotate = !backup && resolver_index != 0 && data_filled && animated;

		player->set_abs_angles(QAngle(0, (need_rotate && !player->IsBot() ? (resolver_index == 1 ? left_side : right_side) : abs_angles), 0));
		player->m_vecOrigin() = origin;
		player->set_collision_bounds(object_mins, object_maxs);
		player->set_abs_origin(backup ? abs_origin : origin);

		if (backup && player->get_animation_layers_count() > 0)
			memcpy(player->animation_layers_ptr(), anim_layers, sizeof(C_AnimationLayer) * player->get_animation_layers_count());

		if (!dont_force_matrix || backup)
		{
			if (bones_count > 0) {

				if (!need_rotate || player->IsBot())
					memcpy(player->m_CachedBoneData().Base(), matrixes, bones_count * sizeof(matrix3x4_t));
				else
					memcpy(player->m_CachedBoneData().Base(), (resolver_index == 1 ? leftmatrixes : rightmatrixes), bones_count * sizeof(matrix3x4_t));

				player->m_bone_count() = bones_count;
				player->force_bone_cache();
			}
		}
}

void c_lagcomp::backup_players(bool restore) {

	if (!ctx.m_local() || ctx.m_local()->IsDead())
		return;

	if (restore) {
		// restore stuff.
		for (int i{ 0 }; i <= csgo.m_globals()->maxClients; ++i) {
			auto player = csgo.m_entity_list()->GetClientEntity(i);

			if (player == nullptr || player == ctx.m_local() || player->entindex() <= 0 || player->entindex() >= 64 || player->IsDead() || player->IsDormant())
				continue;

			auto data = &feature::lagcomp->records[player->entindex() - 1];

			if (!data || data->records_count <= 1 || data->player != player || !data->restore_record.data_filled)
				continue;

			data->restore_record.apply(player, true);
			data->is_restored = true;
			data->restore_record.data_filled = false;
		}
	}

	else {
		// backup stuff.
		for (int i{ 0 }; i <= csgo.m_globals()->maxClients; ++i) {
			auto player = csgo.m_entity_list()->GetClientEntity(i);

			if (player == nullptr || player == ctx.m_local() || player->entindex() <= 0 || player->entindex() >= 64 || player->IsDead() || player->IsDormant())
				continue;

			auto data = &feature::lagcomp->records[player->entindex() - 1];

			if (!data || data == nullptr || data->player != player || data->records_count <= 1 )
				continue;

			data->restore_record.store(player, true);
		}
	}
}

//bool c_lagcomp::StartPrediction(C_BasePlayer* player, c_player_records* data) {
//
//	static int tick = ctx.current_tickcount;
//
//	static auto sv_gravity = csgo.m_engine_cvars()->FindVar("sv_gravity");
//	static auto sv_enablebunnyhopping = csgo.m_engine_cvars()->FindVar("sv_enablebunnyhopping");
//	static auto sv_jump_impulse = csgo.m_engine_cvars()->FindVar("sv_jump_impulse");
//
//	// we have no data to work with.
//	// this should never happen if we call this
//	if (data->tick_records.empty())
//		return false;
//
//	// meme.
//	if (!player || player != data->player || data->player->IsDormant())
//		return false;
//
//	// compute the true amount of updated records
//	// since the last time the player entered pvs.
//	size_t size{};
//
//	// iterate records.
//	for (const auto& it : data->tick_records) {
//		if (it.dormant)
//			break;
//
//		// increment total amount of data.
//		++size;
//	}
//
//	static float m_latency;
//	static int m_latency_ticks;
//	static int m_server_tick;
//	static int m_arrival_tick;
//
//	if (tick != ctx.current_tickcount)
//	{
//		m_latency = csgo.m_engine()->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
//		m_latency = Math::clamp(m_latency, 0.f, 1.f);
//		m_latency_ticks = TIME_TO_TICKS(m_latency);
//		m_server_tick = *(int*)(uintptr_t(csgo.m_client_state()) + 0x170);
//		m_arrival_tick = m_server_tick + m_latency_ticks;
//		tick = ctx.current_tickcount;
//	}
//
//	// get first record.
//	C_Tickrecord* record = &data->tick_records[0];
//
//	// reset all prediction related variables.
//	// this has been a recurring problem in all my hacks lmfao.
//	// causes the prediction to stack on eachother.
//	record->setup_simulation();
//
//	// check if lc broken.
//	if (size > 1 && ((record->origin - data->tick_records[1].origin).LengthSquared() > 4096.f
//		|| size > 2 && (data->tick_records[1].origin - data->tick_records[2].origin).LengthSquared() > 4096.f))
//		record->m_broke_lc = true;
//
//	// we are not breaking lagcomp at this point.
//	// return false so it can aim at all the records it once
//	// since server-sided lagcomp is still active and we can abuse that.
//	if (!record->m_broke_lc)
//		return false;
//
//	int simulation = TIME_TO_TICKS(record->simulation_time);
//
//	// this is too much lag to fix.
//	if (std::abs(m_arrival_tick - simulation) >= 128)
//		return true;
//
//	// compute the amount of lag that we will predict for, if we have one set of data, use that.
//	// if we have more data available, use the prevoius lag delta to counter weird fakelags that switch between 14 and 2.
//	int lag = (size <= 2) ? TIME_TO_TICKS(record->simulation_time - data->tick_records[1].simulation_time)
//		: TIME_TO_TICKS(data->tick_records[1].simulation_time - data->tick_records[2].simulation_time);
//
//	// clamp this just to be sure.
//	Math::clamp(lag, 1, 15);
//
//	// get the delta in ticks between the last server net update
//	// and the net update on which we created this record.
//	int updatedelta = (*(int*)(uintptr_t(csgo.m_client_state()) + 0x170)) - record->m_tick;
//
//	// if the lag delta that is remaining is less than the current netlag
//	// that means that we can shoot now and when our shot will get processed
//	// the origin will still be valid, therefore we do not have to predict.
//	if (m_latency_ticks <= lag - updatedelta)
//		return true;
//
//	// the next update will come in, wait for it.
//	int next = record->m_tick + 1;
//	if (next + lag >= m_arrival_tick)
//		return true;
//
//	float change = 0.f, dir = 0.f;
//
//	// get the direction of the current velocity.
//	if (record->velocity.y != 0.f || record->velocity.x != 0.f)
//		dir = RAD2DEG(std::atan2(record->velocity.y, record->velocity.x));
//
//	// we have more than one update
//	// we can compute the direction.
//	if (size > 1) {
//		// get the delta time between the 2 most recent records.
//		float dt = record->simulation_time - data->tick_records[1].simulation_time;
//
//		// init to 0.
//		float prevdir = 0.f;
//
//		// get the direction of the prevoius velocity.
//		if (data->tick_records[1].velocity.y != 0.f || data->tick_records[1].velocity.x != 0.f)
//			prevdir = RAD2DEG(std::atan2(data->tick_records[1].velocity.y, data->tick_records[1].velocity.x));
//
//		// compute the direction change per tick.
//		change = (Math::normalize_angle(dir - prevdir) / dt) * csgo.m_globals()->interval_per_tick;
//	}
//
//	if (std::abs(change) > 6.f)
//		change = 0.f;
//
//	// get the pointer to the players animation state.
//	CCSGOPlayerAnimState* state = player->get_animation_state();
//
//	// backup the animation state.
//	CCSGOPlayerAnimState backup{};
//
//	if (state)
//		std::memcpy(&backup, state, sizeof(CCSGOPlayerAnimState));
//
//	// add in the shot prediction here.
//	//int shot = 0;
//
//	//auto pWeapon = player->get_weapon( );
//	//if( pWeapon && pWeapon->IsGun()/*&& !data->m_fire_bullet.empty( )*/ ) {
//
//	//	//static Address offset = g_netvars.get( HASH( "DT_BaseCombatWeapon" ), HASH( "m_fLastShotTime" ) );
//	//	//float last = pWeapon->get< float >( offset );
//
//	//	if( TIME_TO_TICKS( player->m_flSimulationTime() - pWeapon->m_flLastShotTime() ) == 1 ) {
//	//		auto wpndata = pWeapon->GetCSWeaponData( );
//
//	//		if( wpndata )
//	//			shot = TIME_TO_TICKS(player->m_flSimulationTime() + wpndata->flCycleTime) + 1;
//	//	}
//	//}
//
//	int pred = 0;
//
//	// start our predicton loop.
//	while (true) {
//		// can the player shoot within his lag delta.
//		//if( shot && shot >= simulation && shot < simulation + lag ) {
//
//		//	// if so his new lag will be the time until he shot again.
//		//	lag = shot - simulation;
//		//	Math::clamp( lag, 3, 15 );
//
//		//	// only predict a shot once.
//		//	shot = 0;
//		//}
//
//		// see if by predicting this amount of lag
//		// we do not break stuff.
//		next += lag;
//		if (next >= m_arrival_tick)
//			break;
//
//		// predict lag.
//		for (int sim{}; sim < lag; ++sim) {
//			// predict movement direction by adding the direction change per tick to the previous direction.
//			// make sure to normalize it, in case we go over the -180/180 turning point.
//			dir = Math::normalize_angle(dir + change);
//
//			// pythagorean theorem
//			// a^2 + b^2 = c^2
//			// we know a and b, we square them and add them together, then root.
//			float hyp = record->m_pred_velocity.Length2D();
//
//			// compute the base velocity for our new direction.
//			// since at this point the hypotenuse is known for us and so is the angle.
//			// we can compute the adjacent and opposite sides like so:
//			// cos(x) = a / h -> a = cos(x) * h
//			// sin(x) = o / h -> o = sin(x) * h
//			record->m_pred_velocity.x = std::cos(DEG2RAD(dir)) * hyp;
//			record->m_pred_velocity.y = std::sin(DEG2RAD(dir)) * hyp;
//
//			// we hit the ground, set the upwards impulse and apply CS:GO speed restrictions.
//			if (record->m_pred_flags & FL_ONGROUND) {
//				if (!sv_enablebunnyhopping->GetInt()) {
//
//					// 260 x 1.1 = 286 units/s.
//					float max = (player->get_weapon() ? player->get_weapon()->GetMaxWeaponSpeed() : 260.f) * 1.1f;
//
//					// get current velocity.
//					float speed = record->m_pred_velocity.Length();
//
//					// reset velocity to 286 units/s.
//					if (max > 0.f && speed > max)
//						record->m_pred_velocity *= (max / speed);
//				}
//
//				// assume the player is bunnyhopping here so set the upwards impulse.
//				record->m_pred_velocity.z = sv_jump_impulse->GetFloat();
//			}
//
//			// we are not on the ground
//			// apply gravity and airaccel.
//			else {
//				// apply one tick of gravity.
//				record->m_pred_velocity.z -= sv_gravity->GetFloat() * csgo.m_globals()->interval_per_tick;
//
//				// compute the ideal strafe angle for this velocity.
//				float speed2d = record->m_pred_velocity.Length2D();
//				float ideal = (speed2d > 0.f) ? RAD2DEG(std::asin(15.f / speed2d)) : 90.f;
//				Math::clamp(ideal, 0.f, 90.f);
//
//				float smove = 0.f;
//				float abschange = std::abs(change);
//
//				if (abschange <= ideal || abschange >= 30.f) {
//					static float mod{ 1.f };
//
//					dir += (ideal * mod);
//					smove = 450.f * mod;
//					mod *= -1.f;
//				}
//
//				else if (change > 0.f)
//					smove = -450.f;
//
//				else
//					smove = 450.f;
//
//				// apply air accel.
//				AirAccelerate(player, record, QAngle{ 0.f, dir, 0.f }, 0.f, smove);
//			}
//
//			// predict player.
//			// convert newly computed velocity
//			// to origin and flags.
//			PlayerMove(record);
//
//			// move time forward by one.
//			record->m_pred_time += csgo.m_globals()->interval_per_tick;
//
//			// increment total amt of predicted ticks.
//			++pred;
//
//			// the server animates every first choked command.
//			// therefore we should do that too.
//			if (/*sim == 0 && */state)
//				PredictAnimations(player, state, record);
//		}
//	}
//
//	// restore state.
//	if (state)
//		std::memcpy(state, &backup, sizeof(CCSGOPlayerAnimState));
//
//	if (pred <= 0)
//		return true;
//
//	// lagcomp broken, invalidate bones.
//	//record->invalidate();
//
//	// re-setup bones for this record.
//	//g_bones.setup(data->player, nullptr, record);
//	player->force_bone_rebuild();
//	player->SetupBonesEx();
//
//	return true;
//}
//
//void c_lagcomp::PlayerMove(C_Tickrecord* record) {
//	Vector                start, end, normal;
//	CGameTrace            trace;
//	CTraceFilterWorldOnly filter;
//
//	// define trace start.
//	start = record->m_pred_origin;
//
//	// move trace end one tick into the future using predicted velocity.
//	end = start + (record->m_pred_velocity * csgo.m_globals()->interval_per_tick);
//
//	// trace.
//	Ray_t t; t.Init(start, end, record->object_mins, record->object_maxs);
//
//	csgo.m_engine_trace()->TraceRay(t, CONTENTS_SOLID, &filter, &trace);
//
//	// we hit shit
//	// we need to fix hit.
//	if (trace.fraction != 1.f) {
//
//		// fix sliding on planes.
//		for (int i{}; i < 2; ++i) {
//			record->m_pred_velocity -= trace.plane.normal * record->m_pred_velocity.Dot(trace.plane.normal);
//
//			float adjust = record->m_pred_velocity.Dot(trace.plane.normal);
//			if (adjust < 0.f)
//				record->m_pred_velocity -= (trace.plane.normal * adjust);
//
//			start = trace.endpos;
//			end = start + (record->m_pred_velocity * (csgo.m_globals()->interval_per_tick * (1.f - trace.fraction)));
//
//			Ray_t td; td.Init(start, end, record->object_mins, record->object_maxs);
//			csgo.m_engine_trace()->TraceRay(td, CONTENTS_SOLID, &filter, &trace);
//			if (trace.fraction == 1.f)
//				break;
//		}
//	}
//
//	// set new final origin.
//	start = end = record->m_pred_origin = trace.endpos;
//
//	// move endpos 2 units down.
//	// this way we can check if we are in/on the ground.
//	end.z -= 2.f;
//
//	Ray_t tz; tz.Init(start, end, record->object_mins, record->object_maxs);
//
//	// trace.
//	csgo.m_engine_trace()->TraceRay(tz, CONTENTS_SOLID, &filter, &trace);
//
//	// strip onground flag.
//	record->m_pred_flags &= ~FL_ONGROUND;
//
//	// add back onground flag if we are onground.
//	if (trace.fraction != 1.f && trace.plane.normal.z > 0.7f)
//		record->m_pred_flags |= FL_ONGROUND;
//}
//
//void c_lagcomp::AirAccelerate(C_BasePlayer* m_player,C_Tickrecord* record, QAngle angle, float fmove, float smove) {
//	Vector fwd, right, wishvel, wishdir;
//	float  maxspeed, wishspd, wishspeed, currentspeed, addspeed, accelspeed;
//	static auto sv_airaccelerate = csgo.m_engine_cvars()->FindVar("sv_airaccelerate");
//
//	// determine movement angles.
//	Math::AngleVectors(angle, &fwd, &right, nullptr);
//
//	// zero out z components of movement vectors.
//	fwd.z = 0.f;
//	right.z = 0.f;
//
//	// normalize remainder of vectors.
//	fwd.Normalize();
//	right.Normalize();
//
//	// determine x and y parts of velocity.
//	for (int i{}; i < 2; ++i)
//		wishvel[i] = (fwd[i] * fmove) + (right[i] * smove);
//
//	// zero out z part of velocity.
//	wishvel.z = 0.f;
//
//	// determine maginitude of speed of move.
//	wishdir = wishvel;
//	wishspeed = wishdir.Normalize();
//
//	// get maxspeed.
//	// TODO; maybe global this or whatever its 260 anyway always.
//	maxspeed = m_player->get_weapon() ? m_player->get_weapon()->GetMaxWeaponSpeed() : 260.f;
//
//	// clamp to server defined max speed.
//	if (wishspeed != 0.f && wishspeed > maxspeed)
//		wishspeed = maxspeed;
//
//	// make copy to preserve original variable.
//	wishspd = wishspeed;
//
//	// cap speed.
//	if (wishspd > 30.f)
//		wishspd = 30.f;
//
//	// determine veer amount.
//	currentspeed = record->m_pred_velocity.Dot(wishdir);
//
//	// see how much to add.
//	addspeed = wishspd - currentspeed;
//
//	// if not adding any, done.
//	if (addspeed <= 0.f)
//		return;
//
//	// Determine acceleration speed after acceleration
//	accelspeed = sv_airaccelerate->GetFloat() * wishspeed * csgo.m_globals()->interval_per_tick;
//
//	// cap it.
//	if (accelspeed > addspeed)
//		accelspeed = addspeed;
//
//	// add accel.
//	record->m_pred_velocity += (wishdir * accelspeed);
//}
//
//void c_lagcomp::PredictAnimations(C_BasePlayer* m_player, CCSGOPlayerAnimState* state, C_Tickrecord* record) {
//	struct AnimBackup_t {
//		float  curtime;
//		float  frametime;
//		int    flags;
//		int    eflags;
//		Vector velocity;
//	};
//
//	const auto resolver_info = &feature::resolver->player_records[m_player->entindex()-1];
//
//	// backup data.
//	AnimBackup_t backup;
//	backup.curtime = csgo.m_globals()->curtime;
//	backup.frametime = csgo.m_globals()->frametime;
//	backup.flags = m_player->m_fFlags();
//	backup.eflags = m_player->m_iEFlags();
//	backup.velocity = m_player->m_vecAbsVelocity();
//
//	state->last_anim_upd_time = record->m_pred_time - csgo.m_globals()->interval_per_tick;
//
//	// set globals appropriately for animation.
//	csgo.m_globals()->curtime = record->m_pred_time;
//	csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
//
//	// EFL_DIRTY_ABSVELOCITY
//	// skip call to C_BaseEntity::CalcAbsoluteVelocity
//	m_player->m_iEFlags() &= ~0x1000;
//
//	// set predicted flags and velocity.
//	m_player->m_fFlags() = record->m_pred_flags;
//	m_player->m_vecAbsVelocity() = record->m_pred_velocity;
//
//	// enable re-animation in the same frame if animated already.
//	if (state->last_anim_upd_tick >= csgo.m_globals()->framecount)
//		state->last_anim_upd_tick = csgo.m_globals()->framecount - 1;
//
//	////bool fake = g_menu.main.aimbot.correct.get();
//	//
//	//// rerun the resolver since we edited the origin.
//	//if (fake)
//	//	g_resolver.ResolveAngles(player, record);
//	//
//	//// update animations.
//	//game::UpdateAnimationState(state, record->m_eye_angles);
//
//	record->can_rotate = record->valid && !m_player->IsBot();
//
//	m_player->m_angEyeAngles() = record->eye_angles;
//
//	feature::resolver->store_data(m_player, record, true);
//
//	if (record->can_rotate && resolver_info->resolving_method > 0)
//		m_player->get_animation_state()->abs_yaw = Math::normalize_angle(record->eye_angles.y + (resolver_info->resolving_method == 1 ? -60.f : 60.f));
//
//	/* update animations. */
//	ctx.updating_resolver = true;
//	m_player->update_clientside_animations();
//	ctx.updating_resolver = false;
//
//	//// rerun the pose correction cuz we are re-setupping them.
//	//if (fake)
//	//	g_resolver.ResolvePoses(player, record);
//
//	// get new rotation poses and layers.
//	//player->GetPoseParameters(record->m_poses);
//	//player->GetAnimLayers(record->m_layers);
//	//record->m_abs_ang = player->GetAbsAngles();
//
//	record->animstate = *m_player->get_animation_state();
//
//	// restore globals.
//	csgo.m_globals()->curtime = backup.curtime;
//	csgo.m_globals()->frametime = backup.frametime;
//
//	// restore player data.
//	m_player->m_fFlags() = backup.flags;
//	m_player->m_iEFlags() = backup.eflags;
//	m_player->m_vecAbsVelocity() = backup.velocity;
//}

void c_lagcomp::update_lerp()
{
	VIRTUALIZER_FISH_LITE_START;
	static auto cl_interp = csgo.m_engine_cvars()->FindVar(sxor("cl_interp"));
	static auto cl_updaterate = csgo.m_engine_cvars()->FindVar(sxor("cl_updaterate"));
	static auto cl_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("cl_interp_ratio"));

	const auto a2 = cl_updaterate->GetFloat();
	const auto a1 = cl_interp->GetFloat();
	const auto v2 = cl_interp_ratio->GetFloat() / a2;
	ctx.lerp_time = fmaxf(a1, v2);

	VIRTUALIZER_FISH_LITE_END;
}

void c_lagcomp::update_network_info()
{
	VIRTUALIZER_FISH_LITE_START;
	VIRTUALIZER_STR_ENCRYPT_START;
	//static auto cl_interp = csgo.m_engine_cvars()->FindVar(sxor("cl_interp"));
	//static auto cl_updaterate = csgo.m_engine_cvars()->FindVar(sxor("cl_updaterate"));
	//static auto cl_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("cl_interp_ratio"));
	////static auto pMin = csgo.m_engine_cvars()->FindVar("sv_client_min_interp_ratio");
	////static auto pMax = csgo.m_engine_cvars()->FindVar("sv_client_max_interp_ratio");

	///*static auto sv_client_min_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("sv_client_min_interp_ratio"));
	//static auto sv_client_max_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("sv_client_max_interp_ratio"));
	//static auto sv_minupdaterate = csgo.m_engine_cvars()->FindVar(sxor("sv_minupdaterate"));
	//static auto sv_maxupdaterate = csgo.m_engine_cvars()->FindVar(sxor("sv_maxupdaterate"));

	//auto updaterate = std::clamp(cl_updaterate->GetFloat(), sv_minupdaterate->GetFloat(), sv_maxupdaterate->GetFloat());
	//auto lerp_ratio = std::clamp(cl_interp_ratio->GetFloat(), sv_client_min_interp_ratio->GetFloat(), sv_client_max_interp_ratio->GetFloat());

	//ctx.lerp_time = std::clamp(lerp_ratio / updaterate, cl_interp->GetFloat(), 1.0f);*/

	//const auto a2 = cl_updaterate->GetFloat();
	//const auto a1 = cl_interp->GetFloat();
	//const auto v2 = cl_interp_ratio->GetFloat() / a2;
	//ctx.lerp_time = fmaxf(a1, v2);

	/*float flUpdateRateValue = double(cl_updaterate->GetFloat());

	if (!csgo.m_engine()->IsHLTV())
	{
		static const ConVar* pMinUpdateRate = csgo.m_engine_cvars()->FindVar("sv_minupdaterate");
		static const ConVar* pMaxUpdateRate = csgo.m_engine_cvars()->FindVar("sv_maxupdaterate");
		if (pMinUpdateRate && pMaxUpdateRate)
			flUpdateRateValue = Math::clamp(flUpdateRateValue, pMinUpdateRate->GetFloat(), pMaxUpdateRate->GetFloat());
	}

	float flLerpRatio = double(cl_interp_ratio->GetFloat());

	if (flLerpRatio == 0)
		flLerpRatio = 1.0f;

	float flLerpAmount = double(cl_interp->GetFloat());

	if (pMin && pMax && pMin->GetFloat() != -1)
	{
		flLerpRatio = Math::clamp(flLerpRatio, pMin->GetFloat(), pMax->GetFloat());
	}
	else
	{
		if (flLerpRatio == 0)
			flLerpRatio = 1.0f;
	}

	ctx.lerp_time = fmax(flLerpAmount, flLerpRatio / flUpdateRateValue);*/

	//static float latency;
	//static int latency_count;

	//else {
	//	latency = 0.f;
	//	latency_count = 0;
	//}

	if (auto net = csgo.m_engine()->GetNetChannelInfo(); net != nullptr) {

		//latency_count++;
		//latency += net->GetLatency(FLOW_OUTGOING);

		//if (latency_count <= 3) {
		ctx.latency[FLOW_OUTGOING] = net->GetLatency(FLOW_OUTGOING);
		//}
		//else
		//{
		//	ctx.latency[FLOW_OUTGOING] = latency / latency_count;
		//}

		ctx.latency[FLOW_INCOMING] = net->GetLatency(FLOW_INCOMING);

		ctx.avglatency[FLOW_OUTGOING] = net->GetAvgLatency(FLOW_OUTGOING);
		ctx.avglatency[FLOW_INCOMING] = net->GetAvgLatency(FLOW_INCOMING);
	}
	else {
		ctx.latency[FLOW_OUTGOING] = csgo.m_globals()->interval_per_tick;
		ctx.latency[FLOW_OUTGOING] = 0;
	}

	//if (latency_count > 200)
	//{
	//	latency = 0.f;
	//	latency_count = 0;
	//}
	/*const auto update_rate = cl_updaterate->GetInt();
	const auto interp_ratio = cl_interp->GetFloat();

	auto lerp = interp_ratio / update_rate;

	if (lerp <= cl_interp->GetFloat())
		lerp = cl_interp->GetFloat();

	ctx.lerp_time = lerp;*/
	VIRTUALIZER_STR_ENCRYPT_END;
	VIRTUALIZER_FISH_LITE_END;
}

void c_lagcomp::simulate_movement(C_Simulationdata& data) {

	static auto sv_gravity = csgo.m_engine_cvars()->FindVar(sxor("sv_gravity"));
	static auto sv_jump_impulse = csgo.m_engine_cvars()->FindVar(sxor("sv_jump_impulse"));

	if (!(data.flags & FL_ONGROUND)) {
		data.velocity.z -= (csgo.m_globals()->interval_per_tick * sv_gravity->GetFloat() * 0.5f);
	}
	else if (data.jumped) {
		data.jumped = false;
		data.velocity.z = sv_jump_impulse->GetFloat();
		data.flags &= ~FL_ONGROUND;
	}

	// can't step up onto very steep slopes
	static const float MIN_STEP_NORMAL = 0.7f;

	if (!data.velocity.IsZero()) {
		auto* collidable = data.entity->GetCollideable();
		const Vector mins = collidable->OBBMins();
		const Vector max = collidable->OBBMaxs();

		const Vector src = data.origin;
		Vector end = src + (data.velocity * csgo.m_globals()->interval_per_tick);

		Ray_t ray;
		ray.Init(src, end, mins, max);

		CGameTrace trace;
		CTraceFilter filter;
		filter.pSkip = data.entity;

		csgo.m_engine_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);

		// CGameMovement::TryPlayerMove
		if (trace.fraction != 1) {
			// BUGFIXME: is it should be 4? ( not 2 )
			for (int i = 0; i < 2; i++) {
				// decompose velocity into plane
				data.velocity -= trace.plane.normal * data.velocity.Dot(trace.plane.normal);

				const float dot = data.velocity.Dot(trace.plane.normal);
				if (dot < 0.f) { // moving against plane
					data.velocity.x -= dot * trace.plane.normal.x;
					data.velocity.y -= dot * trace.plane.normal.y;
					data.velocity.z -= dot * trace.plane.normal.z;
				}

				end = trace.endpos + (data.velocity * (csgo.m_globals()->interval_per_tick * (1.f - trace.fraction)));

				ray.Init(trace.endpos, end, mins, max);
				csgo.m_engine_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);
				if (trace.fraction == 1)
					break;
			}
		}

		data.origin = trace.endpos;
		end = trace.endpos;
		end.z -= 2;

		ray.Init(data.origin, end, mins, max);
		csgo.m_engine_trace()->TraceRay(ray, MASK_SOLID, &filter, &trace);

		data.flags &= ~FL_ONGROUND;

		if (trace.DidHit() && trace.plane.normal.z >= MIN_STEP_NORMAL) {
			data.flags |= FL_ONGROUND;
		}

		if (data.flags & FL_ONGROUND)
			data.velocity.z = 0;
		else
			data.velocity.z -= csgo.m_globals()->interval_per_tick * sv_gravity->GetFloat() * 0.5f;
	}
}

bool c_lagcomp::is_time_delta_too_large(C_Tickrecord* wish_record, bool ignore_deadtime)
{
	//if (max(ctx.latency[FLOW_INCOMING], ctx.latency[FLOW_OUTGOING]) <= 0.0f) // how???
	//	return true;

	static auto sv_maxunlag = csgo.m_engine_cvars()->FindVar(sxor("sv_maxunlag"));
	//static auto sv_max_usercmd_future_ticks = csgo.m_engine_cvars()->FindVar(sxor("sv_max_usercmd_future_ticks"));

	// server tickcount when cmd will arrive
	/*auto latency_ticks = 1;

	if (ctx.last_4_deltas.size() >= 2) {
		latency_ticks = 0;
		int added = 0;
		for (auto d : ctx.last_4_deltas)
		{
			latency_ticks += d;
			++added;
			if (added >= 7)
				break;
		}
		latency_ticks /= 7;
	}*/

	auto serverTickcount = csgo.m_engine()->GetTick() + TIME_TO_TICKS(ctx.latency[FLOW_OUTGOING]);

	if (ctx.fakeducking)
		serverTickcount += 15 - csgo.m_client_state()->m_iChockedCommands;

	float total_latency = ctx.latency[FLOW_OUTGOING] + ctx.latency[FLOW_INCOMING];
	total_latency = fminf(fmaxf(total_latency, 0.0f), 1.0f);

	float correct = total_latency + ctx.lerp_time;
	correct = Math::clamp(correct, 0.f, sv_maxunlag->GetFloat());

	auto shift = 0;

	if (ctx.fakeducking || !ctx.exploit_allowed || !ctx.has_exploit_toggled || ctx.main_exploit == 0 || ctx.applied_tickbase)
		shift = 0;
	else {
		if (ctx.exploit_allowed && ctx.has_exploit_toggled && !ctx.fakeducking)
		{
			shift = max(ctx.shift_amount, ctx.next_shift_amount);

			if (ctx.main_exploit == 2)
				shift = 0;

			if (ctx.main_exploit == 2 && ctx.force_aimbot < 1 && ctx.ticks_allowed > 13 && abs(ctx.current_tickcount - ctx.started_speedhack) > 16)
				shift = 13;
			//else
			//	if (ctx.m_settings.aimbot_extra_doubletap_options[3] && ctx.ticks_allowed > 12 && std::fabsf(csgo.m_globals()->realtime - ctx.last_speedhack_time) > 0.5f && ctx.allow_shooting < 1 && ctx.force_aimbot < 1)
			//		shift = 12;
		}
	}

	auto time = csgo.m_globals()->curtime;

	if (ctx.m_local() && !ctx.m_local()->IsDead())
		time = TICKS_TO_TIME(ctx.m_local()->m_nTickBase() - shift);

	float deltaTime = abs(correct - (time - wish_record->simulation_time));

	if (deltaTime >= 0.2f)
		return true;

	/*if (!(ctx.fakeducking || !ctx.exploit_allowed || !ctx.has_exploit_toggled || ctx.main_exploit != 2)
		&& ctx.force_aimbot > 1
		&& ctx.ticks_allowed < 13
		&& abs(ctx.started_speedhack - ctx.current_tickcount) <= 16)
		return false;*/

	return ignore_deadtime || wish_record->simulation_time < float(int(TICKS_TO_TIME(serverTickcount) - sv_maxunlag->GetFloat()));
}

//bool c_lagcomp::is_time_delta_too_large(const float& simulation_time)
//{
//	//if (max(ctx.latency[FLOW_INCOMING], ctx.latency[FLOW_OUTGOING]) <= 0.0f) // how???
//	//	return true;
//
//	static auto sv_maxunlag = csgo.m_engine_cvars()->FindVar(sxor("sv_maxunlag"));
//	static auto sv_max_usercmd_future_ticks = csgo.m_engine_cvars()->FindVar(sxor("sv_max_usercmd_future_ticks"));
//
//	// server tickcount when cmd will arrive
//	//auto serverTickcount = ctx.fakeducking ? ctx.fakeduck_will_choke : (ctx.m_settings.fake_lag_shooting ? (ctx.current_tickcount + 2) : (ctx.current_tickcount + 1));
//
//	float correct = 0.f;
//	correct += ctx.latency[FLOW_OUTGOING];
//	correct += ctx.latency[FLOW_INCOMING];
//	correct += ctx.lerp_time;
//
//	correct = Math::clamp(correct, 0.f, sv_maxunlag->GetFloat());
//
//	auto shift = max(ctx.shift_amount, ctx.next_shift_amount);
//
//	if (ctx.fakeducking || !ctx.exploit_allowed || !ctx.has_exploit_toggled)
//		shift = 0;
//	else {
//		if (ctx.exploit_allowed && ctx.has_exploit_toggled)
//		{
//			if (ctx.has_exploit_toggled && ctx.main_exploit == 2)
//				shift = 0;
//
//			if (ctx.exploit_allowed && ctx.has_exploit_toggled && ctx.main_exploit == 1 && (!m_weapon() || m_weapon()->can_exploit(11)))
//				shift = 11;
//
//			if (!ctx.fakeducking && ctx.exploit_allowed && ctx.main_exploit == 2 && ctx.allow_shooting <= 0 && fabs(csgo.m_globals()->realtime - ctx.last_speedhack_time) > 0.5f && ctx.ticks_allowed > 12)
//				shift = 14;
//		}
//	}
//
//		if (simulation_time < (TICKS_TO_TIME(ctx.m_local()->m_nTickBase()) + ctx.latency[FLOW_OUTGOING] - sv_maxunlag->GetFloat()))
//			return true;
//
//	auto time = float(ctx.m_local()->m_nTickBase() - shift) * csgo.m_globals()->interval_per_tick;
//
//	float deltaTime = correct - (time - simulation_time);
//
//	// account for outgoing latency
//	//serverTickcount += (signed int)(float)(ctx.latency[FLOW_OUTGOING] / csgo.m_globals()->interval_per_tick);
//	//serverTickcount += (signed int)(float)(ctx.latency[FLOW_INCOMING] / csgo.m_globals()->interval_per_tick);
//
//	//const auto dead_time = int(TICKS_TO_TIME(serverTickcount + 1) - sv_maxunlag->GetFloat());
//	//const int usercmd_future_ticks = sv_max_usercmd_future_ticks->GetInt();
//
//	//if (/*TIME_TO_TICKS(wish_record->simulation_time) > (serverTickcount + usercmd_future_ticks) || */wish_record->simulation_time < dead_time)
//	//	return true;
//
//	return fabs(deltaTime) > 0.2f;
//}

//void c_lagcomp::update_player_record_data(C_BasePlayer* entity)
//{
//	if (entity == nullptr /*|| net == nullptr*//* || cheat::game::last_cmd == nullptr*/)
//		return;
//
//	auto player_index = entity->entindex() - 1;
//	auto player_record = &records[player_index];
//
//	auto resolve_info = &feature::resolver->player_records[player_index];
//
//	//if (!player_record->m_Tickrecords.empty() && player_record->m_Tickrecords.front().simulation_time >= entity->m_flSimulationTime())
//	//	resolve_info->is_shifting = (player_record->m_Tickrecords.front().eye_angles != entity->m_angEyeAngles() || player_record->m_Tickrecords.front().origin != entity->m_vecOrigin());
//
//	const float tickrate = (1 / csgo.m_globals()->interval_per_tick) / 2.f;
//
//	const auto wtf = abs(csgo.m_globals()->tickcount - TIME_TO_TICKS(entity->m_flSimulationTime()));
//	auto unlag = wtf <= tickrate;
//
//	resolve_info->simtime_updated = false;
//
//	if (entity->m_flSimulationTime() > 0.f && player_record->m_Tickrecords.empty() && unlag || !player_record->m_Tickrecords.empty() && player_record->m_Tickrecords.front().simulation_time != entity->m_flSimulationTime() && unlag)
//	{
//		resolve_info->simtime_updated = true;
//
//		auto pasti = player_record->m_Tickrecords;
//
//		//if (!player_record->m_Tickrecords.empty())
//		update_animations_data(entity);
//		
//		if (resolve_info->resolving_method == -1)
//			return;
//
//		if (entity->m_flSimulationTime() > 0.f && player_record->m_Tickrecords.empty() || !is_time_delta_too_large(entity->m_flSimulationTime()))
//		{
//			C_Tickrecord new_record;
//			store_record_data(entity, &new_record);
//
//			if (new_record.data_filled) {
//				player_record->m_Tickrecords.push_front(new_record);
//
//				if (feature::resolver->player_records[player_index].did_shot_this_tick)
//					player_record->m_ShotBacktracking = new_record;
//				if (!player_record->m_LastTick.data_filled || is_time_delta_too_large(&player_record->m_LastTick))
//					player_record->m_LastTick = new_record;
//
//				player_record->m_NewTick = new_record;
//			}
//		}
//	}
//
//	if (player_record->m_Tickrecords.size() > LAG_COMPENSATION_TICKS)
//		player_record->m_Tickrecords.resize(LAG_COMPENSATION_TICKS);
//
//	while (!player_record->m_Tickrecords.empty() && is_time_delta_too_large(&player_record->m_Tickrecords.back()))
//		player_record->m_Tickrecords.pop_back();
//}

float spawntime;

void c_lagcomp::reset(CCSGOPlayerAnimState* state)
{
	if (!state)
		return;

	state->m_last_update_frame = 0;
	state->m_step_height_left = 0;
	state->m_step_height_right = 0;

	state->m_weapon = state->m_player->get_weapon();
	state->m_weapon_last = state->m_weapon;

	state->m_weapon_last_bone_setup = state->m_weapon;
	state->m_eye_position_smooth_lerp = 0;
	state->m_strafe_change_weight_smooth_fall_off = 0;
	state->m_first_foot_plant_since_init = true;

	state->m_last_update_time = 0;
	state->m_last_update_increment = 0;

	state->m_eye_yaw = 0;
	state->m_eye_pitch = 0;
	state->m_abs_yaw = 0;
	state->m_abs_yaw_last = 0;
	state->m_move_yaw = 0;
	state->m_move_yaw_ideal = 0;
	state->m_move_yaw_current_to_ideal = 0;

	state->m_stand_walk_how_long_to_wait_until_transition_can_blend_in = 0.4f;
	state->m_stand_walk_how_long_to_wait_until_transition_can_blend_out = 0.2f;
	state->m_stand_run_how_long_to_wait_until_transition_can_blend_in = 0.2f;
	state->m_stand_run_how_long_to_wait_until_transition_can_blend_out = 0.4f;
	state->m_crouch_walk_how_long_to_wait_until_transition_can_blend_in = 0.3f;
	state->m_crouch_walk_how_long_to_wait_until_transition_can_blend_out = 0.3f;

	state->m_primary_cycle = 0;
	state->m_move_weight = 0;
	state->m_move_weight_smoothed = 0;
	state->m_anim_duck_amount = 0;
	state->m_duck_additional = 0; // for when we duck a bit after landing from a jump
	state->m_recrouch_weight = 0;

	state->m_position_current.clear();
	state->m_position_last.clear();

	state->m_velocity.clear();
	state->m_velocity_normalized.clear();
	state->m_velocity_normalized_non_zero.clear();
	state->m_velocity_length_xy = 0;
	state->m_velocity_length_z = 0;

	state->m_speed_as_portion_of_run_top_speed = 0;
	state->m_speed_as_portion_of_walk_top_speed = 0;
	state->m_speed_as_portion_of_crouch_top_speed = 0;

	state->m_duration_moving = 0;
	state->m_duration_still = 0;

	state->m_on_ground = true;

	state->m_land_anim_multiplier = 1.0f;
	state->m_left_ground_height = 0;
	state->m_landing = false;
	state->m_jump_to_fall = 0;
	state->m_duration_in_air = 0;

	state->m_walk_run_transition = 0;

	state->m_landed_on_ground_this_frame = false;
	state->m_left_the_ground_this_frame = false;
	state->m_in_air_smooth_value = 0;

	state->m_on_ladder = false;
	state->m_ladder_weight = 0;
	state->m_ladder_speed = 0;

	state->m_walk_to_run_transition_state = 0;

	state->m_defuse_started = false;
	state->m_plant_anim_started = false;
	state->m_twitch_anim_started = false;
	state->m_adjust_started = false;

	state->m_next_twitch_time = 0;

	state->m_time_of_last_known_injury = 0;

	state->m_last_velocity_test_time = 0;
	state->m_velocity_last.clear();
	state->m_target_acceleration.clear();
	state->m_acceleration.clear();
	state->m_acceleration_weight = 0;

	state->m_aim_matrix_transition = 0;
	state->m_aim_matrix_transition_delay = 0;

	state->m_flashed = 0;

	state->m_strafe_change_weight = 0;
	state->m_strafe_change_target_weight = 0;
	state->m_strafe_change_cycle = 0;
	state->m_strafe_sequence = -1;
	state->m_strafe_changing = false;
	state->m_duration_strafing = 0;

	state->m_foot_lerp = 0;

	state->m_feet_crossed = false;

	state->m_player_is_accelerating = false;

	state->m_duration_move_weight_is_too_high = 0;
	state->m_static_approach_speed = 80;

	state->m_stutter_step = 0;
	state->m_previous_move_state = 0;

	state->m_action_weight_bias_remainder = 0;

	state->m_aim_yaw_min = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN;
	state->m_aim_yaw_max = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX;
	state->m_aim_pitch_min = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN;
	state->m_aim_pitch_max = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX;

	//state->m_activity_modifiers.Purge();
	memset(&state->m_activity_modifiers_server[0], 0, 20);

	state->m_first_run_since_init = true;

	state->m_camera_smooth_height = 0;
	state->m_smooth_height_valid = false;
	state->m_last_time_velocity_over_ten = 0;
}

//void c_lagcomp::update_local_animations_data(CUserCmd* cmd, bool* send_packet)
//{
//	auto animstate = ctx.m_local()->get_animation_state();
//
//	if (!animstate || ctx.m_local()->IsDead() || ctx.m_local()->IsDormant()) {
//		stored = false;
//		spawntime = 0.f;
//		return;
//	}
//
//	//ctx.m_local()->get_animation_state()->ent = (void*)ctx.m_local();
//
//	static float prev_simtime = ctx.m_local()->m_flSimulationTime();
//
//	if (ctx.m_local()->m_flSpawnTime() != spawntime || ctx.m_local()->get_animation_state()->m_player != ctx.m_local())
//	{
//		//using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
//		//static auto ResetAnimState = (ResetAnimState_t)Memory::Scan("client.dll", "56 6A 01 68 ? ? ? ? 8B F1");
//
//		//if (ResetAnimState != nullptr)
//		//	ResetAnimState(ctx.m_local()->get_animation_state());
//		ctx.updating_anims = true;
//		ctx.m_local()->update_clientside_animations();
//		ctx.updating_anims = false;
//		memcpy(ctx.local_layers[ANGLE_FAKE], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//		memcpy(ctx.local_layers[ANGLE_REAL], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//
//		//memcpy(&ctx.fake_state, ctx.m_local()->get_animation_state(), 836);
//		stored = false;
//		feature::anti_aim->stop_to_full_running_fraction = 0.f;
//
//		ctx.m_local()->update_clientside_animations();
//		ctx.fake_state = *ctx.m_local()->get_animation_state();
//		ctx.poses[ANGLE_FAKE] = ctx.m_local()->m_flPoseParameter();
//		ctx.poses[ANGLE_REAL] = ctx.m_local()->m_flPoseParameter();
//		memset(&ctx.fake_state.m_activity_modifiers_server[0], 0, sizeof(char) * 20);
//
//		spawntime = ctx.m_local()->m_flSpawnTime();
//	}
//
//	//Engine::Prediction::Instance()->DoInterpolateVars(true);
//
//	CCSGOPlayerAnimState state;
//	C_AnimationLayer backuplayers[15];
//	memcpy(backuplayers, ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//
//	auto smoothstep_bounds = [](float edge0, float edge1, float x) -> float
//	{
//		x = Math::clamp((x - edge0) / (edge1 - edge0), 0, 1);
//		return x * x * (3 - 2 * x);
//	};
//
//	/*const auto sequence = select_weighted_sequence_from_modifiers(mapping, player->get_studio_hdr(), activity, &modifiers[0], modifiers.count());
//
//	if (sequence < 2)
//		return;*/
//
//	/*static bool m_bOnLadder = ctx.m_local()->m_MoveType() == MOVETYPE_LADDER;
//	static bool m_bOnGround = ctx.m_local()->m_fFlags() & FL_ONGROUND;
//	static float m_flDurationInAir = 0.f;
//	static bool m_bLanding = !(ctx.m_local()->m_fFlags() & FL_ONGROUND);
//	bool m_bJumping = cmd->buttons & IN_JUMP;
//	static float m_flLadderWeight = 0.f;
//	const bool bPreviousGroundState = m_bOnGround;
//	const bool bPreviouslyOnLadder = m_bOnLadder;
//	m_bOnGround = ctx.m_local()->m_fFlags() & FL_ONGROUND;
//	m_bOnLadder = !(ctx.m_local()->m_fFlags() & FL_ONGROUND) && ctx.m_local()->m_MoveType() == MOVETYPE_LADDER;
//	const bool bStartedLadderingThisFrame = (!bPreviouslyOnLadder && m_bOnLadder);
//	const bool bStoppedLadderingThisFrame = (bPreviouslyOnLadder && !m_bOnLadder);
//
//	const bool m_bLandedOnGroundThisFrame = (bPreviousGroundState != m_bOnGround && m_bOnGround);
//	const bool m_bLeftTheGroundThisFrame = (bPreviousGroundState != m_bOnGround && !m_bOnGround);*/
//
//	auto current_flags = ctx.m_local()->m_fFlags();
//
//	if (!(current_flags & 0x40) && csgo.m_game_rules() && !csgo.m_game_rules()->IsFreezeTime())
//	{
//		auto land = &ctx.m_local()->animation_layer(4);
//		auto jump_fall = &ctx.m_local()->animation_layer(5);
//
//		auto previous = Engine::Prediction::Instance()->GetFlags();
//		auto anim_state = ctx.m_local()->get_animation_state();
//
//		// ref: SetupMovement
//		// this is all anim events from setupmovement
//		if (ctx.m_local()->m_MoveType() != MOVETYPE_LADDER)
//		{
//			auto on_ground = (current_flags & FL_ONGROUND) != 0;
//			auto was_on_ground = (previous & FL_ONGROUND) != 0;
//
//			if (was_on_ground && !on_ground)
//			{
//				if (cmd->buttons & IN_JUMP)
//				{
//					//DoAnimEvent(4, ACT_CSGO_JUMP);
//					land->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(land, 15);
//					land->m_nSequence = 15;
//					land->m_flCycle = land->m_flWeight = 0.f;
//				}
//				else
//				{
//					//DoAnimEvent(4, ACT_CSGO_FALL);
//					land->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(land, 14);
//					land->m_nSequence = 14;
//					land->m_flCycle = land->m_flWeight = 0.f;
//				}
//			}
//			else if (on_ground && !was_on_ground && !anim_state->m_landing)
//			{
//				//DoAnimEvent(5, (animState->m_duration_in_air > 1.0f) ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT);
//				jump_fall->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(jump_fall, (anim_state->m_duration_in_air > 1) ? 23 : 22);
//				jump_fall->m_nSequence = (anim_state->m_duration_in_air > 1) ? 23 : 22;
//				jump_fall->m_flCycle = jump_fall->m_flWeight = 0.f;
//			}
//		}
//		else
//		{
//			auto was_on_ladder = false;
//			if (Engine::Prediction::Instance()->GetMoveType() != MOVETYPE_LADDER || (was_on_ladder = true, previous & FL_ONGROUND))
//				was_on_ladder = false;
//
//			if (!was_on_ladder && !(ctx.m_local()->m_fFlags() & FL_ONGROUND))
//			{
//				//DoAnimEvent(5, ACT_CSGO_CLIMB_LADDER);
//
//				jump_fall->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(jump_fall, 13);
//				jump_fall->m_nSequence = 13;
//				jump_fall->m_flCycle = jump_fall->m_flWeight = 0.f;
//			}
//		}
//
//		//if (m_flLadderWeight > 0 || m_bOnLadder)
//		//{
//		//	if (bStartedLadderingThisFrame)
//		//	{
//		//		//SetLayerSequence(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, SelectSequenceFromActMods(ACT_CSGO_CLIMB_LADDER));
//		//		land->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(land, 13);
//		//		land->m_nSequence = 13;
//		//		land->m_flCycle = land->m_flWeight = 0.f;
//		//	}
//
//		//	if (m_bOnLadder)
//		//		m_flLadderWeight = Math::Approach(1, m_flLadderWeight, csgo.m_globals()->interval_per_tick * 5.0f);
//		//	else
//		//		m_flLadderWeight = Math::Approach(0, m_flLadderWeight, csgo.m_globals()->interval_per_tick * 10.0f);
//
//		//	m_flLadderWeight = Math::clamp(m_flLadderWeight, 0, 1);
//		//}
//		//else
//		//	m_flLadderWeight = 0;
//
//		////jumping
//		//if (m_bOnGround)
//		//{
//		//	if (!m_bLanding && (m_bLandedOnGroundThisFrame || bStoppedLadderingThisFrame))
//		//	{
//		//		//SetLayerSequence(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, SelectSequenceFromActMods((m_flDurationInAir > 1) ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT));
//		//		land->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(land, (m_flDurationInAir > 1) ? 23 : 22);
//		//		land->m_nSequence = (m_flDurationInAir > 1) ? 23 : 22;
//		//		land->m_flCycle = land->m_flWeight = 0.f;
//		//	}
//
//		//	if (m_bLanding && ctx.m_local()->get_sec_activity(land->m_nSequence) != ACT_CSGO_CLIMB_LADDER)
//		//		m_bJumping = false;
//
//		//	if (!m_bLanding && !m_bJumping && m_flLadderWeight <= 0)
//		//	{
//		//		//SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, 0);
//		//		land->m_flWeight = 0.f;
//		//	}
//		//}
//		//else if (!m_bOnLadder)
//		//{
//		//	m_bLanding = false;
//
//		//	// we're in the air
//		//	if (m_bLeftTheGroundThisFrame || bStoppedLadderingThisFrame)
//		//	{
//		//		if (!m_bJumping)
//		//		{
//		//			jump_fall->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(jump_fall, 15);
//		//			jump_fall->m_nSequence = 15;
//		//			jump_fall->m_flCycle = jump_fall->m_flWeight = 0.f;
//		//		}
//
//		//		m_flDurationInAir = 0;
//		//	}
//
//		//	m_flDurationInAir += csgo.m_globals()->interval_per_tick;
//
//		//	if (ctx.m_local()->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_JUMP_FALL].get_index() > -1)
//		//		ctx.poses[ANGLE_REAL][ctx.m_local()->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_JUMP_FALL].get_index()] = Math::clamp(smoothstep_bounds(0.72f, 1.52f, m_flDurationInAir), 0, 1);
//		//}
//	}
//
//	const auto dword_3CF22C70 = csgo.m_globals()->realtime;
//	const auto dword_3CF22C74 = csgo.m_globals()->curtime;
//	const auto dword_3CF22C78 = csgo.m_globals()->frametime;
//	const auto dword_3CF22C7C = csgo.m_globals()->absoluteframetime;
//	const auto dword_3CF22C80 = csgo.m_globals()->interpolation_amount;
//	const auto dword_3CF22C84 = csgo.m_globals()->framecount;
//	const auto dword_3CF22C88 = csgo.m_globals()->tickcount;
//
//	csgo.m_globals()->realtime = TICKS_TO_TIME(feature::anti_aim->sent_data.tickbase);
//	csgo.m_globals()->curtime = TICKS_TO_TIME(feature::anti_aim->sent_data.tickbase);
//	csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
//	csgo.m_globals()->absoluteframetime = csgo.m_globals()->interval_per_tick;
//	csgo.m_globals()->framecount = feature::anti_aim->sent_data.tickbase;
//	csgo.m_globals()->tickcount = feature::anti_aim->sent_data.tickbase;
//	csgo.m_globals()->interpolation_amount = 0.0f;
//
//	const auto backup_poses = ctx.m_local()->m_flPoseParameter();
//	const auto backup_flags = ctx.m_local()->m_fFlags();
//	const auto backup_duckamt = ctx.m_local()->m_flDuckAmount();
//	const auto backup_lby = ctx.m_local()->m_flLowerBodyYawTarget();
//	const auto backup_renderang = ctx.m_local()->get_render_angles();
//	const auto backup_absvel = ctx.m_local()->m_vecAbsVelocity();
//
//	//ctx.m_local()->get_render_angles() = QAngle(feature::anti_aim->visual_real_angle.x, ctx.m_local()->m_angEyeAngles().y, ctx.m_local()->m_angEyeAngles().z);
//
//	ctx.m_local()->client_side_animation() = true;
//
//	const auto& sent_ucmd = csgo.m_input()->GetUserCmd(feature::anti_aim->sent_data.command_number);
//	auto fake_angle = sent_ucmd ? sent_ucmd->viewangles : feature::anti_aim->visual_real_angle;
//
//	/*update fake*/
//	if (*send_packet/*csgo.m_client_state()->m_iChockedCommands == 0*/)
//	{
//		state = *ctx.m_local()->get_animation_state();
//		*ctx.m_local()->get_animation_state() = ctx.fake_state;
//
//		//ctx.m_local()->get_animation_state()->feet_rate = 0.f;
//		/*if (!prev_poses.empty()) {
//			ctx.m_local()->get_animation_state()->feet_cycle = animlayers[6].m_flCycle;
//
//			if (ctx.m_local()->m_fFlags() & FL_ONGROUND)
//				ctx.m_local()->get_animation_state()->feet_rate = animlayers[6].m_flWeight;
//		}*/
//
//		//ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
//
//		ctx.m_local()->get_animation_state()->m_move_weight = 0;
//		//ctx.m_local()->get_animation_state()->unk_frac = 0.f;
//
//		//ctx.m_local()->get_animation_state()->stop_to_full_run_frac = feature::anti_aim->stop_to_full_running_fraction;
//
//		if (ctx.m_local()->get_animation_state()->m_last_update_frame >= csgo.m_globals()->framecount)
//			ctx.m_local()->get_animation_state()->m_last_update_frame = csgo.m_globals()->framecount - 1;
//
//		//if (feature::anti_aim->last_chocked_amount <= 2)
//		//ctx.m_local()->get_animation_state()->last_anim_upd_time = csgo.m_globals()->curtime - (ctx.m_local()->m_flSimulationTime() - ctx.m_local()->m_flOldSimulationTime());
//
//		ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
//
//		/*memcpy(ctx.m_local()->animation_layers_ptr(), ctx.local_layers[ANGLE_REAL], 0x38 * ctx.m_local()->get_animation_layers_count());*/
//		//ctx.m_local()->get_animation_state()->ent = (void*)ctx.m_local();
//		//ctx.m_local()->get_animation_state()->stop_to_full_run_frac = feature::anti_aim->stop_to_full_running_fraction;
//		ctx.is_updating_fake = true;
//		ctx.m_local()->m_fFlags() = feature::anti_aim->sent_data.m_fFlags;
//		ctx.m_local()->m_vecAbsVelocity() = (feature::anti_aim->animation_speed > 6.f ? feature::anti_aim->sent_data.m_vecVelocity : Vector::Zero);
//		//ctx.m_local()->update_animstate(ctx.m_local()->get_animation_state(), fake_angle);
//		ctx.m_local()->get_render_angles() = fake_angle;
//		ctx.m_local()->update_clientside_animations();
//		//ctx.m_local()->invalidate_anims();
//		ctx.is_updating_fake = false;
//		ctx.m_local()->m_vecAbsVelocity() = backup_absvel;
//		memcpy(ctx.local_layers[ANGLE_FAKE], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//		ctx.m_local()->m_fFlags() = backup_flags;
//
//		ctx.angles[ANGLE_FAKE] = ctx.m_local()->get_animation_state()->m_abs_yaw;
//		feature::anti_aim->fake_yaw_diff_with_backwards = Math::AngleDiff(Engine::Movement::Instance()->m_qRealAngles.y, ctx.m_local()->get_animation_state()->m_abs_yaw);
//		feature::anti_aim->real_yaw_diff_with_backwards = Math::AngleDiff(Engine::Movement::Instance()->m_qRealAngles.y, ctx.m_local()->get_animation_state()->m_abs_yaw);
//		ctx.poses[ANGLE_FAKE] = ctx.m_local()->m_flPoseParameter();
//
//		if (!ctx.m_settings.fake_lag_enabled && !ctx.m_settings.anti_aim_enabled)
//		{
//			ctx.angles[ANGLE_REAL] = ctx.m_local()->get_animation_state()->m_abs_yaw;
//			ctx.poses[ANGLE_REAL] = ctx.m_local()->m_flPoseParameter();
//			memcpy(ctx.local_layers[ANGLE_REAL], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//		}
//
//		ctx.fake_state = *ctx.m_local()->get_animation_state();
//
//		ctx.local_layers[ANGLE_FAKE][12].m_flWeight = 0.f;
//		ctx.local_layers[ANGLE_FAKE][3].m_flWeight = 0.f;
//		ctx.local_layers[ANGLE_FAKE][3].m_flCycle = 0.f;
//
//		/*if (feature::anti_aim->last_chocked_amount <= 2)
//			ctx.m_local()->get_animation_state()->last_anim_upd_time = csgo.m_globals()->curtime - max(csgo.m_globals()->interval_per_tick, ctx.m_local()->m_flSimulationTime() - ctx.m_local()->m_flOldSimulationTime());
//
//		ctx.m_local()->get_animation_state()->abs_yaw = feature::anti_aim->visual_real_angle.y + 60.f;
//		ctx.m_local()->update_animstate(ctx.m_local()->get_animation_state(), feature::anti_aim->visual_real_angle);
//		memcpy(ctx.local_layers[1], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//		ctx.angles[ANGLE_POSDELTA] = ctx.m_local()->get_animation_state()->abs_yaw;
//
//		if (feature::anti_aim->last_chocked_amount <= 2)
//			ctx.m_local()->get_animation_state()->last_anim_upd_time = csgo.m_globals()->curtime - max(csgo.m_globals()->interval_per_tick, ctx.m_local()->m_flSimulationTime() - ctx.m_local()->m_flOldSimulationTime());
//
//		ctx.m_local()->get_animation_state()->abs_yaw = feature::anti_aim->visual_real_angle.y - 60.f;
//		ctx.m_local()->update_animstate(ctx.m_local()->get_animation_state(), feature::anti_aim->visual_real_angle);
//		memcpy(ctx.local_layers[0], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//		ctx.angles[ANGLE_NEGDELTA] = ctx.m_local()->get_animation_state()->abs_yaw;*/
//		
//		ctx.m_local()->m_flPoseParameter() = backup_poses;
//		//memcpy(ctx.m_local()->animation_layers_ptr(), backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());
//		ctx.m_local()->m_fFlags() = backup_flags;
//		ctx.m_local()->m_flDuckAmount() = backup_duckamt;
//		ctx.m_local()->m_flLowerBodyYawTarget() = backup_lby;
//		ctx.m_local()->get_render_angles() = backup_renderang;
//		*ctx.m_local()->get_animation_state() = state;
//
//		csgo.m_globals()->realtime = dword_3CF22C70;
//		csgo.m_globals()->curtime = dword_3CF22C74;
//		csgo.m_globals()->frametime = dword_3CF22C78;
//		csgo.m_globals()->absoluteframetime = dword_3CF22C7C;
//		csgo.m_globals()->interpolation_amount = dword_3CF22C80;
//		csgo.m_globals()->framecount = dword_3CF22C84;
//		csgo.m_globals()->tickcount = dword_3CF22C88;
//	}
//
//	//// fix approachangles too big when fakelagging
//	//float eyeFeetDelta = Math::AngleDiff(fake_angle.y, ctx.angles[ANGLE_FAKE]);
//	//
//	//float maxFraction = feature::anti_aim->get_max_desync_delta(ctx.m_local());
//	//float flMaxYawModifier = maxFraction * ctx.fake_state.max_yaw;
//	//float flMinYawModifier = maxFraction * ctx.fake_state.min_yaw;
//	//
//	//if (eyeFeetDelta <= flMaxYawModifier) {
//	//	if (flMinYawModifier > eyeFeetDelta)
//	//		ctx.angles[ANGLE_FAKE] = fabs(flMinYawModifier) + fake_angle.y;
//	//}
//	//else {
//	//	ctx.angles[ANGLE_FAKE] = fake_angle.y - (fabs(flMaxYawModifier));
//	//}
//	//
//	//ctx.angles[ANGLE_FAKE] = std::remainderf(ctx.angles[ANGLE_FAKE], 360.0f);
//	//
//	//ctx.angles[ANGLE_FAKE] = Math::ApproachAngle(fake_angle.y, ctx.angles[ANGLE_FAKE],
//	//	(ctx.fake_state.speed_2d > 0.1f || std::fabsf(ctx.fake_state.speed_up) > 100.0f)
//	//	? (ctx.fake_state.stop_to_full_run_frac * 20.0f + 30.0f) * csgo.m_globals()->interval_per_tick
//	//	: csgo.m_globals()->interval_per_tick * 100.0f);
//	//
//	//ctx.angles[ANGLE_FAKE] = std::remainderf(ctx.angles[ANGLE_FAKE], 360.0f);
//
//	if (feature::usercmd->command_numbers.size() > 0 && *send_packet)
//	{
//		//if (stored) {
//		//	ctx.m_local()->get_animation_state()->feet_cycle = animlayers[6].m_flCycle;
//		//
//		//	if (ctx.m_local()->m_fFlags() & FL_ONGROUND)
//		//		ctx.m_local()->get_animation_state()->feet_rate = animlayers[6].m_flWeight;
//		//}
//
//		//ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_REAL];
//
//		for (auto i = int(feature::usercmd->command_numbers.size() - 1); i >= 0; i--)
//			//for (auto i = 0; i < int(feature::usercmd->command_numbers.size()); i++)
//		{
//			const auto& c_cmd = feature::usercmd->command_numbers[i];
//			const auto& ucmd = csgo.m_input()->GetUserCmd(c_cmd.command_number);
//
//			const auto curtime = csgo.m_globals()->curtime;
//			const auto framecount = csgo.m_globals()->framecount;
//			const auto frametime = csgo.m_globals()->frametime;
//
//			const auto viewangles = (ucmd == nullptr ? c_cmd.view_angles : ucmd->viewangles);
//
//			/*csgo.m_globals()->curtime = TICKS_TO_TIME(c_cmd.tickbase);
//			csgo.m_globals()->framecount = c_cmd.tickbase;
//			csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;*/
//			csgo.m_globals()->realtime = TICKS_TO_TIME(c_cmd.tickbase);
//			csgo.m_globals()->curtime = TICKS_TO_TIME(c_cmd.tickbase);
//			csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
//			csgo.m_globals()->absoluteframetime = csgo.m_globals()->interval_per_tick;
//			csgo.m_globals()->framecount = c_cmd.tickbase;
//			csgo.m_globals()->tickcount = c_cmd.tickbase;
//			csgo.m_globals()->interpolation_amount = 0.0f;
//
//
//			ctx.m_local()->get_animation_state()->m_move_weight = 0;
//			ctx.m_local()->m_fFlags() = c_cmd.flags;
//			ctx.m_local()->m_flDuckAmount() = c_cmd.duck_amount;
//			ctx.m_local()->m_flLowerBodyYawTarget() = c_cmd.lby;
//			//ctx.m_local()->m_vecAbsVelocity() = c_cmd.velocity;
//			ctx.m_local()->m_vecAbsVelocity() = (c_cmd.velocity.Length2D() > 6.f ? c_cmd.velocity : Vector::Zero);
//
//			if (ctx.m_local()->get_animation_state()->m_last_update_frame >= csgo.m_globals()->framecount)
//				ctx.m_local()->get_animation_state()->m_last_update_frame = csgo.m_globals()->framecount - 1;
//
//			//ctx.m_local()->get_animation_state()->feet_rate = 0;
//			//ctx.m_local()->get_animation_state()-> = (void*)ctx.m_local();
//			//ctx.m_local()->get_animation_state()->stop_to_full_run_frac = feature::anti_aim->stop_to_full_running_fraction;
//			//ctx.m_local()->get_animation_state()->landing_duck = 0.f;
//
//			//if (c_cmd.flags & FL_ONGROUND)
//			//	ctx.m_local()->get_animation_state()->time_since_inair = 0.f;
//
//			ctx.m_local()->get_animation_state()->m_abs_yaw = (ctx.m_ragebot_shot_nr > c_cmd.command_number && feature::usercmd->command_numbers.front().command_number <= ctx.m_ragebot_shot_nr ? feature::usercmd->command_numbers.front().view_angles.y : viewangles.y);
//			ctx.m_local()->get_render_angles() = QAngle(ctx.fake_state.m_eye_pitch, ctx.fake_state.m_eye_yaw, viewangles.z);
//			//if (i <= 1)
//			//	ctx.m_local()->invalidate_anims();
//			ctx.m_local()->update_clientside_animations();
//			//ctx.m_local()->update_animstate(ctx.m_local()->get_animation_state(), QAngle(feature::anti_aim->visual_real_angle.x, ctx.m_local()->m_angEyeAngles().y, ctx.m_local()->m_angEyeAngles().z));
//
//			csgo.m_globals()->curtime = curtime;
//			csgo.m_globals()->frametime = frametime;
//			ctx.m_local()->m_vecAbsVelocity() = backup_absvel;
//		}
//
//		feature::anti_aim->real_yaw_diff_with_backwards = Math::AngleDiff(ctx.m_local()->get_animation_state()->m_abs_yaw, Engine::Movement::Instance()->m_qRealAngles.y);
//
//		ctx.m_local()->m_fFlags() = backup_flags;
//		ctx.m_local()->m_flDuckAmount() = backup_duckamt;
//		ctx.m_local()->m_flLowerBodyYawTarget() = backup_lby;
//
//		memcpy(ctx.local_layers[ANGLE_REAL], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//		ctx.angles[ANGLE_REAL] = ctx.m_local()->get_animation_state()->m_abs_yaw;
//		ctx.poses[ANGLE_REAL] = ctx.m_local()->m_flPoseParameter();
//		feature::usercmd->command_numbers.clear();
//	}
//
//	//memcpy(ctx.m_local()->animation_layers_ptr(), backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());
//
//	ctx.m_local()->m_fFlags() = backup_flags;
//	ctx.m_local()->m_flDuckAmount() = backup_duckamt;
//	ctx.m_local()->m_flLowerBodyYawTarget() = backup_lby;
//	ctx.m_local()->get_render_angles() = backup_renderang;
//	ctx.m_local()->m_flPoseParameter() = backup_poses;
//	ctx.m_local()->m_vecAbsVelocity() = backup_absvel;
//
//	csgo.m_globals()->realtime = dword_3CF22C70;
//	csgo.m_globals()->curtime = dword_3CF22C74;
//	csgo.m_globals()->frametime = dword_3CF22C78;
//	csgo.m_globals()->absoluteframetime = dword_3CF22C7C;
//	csgo.m_globals()->interpolation_amount = dword_3CF22C80;
//	csgo.m_globals()->framecount = dword_3CF22C84;
//	csgo.m_globals()->tickcount = dword_3CF22C88;
//
//	/*if (!prev_poses.empty()) {
//		for (auto i = 0; i < 20; i++)
//		{
//			const auto new_pose = ctx.m_local()->m_flPoseParameter();
//
//			ctx.m_local()->m_flPoseParameter()[i] = Math::interpolate(prev_poses[i], ctx.m_local()->m_flPoseParameter()[i], ctx.m_local()->m_flSimulationTime() - ctx.m_local()->m_flOldSimulationTime());
//		}
//	}*/
//
//	ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
//	ctx.m_local()->force_bone_rebuild();
//	ctx.m_local()->set_abs_angles(QAngle(0, ctx.angles[ANGLE_FAKE], 0));
//	memcpy(ctx.m_local()->animation_layers_ptr(), ctx.local_layers[ANGLE_FAKE], 0x38 * ctx.m_local()->get_animation_layers_count());
//	ctx.m_local()->SetupBonesEx();
//	memcpy(ctx.fake_matrix, ctx.m_local()->m_CachedBoneData().Base(), min(128, ctx.m_local()->m_bone_count()) * sizeof(matrix3x4_t));
//	//ctx.m_local()->m_flPoseParameter() = backup_poses;
//	build_local_bones(ctx.m_local());
//
//	memcpy(ctx.m_local()->animation_layers_ptr(), backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());
//
//	memcpy(animlayers, ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());
//	stored = true;
//	//if (m_weapon()->m_flLastShotTime() <= ctx.m_local()->m_flSimulationTime() && m_weapon()->m_flLastShotTime() > (ctx.m_local()->m_flSimulationTime() - 2.f))
//	//	shot_time = m_weapon()->m_flLastShotTime();
//
//	prev_simtime = ctx.m_local()->m_flSimulationTime();
//
//	//auto ccmd = (CUserCmd*)(uintptr_t(ctx.m_local()) + 0x3288);
//
//	//if (ccmd)
//	//	Engine::Prediction::Instance()->FixNetvarCompression(ccmd->command_number);
//
//	//ctx.m_local()->GetEyePosition();
//}

struct c_sequence_data
{
	c_sequence_data(const int _sequence, const int _index, const int _layer, const int _what)
	{
		sequence = _sequence;
		index = _index;
		layer = _layer;
		what = _what;
	};

	bool operator !=(const c_sequence_data& left)
	{
		return (sequence != left.sequence || layer != left.layer);
	}

	bool operator ==(const c_sequence_data& left)
	{
		return (sequence != left.sequence && layer != left.layer);
	}

	int sequence;
	int index;
	int layer;
	int what;
	//bool is_filled = false;
};

void fix_anim_layer(C_BasePlayer* pl, CCSGOPlayerAnimState* state, int order, int activity)
{
	auto v18 = state->m_anim_duck_amount > 0.55f;
	auto v15 = state->m_velocity_length_xy > 0.25f;
	int sequence = 0;
	auto p_layer = &pl->animation_layer(order);
	switch (activity)
	{
	case 985:
		sequence = (int)v15 + 17;
		if (!v18)
			sequence = (int)v15 + 15;
		goto LABEL_14;
	case 986:
		sequence = 14;
		goto LABEL_14;
	case 987:
		sequence = 13;
		goto LABEL_14;
	case 988:
		sequence = 2 * (int)v15 + 20;
		if (v18)
		{
			sequence = 21;
			if (v15)
				sequence = 19;
		}
		goto LABEL_14;
	case 989:
		sequence = 23;
		if (v18)
			sequence = 24;
	LABEL_14:

		p_layer->m_nSequence = sequence;
		p_layer->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(p_layer, sequence);
		p_layer->m_flWeight = 0;
		p_layer->m_flCycle = 0;
		ctx.last_time_layers_fixed = csgo.m_globals()->realtime;

		ctx.local_layers[ANGLE_POSDELTA][order] = *p_layer;
		break;
	default:
		return;
	}
}

void c_lagcomp::fix_anim_layers(CUserCmd* cmd, CCSGOPlayerAnimState* state)
{
	if (ctx.m_local()->get_animation_layers_count() > 4)
	{
		/*activity_modifiers_wrapper modifier_wrapper{};
		modifier_wrapper.add_modifier(state->GetWeaponPrefix());
		modifier_wrapper.add_activity_modifier("jump");
		if (state->m_speed_as_portion_of_walk_top_speed > 0.25f)
		{
			modifier_wrapper.add_activity_modifier("moving");
		}
		if (state->m_anim_duck_amount > 0.55f)
		{
			modifier_wrapper.add_activity_modifier("crouch");
		}*/

		if (ctx.m_settings.aimbot_enabled)
		{
			C_AnimationLayer& land = ctx.m_local()->get_animation_layer(4);
			C_AnimationLayer& jump_fall = ctx.m_local()->get_animation_layer(5);
			C_AnimationLayer& moving = ctx.m_local()->get_animation_layer(6);

			//v8 = dword_663CC134;
			//v9 = v4;
			//v5 = (*(int(__cdecl**)(int*, char*))(dword_663CC134 + 100))(&v8, &v7);
			/*int new_seq = ctx.m_local()->SelectWeightedSequenceFromModifiers(activity, modifier_wrapper.modifiers.Base(), modifier_wrapper.modifiers.Count());
			if (new_seq != -1)
			{
				v6 = *(_DWORD*)(v3 + v4);
				*(_DWORD*)(v6 + 0x10C) = 0;
				v6 += 224;
				*(_DWORD*)(v6 + 0x20) = 0;
				*(_DWORD*)(v6 + 0x18) = v5;
				(*(void(__thiscall**)(int, int, int))(*(_DWORD*)v4 + 872))(v4, v6, v5);
				*(float*)(v6 + 40) = v2;
			}*/
			if (!(ctx.m_local()->m_fFlags() & 0x40) && csgo.m_game_rules() && !csgo.m_game_rules()->IsFreezeTime())
			{
				const auto previous = Engine::Prediction::Instance()->GetFlags();
				//static bool log = false;
				//static int what_to_log = 0;

				const auto is_crouched = ctx.m_local()->m_flDuckAmount() > .55f;
				const auto is_moving = ctx.m_local()->m_vecVelocity().Length2D() >= 0.25f;

				// ref: SetupMovement
				// this is all anim events from setupmovement
				if (ctx.m_local()->m_MoveType() != MOVETYPE_LADDER)
				{
					const auto on_ground = (ctx.m_local()->m_fFlags() & FL_ONGROUND);
					const auto was_on_ground = (previous & FL_ONGROUND);

					if (was_on_ground && !on_ground)
					{
						if (cmd->buttons & IN_JUMP)
						{
							int sequence = is_moving ? 16 : 15;

							if (is_crouched)
								sequence = is_moving ? 18 : 17;

							land.m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(&land, sequence);
							land.m_nSequence = sequence;
							land.m_flCycle = land.m_flWeight = 0.f;

							ctx.last_time_layers_fixed = csgo.m_globals()->realtime;
							ctx.local_layers[ANGLE_POSDELTA][4] = land;
							//fix_anim_layer(ctx.m_local(), state, 4, 985);
						}
						else
						{
							//fix_anim_layer(ctx.m_local(), state, 4, 986);
							static int sequence = 14;

							land.m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(&land, sequence);
							land.m_nSequence = sequence;
							land.m_flCycle = land.m_flWeight = 0.f;

							ctx.last_time_layers_fixed = csgo.m_globals()->realtime;
							ctx.local_layers[ANGLE_POSDELTA][4] = land;
						}

						//state->m_duration_in_air = 0;
					}
					else if (on_ground)
					{
						if (!was_on_ground && !state->m_landing) {
							auto sequence = is_moving ? 22 : 20;

							if (is_crouched)
								sequence = is_moving ? 19 : 21;

							if (state->m_duration_in_air > 1)
								sequence = is_crouched ? 24 : 23;

							jump_fall.m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(&jump_fall, sequence);
							jump_fall.m_nSequence = sequence;
							jump_fall.m_flCycle = jump_fall.m_flWeight = 0.f;
							ctx.local_layers[ANGLE_POSDELTA][5] = jump_fall;
							//state->m_landing = true;
							//state->m_on_ground = true;
							//state->m_landed_on_ground_this_frame = true;
							ctx.last_time_layers_fixed = csgo.m_globals()->realtime;
						}

						//state->m_duration_in_air = 0.f;
						//fix_anim_layer(ctx.m_local(), state, 4, int(state->m_duration_in_air > 1) + 988);
					}

					if (!(!was_on_ground && on_ground) && !(cmd->buttons & IN_JUMP))
					{
						//	//SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, 0);
						ctx.last_time_layers_fixed = csgo.m_globals()->realtime;
						jump_fall.m_flWeight = 0;
						ctx.local_layers[ANGLE_POSDELTA][5] = jump_fall;
					}
				}
				else
				{
					auto was_on_ladder = false;
					if (Engine::Prediction::Instance()->GetMoveType() != MOVETYPE_LADDER || (was_on_ladder = true, previous & FL_ONGROUND))
						was_on_ladder = false;

					if (!was_on_ladder && !(ctx.m_local()->m_fFlags() & FL_ONGROUND))
					{
						//fix_anim_layer(ctx.m_local(), state, 4, 987);
						jump_fall.m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(&jump_fall, 13);
						jump_fall.m_nSequence = 13;
						jump_fall.m_flCycle = jump_fall.m_flWeight = 0.f;
						ctx.last_time_layers_fixed = csgo.m_globals()->realtime;
						ctx.local_layers[ANGLE_POSDELTA][5] = jump_fall;
					}
				}
			}
		}
	}
}

void c_lagcomp::update_local_animations(CUserCmd* cmd, bool* send_packet)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

	if (!ctx.m_local() || csgo.m_client_state()->m_iDeltaTick < 0)
		return;

	static auto smoothstep_bounds = [](float edge0, float edge1, float x) -> float
	{
		x = Math::clamp((x - edge0) / (edge1 - edge0), 0, 1);
		return x * x * (3 - 2 * x);
	};

	static bool lock_viewangles = false;
	static float m_flDurationInAir = 0;
	static QAngle target_angle = cmd->viewangles;
	static QAngle non_shot_target_angle = cmd->viewangles;
	//const auto prev_lock_state = lock_viewangles;

	if (ctx.m_ragebot_shot_nr == cmd->command_number)
	{
		lock_viewangles = true;
		target_angle = cmd->viewangles;
	}

	if (!lock_viewangles)
		target_angle = cmd->viewangles;

	auto animstate = ctx.m_local()->get_animation_state();

	//static std::vector<c_sequence_data> saved_sequences = {};

	if (!animstate || ctx.m_local()->IsDead() || ctx.m_local()->IsDormant()) {
		//stored = false;
		spawntime = 0;
		return;
	}

	ctx.m_local()->m_flThirdpersonRecoil() = ctx.m_local()->m_aimPunchAngleScaled().x;
	//static float prev_simtime = ctx.m_local()->m_flSimulationTime();

	const auto current_flags = ctx.m_local()->m_fFlags();

	C_AnimationLayer old_anim_layers[14];
	memcpy(old_anim_layers, ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());

	//fix_anim_layers(cmd, animstate);
	//old_anim_layers[4] = ctx.m_local()->get_animation_layer(4);
	//old_anim_layers[5] = ctx.m_local()->get_animation_layer(5);

	if (ctx.m_local()->m_flSpawnTime() != spawntime || animstate->m_player != ctx.m_local())
	{
		//reset animstate.
		ctx.m_local()->update_clientside_animations();

		//collect fake state
		//memset is done to make sure activity_modifiers wont mess anything up. credits @esoterik XD
		ctx.fake_state = *ctx.m_local()->get_animation_state();
		memset(&ctx.fake_state.m_activity_modifiers_server[0], 0, /*sizeof(char) * */20);

		//fix shit
		memcpy(ctx.m_local()->animation_layers_ptr(), old_anim_layers, 0x38 * ctx.m_local()->get_animation_layers_count());

		ctx.last_time_layers_fixed = 0;

		/*if (backup_layers_saved) {
			auto land = &ctx.m_local()->get_animation_layer(4);
			auto jump_fall = &ctx.m_local()->get_animation_layer(5);
			auto moving = &ctx.m_local()->get_animation_layer(6);
			auto adjust = &ctx.m_local()->get_animation_layer(3);

			*land = backup_saved_layers[0];
			*jump_fall = backup_saved_layers[1];
			*moving = backup_saved_layers[2];
			*adjust = backup_saved_layers[3];
		}*/

		spawntime = ctx.m_local()->m_flSpawnTime();
	}

	const auto old_absvel = ctx.m_local()->m_vecAbsVelocity();
	const auto old_render_angles = ctx.m_local()->get_render_angles();
	const auto old_pose_params = ctx.m_local()->m_flPoseParameter();
	//const auto old_curtime = csgo.m_globals()->curtime;
	//const auto old_frametime = csgo.m_globals()->frametime;

	//csgo.m_globals()->curtime = ctx.m_local()->m_nTickBase();

	//set abs velocity to fix ApproachAngle
	//
	if (feature::anti_aim->animation_speed < 3)
		ctx.m_local()->m_vecAbsVelocity().clear();// (feature::anti_aim->animation_speed > 3 ? ((ctx.m_local()->m_vecOrigin() - Engine::Prediction::Instance()->m_vecOrigin) * (1.f / csgo.m_globals()->interval_per_tick)) : Vector::Zero);
	
	//if (ctx.m_local()->m_vecAbsVelocity().Length() > 260.f)
	//	ctx.m_local()->m_vecAbsVelocity() = ctx.m_local()->m_vecAbsVelocity().Normalized() * 260.f;

	ctx.m_local()->get_render_angles() = ((ctx.main_exploit == 1 || ctx.main_exploit == 3 && ctx.double_tapped < 1) && !ctx.fakeducking && ctx.has_exploit_toggled && ctx.exploit_allowed && lock_viewangles) ? non_shot_target_angle : target_angle;

	//#TODO: fix anim overlays

	animstate->m_move_weight = 0;

	/*if (feet_animation_data[0] != FLT_MAX && feet_animation_data[1] != FLT_MAX)
	{
		ctx.m_local()->get_animation_state()->m_move_weight = feet_animation_data[0];
		ctx.m_local()->get_animation_state()->m_primary_cycle = feet_animation_data[1];
	}*/

	//if (ctx.m_local()->m_fFlags() & FL_ONGROUND)
	//	animstate->m_duration_in_air = 0;

	//C_AnimationLayer* land = &ctx.m_local()->get_animation_layer(4);
	//C_AnimationLayer* jump_fall = &ctx.m_local()->get_animation_layer(5);
	//C_AnimationLayer* moving = &ctx.m_local()->get_animation_layer(6);

	//if (!(current_flags & 0x40) && csgo.m_game_rules() && !csgo.m_game_rules()->IsFreezeTime())
	//{
	//	//auto moving = &ctx.m_local()->get_animation_layer(6);
	//	//auto adjust = &ctx.m_local()->get_animation_layer(3);

	//	/*backup_saved_layers[0] = *land;
	//	backup_saved_layers[1] = *jump_fall;
	//	backup_saved_layers[2] = *moving;
	//	backup_saved_layers[3] = *adjust;
	//	backup_layers_saved = true;*/
	//	//memcpy(&backup_saved_layers[0], *(void**)(uintptr_t(ctx.m_local()->animation_layers_ptr()) + int(sizeof(C_AnimationLayer) * 4)), sizeof(C_AnimationLayer) * 4);

	//	const auto previous = Engine::Prediction::Instance()->GetFlags();
	//	//static bool log = false;
	//	//static int what_to_log = 0;

	//	const auto is_crouched = ctx.m_local()->get_animation_state()->m_anim_duck_amount > .55f;
	//	const auto is_moving = ctx.m_local()->get_animation_state()->m_velocity_length_xy > .25f;

	//	// ref: SetupMovement
	//	// this is all anim events from setupmovement
	//	if (ctx.m_local()->m_MoveType() != MOVETYPE_LADDER)
	//	{
	//		const auto on_ground = (current_flags & FL_ONGROUND) != 0;
	//		const auto was_on_ground = (previous & FL_ONGROUND) != 0;

	//		if (was_on_ground && !on_ground)
	//		{
	//			if (cmd->buttons & IN_JUMP)
	//			{
	//				int sequence = is_moving ? 16 : 15;

	//				if (is_crouched)
	//					sequence = is_moving ? 18 : 17;

	//				land->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(land, sequence);
	//				land->m_nSequence = sequence;
	//				land->m_flCycle = land->m_flWeight = 0.f;
	//			}
	//			else
	//			{
	//				static int sequence = 14;

	//				land->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(land, sequence);
	//				land->m_nSequence = sequence;
	//				land->m_flCycle = land->m_flWeight = 0.f;
	//			}

	//			m_flDurationInAir = 0;
	//		}
	//		else if (on_ground && !was_on_ground && !animstate->m_landing)
	//		{
	//			auto sequence = is_moving ? 22 : 20;

	//			if (is_crouched)
	//				sequence = is_moving ? 19 : 21;

	//			if (cmd->buttons & IN_JUMP)
	//				sequence = animstate->m_duration_in_air > 1 ? 24 : 23;

	//			jump_fall->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(jump_fall, sequence);
	//			jump_fall->m_nSequence = sequence;
	//			jump_fall->m_flCycle = jump_fall->m_flWeight = 0.f;

	//			/*anim_state->m_landing = true;
	//			anim_state->m_on_ground = true;
	//			anim_state->m_landed_on_ground_this_frame = true;
	//			anim_state->m_duration_in_air = 0.f;*/
	//			//ctx.fake_state.m_landing = true;
	//		}

	//		if ((!was_on_ground && on_ground) && !(cmd->buttons & IN_JUMP))
	//		{
	//			//SetLayerWeight(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB, 0);
	//			jump_fall->m_flWeight = 0;
	//		}
	//	}
	//	else
	//	{
	//		auto was_on_ladder = false;
	//		if (Engine::Prediction::Instance()->GetMoveType() != MOVETYPE_LADDER || (was_on_ladder = true, previous & FL_ONGROUND))
	//			was_on_ladder = false;

	//		if (!was_on_ladder && !(ctx.m_local()->m_fFlags() & FL_ONGROUND))
	//		{
	//			jump_fall->m_flPlaybackRate = ctx.m_local()->GetLayerSequenceCycleRate(jump_fall, 13);
	//			jump_fall->m_nSequence = 13;
	//			jump_fall->m_flCycle = jump_fall->m_flWeight = 0.f;
	//		}
	//	}

	//	/*char szWeaponMoveSeq[64];
	//	sprintf_s(szWeaponMoveSeq, sxor("move_%s"), animstate->GetWeaponPrefix());

	//	int nWeaponMoveSeq = ctx.m_local()->LookupSequence(szWeaponMoveSeq);

	//	if (nWeaponMoveSeq == -1)
	//		nWeaponMoveSeq = ctx.m_local()->LookupSequence(sxor("move"));

	//	moving->m_nSequence = nWeaponMoveSeq;*/

	//	//old_anim_layers[6] = *moving;
	//	//old_anim_layers[3] = *adjust;

	//	/*auto pressed_move_key = (cmd->buttons & IN_MOVELEFT
	//		|| cmd->buttons & IN_MOVERIGHT
	//		|| cmd->buttons & IN_BACK
	//		|| cmd->buttons & IN_FORWARD);*/

	//	//char szWeaponMoveSeq[64];
	//	//sprintf_s(szWeaponMoveSeq, sxor("move_%s"), animstate->GetWeaponPrefix());

	//	//int nWeaponMoveSeq = ctx.m_local()->LookupSequence(szWeaponMoveSeq);

	//	//if (nWeaponMoveSeq == -1)
	//	//	nWeaponMoveSeq = ctx.m_local()->LookupSequence(sxor("move"));

	//	////if (nWeaponMoveSeq) 
	//	////{
	//	//auto m_flStutterStep = animstate->m_stutter_step;

	//	//if (ctx.m_local()->m_iMoveState() != animstate->m_previous_move_state)
	//	//	m_flStutterStep += 10;

	//	////animstate->m_previous_move_state = ctx.m_local()->m_iMoveState();
	//	//m_flStutterStep = Math::clamp(Math::Approach(0, m_flStutterStep, csgo.m_globals()->interval_per_tick * 40), 0, 100);

	//	//float m_flMoveWeight = ctx.m_local()->get_animation_state()->m_move_weight;
	//	//float m_flPrimaryCycle = ctx.m_local()->get_animation_state()->m_primary_cycle;

	//	//float flTargetMoveWeight = Math::Lerp(ctx.m_local()->m_flDuckAmount(), Math::clamp(anim_state->m_speed_as_portion_of_walk_top_speed, 0, 1), Math::clamp(anim_state->m_speed_as_portion_of_crouch_top_speed, 0, 1));
	//	////flTargetMoveWeight *= Math::RemapValClamped( m_flStutterStep, 90, 100, 1, 0 );

	//	//if (m_flMoveWeight <= flTargetMoveWeight)
	//	//	m_flMoveWeight = flTargetMoveWeight;
	//	//else
	//	//	m_flMoveWeight = Math::Approach(flTargetMoveWeight, m_flMoveWeight, csgo.m_globals()->interval_per_tick * Math::RemapValClamped(m_flStutterStep, 0.0f, 100.0f, 2, 20));

	//	//Vector vecMoveYawDir;
	//	//Math::AngleVectors(QAngle(0, Math::normalize_angle(animstate->m_abs_yaw + animstate->m_move_yaw + 180), 0), &vecMoveYawDir);
	//	//float flYawDeltaAbsDot = abs(animstate->m_velocity_normalized_non_zero.Dot(vecMoveYawDir));
	//	//m_flMoveWeight *= Math::Bias(flYawDeltaAbsDot, 0.2f);

	//	//float flMoveWeightWithAirSmooth = m_flMoveWeight * animstate->m_in_air_smooth_value;

	//	//// dampen move weight for landings
	//	//flMoveWeightWithAirSmooth *= max((1.0f - jump_fall->m_flWeight), 0.55f);

	//	//const auto m_flVelocityLengthXY = ctx.m_local()->m_vecAbsVelocity().Length2D();

	//	//float flMoveCycleRate = 0;
	//	//if (m_flVelocityLengthXY > 0 /*&& pressed_move_key*/)
	//	//{
	//	//	flMoveCycleRate = ctx.m_local()->GetSequenceCycleRate(ctx.m_local()->GetModelPtr(), nWeaponMoveSeq);
	//	//	float flSequenceGroundSpeed = max(ctx.m_local()->GetSequenceMoveDist(ctx.m_local()->GetModelPtr(), nWeaponMoveSeq) / (1.0f / flMoveCycleRate), 0.001f);
	//	//	flMoveCycleRate *= m_flVelocityLengthXY / flSequenceGroundSpeed;

	//	//	flMoveCycleRate *= Math::Lerp(feature::anti_aim->stop_to_full_running_fraction, 1.0f, 0.85f);
	//	//}

	//	//float flLocalCycleIncrement = (anim_state->m_move_cycle_rate * csgo.m_globals()->interval_per_tick);
	//	//m_flPrimaryCycle = Math::ClampCycle(m_flPrimaryCycle + flLocalCycleIncrement);

	//	//flMoveWeightWithAirSmooth = Math::clamp(flMoveWeightWithAirSmooth, 0, 1);

	//	//moving->m_nSequence = nWeaponMoveSeq;
	//	//moving->m_flPlaybackRate = flLocalCycleIncrement;
	//	//moving->m_flWeight = flMoveWeightWithAirSmooth;
	//	//moving->m_flCycle = m_flPrimaryCycle;

	//	//ctx.m_local()->get_animation_state()->m_move_weight = m_flMoveWeight;
	//	//ctx.m_local()->get_animation_state()->m_primary_cycle = m_flPrimaryCycle;
	//	//}
	//}
	//if (!send_packet)

	//old_anim_layers[4] = ctx.m_local()->get_animation_layer(4);
	//old_anim_layers[5] = ctx.m_local()->get_animation_layer(5);

//memcpy(&ctx.local_layers[ANGLE_POSDELTA][4], &ctx.m_local()->get_animation_layer(4), sizeof(C_AnimationLayer));
//memcpy(&ctx.local_layers[ANGLE_POSDELTA][5], &ctx.m_local()->get_animation_layer(5), sizeof(C_AnimationLayer));
//ctx.m_local()->get_animation_layer(4).m_nSequence = ctx.local_layers[ANGLE_POSDELTA][4].m_nSequence;
//ctx.m_local()->get_animation_layer(4).m_flCycle = ctx.local_layers[ANGLE_POSDELTA][4].m_flCycle;
//ctx.m_local()->get_animation_layer(4).m_flPlaybackRate = ctx.local_layers[ANGLE_POSDELTA][4].m_flPlaybackRate;
//ctx.m_local()->get_animation_layer(4).m_flWeight = ctx.local_layers[ANGLE_POSDELTA][4].m_flWeight;
//
//ctx.m_local()->get_animation_layer(5).m_nSequence = ctx.local_layers[ANGLE_POSDELTA][5].m_nSequence;
//ctx.m_local()->get_animation_layer(5).m_flCycle = ctx.local_layers[ANGLE_POSDELTA][5].m_flCycle;
//ctx.m_local()->get_animation_layer(5).m_flPlaybackRate = ctx.local_layers[ANGLE_POSDELTA][5].m_flPlaybackRate;
//ctx.m_local()->get_animation_layer(5).m_flWeight = ctx.local_layers[ANGLE_POSDELTA][5].m_flWeight;
	//if (csgo.m_client_state()->m_iChockedCommands != 0) 
	//{
	//}
	ctx.m_local()->update_clientside_animations();
	
	//if (!(current_flags & 0x40) && csgo.m_game_rules() && !csgo.m_game_rules()->IsFreezeTime() && animstate->m_pose_param_mappings[PLAYER_POSE_PARAM_JUMP_FALL].get_index() >= 0)
	//	ctx.m_local()->set_pose_param(animstate->m_pose_param_mappings[PLAYER_POSE_PARAM_JUMP_FALL].get_index(), (ctx.poses[ANGLE_REAL][animstate->m_pose_param_mappings[PLAYER_POSE_PARAM_JUMP_FALL].get_index()] = Math::clamp(smoothstep_bounds(0.72f, 1.52f, m_flDurationInAir), 0, 1)));

	ctx.m_local()->force_bone_rebuild();
	ctx.m_local()->SetupBonesEx();

	ctx.m_local()->get_animation_state()->m_primary_cycle = old_anim_layers[6].m_flCycle;
	ctx.m_local()->get_animation_state()->m_move_weight = old_anim_layers[6].m_flWeight;
	ctx.m_local()->get_animation_state()->m_strafe_change_cycle = old_anim_layers[7].m_flCycle;
	ctx.m_local()->get_animation_state()->m_strafe_sequence = old_anim_layers[7].m_nSequence;
	ctx.m_local()->get_animation_state()->m_strafe_change_weight = old_anim_layers[7].m_flWeight;
	ctx.m_local()->get_animation_state()->m_acceleration_weight = old_anim_layers[12].m_flWeight;

	if (*send_packet == false) {
		//if (!(current_flags & 0x40) && csgo.m_game_rules() && !csgo.m_game_rules()->IsFreezeTime()) {
		//	//if (abs(csgo.m_globals()->realtime - ctx.last_time_layers_fixed) <= 0.5f) {
		//		memcpy(&ctx.local_layers[ANGLE_POSDELTA][4], &ctx.m_local()->get_animation_layer(4), sizeof(C_AnimationLayer));
		//		memcpy(&ctx.local_layers[ANGLE_POSDELTA][5], &ctx.m_local()->get_animation_layer(5), sizeof(C_AnimationLayer));
		//		old_anim_layers[4].m_nSequence = ctx.m_local()->get_animation_layer(4).m_nSequence;
		//		old_anim_layers[4].m_flCycle = ctx.m_local()->get_animation_layer(4).m_flCycle;
		//		old_anim_layers[4].m_flPlaybackRate = ctx.m_local()->get_animation_layer(4).m_flPlaybackRate;
		//		old_anim_layers[4].m_flWeight = ctx.m_local()->get_animation_layer(4).m_flWeight;

		//		old_anim_layers[5].m_nSequence = ctx.m_local()->get_animation_layer(5).m_nSequence;
		//		old_anim_layers[5].m_flCycle = ctx.m_local()->get_animation_layer(5).m_flCycle;
		//		old_anim_layers[5].m_flPlaybackRate = ctx.m_local()->get_animation_layer(5).m_flPlaybackRate;
		//		old_anim_layers[5].m_flWeight = ctx.m_local()->get_animation_layer(5).m_flWeight;
		//	//}
		//}

		//old_anim_layers[4] = ctx.m_local()->get_animation_layer(4);
		//old_anim_layers[5] = ctx.m_local()->get_animation_layer(5);
		memcpy(ctx.m_local()->animation_layers_ptr(), old_anim_layers, 0x38 * ctx.m_local()->get_animation_layers_count());
		return;
	}

	if (!lock_viewangles)
		non_shot_target_angle = cmd->viewangles;

	/*if (ctx.pressed_keys['R']) {
		feature::ragebot->visualize_hitboxes(ctx.m_local(), ctx.m_local()->m_CachedBoneData().Base(), Color::White(), 4.f);
		ctx.m_local()->DrawServerHitboxes(4.f);
	}*/
	//feature::ragebot->visualize_hitboxes(ctx.m_local(), ctx.m_local()->m_CachedBoneData().Base(), Color::White(), 4.f);

	//save off real data.
	ctx.poses[ANGLE_REAL] = ctx.m_local()->m_flPoseParameter();
	ctx.angles[ANGLE_REAL] = ctx.m_local()->get_animation_state()->m_abs_yaw;
	ctx.abs_origin[ANGLE_REAL] = ctx.m_local()->get_abs_origin();
	memcpy(ctx.local_layers[ANGLE_REAL], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());

	//csgo.m_engine()->GetViewAngles(ctx.camera_angles[ANGLE_REAL]);

	//restore animation backup
	ctx.m_local()->m_flPoseParameter() = old_pose_params;
	memcpy(ctx.m_local()->animation_layers_ptr(), old_anim_layers, 0x38 * ctx.m_local()->get_animation_layers_count());

	//static auto previous_org = ctx.m_local()->m_vecOrigin();

	//const auto choke = TICKS_TO_TIME(csgo.m_client_state()->m_iChockedCommands);
	//auto fake_velocity = feature::anti_aim->animation_speed > 2 ? ((ctx.m_local()->m_vecOrigin() - previous_org) * (1.f / max(csgo.m_globals()->interval_per_tick, choke))) : Vector::Zero;
	//previous_org = ctx.m_local()->m_vecOrigin();

	if (feature::anti_aim->animation_speed < 3)
		ctx.m_local()->m_vecAbsVelocity().clear();

	//if (ctx.m_local()->m_vecAbsVelocity().Length() > 260.f)
	//	ctx.m_local()->m_vecAbsVelocity() = ctx.m_local()->m_vecAbsVelocity().Normalized() * 260.f;

	ctx.m_local()->get_render_angles() = target_angle;

	const auto backup_state = *ctx.m_local()->get_animation_state();
	//ctx.fake_state.m_on_ground = false;
	ctx.fake_state.m_landing = false;
	ctx.fake_state.m_last_update_frame = 0;
	*ctx.m_local()->get_animation_state() = ctx.fake_state;
	ctx.is_updating_fake = true;
	ctx.m_local()->update_clientside_animations();
	ctx.is_updating_fake = false;
	ctx.fake_state = *ctx.m_local()->get_animation_state();
	*ctx.m_local()->get_animation_state() = backup_state;
	
	//save off fake data.
	ctx.poses[ANGLE_FAKE] = ctx.m_local()->m_flPoseParameter();
	ctx.angles[ANGLE_FAKE] = ctx.fake_state.m_abs_yaw;
	ctx.abs_origin[ANGLE_FAKE] = ctx.m_local()->get_abs_origin();
	//csgo.m_engine()->GetViewAngles(ctx.camera_angles[ANGLE_FAKE]);
	memcpy(ctx.local_layers[ANGLE_FAKE], ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());

	ctx.local_layers[ANGLE_FAKE][12].m_flWeight = 0;
	ctx.local_layers[ANGLE_FAKE][3].m_flWeight = 0;

	//ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
	ctx.m_local()->force_bone_rebuild();
	//ctx.m_local()->set_abs_angles(QAngle(0, ctx.angles[ANGLE_FAKE], 0));
	memcpy(ctx.m_local()->animation_layers_ptr(), ctx.local_layers[ANGLE_FAKE], 0x38 * ctx.m_local()->get_animation_layers_count());

	ctx.m_local()->SetupBonesEx();
	memcpy(ctx.fake_matrix, ctx.m_local()->m_CachedBoneData().Base(), min(128, ctx.m_local()->m_bone_count()) * sizeof(matrix3x4_t));

	//restore animation backup
	ctx.m_local()->m_flPoseParameter() = old_pose_params;

	//old_anim_layers[4] = ctx.local_layers[ANGLE_REAL][4];
	//old_anim_layers[5] = ctx.local_layers[ANGLE_REAL][5];
	memcpy(ctx.m_local()->animation_layers_ptr(), old_anim_layers, 0x38 * ctx.m_local()->get_animation_layers_count());
	//fix_anim_layers(animstate);

	ctx.m_local()->m_vecAbsVelocity() = old_absvel;
	ctx.m_local()->get_render_angles() = old_render_angles;
	ctx.m_local()->m_iEFlags() &= ~EFL_DIRTY_ABSANGVELOCITY;

	//if (ctx.pressed_keys['R']) {
		//_events.emplace_back("" + std::to_string(ctx.local_layers[ANGLE_REAL][3].m_flPlaybackRate - old_anim_layers[3].m_flPlaybackRate));
	//}

	lock_viewangles = false;

	//target angle = cmd;
	//if (did_shot_this_tick)
	//{
	//	shot angle = cmd;
	//	lock viewangles = true
	//}

	//if (lock viewangles)
	//	target angle = shot angle
	//	set abs velocity
	//	set visual angles = target angle;
	//  fix anim overlays
	//	update client side animation
	//	lby prediction
	//	setup bones
	//	if (*bSendPacket == false)
	//		return;
	//collect anim data for visual anim fix(bones, position, etc.)
	//	backup anim data(layers, poses, abs angles)
	//	fake velocity = (origin - old_origin) / choke_delta
	//	set abs velocity = fake velocity

	//	update anim state
	//	collect fake data for visual anim fix
	//	restore anim data

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}

void c_lagcomp::build_local_bones(C_BasePlayer* local)
{
	const auto dolboeb = local->m_flPoseParameter();
	local->force_bone_rebuild();
	local->m_flPoseParameter() = ctx.poses[ANGLE_REAL];
	local->set_abs_angles(QAngle(0, ctx.angles[ANGLE_REAL], 0));
	memcpy(ctx.m_local()->animation_layers_ptr(), ctx.local_layers[ANGLE_REAL], 0x38 * ctx.m_local()->get_animation_layers_count());
	/*cheat::main::setuped_bones = */local->SetupBonesEx();
	//memcpy(ctx.matrix, ctx.m_local()->m_CachedBoneData().Base(), min(128, ctx.m_local()->GetBoneCount()) * sizeof(matrix3x4_t));
	local->m_flPoseParameter() = dolboeb;
}

bool c_lagcomp::can_resolve_by_anims(C_Tickrecord* record, C_Tickrecord* prev_record)
{
	if (prev_record->anim_layers[6].m_nSequence != record->anim_layers[6].m_nSequence
		|| int(record->anim_layers[6].m_flWeight * 1000.f) == int(prev_record->anim_layers[6].m_flWeight * 1000.f)
		|| record->anim_layers[12].m_flWeight != 0 )
		return false;
	
	return true;
}

//float rotate(C_BasePlayer* m_player, C_Tickrecord* record, int side, float delta_multiplier)
//{
//	if (m_player->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_MOVE_YAW].get_index() < 0 
//		|| m_player->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_BODY_YAW].get_index() < 0)
//		return record->eye_angles.y;
//
//	//const auto original_delta = 58.f * delta_multiplier;
//
//	if (side == 2)
//		delta_multiplier *= -1.f;
//
//	// narrow the available aim matrix width as speed increases
//	float flAimMatrixWidthRange = Math::Lerp(Math::clamp(m_player->get_animation_state()->m_speed_as_portion_of_walk_top_speed, 0, 1), 1.0f, Math::Lerp(m_player->get_animation_state()->m_walk_run_transition, 0.8f, 0.5f));
//
//	if (m_player->get_animation_state()->m_anim_duck_amount > 0)
//		flAimMatrixWidthRange = Math::Lerp(m_player->get_animation_state()->m_anim_duck_amount * Math::clamp(m_player->get_animation_state()->m_speed_as_portion_of_crouch_top_speed, 0, 1), flAimMatrixWidthRange, 0.5f);
//
//	float flTempYawMax = m_player->get_animation_state()->m_aim_yaw_max * flAimMatrixWidthRange;
//	float flTempYawMin = m_player->get_animation_state()->m_aim_yaw_min * flAimMatrixWidthRange;
//		
//	auto resolved_yaw = Math::normalize_angle(side == 0 ? m_player->get_animation_state()->m_abs_yaw : (record->eye_angles.y + (flTempYawMax * delta_multiplier)));
//
//	record->desync_delta = flTempYawMax;
//
//	float flEyeFootDelta = Math::AngleDiff(record->eye_angles.y, resolved_yaw);
//
//	if (flEyeFootDelta > flTempYawMax)
//		resolved_yaw = record->eye_angles.y - abs(flTempYawMax);
//	else if (flEyeFootDelta < flTempYawMin)
//		resolved_yaw = record->eye_angles.y + abs(flTempYawMin);
//	
//	resolved_yaw = Math::normalize_angle(resolved_yaw);
//
//	// pull the lower body direction towards the eye direction, but only when the player is moving
//	if (m_player->get_animation_state()->m_velocity_length_xy > 0.1f || abs(m_player->get_animation_state()->m_velocity_length_z) > 100.f)
//		resolved_yaw = Math::ApproachAngle(record->eye_angles.y, resolved_yaw, csgo.m_globals()->interval_per_tick * (30.0f + 20.0f * m_player->get_animation_state()->m_walk_run_transition));
//	else
//		resolved_yaw = Math::ApproachAngle(record->lower_body_yaw, resolved_yaw, csgo.m_globals()->interval_per_tick * 100.f);
//
//	resolved_yaw = Math::normalize_angle(resolved_yaw);
//
//	//if (ctx.m_settings.aimbot_position_adjustment_old)
//	//{
//	//	resolved_yaw = record->eye_angles.y; // tempfix.
//	//}
//	//else
//	//{
//	//if (m_player->get_animation_state()->m_velocity_length_xy > 0 && m_player->get_animation_state()->m_on_ground)
//	//{
//	//	// convert horizontal velocity vec to angular yaw
//	//	float flRawYawIdeal = (atan2(-m_player->get_animation_state()->m_velocity[1], -m_player->get_animation_state()->m_velocity[0]) * 180 / M_PI);
//
//	//	if (flRawYawIdeal < 0)
//	//		flRawYawIdeal += 360;
//
//	//	m_player->get_animation_state()->m_move_yaw_ideal = Math::normalize_angle(Math::AngleDiff(flRawYawIdeal, resolved_yaw));
//	//}
//
//	//// delta between current yaw and ideal velocity derived target (possibly negative!)
//	//m_player->get_animation_state()->m_move_yaw_current_to_ideal = Math::normalize_angle(Math::AngleDiff(m_player->get_animation_state()->m_move_yaw_ideal, m_player->get_animation_state()->m_move_yaw));
//
//	//if (m_player->get_animation_state()->m_duration_moving <= csgo.m_globals()->interval_per_tick && m_player->get_animation_state()->m_move_weight <= 0)
//	//	m_player->get_animation_state()->m_move_yaw = m_player->get_animation_state()->m_move_yaw_ideal;
//	//else
//	//{
//	//	if (record->anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight >= 1)
//	//		m_player->get_animation_state()->m_move_yaw = m_player->get_animation_state()->m_move_yaw_ideal;
//	//	else
//	//	{
//	//		float flMoveWeight = Math::Lerp(m_player->get_animation_state()->m_anim_duck_amount, Math::clamp(m_player->get_animation_state()->m_speed_as_portion_of_walk_top_speed, 0, 1), Math::clamp(m_player->get_animation_state()->m_speed_as_portion_of_crouch_top_speed, 0, 1));
//	//		float flRatio = Math::Bias(flMoveWeight, 0.18f) + 0.1f;
//
//	//		m_player->get_animation_state()->m_move_yaw = Math::normalize_angle(m_player->get_animation_state()->m_move_yaw + (m_player->get_animation_state()->m_move_yaw_current_to_ideal * flRatio));
//	//	}
//	//}
//
//	auto v52 = ((record->pose_paramaters[m_player->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_MOVE_YAW].get_index()] * 360.f) - 180.f) - (resolved_yaw - m_player->get_animation_state()->m_abs_yaw);// ((poseparam[move_yaw] * 360) - 180) - (abs_yaw - animstate->abs_yaw)
//	if (v52 <= 180.f)
//	{
//		if (v52 < -180.f)
//			v52 = v52 + 360.f;
//	}
//	else
//	{
//		v52 = v52 - 360.0f;
//	}
//
//	m_player->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_MOVE_YAW].set_value(m_player, (v52 * 0.0027777778) + 0.5);
//
//	float flAimYaw = Math::AngleDiff(m_player->get_animation_state()->m_abs_yaw, resolved_yaw);
//
//	if (flAimYaw >= 0 && m_player->get_animation_state()->m_aim_yaw_max != 0)
//		flAimYaw = (flAimYaw / m_player->get_animation_state()->m_aim_yaw_max) * 60;
//	else if (m_player->get_animation_state()->m_aim_yaw_min != 0)
//		flAimYaw = (flAimYaw / m_player->get_animation_state()->m_aim_yaw_min) * -60;
//
//	m_player->get_animation_state()->m_pose_param_mappings[PLAYER_POSE_PARAM_BODY_YAW].set_value(m_player, flAimYaw);
//	//}
//
//	const auto absangles = m_player->get_abs_angles();
//	m_player->set_abs_angles(QAngle(absangles.x, resolved_yaw, absangles.z));
//
//	return resolved_yaw;
//}
//
//float brutelist_shots[] =
//{
//	0.f,
//	119.f,
//	180.f,
//	-119.f,
//	180.f,
//	-82.f,
//};

bool c_lagcomp::has_firing_animation(C_BasePlayer* m_player, C_Tickrecord* record)
{
	auto weapon = m_player->get_weapon();
	if (weapon)
	{
		int iWeaponIndex = weapon->m_iItemDefinitionIndex();
		auto act = m_player->get_sec_activity(record->anim_layers[1].m_nSequence);
		if (act == ACT_CSGO_FIRE_PRIMARY || ((act == ACT_CSGO_FIRE_SECONDARY || act == ACT_CSGO_FIRE_SECONDARY_OPT_1 || act == ACT_CSGO_FIRE_SECONDARY_OPT_2) && (iWeaponIndex == WEAPON_GLOCK || iWeaponIndex == WEAPON_REVOLVER || iWeaponIndex == WEAPON_FAMAS || weapon->is_knife())))
			return true;
	}
	return false;
}

//void fix_jump_fall(C_BasePlayer* m_player, C_Tickrecord* record, C_Tickrecord* previous)
//{
//	if (!(m_player->m_fFlags() & FL_ONGROUND))
//	{
//		int CurrentActivity = m_player->get_sec_activity(record->anim_layers[4].m_nSequence);
//		if (CurrentActivity == ACT_CSGO_FALL || CurrentActivity == ACT_CSGO_JUMP)
//		{
//			float startcycle;
//			if (m_player->get_sec_activity(previous->anim_layers[4].m_nSequence) != CurrentActivity)
//			{
//				//we know the animation was reset, so it started from 0
//				startcycle = 0.0f;
//
//				m_player->get_animation_state()->m_duration_in_air = 0.0f;
//			}
//			else if (previous->anim_layers[4].m_flCycle < previous->anim_layers[4].m_flCycle)
//			{
//				//we know it was reset, so it started from 0
//				startcycle = 0.0f;
//				m_player->get_animation_state()->m_duration_in_air = 0.0f;
//			}
//			else
//			{
//				//it incremented so see how many it incremented by
//				//FIXME: what if it reset to 0 and incremented past the previous value? TODO: handle that case,
//				//could check if delta between target and predicted value in backtrace is too much
//
//				startcycle = previous->anim_layers[4].m_flCycle;
//
//				m_player->get_animation_state()->m_duration_in_air = previous->animstate.m_duration_in_air;
//			}
//
//			int ticks_taken_to_get_to_server_cycle = 0;
//
//			if (m_player->m_MoveType() != MOVETYPE_LADDER && record->anim_layers[4].m_flPlaybackRate > 0.0f)
//			{
//				float flTargetCycle = record->anim_layers[4].m_flCycle;
//
//				if (flTargetCycle != startcycle)
//				{
//					//int totalcmds = pCPlayer->m_iTicksChoked + 1;
//
//					float flCurrentCycle = startcycle;
//					float flTimeDelta = m_player->get_animation_state()->m_last_update_increment != 0.0f ? m_player->get_animation_state()->m_last_update_increment : csgo.m_globals()->interval_per_tick;
//					while (flCurrentCycle < flTargetCycle)
//					{
//						flCurrentCycle += record->anim_layers[4].m_flPlaybackRate * flTimeDelta;
//						++ticks_taken_to_get_to_server_cycle;
//					}
//				}
//			}
//
//			if (ticks_taken_to_get_to_server_cycle == 0 || record->anim_layers[4].m_flCycle >= 0.999f)
//			{
//				//if animation is finished already, fix up the total time in air
//				int total_commands = record->lag;
//				int ticks_left = total_commands - ticks_taken_to_get_to_server_cycle;
//				ticks_taken_to_get_to_server_cycle += ticks_left;
//			}
//
//			float newdelta = (m_player->get_animation_state()->m_last_update_increment * ticks_taken_to_get_to_server_cycle);;
//			m_player->get_animation_state()->m_duration_in_air += newdelta;
//
//			float v203 = (m_player->get_animation_state()->m_duration_in_air - 0.72f) * 1.25f;
//			v203 = Math::clamp(v203, 0.0f, 1.0f);
//			float newpose10 = (3.0f - (v203 + v203)) * (v203 * v203);
//			newpose10 = Math::clamp(newpose10, 0.0f, 1.0f);
//			m_player->set_pose_param(10, newpose10);
//		}
//	}
//}

void c_lagcomp::prepare_player_anim_update(C_BasePlayer* m_player, C_Tickrecord* record, C_Tickrecord* previous, int resolver_side)
{
	VIRTUALIZER_FISH_LITE_START;

	resolver_records* resolver_info = &feature::resolver->player_records[m_player->entindex() - 1];
	c_player_records* log = &records[m_player->entindex() - 1];

	if (previous && !previous->dormant && previous->data_filled && !record->first_after_dormancy)
	{
		m_player->get_animation_state()->m_primary_cycle = previous->m_primary_cycle;
		m_player->get_animation_state()->m_move_weight = previous->m_move_weight;

		m_player->get_animation_state()->m_strafe_sequence = previous->m_strafe_sequence;
		m_player->get_animation_state()->m_strafe_change_weight = previous->m_strafe_change_weight;
		m_player->get_animation_state()->m_strafe_change_cycle = previous->m_strafe_change_cycle;
		m_player->get_animation_state()->m_acceleration_weight = previous->m_acceleration_weight;

		if (resolver_side == 0) {
			m_player->get_animation_state()->m_abs_yaw = previous->animstate.m_abs_yaw;
			m_player->get_animation_state()->m_abs_yaw_last = previous->animstate.m_abs_yaw_last;
			m_player->get_animation_state()->m_move_yaw = previous->animstate.m_move_yaw;
			m_player->get_animation_state()->m_move_yaw_ideal = previous->animstate.m_move_yaw_ideal;
			m_player->get_animation_state()->m_move_yaw_current_to_ideal = previous->animstate.m_move_yaw_current_to_ideal;
			m_player->get_animation_state()->m_move_weight_smoothed = previous->animstate.m_move_weight_smoothed;
			m_player->get_animation_state()->m_in_air_smooth_value = previous->animstate.m_in_air_smooth_value;
			m_player->get_animation_state()->m_time_to_align_lower_body = previous->animstate.m_time_to_align_lower_body;
		}
		else
		{
			if (resolver_side > 0)
			{
				m_player->get_animation_state()->m_abs_yaw = previous->animstate_right_params[0];
				m_player->get_animation_state()->m_abs_yaw_last = previous->animstate_right_params[1];
				m_player->get_animation_state()->m_move_yaw = previous->animstate_right_params[2];
				m_player->get_animation_state()->m_move_yaw_ideal = previous->animstate_right_params[3];
				m_player->get_animation_state()->m_move_yaw_current_to_ideal = previous->animstate_right_params[4];
				m_player->get_animation_state()->m_move_weight_smoothed = previous->animstate_right_params[5];
				m_player->get_animation_state()->m_in_air_smooth_value = previous->animstate_right_params[6];
				m_player->get_animation_state()->m_time_to_align_lower_body = previous->animstate_right_params[7];
			}
			else
			{
				m_player->get_animation_state()->m_abs_yaw = previous->animstate_left_params[0];
				m_player->get_animation_state()->m_abs_yaw_last = previous->animstate_left_params[1];
				m_player->get_animation_state()->m_move_yaw = previous->animstate_left_params[2];
				m_player->get_animation_state()->m_move_yaw_ideal = previous->animstate_left_params[3];
				m_player->get_animation_state()->m_move_yaw_current_to_ideal = previous->animstate_left_params[4];
				m_player->get_animation_state()->m_move_weight_smoothed = previous->animstate_left_params[5];
				m_player->get_animation_state()->m_in_air_smooth_value = previous->animstate_left_params[6];
				m_player->get_animation_state()->m_time_to_align_lower_body = previous->animstate_left_params[7];
			}
		}

		//if (!(record->entity_flags & FL_ONGROUND) || !(previous->entity_flags & FL_ONGROUND) /*&& record->velocity.Length() > 0.1f*/)
		//{
		//	/*if (record->simulation_time == record->ground_accel_last_time)
		//		record->velocity.z = (sv_jump_impulse->GetFloat() - (sv_gravity->GetFloat() * csgo.m_globals()->interval_per_tick) * 0.5f);
		//	else */
		//	if (record->simulation_time >= record->ground_accel_last_time && record->ground_accel_last_time > previous->simulation_time)
		//		resolver_info->tick_jumped = TIME_TO_TICKS(record->simulation_time - record->ground_accel_last_time);
		//}

		memcpy(m_player->animation_layers_ptr(), previous->anim_layers, 0x38 * m_player->get_animation_layers_count());
	}
	else
	{
		/* if entity came out of dormancy we need to fix client so it will properly animate him */
		//if (record->first_after_dormancy)
		//	m_player->get_animation_state()->m_abs_yaw = m_player->m_angEyeAngles().y;

		//record->valid = false;

		//m_player->get_animation_state()->m_move_weight = 0;

		if (record->entity_flags & FL_ONGROUND) {
			m_player->get_animation_state()->m_on_ground = true;
			m_player->get_animation_state()->m_landing = false;
		}

		m_player->get_animation_state()->m_primary_cycle = record->anim_layers[6].m_flCycle;
		m_player->get_animation_state()->m_move_weight = record->anim_layers[6].m_flWeight;
		m_player->get_animation_state()->m_strafe_sequence = record->m_strafe_sequence;
		m_player->get_animation_state()->m_strafe_change_weight = record->m_strafe_change_weight;
		m_player->get_animation_state()->m_strafe_change_cycle = record->m_strafe_change_cycle;
		m_player->get_animation_state()->m_acceleration_weight = record->m_acceleration_weight;
		m_player->get_animation_state()->m_duration_in_air = 0.f;
		m_player->m_flPoseParameter()[6] = 0.f;

		memcpy(m_player->animation_layers_ptr(), record->anim_layers, 0x38 * m_player->get_animation_layers_count());
		m_player->get_animation_state()->m_last_update_time = (record->simulation_time - csgo.m_globals()->interval_per_tick);
	}

	m_player->m_vecOrigin() = record->origin;
	m_player->set_abs_origin(record->origin);

	record->duck_amount_per_tick = 0.0f;
	record->lby_flicked_time = 0;
	m_player->m_flLowerBodyYawTarget() = record->lower_body_yaw;
	m_player->m_flThirdpersonRecoil() = record->thirdperson_recoil;
	record->entity_anim_flags = record->entity_flags;

	if (previous && previous->data_filled && !previous->dormant && !record->first_after_dormancy) {
		if (record->duck_amt != previous->duck_amt 
			&& record->entity_flags & FL_ONGROUND 
			&& previous->entity_flags & FL_ONGROUND)
		{
			//if (record->time_delta != 0.0f)
			//	record->duck_amount_per_tick = csgo.m_globals()->interval_per_tick / record->time_delta;
			auto v210 = record->time_delta - TICKS_TO_TIME(1);
			if (v210 != 0.f)
				record->duck_amount_per_tick = csgo.m_globals()->interval_per_tick / v210;
		}

		if (record->lag > 1)
		{
			m_player->m_flDuckAmount() = previous->duck_amt;

			if (record->shot_this_tick) {
				m_player->m_flThirdpersonRecoil() = previous->thirdperson_recoil;
				m_player->m_flLowerBodyYawTarget() = previous->lower_body_yaw;
			}

			record->lby_flicked_time = 0;
			record->entity_anim_flags = record->entity_flags;

			if (record->land_type > 0)
				record->entity_anim_flags = (record->entity_anim_flags & ~FL_ONGROUND);
		}
		//m_player->get_animation_state()->m_time_to_align_lower_body = record->lby_flicked_time + (1.1f * 0.2f);
	}


	VIRTUALIZER_FISH_LITE_END;
}

void c_lagcomp::update_animation_system(C_BasePlayer* m_player, C_Tickrecord* record, C_Tickrecord* previous, int resolver_side)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

	const auto backup_flags = m_player->m_fFlags();
	const auto backup_ieflags = m_player->m_iEFlags();
	const auto backup_duckamt = m_player->m_flDuckAmount();
	const auto backup_lowerbody = m_player->m_flLowerBodyYawTarget();
	const auto backup_move_weight = m_player->get_animation_state()->m_move_weight;
	const auto backup_primary_cycle = m_player->get_animation_state()->m_primary_cycle;
	
	//const auto backup_poses = m_player->m_flPoseParameter();
	static /*const*/ ConVar* sv_gravity = csgo.m_engine_cvars()->FindVar(sxor("sv_gravity"));
	static /*const*/ ConVar* sv_jump_impulse = csgo.m_engine_cvars()->FindVar(sxor("sv_jump_impulse"));

	resolver_records* resolver_info = &feature::resolver->player_records[m_player->entindex() - 1];
	c_player_records* log = &records[m_player->entindex() - 1];

	//m_player->set_abs_angles(QAngle(0, record->eye_angles.y, 0));
	m_player->m_angEyeAngles() = record->eye_angles;
	m_player->m_angEyeAngles().z = 0.f;

	prepare_player_anim_update(m_player, record, previous, resolver_side);

	m_player->set_abs_angles(QAngle(0, m_player->get_animation_state()->m_abs_yaw, 0));

	if (record->lag > 1 && !log->saved_info.fakeplayer && previous && previous->data_filled)
	{
		const auto velocity_per_tick = (record->velocity - previous->velocity) / record->lag;
		//const auto origin_per_tick = (record->origin - previous->origin) / record->lag;
		//auto ticks_animated = 0;
		//auto velocity_speed_delta = abs(fmaxf(record->velocity.Length2D(),previous->velocity.Length2D()));

		const auto lby_delta = fabsf(Math::angle_diff(record->eye_angles.y, record->lower_body_yaw));

		record->is_landed = false;

		for (auto i = 1; i <= record->lag; i++)
		{
			auto simulated_time = record->animation_update_start_time + TICKS_TO_TIME(i);

			//auto origin = ((origin_per_tick * (float)i) + previous->origin);//Math::interpolate(previous->origin, previous->origin, record->origin, frac);
			auto velocity = /*velocity_speed_delta < 30.f ? */((velocity_per_tick * (float)i) + previous->velocity);//Math::interpolate(previous->velocity, previous->velocity, record->velocity, frac);
			//auto duck_amt = ((duck_amount_per_tick * i) + previous->duck_amt);//Math::interpolate(previous->duck_amt, previous->duck_amt, record->duck_amt, frac);

			if (record->duck_amount_per_tick != 0.0f)
			{
				auto v208 = ((record->duck_amt - m_player->m_flDuckAmount()) * record->duck_amount_per_tick)
					+ m_player->m_flDuckAmount();

				m_player->m_flDuckAmount() = fminf(fmaxf(v208, 0.0f), 1.0f);
			}

			if (i == record->lag)
				simulated_time = record->simulation_time;

			if (record->land_in_cycle && !record->is_landed) // landing animation fix
			{
				if (record->land_time < simulated_time) {
					record->entity_anim_flags |= 1;
					record->is_landed = true;
					auto layer = &m_player->get_animation_layer(4);
					layer->m_flCycle = 0;
					layer->m_flWeight = 0;

				}
			}

			//}
			//else
			//	on_ground = record->entity_flags & FL_ONGROUND;

			m_player->m_fFlags() = record->entity_anim_flags;

			record->can_rotate = resolver_side != 0 && !log->saved_info.fakeplayer && i < record->lag && (!record->shot_this_tick || record->shot_this_tick && record->shot_time <= simulated_time);

			/*if (!(record->anim_layers[6].m_flCycle == 0.0f || previous->anim_layers[6].m_flCycle == 0.0f) && velocity.Length2D() <= 1.1f)
			{
				velocity.x = (i & 1 ? -1.1f : 1.1f);	
				velocity.y = 0.f;
				velocity.z = 0.f;
			}*/

			//if (record->shot_this_tick && record->shot_time <= simulated_time)
			//	m_player->m_angEyeAngles().y = resolver_info->last_non_shot_angle.y;
			//else
			//m_player->m_angEyeAngles() = record->eye_angles;

			if (record->shot_this_tick)
			{
				if (record->shot_time <= simulated_time) {
					m_player->m_flThirdpersonRecoil() = record->thirdperson_recoil;
					m_player->m_flLowerBodyYawTarget() = record->lower_body_yaw;
				}
			}

			//if (record->shot_this_tick && record->shot_time > simulated_time)

			//m_player->m_vecOrigin() = origin;
			//m_player->set_abs_origin(origin);
			if (i == record->lag)
			{
				m_player->m_flDuckAmount() = fminf(fmaxf(record->duck_amt, 0.0f), 1.0f);
				m_player->m_flLowerBodyYawTarget() = record->lower_body_yaw;
				m_player->m_fFlags() = record->entity_flags;
			}

			m_player->m_vecAbsVelocity() = m_player->m_vecVelocity() = velocity;
			
			/*if (record->lower_body_yaw != previous->lower_body_yaw)
			{
				auto delta = record->lag - i;

				auto use_new = true;

				if (lby_delta < 1.f)
					use_new = delta == 0;
				else
					use_new = delta < 2;

				m_player->m_flLowerBodyYawTarget() = use_new ? record->lower_body_yaw : previous->lower_body_yaw;
			}*/

			//m_player->m_flLowerBodyYawTarget() = record->lby_flicked_time <= simulated_time ? record->lower_body_yaw : previous->lower_body_yaw;

			if (record->can_rotate) {
				auto angle = feature::anti_aim->get_max_desync_delta(m_player) * record->resolver_delta_multiplier;

				float yaw = m_player->get_animation_state()->m_abs_yaw;

				if (resolver_side <= 0)
					yaw = record->eye_angles.y - angle;
				else
					yaw = record->eye_angles.y + angle;

				m_player->get_animation_state()->m_abs_yaw = Math::normalize_angle(yaw);
			}
			
			/* update animations. */
			ctx.updating_resolver = true;
			//m_player->set_abs_origin(origin);
			//m_player->m_vecOrigin() = origin;
			resolver_info->new_velocity = velocity;
			resolver_info->force_velocity = true;

			auto realtime_backup = csgo.m_globals()->realtime;
			auto curtime = csgo.m_globals()->curtime;
			auto frametime = csgo.m_globals()->frametime;
			auto absoluteframetime = csgo.m_globals()->absoluteframetime;
			auto framecount = csgo.m_globals()->framecount;
			auto tickcount = csgo.m_globals()->tickcount;
			auto interpolation_amount = csgo.m_globals()->interpolation_amount;

			int ticks = TIME_TO_TICKS(simulated_time);

			csgo.m_globals()->realtime = simulated_time;
			csgo.m_globals()->curtime = simulated_time;
			csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
			csgo.m_globals()->absoluteframetime = csgo.m_globals()->interval_per_tick;
			csgo.m_globals()->framecount = ticks;
			csgo.m_globals()->tickcount = ticks;
			csgo.m_globals()->interpolation_amount = 0.f;

			m_player->m_iEFlags() &= ~EFL_DIRTY_ABSVELOCITY;
			m_player->update_clientside_animations();
			m_player->m_iEFlags() = backup_ieflags;
			resolver_info->force_velocity = false;
			ctx.updating_resolver = false;

			csgo.m_globals()->realtime = realtime_backup;
			csgo.m_globals()->curtime = curtime;
			csgo.m_globals()->frametime = frametime;
			csgo.m_globals()->absoluteframetime = absoluteframetime;
			csgo.m_globals()->framecount = framecount;
			csgo.m_globals()->tickcount = tickcount;
			csgo.m_globals()->interpolation_amount = interpolation_amount;
		}
	}
	else
	{
		m_player->m_flLowerBodyYawTarget() = record->lower_body_yaw;
		auto vel = record->velocity;
		
		m_player->m_flDuckAmount() = fminf(fmaxf(record->duck_amt, 0.0f), 1.0f);
		m_player->m_fFlags() = record->entity_flags;
		m_player->m_iEFlags() &= ~EFL_DIRTY_ABSVELOCITY;

		if (!log->saved_info.fakeplayer && resolver_side != 0)
		{
			float yaw = m_player->get_animation_state()->m_abs_yaw;
			auto angle = feature::anti_aim->get_max_desync_delta(m_player) * record->resolver_delta_multiplier;

			if (resolver_side <= 0)
				yaw = record->eye_angles.y - angle;
			else
				yaw = record->eye_angles.y + angle;

			m_player->get_animation_state()->m_abs_yaw = Math::normalize_angle(yaw);
		}
		//if (m_player->get_animation_state()->m_abs_yaw < 0)
		//	m_player->get_animation_state()->m_abs_yaw += 360.f;

		auto realtime_backup = csgo.m_globals()->realtime;
		auto curtime = csgo.m_globals()->curtime;
		auto frametime = csgo.m_globals()->frametime;
		auto absoluteframetime = csgo.m_globals()->absoluteframetime;
		auto framecount = csgo.m_globals()->framecount;
		auto tickcount = csgo.m_globals()->tickcount;
		auto interpolation_amount = csgo.m_globals()->interpolation_amount;

		int ticks = TIME_TO_TICKS(record->simulation_time);

		csgo.m_globals()->realtime = record->simulation_time;
		csgo.m_globals()->curtime = record->simulation_time;
		csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
		csgo.m_globals()->absoluteframetime = csgo.m_globals()->interval_per_tick;
		csgo.m_globals()->framecount = ticks;
		csgo.m_globals()->tickcount = ticks;
		csgo.m_globals()->interpolation_amount = 0.f;

		m_player->m_vecAbsVelocity() = m_player->m_vecVelocity() = vel;

		/* update animations. */
		ctx.updating_resolver = true;
		resolver_info->new_velocity = record->velocity;
		resolver_info->force_velocity = true;
		m_player->update_clientside_animations();
		resolver_info->force_velocity = false;
		ctx.updating_resolver = false;

		csgo.m_globals()->realtime = realtime_backup;
		csgo.m_globals()->curtime = curtime;
		csgo.m_globals()->frametime = frametime;
		csgo.m_globals()->absoluteframetime = absoluteframetime;
		csgo.m_globals()->framecount = framecount;
		csgo.m_globals()->tickcount = tickcount;
		csgo.m_globals()->interpolation_amount = interpolation_amount;
	}

	//m_player->set_abs_origin(abs_origin);
	m_player->m_fFlags() = backup_flags;
	//m_player->m_vecVelocity() = backup_m_velocity;
	m_player->m_flDuckAmount() = backup_duckamt;
	m_player->m_flLowerBodyYawTarget() = backup_lowerbody;
	m_player->m_iEFlags() = backup_ieflags;
	//m_player->m_angEyeAngles() = record->eye_angles;
	//m_player->m_flPoseParameter() = backup_poses;

	m_player->get_animation_state()->m_primary_cycle = backup_primary_cycle;
	m_player->get_animation_state()->m_move_weight = backup_move_weight;

	if (resolver_side != 0) {
		if (resolver_side > 0)
		{
			record->animstate_right_params[0] = m_player->get_animation_state()->m_abs_yaw;
			record->animstate_right_params[1] = m_player->get_animation_state()->m_abs_yaw_last;
			record->animstate_right_params[2] = m_player->get_animation_state()->m_move_yaw;
			record->animstate_right_params[3] = m_player->get_animation_state()->m_move_yaw_ideal;
			record->animstate_right_params[4] = m_player->get_animation_state()->m_move_yaw_current_to_ideal;
			record->animstate_right_params[5] = m_player->get_animation_state()->m_move_weight_smoothed;
			record->animstate_right_params[6] = m_player->get_animation_state()->m_in_air_smooth_value;
			record->animstate_right_params[7] = m_player->get_animation_state()->m_time_to_align_lower_body;
		}
		else
		{
			record->animstate_left_params[0] = m_player->get_animation_state()->m_abs_yaw;
			record->animstate_left_params[1] = m_player->get_animation_state()->m_abs_yaw_last;
			record->animstate_left_params[2] = m_player->get_animation_state()->m_move_yaw;
			record->animstate_left_params[3] = m_player->get_animation_state()->m_move_yaw_ideal;
			record->animstate_left_params[4] = m_player->get_animation_state()->m_move_yaw_current_to_ideal;
			record->animstate_left_params[5] = m_player->get_animation_state()->m_move_weight_smoothed;
			record->animstate_left_params[6] = m_player->get_animation_state()->m_in_air_smooth_value;
			record->animstate_left_params[7] = m_player->get_animation_state()->m_time_to_align_lower_body;
		}
	}

	/*if (previous && !record->first_after_dormancy && !previous->dormant && previous->data_filled)
		fix_jump_fall(m_player, record, previous);*/

	m_player->invalidate_anims(8);

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}

void c_lagcomp::recalculate_velocity(C_Tickrecord* record, C_BasePlayer* m_player, C_Tickrecord* previous)
{
	VIRTUALIZER_FISH_LITE_START;

	static /*const*/ ConVar* sv_gravity = csgo.m_engine_cvars()->FindVar(sxor("sv_gravity"));
	static /*const*/ ConVar* sv_jump_impulse = csgo.m_engine_cvars()->FindVar(sxor("sv_jump_impulse"));
	static /*const*/ ConVar* sv_enablebunnyhopping = csgo.m_engine_cvars()->FindVar(sxor("sv_enablebunnyhopping"));

	auto log = &records[m_player->entindex() - 1];
	auto r_log = &feature::resolver->player_records[m_player->entindex() - 1];

	/* fix z velocity if enemy is in air. */
	/* https://github.com/ValveSoftware/source-sdk-2013/blob/master/mp/src/game/shared/gamemovement.cpp#L1697 */

	//auto& old_origin = *(Vector*)(uintptr_t(m_player) + 0x3A4);

	if (record->entity_flags & FL_ONGROUND
		&& record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flWeight > 0.0f 
		&& record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flWeight < 1.0f)
	{
		// float val = clamp ( ( speed - 0.55f ) / ( 0.9f - 0.55f), 0.f, 1.f );
		// layer11_weight = 1.f - val;
		auto val = (1.0f - record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flWeight) * 0.35f;

		if (val > 0.0f && val < 1.0f)
			record->animation_speed = val + 0.55f;
		else
			record->animation_speed = -1.f;
	}

	if (_fdtest(&record->velocity.x) > 0
		|| _fdtest(&record->velocity.y) > 0
		|| _fdtest(&record->velocity.z) > 0)
		record->velocity.clear();

	if (!record->first_after_dormancy && previous && !previous->dormant && previous->data_filled) 
	{
		//
		//	calculate new velocity based on (new_origin - old_origin) / (new_time - old_time) formula.
		//
		if (record->lag > 1 && record->lag <= 20)
			record->velocity = (record->origin - previous->origin) / record->time_delta;
		
		if (abs(record->velocity.x) < 0.001f)
			record->velocity.x = 0.0f;
		if (abs(record->velocity.y) < 0.001f)
			record->velocity.y = 0.0f;
		if (abs(record->velocity.z) < 0.001f)
			record->velocity.z = 0.0f;

		if (_fdtest(&record->velocity.x) > 0
			|| _fdtest(&record->velocity.y) > 0
			|| _fdtest(&record->velocity.z) > 0)
			record->velocity.clear();

		auto curr_direction = RAD2DEG(std::atan2f(record->velocity.y, record->velocity.x));
		auto prev_direction = previous == nullptr ? FLT_MAX : RAD2DEG(std::atan2f(previous->velocity.y, previous->velocity.x));

		auto delta = Math::normalize_angle(curr_direction - prev_direction);

		if (record->velocity.Length2D() > 0.1f) {
			if (previous->velocity.Length2D() > 0.1f && abs(delta) >= 60.f)
				r_log->last_time_changed_direction = csgo.m_globals()->realtime;
		}
		else
			r_log->last_time_changed_direction = 0;

		//
		// these requirements pass only when layer[6].weight is accurate to normalized velocity.
		//
		if (record->entity_flags & FL_ONGROUND 
			&& record->velocity.Length2D() >= 0.1f 
			&& std::abs(delta) < 1.0f
			&& std::abs(record->duck_amt - previous->duck_amt) <= 0.0f
			&& record->anim_layers[6].m_flPlaybackRate > previous->anim_layers[6].m_flPlaybackRate
			&& record->anim_layers[6].m_flWeight > previous->anim_layers[6].m_flWeight)
		{
			auto weight_speed = record->anim_layers[6].m_flWeight;

			if (weight_speed <= 0.7f && weight_speed > 0.0f)
			{
				if (record->anim_layers[6].m_flPlaybackRate == 0.0f)
					record->velocity.clear();
				else
				{
					const auto m_post_velocity_lenght = record->velocity.Length2D();

					if (m_post_velocity_lenght != 0.0f)
					{
						float mult = 1;
						if (record->entity_flags & 6)
							mult = 0.34f;
						else if (record->fake_walking)
							mult = 0.52f;

						record->velocity.x = (record->velocity.x / m_post_velocity_lenght) * (weight_speed * (record->max_current_speed * mult));
						record->velocity.y = (record->velocity.y / m_post_velocity_lenght) * (weight_speed * (record->max_current_speed * mult));
					}
				}
			}
		}

		//
		// fix velocity with fakelag.
		//
		if (record->entity_flags & FL_ONGROUND && record->velocity.Length2D() > 0.1f && record->lag > 1)
		{
			//
			// get velocity lenght from 11th layer calc.
			//
			if (record->animation_speed > 0) {
				const auto m_pre_velocity_lenght = record->velocity.Length2D();
				C_WeaponCSBaseGun* weapon = m_player->get_weapon();

				if (weapon) {
					auto wdata = weapon->GetCSWeaponData();
					if (wdata) {
						auto adjusted_velocity = (record->animation_speed * record->max_current_speed) / m_pre_velocity_lenght;
						record->velocity.x *= adjusted_velocity;
						record->velocity.y *= adjusted_velocity;
					}
				}
			}

			/*if (record->entity_flags & FL_ONGROUND && (sv_enablebunnyhopping && !sv_enablebunnyhopping->GetBool() || previous->entity_flags & FL_ONGROUND)) {
				auto max_speed = record->max_current_speed;

				if (record->entity_flags & 6)
					max_speed *= 0.34f;
				else if (record->fake_walking)
					max_speed *= 0.52f;

				if (max_speed < m_pre_velocity_lenght)
					record->velocity *= (max_speed / m_pre_velocity_lenght);

				if (previous->entity_flags & FL_ONGROUND)
					record->velocity.z = 0.f;
			}*/
		}

		if (log->records_count > 2 && record->lag > 1 && !record->first_after_dormancy
			&& previous->velocity.Length() > 0 && !(record->entity_flags & FL_ONGROUND && previous->entity_flags & FL_ONGROUND))
		{
			auto pre_pre_record = &log->tick_records[(log->records_count - 2) & 63];

			if (!pre_pre_record->dormant && pre_pre_record->data_filled) {
				//if (record->velocity.Length2D() > (record->max_current_speed * 0.52f) && previous->velocity.Length2D() > (record->max_current_speed * 0.52f)
				//	|| record->velocity.Length2D() <= (record->max_current_speed * 0.52f) && previous->velocity.Length2D() <= (record->max_current_speed * 0.52f))
				//{
				//	auto manually_calculated = log->tick_records[(log->records_count - 2) & 63].stop_to_full_run_frac;
				//	manually_calculated += (record->velocity.Length2D() > (record->max_current_speed * 0.52f) ? (2.f * previous->time_delta) : -(2.f * previous->time_delta));

				//	manually_calculated = Math::clamp(manually_calculated, 0, 1);

				//	if (abs(manually_calculated - previous->stop_to_full_run_frac) >= 0.1f)// {
				//		m_player->get_animation_state()->m_walk_run_transition = manually_calculated;
				//}

				const auto prev_direction = RAD2DEG(std::atan2f(previous->velocity.y, previous->velocity.x));

				auto real_velocity = record->velocity.Length2D();

				float delta = curr_direction - prev_direction;

				if (delta <= 180.0f)
				{
					if (delta < -180.0f)
						delta = delta + 360.0f;
				}
				else
				{
					delta = delta - 360.0f;
				}

				float v63 = delta * 0.5f + curr_direction;

				auto direction = (v63 + 90.f) * 0.017453292f;

				record->velocity.x = sinf(direction) * real_velocity;
				record->velocity.y = cosf(direction) * real_velocity;
			}
		}

		//bool is_jumping = record->entity_flags & FL_ONGROUND && previous && previous->data_filled && !previous->dormant && !(previous->entity_flags & FL_ONGROUND);

		/*if (is_jumping && record->ground_accel_last_time != record->simulation_time)
		{
			if (sv_enablebunnyhopping->GetInt() == 0) {

				// 260 x 1.1 = 286 units/s.
				float max = m_player->m_flMaxSpeed() * 1.1f;

				// get current velocity.
				float speed = record->velocity.Length();

				// reset velocity to 286 units/s.
				if (max > 0.f && speed > max)
					record->velocity *= (max / speed);
			}

			// assume the player is bunnyhopping here so set the upwards impulse.
			record->velocity.z = sv_jump_impulse->GetFloat();

			record->in_jump = true;
		}
		else */if (!(record->entity_flags & FL_ONGROUND))
		{
			record->velocity.z -= sv_gravity->GetFloat() * record->time_delta * 0.5f;

			record->in_jump = true;
		}
	}
	else if (record->first_after_dormancy)
	{
		auto weight_speed = record->anim_layers[6].m_flWeight;

		if (record->anim_layers[6].m_flPlaybackRate < 0.00001f)
			record->velocity.clear();
		else
		{
			const auto m_post_velocity_lenght = record->velocity.Length2D();

			if (m_post_velocity_lenght != 0.0f && weight_speed > 0.01f && weight_speed < 0.95f)
			{
				float mult = 1;
				if (record->entity_flags & 6)
					mult = 0.34f;
				else if (record->fake_walking)
					mult = 0.52f;

				record->velocity.x = (record->velocity.x / m_post_velocity_lenght) * (weight_speed * (record->max_current_speed * mult));
				record->velocity.y = (record->velocity.y / m_post_velocity_lenght) * (weight_speed * (record->max_current_speed * mult));
			}
		}

		if (record->entity_flags & FL_ONGROUND)
			record->velocity.z = 0;
	}

	if (_fdtest(&record->velocity.x) > 0
		|| _fdtest(&record->velocity.y) > 0
		|| _fdtest(&record->velocity.z) > 0)
		record->velocity.clear();
	//
	//	if server had 0 velocity at animation time -> reset velocity
	//
	if (record->entity_flags & FL_ONGROUND && record->lag > 1 && record->velocity.Length() > 0.1f && record->anim_layers[6].m_flPlaybackRate < 0.00001f)
		record->velocity.clear();

	r_log->tick_stopped = -1;
	r_log->velocity_stopped = Vector::Zero;

	//m_player->invalidate_anims(4);

	/* apply proper velocity and force flags so game will not try to recalculate it. */
	//m_player->m_vecAbsVelocity() = record->velocity;
	m_player->m_vecVelocity() = record->velocity;
	//m_player->invalidate_anims(VELOCITY_CHANGED);

	//*(Vector*)(uintptr_t(m_player) + 0x114) = record->velocity;

	VIRTUALIZER_FISH_LITE_END;
}

void c_lagcomp::parse_player_data(C_Tickrecord* record, C_BasePlayer* m_player)
{
	VIRTUALIZER_FISH_LITE_START;

	const auto idx = m_player->entindex() - 1;
	resolver_records* resolver_info = &feature::resolver->player_records[idx];
	c_player_records* log = &records[idx];
	auto previous = (log->records_count > 1 && !record->first_after_dormancy ? &log->tick_records[(log->records_count - 1) & 63] : nullptr);

	record->fake_walking = m_player->m_bIsWalking();

	//record->eye_angles.x = Math::normalize_angle(record->eye_angles.x);

	if (fabs(Math::normalize_angle(record->thirdperson_recoil + record->eye_angles.x - 180.0f)) <= 0.1f || record->eye_angles.x == 88.9947510f)
		record->eye_angles.x = 89.0f;

	record->eye_angles.x = Math::normalize_angle(record->eye_angles.x);
	record->eye_angles.x = fminf(fmaxf(record->eye_angles.x, -90.0f), 90.0f);
	//record->eye_angles.y = Math::normalize_angle(record->eye_angles.y);

	//if (record->anim_layers[5].m_flWeight > 0 && record->anim_layers[5].m_flCycle < 1)
	//	record->entity_flags |= FL_ONGROUND;

	record->in_jump = !(record->entity_flags & FL_ONGROUND);

	//if (log->records_count <= 1) {
	//	if (m_player->m_flSpawnTime() != resolver_info->spawn_time)
	//	{
	//		using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	//		//auto ResetAnimState = (ResetAnimState_t)Engine::Displacement::Signatures[c_signatures::RESETANIMSTATE];
	//
	//		//if (ResetAnimState != nullptr)
	//		((ResetAnimState_t)Engine::Displacement::Signatures[c_signatures::RESETANIMSTATE])(m_player->get_animation_state());
	//
	//		log->records_count = 1;
	//
	//		resolver_info->spawn_time = m_player->m_flSpawnTime();
	//	}
	//}

	//float data[2] = { m_player->get_animation_state()->m_move_weight, m_player->get_animation_state()->m_primary_cycle };

	record->exploit = false;
	record->animated = false;

	record->breaking_lc = false;
	record->shot_this_tick = false;
	record->not_desyncing = false;
	resolver_info->breaking_lc = false;
	resolver_info->last_simtime = record->simulation_time;

	record->land_time = 0.0f;
	record->is_landed = false;
	record->land_in_cycle = false;

	record->m_move_weight = record->anim_layers[6].m_flWeight;
	record->m_primary_cycle = record->anim_layers[6].m_flCycle;
	record->m_strafe_sequence = record->anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_nSequence;
	record->m_strafe_change_weight = record->anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flWeight;
	record->m_strafe_change_cycle = record->anim_layers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].m_flCycle;
	record->m_acceleration_weight = record->anim_layers[ANIMATION_LAYER_LEAN].m_flWeight;

	//const auto lmao = offsetof(CCSGOPlayerAnimState, m_acceleration_weight);

	//if (!record->shot_this_tick && has_firing_animation(m_player, record))
	//{
	//	record->shot_this_tick = true;
	//	record->shot_time = m_player->get_weapon() ? m_player->get_weapon()->m_flLastShotTime() : record->simulation_time;
	//	//logic here iiii
	//}

	int ticks_to_simulate = 1;

	if (previous && !previous->dormant && previous->data_filled && !record->first_after_dormancy)
	{
		int simulation_ticks = TIME_TO_TICKS(record->simulation_time - previous->simulation_time);

		if ((simulation_ticks - 1) > 31 || previous->simulation_time == 0.f)
			simulation_ticks = 1;

		auto layer_cycle = record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flCycle;
		auto previous_playback = previous->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flPlaybackRate;

		if (previous_playback > 0.f && record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flPlaybackRate > 0.f
			&& previous->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_nSequence == record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_nSequence
			/*&& m_player->get_animation_state()->m_weapon == m_player->get_animation_state()->m_weapon_last*/)
		{
			auto previous_cycle = previous->anim_layers[11].m_flCycle;
			simulation_ticks = 0;

			if (previous_cycle > layer_cycle)
				layer_cycle = layer_cycle + 1.0f;

			while (layer_cycle > previous_cycle)
			{
				const auto ticks_backup = simulation_ticks;
				const auto playback_mult_ipt = csgo.m_globals()->interval_per_tick * previous_playback;

				previous_cycle = previous_cycle + (csgo.m_globals()->interval_per_tick * previous_playback);

				if (previous_cycle >= 1.0f)
					previous_playback = record->anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flPlaybackRate;

				++simulation_ticks;

				if (previous_cycle > layer_cycle && (previous_cycle - layer_cycle) > (playback_mult_ipt * 0.5f))
					simulation_ticks = ticks_backup;
			}
		}

		ticks_to_simulate = simulation_ticks;

		//if (record->exploit)
		//	record->simulation_time = previous->simulation_time + TICKS_TO_TIME(simulation_ticks);
	}
	
	ticks_to_simulate = Math::clamp(ticks_to_simulate, 1, 64);

	//ticks_to_simulate = max(ticks_to_simulate, 1);

	record->lag = ticks_to_simulate;
	record->time_delta = float(ticks_to_simulate) * csgo.m_globals()->interval_per_tick;

	record->animation_update_start_time = record->simulation_time - record->time_delta;

	record->had_use_key = false;

	record->resolver_index = (log->saved_info.fakeplayer ? 0 : resolver_info->resolver_index);
	record->resolver_type = (log->saved_info.fakeplayer ? 0 : resolver_info->resolver_type);
	
	if (previous && previous->data_filled) {
		if (record->simulation_time > record->simulation_time_old) {

			if ((record->origin - previous->origin).Length2D() > 4096.0f)
				record->breaking_lc = resolver_info->breaking_lc = true;

			// check if landed in choke cycle
			if (!record->first_after_dormancy && !previous->dormant)
			{
				if (!(record->entity_flags & 1 && previous->entity_flags & 1)) {
					if (record->anim_layers[4].m_flCycle < 0.5f) {
						record->land_time = record->simulation_time - (record->anim_layers[4].m_flCycle / record->anim_layers[4].m_flPlaybackRate);
						record->land_in_cycle = record->land_time >= record->animation_update_start_time;
					}
				}
				else
					record->velocity.z = 0;

				//auto at_target = Math::CalcAngle(record->origin, ctx.m_local()->get_abs_origin()).y;

				//if (auto eye_delta = fabsf(Math::angle_diff(record->eye_angles.y, previous->eye_angles.y)); eye_delta > 165.f && (abs(at_target) <= 65.f || abs(at_target) >= 165.f) && !record->shot_this_tick) {
				//	resolver_info->desync_swap = !resolver_info->desync_swap;
				//	//resolver_info->desync_swap_angles = record->eye_angles.y;
				//}
				//else 
				//{
				//	//auto previous = (log->records_count > 3 && !record->first_after_dormancy ? &log->tick_records[(log->records_count - 1) & 63] : nullptr);
				//	//auto previous = (log->records_count > 3 && !record->first_after_dormancy ? &log->tick_records[(log->records_count - 1) & 63] : nullptr);
				//
				//	if (log->records_count > 3) {
				//		auto prepre = (log->records_count > 3 && !record->first_after_dormancy ? &log->tick_records[(log->records_count - 2) & 63] : nullptr);
				//		auto preprepre = (log->records_count > 3 && !record->first_after_dormancy ? &log->tick_records[(log->records_count - 3) & 63] : nullptr);
				//
				//		auto eye_delta1 = abs(Math::angle_diff(record->eye_angles.y, previous->eye_angles.y));
				//		auto eye_delta2 = abs(Math::angle_diff(previous->eye_angles.y, preprepre->eye_angles.y));
				//		auto eye_delta3 = abs(Math::angle_diff(record->eye_angles.y, preprepre->eye_angles.y));
				//
				//		if (fmaxf(eye_delta1, fmaxf(eye_delta2, eye_delta3)) < 35.f)
				//			resolver_info->desync_swap = false;
				//	}
				//}
			}

			if (!previous->dormant && record->simulation_time >= record->shot_time && record->shot_time > record->animation_update_start_time)// {
				record->shot_this_tick = true;
		}
	}

	bool had_low_delta = std::abs(csgo.m_globals()->realtime - resolver_info->last_low_delta_time) <= 300.f;

	if ((resolver_info->missed_shots[R_60_DELTA] > 0 || had_low_delta && !record->shot_this_tick)
		&& (resolver_info->missed_shots[R_40_DELTA] <= 0 || (resolver_info->missed_shots[R_40_DELTA] % 3) != 0))
	{
		if (resolver_info->missed_shots[R_60_DELTA] > 1 && (resolver_info->missed_shots[R_60_DELTA] % 10) <= 5) {
			record->resolver_delta_multiplier = .66f;
			resolver_info->last_low_delta_time = csgo.m_globals()->realtime;
		}
		else {
			if (had_low_delta)
				record->resolver_delta_multiplier = RandomFloat(0.7f, 0.9f);
		}
	}

	//record->corrected_velocity = false;
	record->max_current_speed = m_player->m_flMaxSpeed();

	resolver_info->tick_jumped = -1;

	//float anim_speed = FLT_MAX;
	C_WeaponCSBaseGun* weapon = m_player->get_weapon();

	if (weapon) {
		auto wdata = weapon->GetCSWeaponData();

		if (wdata)
			record->max_current_speed = std::fminf(m_player->m_flMaxSpeed(), m_player->m_bIsScoped() ? *(float*)(uintptr_t(wdata) + 0x134) : *(float*)(uintptr_t(wdata) + 0x130));
	}

	record->max_current_speed = fmaxf(record->max_current_speed, 0.001f);

	record->animation_speed = -1.f;
	VIRTUALIZER_FISH_LITE_END;
}

void c_lagcomp::run_animation_data_resolver(C_Tickrecord* record, C_BasePlayer* m_player, C_Tickrecord* previous)
{
	VIRTUALIZER_FISH_LITE_START;
	const auto idx = m_player->entindex() - 1;

	resolver_records* resolver_info = &feature::resolver->player_records[idx];
	c_player_records* log = &records[idx];

	//m_player->get_animation_state()->m_primary_cycle = record->anim_layers[6].m_flCycle;
	//m_player->get_animation_state()->m_strafe_change_cycle = record->anim_layers[7].m_flCycle;
	//m_player->get_animation_state()->m_strafe_sequence = record->anim_layers[7].m_nSequence;
	//m_player->get_animation_state()->m_strafe_change_weight = record->anim_layers[7].m_flWeight;
	//m_player->get_animation_state()->m_acceleration_weight = record->anim_layers[12].m_flWeight;

	/*auto accurate_fake = int(record->anim_layers[6].m_flPlaybackRate * 10000.f) == int(resolver_info->resolver_layers[0][6].m_flPlaybackRate * 10000.f)
		&& int(record->anim_layers[6].m_flWeight * 10000.f) == int(resolver_info->resolver_layers[0][6].m_flWeight * 10000.f);

	if (abs(record->eye_angles.x) < 45.f && accurate_fake)
	{
		if (resolver_info->desync_swap)
		{
			record->had_use_key = true;
			resolver_info->use_fix_trigger = csgo.m_globals()->realtime;
		}
		else
		{
			if ((csgo.m_globals()->realtime - resolver_info->use_fix_trigger) > 1.5f)
				record->had_use_key = false;
		}
	}
	else if (accurate_fake)
	{
		if (!record->fake_walking)
			record->had_use_key = false;
	}
	else
	{
		if (!resolver_info->desync_swap)
			record->had_use_key = false;
	}*/

	if (record->valid && !log->saved_info.fakeplayer)
	{
		auto delta = Math::angle_diff(record->left_side, record->eye_angles.y);
		bool negative_delta = delta < 0.0f;

		/*if (resolver_info->desync_swap && log->records_count > 1 && record->resolver_index != 0)
		{
			std::swap(record->leftmatrixes, record->rightmatrixes);
			std::swap(record->left_poses, record->right_poses);
			std::swap(record->left_side, record->right_side);
			std::swap(resolver_info->left_side, resolver_info->right_side);
		}
		else */if (negative_delta) {
			std::swap(record->leftmatrixes, record->rightmatrixes);
			std::swap(record->left_poses, record->right_poses);
			std::swap(record->left_side, record->right_side);
			//std::swap(resolver_info->resolver_layers[2], resolver_info->resolver_layers[1]);
			std::swap(resolver_info->left_side, resolver_info->right_side);
		}
	}

	record->animations_index = 0;
	record->animations_updated = false;

	if (m_player->m_fFlags() & FL_ONGROUND && previous->entity_flags & FL_ONGROUND/* && !record->shot_this_tick && !previous->shot_this_tick*/)
	{
		auto speed = record->velocity.Length2D();

		//auto curr_direction = RAD2DEG(std::atan2f(record->velocity.y, record->velocity.x));
		//auto prev_direction = previous == nullptr ? FLT_MAX : RAD2DEG(std::atan2f(previous->velocity.y, previous->velocity.x));

		//auto delta = Math::normalize_angle(curr_direction - prev_direction);

		if (speed > 0.1f 
			&& std::abs(record->duck_amt - previous->duck_amt) <= 0.0f)
			//{
			//	if (/*speed < 0.1f && */record->ground_accel_last_time != record->simulation_time
			//		&& record->anim_layers[3].m_flWeight == 0.0f
			//		&& record->anim_layers[3].m_flCycle == 0.0f
			//		&& record->anim_layers[6].m_flWeight == 0.0f)
			//	{
			//		float delta = Math::AngleDiff(record->left_side, record->eye_angles.y);
			//		record->animations_index = (1 * int(delta > 0.0f)) + 1;
			//		record->animations_updated = true;
			//		resolver_info->anim_time = csgo.m_globals()->realtime;
			//		resolver_info->anims_pre_resolving = (1 * int(delta > 0.0f)) + 1;
			//		resolver_info->did_anims_update = true;
			//	}
			//}
			//else
		{
			if ((record->anim_layers[12].m_flWeight < 0.01f
				|| abs(previous->anim_layers[12].m_flWeight - record->anim_layers[12].m_flWeight) < 0.01f)
				&& previous->anim_layers[6].m_nSequence == record->anim_layers[6].m_nSequence)
			{
				//2 = -1; 3 = 1; 1 = fake;
				if (std::abs(record->anim_layers[6].m_flWeight - previous->anim_layers[6].m_flWeight) < 0.01f)
				{
					/*int result = 0;
					if (feature::resolver->resolve_using_animations(m_player, record, result))
					{
						record->animations_index = result;
						record->animations_updated = true;
						resolver_info->anims_pre_resolving = result;
						resolver_info->anim_time = csgo.m_globals()->realtime;
						resolver_info->did_anims_update = true;
					}*/
					float delta1 = std::abs(resolver_info->resolver_layers[0][6].m_flPlaybackRate - record->anim_layers[6].m_flPlaybackRate);
					float delta2 = std::abs(resolver_info->resolver_layers[1][6].m_flPlaybackRate - record->anim_layers[6].m_flPlaybackRate);
					float delta3 = std::abs(resolver_info->resolver_layers[2][6].m_flPlaybackRate - record->anim_layers[6].m_flPlaybackRate);

					if (int(delta1 * 1000.f) < int(delta2 * 1000.f) || int(delta3 * 1000.f) <= int(delta2 * 1000.f) || int(delta2 * 1000.0f)) {
						if (int(delta1 * 1000.f) >= int(delta3 * 1000.f) && int(delta2 * 1000.f) > int(delta3 * 1000.f) && !int(delta3 * 1000.0f))
						{
							record->animations_index = 1;
							resolver_info->anims_pre_resolving = 1;
							resolver_info->anim_time = csgo.m_globals()->realtime;
							resolver_info->did_anims_update = true;
							record->animations_updated = true;
						}
					}
					else
					{
						record->animations_index = 2;
						resolver_info->anims_pre_resolving = 2;
						resolver_info->anim_time = csgo.m_globals()->realtime;
						resolver_info->did_anims_update = true;
						record->animations_updated = true;
					}
				}
			}
		}

		if (fabs(record->eye_angles.x) < 60.0f && !record->shot_this_tick)
		{
			auto result = m_player->get_sec_activity(record->anim_layers[3].m_nSequence);
			if (result == 979)
			{
				if (record->anim_layers[3].m_flCycle != previous->anim_layers[3].m_flCycle)
				{
					if (record->velocity.Length2D() <= 0.1f)
					{
						if (record->entity_flags & 1)
						{
							if (record->anim_layers[3].m_flWeight == 0.0f && record->anim_layers[3].m_flCycle == 0.0f)
							{
								record->animations_index = 0;
								resolver_info->anims_pre_resolving = 0;
								resolver_info->anim_time = csgo.m_globals()->realtime;
								resolver_info->did_anims_update = true;
								record->animations_updated = true;
							}
						}
					}
				}
			}
		}
	}

	VIRTUALIZER_FISH_LITE_END;
}

void c_lagcomp::update_animations_data(C_Tickrecord* record, C_BasePlayer* m_player)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
	CCSGOPlayerAnimState* state = m_player->get_animation_state();

	const auto idx = m_player->entindex() - 1;

	if (m_player == ctx.m_local())
		return;

	if (!state) {
		m_player->update_clientside_animations();
		return;
	}

	record->valid = true;
	record->animations_updated = false;
	record->accurate_anims = false;
	//record->can_aim_at_foot = false;

	resolver_records* resolver_info = &feature::resolver->player_records[idx];
	c_player_records* log = &records[idx];

	resolver_info->did_store_abs_yaw = false;
	resolver_info->last_angle = record->eye_angles;
	record->resolved = false;

	auto previous = (log->records_count > 1 && !record->first_after_dormancy ? &log->tick_records[(log->records_count - 1) & 63] : nullptr);

	const auto backup_origin = m_player->m_vecOrigin();
	const auto backup_m_velocity = m_player->m_vecVelocity();
	const auto backup_m_abs_velocity = m_player->m_vecAbsVelocity();
	const auto backup_m_abs_angles = m_player->get_abs_angles();
	const auto backup_m_flags = m_player->m_fFlags();
	const auto backup_m_eflags = m_player->m_iEFlags();
	const auto backup_m_duck = m_player->m_flDuckAmount();
	const auto backup_m_body = m_player->m_flLowerBodyYawTarget();
	const auto backup_m_absorigin = m_player->get_abs_origin();
	const auto backup_m_tp_recoil = m_player->m_flThirdpersonRecoil();
	const auto backup_animstate = *m_player->get_animation_state();
	const auto backup_poses = m_player->m_flPoseParameter();

	record->resolver_delta_multiplier = 1.f;
	RandomSeed(csgo.m_client_state()->m_clockdrift_manager.m_nServerTick & 255);

	parse_player_data(record, m_player);
	recalculate_velocity(record, m_player, previous);

	update_animation_system(m_player, record, previous, 0);
	record->pose_paramaters = m_player->m_flPoseParameter();
	record->stop_to_full_run_frac = m_player->get_animation_state()->m_walk_run_transition;

	/* save original abs_yaw to properly brute with 0 resolving method. */
	resolver_info->original_abs_yaw = m_player->get_animation_state()->m_abs_yaw;
	record->original_abs_yaw = m_player->get_animation_state()->m_abs_yaw;
	memcpy(resolver_info->resolver_layers[0], m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());

	resolver_info->last_desync_delta = record->desync_delta;
	record->animstate = *m_player->get_animation_state();

	m_player->set_abs_angles(QAngle(0, m_player->get_animation_state()->m_abs_yaw, 0));
	memcpy(m_player->animation_layers_ptr(), record->anim_layers, 0x38 * m_player->get_animation_layers_count());

	m_player->force_bone_rebuild();
	m_player->SetupBonesEx(); 

	memcpy(record->matrixes, m_player->m_CachedBoneData().Base(), m_player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));

	*m_player->get_animation_state() = backup_animstate;
	memcpy(m_player->m_flPoseParameter().data(), backup_poses.data(), 80);

	update_animation_system(m_player, record, previous, -1);

	resolver_info->right_side = record->right_side = m_player->get_animation_state()->m_abs_yaw;
	record->right_poses = m_player->m_flPoseParameter();
	memcpy(resolver_info->resolver_layers[1], m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());
	memcpy(m_player->animation_layers_ptr(), record->anim_layers, 0x38 * m_player->get_animation_layers_count());

	if (record->valid) {
		m_player->set_abs_angles(QAngle(0, m_player->get_animation_state()->m_abs_yaw, 0));
		m_player->force_bone_rebuild();
		m_player->SetupBonesEx();

		memcpy(record->rightmatrixes, m_player->m_CachedBoneData().Base(), m_player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
	}

	*m_player->get_animation_state() = backup_animstate;
	memcpy(m_player->m_flPoseParameter().data(), backup_poses.data(), 80);

	update_animation_system(m_player, record, previous, 1);

	resolver_info->left_side = record->left_side = m_player->get_animation_state()->m_abs_yaw;
	record->left_poses = m_player->m_flPoseParameter();
	memcpy(resolver_info->resolver_layers[2], m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());
	memcpy(m_player->animation_layers_ptr(), record->anim_layers, 0x38 * m_player->get_animation_layers_count());

	if (record->valid) {
		m_player->set_abs_angles(QAngle(0, m_player->get_animation_state()->m_abs_yaw, 0));
		m_player->force_bone_rebuild();
		m_player->SetupBonesEx();

		memcpy(record->leftmatrixes, m_player->m_CachedBoneData().Base(), m_player->m_CachedBoneData().Count() * sizeof(matrix3x4_t));
	}

	record->bones_count = m_player->m_CachedBoneData().Count();
	record->not_desyncing = false; //!(abs(Math::angle_diff(record->right_side, record->original_abs_yaw)) > 0.0f || abs(Math::angle_diff(record->left_side, record->original_abs_yaw)) > 0.0f) || record->lag < 1;

	if (previous && previous->data_filled && !previous->dormant)
		run_animation_data_resolver(record, m_player, previous);

	memcpy(m_player->m_CachedBoneData().Base(), ((log->saved_info.fakeplayer || record->resolver_index == 0) ? record->matrixes : (record->resolver_index == 1 ? record->leftmatrixes : record->rightmatrixes)), record->bones_count * sizeof(matrix3x4_t));

	m_player->set_abs_angles(QAngle(backup_m_abs_angles.x, (log->saved_info.fakeplayer || record->resolver_index == 0) ? record->original_abs_yaw : (record->resolver_index == 1 ? record->left_side : record->right_side), backup_m_abs_angles.z));

	if (log->saved_info.fakeplayer /*|| record->not_desyncing */ || record->resolver_index == 0) {
		m_player->m_flPoseParameter() = record->pose_paramaters;
	}
	else {
		m_player->m_flPoseParameter() = (record->resolver_index == 1 ? record->left_poses : record->right_poses);
	}

	record->abs_angles = record->animstate.m_abs_yaw;

	m_player->set_abs_origin(backup_m_absorigin);

	memcpy(m_player->animation_layers_ptr(), record->anim_layers, 0x38 * m_player->get_animation_layers_count());

	record->animation_time = feature::lagcomp->get_interpolated_time();
	log->interpolated_count = 0;

	m_player->m_vecVelocity() = record->velocity;

	record->head_pos = m_player->get_bone_pos(8);

	m_player->m_vecOrigin() = backup_origin;
	m_player->m_fFlags() = backup_m_flags;
	m_player->m_iEFlags() = backup_m_eflags;
	m_player->m_flDuckAmount() = backup_m_duck;
	m_player->m_flThirdpersonRecoil() = backup_m_tp_recoil;
	m_player->m_flLowerBodyYawTarget() = backup_m_body;
	record->animated = true;

	//m_player->DrawServerHitboxes(csgo.m_globals()->frametime * 2.f);

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
}

void c_lagcomp::interpolate(ClientFrameStage_t stage)
{
	//if (stage != FRAME_RENDER_START || !ctx.m_local() || ctx.m_local()->IsDead() || ctx.updated_skin || csgo.m_globals()->interval_per_tick <= Engine::Prediction::Instance()->m_flFrameTime)
		return;
	
	for (auto i = 1; i < csgo.m_globals()->maxClients; i++)
	{
		auto m_player = csgo.m_entity_list()->GetClientEntity(i);

		if (!m_player || m_player->entindex() <= 0 || m_player->entindex() > 63 || m_player == ctx.m_local() || m_player->IsDormant() || m_player->IsDead() || m_player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || !m_player->IsPlayer())
			continue;

		auto log = &records[m_player->entindex() - 1];

		if (!log
			|| log->records_count < 2 
			|| log->player != m_player)
			continue;

		auto rlog = &feature::resolver->player_records[m_player->entindex() - 1];

		//auto first_record = &log->tick_records[(log->records_count-1) & 63];
		auto newest_record = &log->tick_records[log->records_count & 63];

		//auto studio = m_player->GetModelPtr()->m_pStudioHdr;

		if (!log->is_restored 
			|| newest_record->first_after_dormancy 
			|| rlog->breaking_lc 
			|| newest_record->exploit 
			|| abs(csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - newest_record->m_tick) <= 2)
			continue;

		//auto delta_ticks = (newest_record->m_tick - first_record->m_tick);

		//if (delta_ticks > 19 || delta_ticks <= 1)
		//	continue;

		//auto current_interp_iteration = Math::clamp(log->interpolated_count, 0, delta_ticks);

		//auto origin = first_record->origin;
		//const auto origin_delta = (newest_record->origin - first_record->origin) / (float)delta_ticks;

		//origin += (origin_delta * (float)current_interp_iteration);

		auto delta = m_player->get_abs_origin() - m_player->m_vecOrigin();

		if (delta.LengthSquared() > 1.f)
		{
			//m_player->interpolate(csgo.m_globals()->curtime - ((csgo.m_globals()->interval_per_tick * 2.f) * (float(delta_ticks) / float(current_interp_iteration))));
			const auto o_origin = m_player->m_vecOrigin();
			m_player->m_vecOrigin() = m_player->get_abs_origin();
			m_player->force_bone_rebuild();
			m_player->SetupBonesEx();
			m_player->m_vecOrigin() = o_origin;

			//csgo.m_debug_overlay()->AddLineOverlay(first_record->origin, origin, 255, 0, 0, true, 1.f);
		}

		++log->interpolated_count;
	}
}

int GetCorrectionTicks()
{
	float v1; // st7
	float v4; // xmm0_4
	float v6; // [esp+0h] [ebp-10h]

	static auto sv_clockcorrection_msecs = csgo.m_engine_cvars()->FindVar(sxor("sv_clockcorrection_msecs"));

	if (!sv_clockcorrection_msecs || csgo.m_globals()->maxClients <= 1)
		return -1;

	v1 = sv_clockcorrection_msecs->GetFloat();
	v4 = 1.0f;
	v6 = v1 / 1000.0f;
	if (v6 <= 1.0f)
	{
		v4 = 0.0f;
		if (v6 >= 0.0f)
			v4 = v1 / 1000.0f;
	}
	return ((v4 / csgo.m_globals()->interval_per_tick) + 0.5f);
}

void c_lagcomp::store_records(ClientFrameStage_t stage)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

	if (stage != FRAME_NET_UPDATE_END)
		return;

	csgo.m_engine()->FireEvents();

	if (!ctx.m_local())
		return;

	if (stage == FRAME_NET_UPDATE_END)
	{
		auto v18 = GetCorrectionTicks();

		if (v18 == -1)
			ctx.last_cmd_delta = 0;
		else
		{
			auto v19 = csgo.m_client_state()->m_clockdrift_manager.m_nServerTick; //csgo.m_engine()->GetTick();

			auto m_sim_ticks = TIME_TO_TICKS(ctx.m_local()->m_flSimulationTime());

			if (m_sim_ticks > TIME_TO_TICKS(ctx.m_local()->m_flOldSimulationTime())
				&& abs(m_sim_ticks - v19) <= v18)
				ctx.last_cmd_delta = m_sim_ticks - v19;
		}
	}

	//feature::sound_parser->get_active_sounds();

	if (ctx.m_local()->IsDead()) {
		if (spawntime > 0)
			spawntime = 0;
	}
	//else
	//{
	//	static auto m_flVelocityModifier = Engine::PropManager::Instance()->GetOffset(sxor("DT_CSPlayer"), sxor("m_flVelocityModifier"));
	//	if (m_flVelocityModifier > 0) {
	//		// two birds with one stone
	//		// 1. m_vphysicsCollisionState is not actually set on server, this creates a prediction error when its not 0 (crouching or noclipping)
	//		// 2. m_flVelocityModifier is not in datamap
	//		// the solution, replace m_vphysicsCollisionState with m_flVelocityModifier in the datamap!
	//		datamap_t* datamap = ctx.m_local()->GetPredDescMap();
	//		typedescription_t* typedesc = ctx.m_local()->get_datamap_entry(datamap, sxor("m_vphysicsCollisionState"));

	//		if (typedesc)
	//		{
	//			const float recovery_rate = 1.f / 2.5f;
	//			const float tolerance = recovery_rate * csgo.m_globals()->interval_per_tick;

	//			if (typedesc->fieldTolerance != tolerance)
	//			{
	//				int offset = (int)m_flVelocityModifier;

	//				typedesc->fieldType = FIELD_FLOAT;
	//				typedesc->fieldTolerance = tolerance;
	//				typedesc->fieldOffset = offset;
	//				typedesc->fieldSizeInBytes = sizeof(float);
	//				typedesc->flatOffset[TD_OFFSET_NORMAL] = offset;

	//				datamap->m_pOptimizedDataMap = nullptr;
	//			}
	//		}
	//	}
	//}

	for (auto i = 0; i < csgo.m_globals()->maxClients; i++)
	{
		auto player = csgo.m_entity_list()->GetClientEntity(i);

		if (!player || player == ctx.m_local() || !player->GetClientClass())
			continue;

		net_update(player);
	}

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	//if (changed)
	//	filter_records(false);
}

void c_lagcomp::net_update(C_BasePlayer* m_player)
{
#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
	player_info pinfo;
	const auto idx = m_player->entindex() - 1;

	const auto& curlog = &records[idx];
	const auto& resolver_log = &feature::resolver->player_records[idx];

	if (!m_player->GetClientClass() 
		|| m_player->GetClientClass()->m_ClassID != class_ids::CCSPlayer 
		|| m_player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() 
		|| m_player->get_animation_layers_count() <= 0)
		return;

	if (!csgo.m_engine()->GetPlayerInfo(idx + 1, &pinfo))
		return;

	if (ctx.m_local()->IsDead())
	{
		m_player->client_side_animation() = true;
		ctx.updating_anims = true;
		ctx.setup_bones = true;
		return;
	}
	else
	{
		ctx.updating_anims = false;
		ctx.setup_bones = false;
		m_player->client_side_animation() = true;
	}

	if (curlog->player != m_player || pinfo.userid != curlog->userid) {
		curlog->reset();
		resolver_log->reset();
		ctx.shots_fired[idx] = 0;
		ctx.shots_total[idx] = 0;

		resolver_log->missed_shots[R_SHOT] = 0;
		resolver_log->missed_shots[R_60_DELTA] = 0;
		resolver_log->missed_shots[R_40_DELTA] = 0;
		resolver_log->missed_shots[R_USUAL] = 0;
	}

	curlog->player = m_player;
	curlog->userid = pinfo.userid;
	curlog->saved_info = pinfo;

	const auto reset = m_player->IsDead() || m_player->m_iHealth() <= 0;

	// if this happens, delete all the lagrecords.
	if (reset)
	{
		m_player->client_side_animation() = true;
		curlog->reset();
		resolver_log->reset();
		ctx.shots_fired[idx] = 0;
		resolver_log->missed_shots[R_SHOT]=0;
		resolver_log->missed_shots[R_60_DELTA]=0;
		resolver_log->missed_shots[R_40_DELTA]=0;
		resolver_log->missed_shots[R_USUAL]=0;
		ctx.shots_total[idx] = 0;
		curlog->saved_info.userid = -1;
		return;
	}

	// indicate that this player has been out of pvs.
	// insert dummy record to separate records
	// to fix stuff like animation and prediction.
	if (m_player->IsDormant())
	{
		if (curlog->records_count > 0) {
			// get reference to latest added record.
			auto current = &curlog->tick_records[curlog->records_count & 63];

			// mark as dormant.
			if (!current->dormant)
				current->dormant = true;
		}

		//curlog->m_cur_sim = 0;
		curlog->records_count = 0;

		return;
	}

	//if (curlog->records_count > 0)
	//{
	//	auto current = &curlog->tick_records[curlog->records_count & 63];

	//	if (current && current->data_filled && current->dormant)
	//	{
	//		const auto layer_count = m_player->get_animation_layers_count();
	//		if (layer_count > 0)
	//		{
	//			bool animlayers_changed = false;

	//			for (auto i = 0; i < layer_count; i++)
	//			{
	//				auto& layer = m_player->get_animation_layer(i);
	//				auto& p_layer = current->anim_layers[i];

	//				if (layer.m_flCycle != p_layer.m_flCycle
	//					|| layer.m_flPlaybackRate != p_layer.m_flPlaybackRate
	//					|| layer.m_flWeight != p_layer.m_flWeight
	//					|| layer.m_nSequence != p_layer.m_nSequence)
	//				{
	//					animlayers_changed = true;
	//					break;
	//				}
	//			}

	//			if (!animlayers_changed)
	//			{
	//				m_player->m_flSimulationTime() = m_player->m_flOldSimulationTime();
	//				//*m_player->get_animation_state() = current->animstate;
	//				//m_player->m_vecVelocity() = m_player->m_vecAbsVelocity() = current->velocity;
	//				//m_player->m_vecOrigin() = current->origin;
	//				//m_player->set_abs_origin(current->abs_origin);

	//				update = false;
	//			}
	//			else {
	//				if (!update) {
	//					silent_update = true;
	//					update = true;
	//				}
	//			}
	//		}
	//	}
	//}

	int player_updated = false;
	int invalid_simulation = false;

	if (m_player->m_flSimulationTime() != 0.0f)
	{
		if (m_player->get_animation_layer(11).m_flCycle != curlog->m_sim_cycle
			|| m_player->get_animation_layer(11).m_flPlaybackRate != curlog->m_sim_rate)
			player_updated = 1;
		else
		{
			m_player->m_flOldSimulationTime() = curlog->m_old_sim;
			invalid_simulation = 1;
			m_player->m_flSimulationTime() = curlog->m_cur_sim;
		}
	}
	else
		return;

	bool silent_update = false;

	auto update = 0;
	if (!invalid_simulation)
	{

		auto v23 = curlog->m_cur_sim;
		curlog->m_old_sim = v23;
		auto v24 = m_player->m_flSimulationTime();
		curlog->m_cur_sim = v24;
		if (player_updated || v24 != v23 && (curlog->m_cur_sim == 0))
			update = 1;

		if (player_updated && v24 == v23)
			silent_update = true;
	}

	// this is the first data update we are receving
	// OR we received data with a newer simulation context.
	if (update || silent_update/* && wtf <= float(1 / csgo.m_globals()->interval_per_tick) * 2.f*/)
	{
		++curlog->records_count;

		curlog->m_sim_cycle = m_player->get_animation_layer(11).m_flCycle;
		curlog->m_sim_rate = m_player->get_animation_layer(11).m_flPlaybackRate;

		// add new record.
		auto current = Encrypted_t<C_Tickrecord>(&curlog->tick_records[curlog->records_count & 63]);

		current->store(m_player, false);

		current->record_index = curlog->records_count;
		curlog->render_origin = current->origin;

		// mark as non dormant.
		current->dormant = false;
		current->first_after_dormancy = curlog->records_count <= 1 || curlog->m_cur_sim < 1;
		current->animated = false;

		// update animations on current record & resolve & build matrixes.
		update_animations_data(current.get(), m_player);

		// if record is marked as exploit. do not store simulation time and let aimbot ignore it.
		if (silent_update)
			current->valid = false;
		//else
		//	resolver_log->last_valid_non_exp_origin = current->origin;
		//else
		//m_player->m_flOldSimulationTime() = m_player->m_flSimulationTime();
	}

#ifdef VIRTUALIZER
	VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	// no need to store insane amt of data.
	//while (curlog->tick_records.size() > 64)
	//	curlog->tick_records.pop_back();
}

//void c_lagcomp::store_records()
//{
//	get_interpolation();
//
//	if (ctx.m_local() == nullptr)
//	{
//		reset();
//		return;
//	}
//
//	for (auto index = 1; index < 64; index++)
//	{
//		auto entity = (C_BasePlayer*)csgo.m_entity_list()->GetClientEntity(index);
//
//		if (!entity || !entity->IsPlayer() || entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum())
//			continue;
//
//		//auto entidx = entity->entindex();
//
//		if (entity->IsDormant() ||
//			entity->m_iHealth() <= 0 ||
//			!entity->IsPlayer() ||
//			entity->IsDead())
//		{
//			records[index - 1].reset(true);
//
//			entity->client_side_animation() = true;
//			feature::resolver->player_records[index-1].was_dormant = true;
//			feature::resolver->player_records[index-1].has_been_resolved = false;
//
//			continue;
//		}
//
//		//cheat::main::updating_skins = false;
//
//		if (!ctx.m_local()->IsDead()) {
//			update_player_record_data(entity);
//		}
//		else {
//			entity->client_side_animation() = true;
//		}
//	}
//}

void c_lagcomp::reset()
{
	for (auto i = 0; i < 64; i++) {
		records[i].reset();
	}
}

float c_lagcomp::get_interpolated_time()
{
	VIRTUALIZER_FISH_LITE_START;

	auto v20 = csgo.m_globals()->tickcount;

	if (ctx.m_local() && !ctx.m_local()->IsDead())
		v20 = ctx.m_local()->m_nFinalPredictedTick();// 0x00003434

	if (ctx.exploit_allowed && !ctx.fakeducking && ctx.main_exploit > 0)
		v20 -= ctx.next_shift_amount;

	float time = ((TICKS_TO_TIME(v20) - csgo.m_globals()->interval_per_tick)
		+ (csgo.m_globals()->interpolation_amount * csgo.m_globals()->interval_per_tick))
		- (ctx.lerp_time + (ctx.latency[FLOW_OUTGOING] + ctx.latency[FLOW_INCOMING]));

	VIRTUALIZER_FISH_LITE_END;

	return time;
};