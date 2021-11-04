#include "resolver.hpp"
#include "sdk.hpp"
#include "source.hpp"
#include "rage_aimbot.hpp"
#include "autowall.hpp"
#include "visuals.hpp"
#include "misc.hpp"
#include "auto.hpp"
#include "displacement.hpp"
#include "hooked.hpp"
#include "anti_aimbot.hpp"
#include "prediction.hpp"
#include "weapon.hpp"

//bool equal(C_AnimationLayer layer, C_AnimationLayer layer2)
//{
//	return (layer.m_flCycle == layer2.m_flCycle && layer.m_flWeight == layer2.m_flWeight && layer.m_flPlaybackRate == layer2.m_flPlaybackRate && layer.m_nOrder == layer2.m_nOrder && layer.m_nSequence == layer2.m_nSequence);
//}

float AngleDiff(float angle_1 , float angle_2)
{
	float delta = 0.f; // xmm1_4

	for (; angle_1 > 180.0f; angle_1 = angle_1 - 360.0f)
		;
	for (; angle_1 < -180.0f; angle_1 = angle_1 + 360.0f)
		;
	for (; angle_2 > 180.0f; angle_2 = angle_2 - 360.0f)
		;
	for (; angle_2 < -180.0f; angle_2 = angle_2 + 360.0f)
		;
	for (delta = (angle_2 - angle_1); delta > 180.0f; delta = (delta - 360.0f))
		;
	for (; delta < -180.0f; (delta = delta + 360.0f))
		;
	return delta;
}

//void c_resolver::store_data(C_BasePlayer* m_player, C_Tickrecord* record, bool can_store)
//{
//	/*if (!record->valid)
//		return;*/
//	if (m_player == nullptr || m_player->IsDead() || record == nullptr)
//		return;
//	
//	const auto backup_absangles = m_player->get_abs_angles();
//
//	auto setup_side = [&](CCSGOPlayerAnimState& state, matrix3x4_t* matrixes, C_AnimationLayer layers[], float& abs_rotation, std::array<float, 24u>& rposes, int side, int flags, std::array<float,24u> poses, int idx) {
//		if (!can_store || record->lag <=1)
//			memcpy(&layers[0], m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());
//		
//		if (can_store)
//		{
//			//csgo.m_globals()->framecount = framecount;
//			m_player->invalidate_anims();
//			m_player->force_bone_rebuild();
//			if (ctx.m_settings.security_safety_mode == 0 && !record->shot_this_tick)
//			{
//				if ((ctx.shots_fired[idx] % 6) == 5)
//					*(float*)(uintptr_t(m_player) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = 0.f;
//				else
//					*(float*)(uintptr_t(m_player) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = 1.f;
//			}
//			//if (record->duck_amt >= 0.99f && m_player->m_bDucked() && !m_player->get_animation_state()->on_ground)
//			//	m_player->m_flPoseParameter()[6] = 0.f;
//			m_player->set_abs_angles(QAngle(backup_absangles.x, m_player->get_animation_state()->m_abs_yaw, backup_absangles.z));
//			/*if (record->lag > 1)*/
//			memcpy(m_player->animation_layers_ptr(), record->anim_layers, 0x38 * m_player->get_animation_layers_count());
//			m_player->SetupBonesEx(0x100);
//			memcpy(matrixes, m_player->m_CachedBoneData().Base(), record->bones_count * sizeof(matrix3x4_t));
//
//			abs_rotation = Math::normalize_angle(m_player->get_animation_state()->m_abs_yaw);
//		}
//
//		//state = *m_player->get_animation_state();
//		if (!can_store || record->lag<=1)
//			memcpy(&state, m_player->get_animation_state(), 836);
//
//		m_player->m_fFlags() = flags;
//		m_player->m_angEyeAngles() = record->eye_angles;
//		//memcpy(m_player->animation_layers_ptr(), layer, 0x38 * m_player->get_animation_layers_count());
//
//		m_player->m_flPoseParameter() = poses;
//	};
//
//	const auto idx = m_player->entindex() - 1;
//	auto r_log = &player_records[idx];
//	auto log = &feature::lagcomp->records[idx];
//
//	C_AnimationLayer layer[15];
//
//	CCSGOPlayerAnimState animstate_backup = *m_player->get_animation_state();
//	memcpy(layer, m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());
//	//const auto framecount = csgo.m_globals()->framecount;
//
//	const auto flags = m_player->m_fFlags();
//	const auto poses = m_player->m_flPoseParameter();
//
//	//*m_player->get_animation_state() = r_log->simulated_animstates[0];
//	//resolver_info->simulated_animstates[1] = *m_player->get_animation_state();
//	//resolver_info->simulated_animstates[2] = *m_player->get_animation_state();
//
//	///* update original abs yaw */
//	//{
//	//	memcpy(m_player->animation_layers_ptr(), r_log->resolver_anim_layers[0], 0x38 * m_player->get_animation_layers_count());
//	//	r_log->force_velocity = true;
//	//	ctx.updating_resolver = true;
//	//	m_player->update_clientside_animations();	/* update animations. */
//	//	ctx.updating_resolver = false;
//
//	//	if (can_store)
//	//	{
//	//		r_log->original_abs_yaw = m_player->get_animation_state()->abs_yaw;
//
//	//		m_player->force_bone_rebuild();
//	//		const auto success = m_player->SetupBonesEx();
//	//		if (success)
//	//			memcpy(record->zeromatrixes, m_player->m_CachedBoneData().Base(), record->bones_count * sizeof(matrix3x4_t));
//	//	}
//
//	//	memcpy(r_log->resolver_anim_layers[0], m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());
//	//	r_log->simulated_animstates[0] = *m_player->get_animation_state();
//	//	m_player->m_fFlags() = flags;
//	//	m_player->m_flPoseParameter() = poses;
//	//	
//	//	/if (can_store)
//	//	m_player->invalidate_anims();
//	//}
//
//	/*if (can_store)
//	{
//		memcpy(&r_log->resolver_anim_layers[2], record->anim_layers, 0x38 * m_player->get_animation_layers_count());
//		memcpy(&r_log->resolver_anim_layers[1], record->anim_layers, 0x38 * m_player->get_animation_layers_count());
//	}*/
//
//	float delta = 1.f;
//
//	//bool delta1 = false;// log->tick_records.size() < 3 ? 0.f : AngleDiff(m_player->m_angEyeAngles().y, log->tick_records[2].eye_angles.y);
//
//	//*m_player->get_animation_state() = r_log->simulated_animstates[1];
//	memcpy(m_player->get_animation_state(), &r_log->simulated_animstates[1], 836);
//	/* update first desync matrix */
//	{
//		if (record->can_rotate) {
//			/*if (log->tick_records.size() > 1 && !log->tick_records[1].dormant && record->lag <= 1) {
//				m_player->get_animation_state()->abs_yaw = log->tick_records[1].left_side;
//			}
//			else*/
//				m_player->get_animation_state()->m_abs_yaw = Math::normalize_angle(record->eye_angles.y + 60.f);
//
//			m_player->get_animation_state()->m_abs_yaw = Math::normalize_angle(m_player->get_animation_state()->m_abs_yaw);
//		}
//
//		/*if (can_store)
//			m_player->invalidate_anims();*/
//
//		//m_player->m_flPoseParameter() = record->pose_paramaters[1];
//
//		memcpy(m_player->animation_layers_ptr(), r_log->resolver_anim_layers[2], 0x38 * m_player->get_animation_layers_count());
//		//csgo.m_globals()->framecount = TIME_TO_TICKS(record->simulation_time);
//		//r_log->force_velocity = true;
//		ctx.updating_resolver = true;
//		m_player->update_clientside_animations();	/* update animations. */
//		//m_player->update_animstate(m_player->get_animation_state(), m_player->m_angEyeAngles());
//		ctx.updating_resolver = false;
//		//r_log->force_velocity = false;
//
//		//record->pose_paramaters[1] = m_player->m_flPoseParameter();
//
//		delta = AngleDiff(record->eye_angles.y, m_player->get_animation_state()->m_abs_yaw);
//
//		if (!can_store || record->lag <= 1) 
//			setup_side(r_log->simulated_animstates[1], record->leftmatrixes, r_log->resolver_anim_layers[1], record->left_side, record->left_poses, 1, flags, poses, idx);
//		else {
//			if (delta < 0.0f) {
//				setup_side(r_log->simulated_animstates[2], record->rightmatrixes, r_log->resolver_anim_layers[2], record->right_side, record->right_poses, 1, flags, poses, idx);
//			}
//			else {
//				setup_side(r_log->simulated_animstates[1], record->leftmatrixes, r_log->resolver_anim_layers[1], record->left_side, record->left_poses, 1, flags, poses, idx);
//			}
//		}
//		//*m_player->get_animation_state() = animstate_backup;
//	}
//	/* update second desync matrix */
//
//	//*m_player->get_animation_state() = r_log->simulated_animstates[2];
//	m_player->m_flPoseParameter() = poses;
//	memcpy(m_player->get_animation_state(), &r_log->simulated_animstates[2], 836);
//
//	{
//		if (record->can_rotate) {
//			/*if (log->tick_records.size() > 1 && !log->tick_records[1].dormant && record->lag <= 1) {
//				m_player->get_animation_state()->abs_yaw = log->tick_records[1].right_side;
//			}
//			else*/
//				m_player->get_animation_state()->m_abs_yaw = Math::normalize_angle(record->eye_angles.y - 60.f);
//
//			m_player->get_animation_state()->m_abs_yaw = Math::normalize_angle(m_player->get_animation_state()->m_abs_yaw);
//		}
//
//		//m_player->m_flPoseParameter() = record->pose_paramaters[2];
//
//		/* prepare data for velocity fix */
//		memcpy(m_player->animation_layers_ptr(), r_log->resolver_anim_layers[1], 0x38 * m_player->get_animation_layers_count());
//		//csgo.m_globals()->framecount = TIME_TO_TICKS(record->simulation_time);
//		//r_log->force_velocity = true;
//		ctx.updating_resolver = true;
//		m_player->update_clientside_animations();	/* update animations. */
//		//m_player->update_animstate(m_player->get_animation_state(), m_player->m_angEyeAngles());
//		ctx.updating_resolver = false;
//
//		//record->pose_paramaters[2] = m_player->m_flPoseParameter();
//		if (!can_store || record->lag <= 1)
//			setup_side(r_log->simulated_animstates[2], record->rightmatrixes, &r_log->resolver_anim_layers[2][0], record->right_side, record->right_poses, 1, flags, poses, idx);
//		else {
//			if (delta < 0.0f) {
//				setup_side(r_log->simulated_animstates[1], record->leftmatrixes, &r_log->resolver_anim_layers[1][0], record->left_side, record->left_poses, 1, flags, poses, idx);
//			}
//			else {
//				//setup_side(left, &record->fakeLayersLeft[0], &record->m_flAbsRotationLeft, 1);
//				setup_side(r_log->simulated_animstates[2], record->rightmatrixes, &r_log->resolver_anim_layers[2][0], record->right_side, record->right_poses, 1, flags, poses, idx);
//			}
//		}
//		//r_log->force_velocity = false;
//		//if (can_store)
//		//{
//		//	//csgo.m_globals()->framecount = framecount;
//
//		//	m_player->force_bone_rebuild();
//		//	if (ctx.m_settings.security_safety_mode == 0 && !record->shot_this_tick)
//		//	{
//		//		if ((ctx.shots_fired[idx] % 6) == 5)
//		//			*(float*)(uintptr_t(m_player) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = 0.f;
//		//		else
//		//			*(float*)(uintptr_t(m_player) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = 1.f;
//		//	}
//		//	m_player->SetupBonesEx();
//		//	memcpy(record->rightmatrixes, m_player->m_CachedBoneData().Base(), record->bones_count * sizeof(matrix3x4_t));
//
//		//	record->right_side = m_player->get_animation_state()->abs_yaw;
//
//		//	//record->desync_delta = abs(Math::AngleDiff(r_log->original_abs_yaw, m_player->get_animation_state()->abs_yaw));
//		//	r_log->negative_abs_yaw = m_player->get_animation_state()->abs_yaw;
//		//}
//		
//
//		//memcpy(r_log->resolver_anim_layers[1], m_player->animation_layers_ptr(), 0x38 * m_player->get_animation_layers_count());
//		//r_log->simulated_animstates[2] = *m_player->get_animation_state();
//		//m_player->m_fFlags() = flags;
//		////m_player->m_angEyeAngles() = record->eye_angles;
//		//m_player->m_flPoseParameter() = poses;
//
//		/*if (can_store)
//			m_player->invalidate_anims();*/
//
//		//*m_player->get_animation_state() = animstate_backup;
//	} 
//
//	m_player->m_flPoseParameter() = poses;
//
//	//record->accurate_anims = equal(r_log->resolver_anim_layers[1][6], record->anim_layers[6]) && equal(r_log->resolver_anim_layers[1][3], record->anim_layers[3]) && equal(r_log->resolver_anim_layers[1][12], record->anim_layers[12])
//	//	|| equal(r_log->resolver_anim_layers[0][6], record->anim_layers[6]) && equal(r_log->resolver_anim_layers[0][3], record->anim_layers[3]) && equal(r_log->resolver_anim_layers[0][12], record->anim_layers[12])
//	//	|| equal(r_log->resolver_anim_layers[2][6], record->anim_layers[6]) && equal(r_log->resolver_anim_layers[2][3], record->anim_layers[3]) && equal(r_log->resolver_anim_layers[2][12], record->anim_layers[12]);
//
//	m_player->set_abs_angles(backup_absangles);
//
//	memcpy(m_player->animation_layers_ptr(), layer, 0x38 * m_player->get_animation_layers_count());
//	*m_player->get_animation_state() = animstate_backup;
//}

//float difference(const float& v1, const float& v2)
//{
//	const auto mmin = min(v1, v2);
//	const auto mmax = max(v1, v2);
//
//	return fabs(mmax - mmin);
//}
//
//bool point_in_bbox(const Vector& point, const Vector& origin, const Vector& min, const Vector& max)
//{
//	Vector normalized = point - origin;
//	return (normalized.x >= min.x && normalized.x <= max.x && normalized.y >= min.y && normalized.y <= max.y && normalized.z >= min.z && normalized.z <= max.z);
//}

//float difference(const Vector& point, const Vector& origin)
//{
//	const auto diff_x = abs(point.x - origin.x);
//	const auto diff_y = abs(point.y - origin.y);
//	const auto diff_z = abs(point.z - origin.z);
//
//	return max(diff_x, max(diff_z, diff_y));
//}

//bool are_equal(const Vector& point, const Vector& origin, const float &max_diff = 0.1f)
//{
//	return (difference(point,origin) <= max_diff);
//}

//bool c_resolver::hurt_resolver(C_BasePlayer* m_player, _shotinfo* record)
//{
//	if (record == nullptr || !record->_tickrecord || ctx.m_local() == nullptr || ctx.m_local()->IsDead() || m_player->IsDead() || m_player->IsDormant())
//		return false;
//
//	auto start = record->_eyepos;
//	auto end = record->_impact_pos;
//
//	if (!record->_hurt_called) //lol
//		return false;
//
//	C_Tickrecord backup = C_Tickrecord(m_player, true);
//	record->_tickrecord->apply(m_player, false, true);
//
//	memcpy(m_player->m_CachedBoneData().Base(), record->_tickrecord->leftmatrixes, record->_tickrecord->bones_count * sizeof(matrix3x4_t));
//	const auto left_damage = feature::autowall->CanHit(start, end, ctx.m_local(), m_player, record->_hitboxid);
//
//	//m_player->GetBoneAccessor().m_ReadableBones = record->_tickrecord.bones_count;
//	//m_player->GetBoneAccessor().m_WritableBones = 0;
//	memcpy(m_player->m_CachedBoneData().Base(), record->_tickrecord->rightmatrixes, record->_tickrecord->bones_count * sizeof(matrix3x4_t));
//	const auto right_damage = feature::autowall->CanHit(start, end, ctx.m_local(), m_player, record->_hitboxid);
//
//	backup.apply(m_player, true);
//
//	auto r_log = &player_records[m_player->entindex() - 1];
//	const auto prev_resolving = r_log->resolving_method;
//
//	const auto left_mul = difference(record->_final_damage, left_damage);
//	const auto right_mul = difference(record->_final_damage, right_damage);
//
//	if (left_mul == right_mul)
//		return false;
//
//	if (left_mul > 25.f && right_mul > 25.f)
//	{
//		ctx.shots_fired[m_player->entindex() - 1] = 0;
//		r_log->resolving_method = r_log->prev_resolving_method = 0;
//	}
//	else
//	{
//		ctx.shots_fired[m_player->entindex() - 1] = 0;
//
//		if (left_mul < right_mul)
//			r_log->resolving_method = r_log->prev_resolving_method = 1; //- 60.f
//		else
//			r_log->resolving_method = r_log->prev_resolving_method = 2; //+ 60.f
//
//		//_events.push_back(_event(std::string("hitbox missmatch [" + std::to_string(left_mul) + "]:[" + std::to_string(right_mul) + "]")));
//	}
//
//	//_events.push_back(_event(std::string("resolving method changed due to hitbox missmatch [" + std::to_string(prev_resolving) + "]:[" + std::to_string(r_log->resolving_method) + "]")));
//
//	return true;
//}

//bool c_resolver::hurt_resolver(C_BasePlayer* m_player, _shotinfo* record)
//{
//	if (record == nullptr || record->_record.player == nullptr || !record->_tickrecord.data_filled || record->_tickrecord.dormant || ctx.m_local() == nullptr || ctx.m_local()->IsDead() || m_player->IsDead() || m_player->IsDormant())
//		return false;
//
//	auto start = record->_eyepos;
//	auto end = record->_impact_pos;
//
//	if (!record->_hurt_called) //lol
//		return false;
//
//	for (auto imp : record->_impacts)
//	{
//		if (point_in_bbox(imp, record->_tickrecord.origin, record->_tickrecord.object_mins, record->_tickrecord.object_maxs)) {
//			end = imp;
//			break;
//		}
//	}
//
//	const auto angle_hitbox = Math::CalcAngle(start, record->_hitbox);
//	const auto angle_impact = Math::CalcAngle(start, end);
//
//	C_Tickrecord backup = C_Tickrecord(m_player, true);
//	record->_tickrecord.apply(m_player, false, true);
//
//	//CTraceFilter _filter;
//	//_filter.pSkip = ctx.m_local();
//	trace_t tr;
//	Ray_t ray;
//
//	Vector forward;
//	Math::AngleVectors(angle_impact, &forward);
//	auto endpos = start + forward * 8096.f;
//
//	ray.Init(start, endpos);
//
//	bool failed = false;
//
//	memcpy(m_player->m_CachedBoneData().Base(), record->_tickrecord.leftmatrixes, m_player->GetBoneCount() * sizeof(matrix3x4_t));
//	csgo.m_engine_trace()->ClipRayToEntity(ray, 0x4600400B, m_player, &tr);
//
//	const auto left_tr_hg = tr.hitgroup;
//	const auto left_tr_endpos = tr.endpos;
//
//	if (tr.m_pEnt != m_player)
//		failed = true;
//
//	memcpy(m_player->m_CachedBoneData().Base(), record->_tickrecord.rightmatrixes, m_player->GetBoneCount() * sizeof(matrix3x4_t));
//	csgo.m_engine_trace()->ClipRayToEntity(ray, 0x4600400B, m_player, &tr);
//
//	const auto right_tr_hg = tr.hitgroup;
//	const auto right_tr_endpos = tr.endpos;
//
//	if (tr.m_pEnt != m_player)
//		failed = true;
//
//	//memcpy(m_player->m_CachedBoneData().Base(), record->_tickrecord.zeromatrixes, m_player->GetBoneCount() * sizeof(matrix3x4_t));
//	//csgo.m_engine_trace()->ClipRayToEntity(ray, 0x4600400B, m_player, &tr);
//
//	const auto zero_tr_hg = tr.hitgroup;
//	const auto zero_tr_endpos = tr.endpos;
//
//	if (tr.m_pEnt != m_player)
//		failed = true;
//
//	backup.apply(m_player, true);
//
//	if (failed)
//		return false;
//
//	auto r_log = &player_records[m_player->entindex() - 1];
//	const auto prev_resolving = r_log->resolving_method;
//
//	if (left_tr_hg == record->_final_hitgroup && right_tr_hg == record->_final_hitgroup)
//	{
//		/*const auto zero_diff	= difference(zero_tr_endpos, end);
//		const auto right_diff	= difference(right_tr_endpos, end);
//		const auto left_diff	= difference(left_tr_endpos, end);
//
//		if (zero_diff != right_diff && left_diff != right_diff && zero_diff != left_diff)
//		{
//			ctx.shots_fired[m_player->entindex() - 1] = 0;
//
//			if (right_diff < zero_diff && left_diff < zero_diff)
//			{
//				if (right_diff < left_diff)
//					r_log->last_hurt_resolved = 2;
//				else
//					r_log->last_hurt_resolved = 1;
//			}
//			else
//				r_log->last_hurt_resolved = 0;
//		}
//		else
//		{
//			_events.push_back(_event("none impacts are right."));
//			return false;
//		}
//
//		_events.push_back(_event(std::string("resolving method changed due to hitbox missmatch [i]:[" + std::to_string(prev_resolving) + "]:[" + std::to_string(r_log->last_hurt_resolved) + "]")));*/
//		return false;
//	}
//
//	ctx.shots_fired[m_player->entindex() - 1] = 0;
//
//	if (right_tr_hg == record->_final_hitgroup)
//		r_log->anims_resolving = 2; //- 60.f
//	else if (left_tr_hg == record->_final_hitgroup)
//		r_log->anims_resolving = 1; //+ 60.f
//	else 
//		r_log->anims_resolving = 0; //+ 0.f
//
//	_events.push_back(_event(std::string("resolving method changed due to hitbox missmatch [" + std::to_string(prev_resolving) + "]:[" + std::to_string(r_log->anims_resolving) + "]")));
//
//	return true;
//}

//bool c_resolver::find_side(C_BasePlayer* m_player, int& side)
//{
//	//const float at_target_yaw = Math::CalcAngle(ctx.m_local()->m_vecOrigin(), m_player->m_vecOrigin()).y;
//
//	if (player_records[m_player->entindex() - 1].did_shot_this_tick) {
//		side = 0;
//		return false;
//	}
//
//	auto freestand_info = player_records[m_player->entindex()-1].freestand_info;
//
//	if (player_records[m_player->entindex() - 1].did_shot_this_tick || freestand_info.left_damage <= 0 && freestand_info.right_damage <= 0) {
//		side = 0;
//		return false;
//	}
//
//	if (freestand_info.left_damage > freestand_info.right_damage)
//		side = 1.f;
//	else
//		side = -1.f;
//
//	return true;
//}

bool c_resolver::resolve_using_animations(C_BasePlayer* m_player, C_Tickrecord* record, int& result)
{
	//static float m_flLastFlickTime[64] = {};
	
	auto r_log = &player_records[m_player->entindex() - 1];
	auto log = &feature::lagcomp->records[m_player->entindex() - 1];

	auto previous = log->records_count > 1 ? &log->tick_records[(log->records_count - 1) & 63] : nullptr;

	if (!r_log || !previous || !previous->data_filled || record->first_after_dormancy || record->anim_layers[6].m_flPlaybackRate == 0.0f)
		return false;

	auto anim_layer = &record->anim_layers[6];
	auto animState = m_player->get_animation_state();

	//float move_distance = animState->m_flSpeed2D / (lag_data.m_flRate / Source::m_pGlobalVars->interval_per_tick);

	//float best_move = FLT_MAX;
	//float best_delta = FLT_MAX;

	//// estiminate server torso yaw/move_yaw pose
	//for (float i = 0.f; i <= 1.0f; i += 0.005f) {
	//	auto move_yaw = player->m_flPoseParameter()[7];
	//	player->m_flPoseParameter()[7] = i;

	//	float dist = player->GetSequenceMoveDist((*(CStudioHdr**)((uintptr_t)player + 0x294C)), anim_layer->m_nSequence);

	//	float delta = std::fabsf(dist - move_distance);
	//	if (best_delta > delta) {
	//		best_move = i;
	//		best_delta = delta;
	//	}

	//	player->m_flPoseParameter()[7] = move_yaw;
	//}

	//if (best_move == FLT_MAX)
	//	return;

	//auto eyeAngles = m_player->m_angEyeAngles();

	//float lby = m_player->m_flLowerBodyYawTarget();
	//float absRotation = 0.0f;
	//float ResolvedYaw = 0.0f;

	//float desync = feature::anti_aim->get_max_desync_delta(m_player);

	//auto CalculatePlaybackRate = [&](C_BasePlayer* player, const float& velocity_delta, float move_dist = -1.0f) {
	//	auto state = player->get_animation_state();
	//	auto anim_layer = &record->anim_layers[6];

	//	auto move_yaw = player->m_flPoseParameter()[7];
	//	player->m_flPoseParameter()[7] = velocity_delta;

	//	auto seq_dur = player->GetSequenceCycleRate(player->GetModelPtr(), anim_layer->m_nSequence);

	//	float v56;
	//	if (seq_dur <= 0.0f)
	//		v56 = 10.0f;
	//	else
	//		v56 = 1.0f / seq_dur;

	//	float v237 = 1.0f / (1.0f / v56);

	//	auto dist = move_dist;
	//	if (move_dist == -1.0f) {
	//		dist = player->GetSequenceMoveDist(player->GetModelPtr(), anim_layer->m_nSequence);
	//	}

	//	if (dist * v237 <= 0.001f) {
	//		dist = 0.001f;
	//	}
	//	else {
	//		dist *= v237;
	//	}

	//	player->m_flPoseParameter()[7] = move_yaw;

	//	float speed = record->velocity.Length2D();
	//	float v50 = (1.0f - (state->m_walk_run_transition * 0.15f)) * ((speed / dist) * v56); // 0x11C
	//	float new_playback_rate = csgo.m_globals()->interval_per_tick * v50;
	//	return new_playback_rate;
	//};

	//float left_move = (Math::normalize_angle(animState->m_flCurrentTorsoYaw + desync));
	//float right_move = (Math::normalize_angle(animState->m_flCurrentTorsoYaw - desync));

	//float torso_yaw = Math::normalize_angle(best_move * 360.0f);

	//float left_delta = std::fabsf(Math::normalize_angle(torso_yaw - left_move));
	//float right_delta = std::fabsf(Math::normalize_angle(torso_yaw - right_move));

	//int side = -1;
	//float move_yaw = best_move;
	//if (right_delta > left_delta) {
	//	side = 1;
	//}
	//else {
	//	side = 0;
	//}

	//bool isJittering = false;
	//if (history.size() > 0) {
	//	int jitter_ticks = 0;
	//	float yaw = eyeAngles.y;
	//	for (const auto tick : history) {
	//		float eyeDelta = Math::normalize_angle(tick.eye_angles.y - yaw);
	//		jitter_ticks += (std::fabsf(eyeDelta) > desync);
	//		yaw = tick.eye_angles.y;
	//		if (jitter_ticks > 1)
	//			break;
	//	}
	//	isJittering = jitter_ticks > 1;
	//}

	//if (lag_data.m_iSide != -1) {
	//	// have side, start to catch flicks
	//	if (lag_data.m_iSide != side) {
	//		// catch flick, setup timer
	//		if (lag_data.m_flLastFlickTime == FLT_MAX || lag_data.m_iCurrentSide == -1) {
	//			lag_data.m_flLastFlickTime = 0.3f + TICKS_TO_TIME(choked_ticks);
	//			lag_data.m_iCurrentSide = side;
	//			side = lag_data.m_iSide;
	//		}

	//		if (lag_data.m_flLastFlickTime <= 0.0f) {
	//			lag_data.m_flLastFlickTime = FLT_MAX;
	//			lag_data.m_iCurrentSide = -1;
	//		}
	//		else {
	//			lag_data.m_flLastFlickTime -= TICKS_TO_TIME(choked_ticks);
	//			side = lag_data.m_iSide;
	//		}
	//	}
	//	else if (!isJittering && lag_data.m_iCurrentSide != -1 && lag_data.m_iCurrentSide != lag_data.m_iSide && lag_data.m_flLastFlickTime <= 0.3f + TICKS_TO_TIME(choked_ticks)) {
	//		// flicked, but it changed
	//		lag_data.m_flLastFlickTime = FLT_MAX;
	//		lag_data.m_iCurrentSide = -1;
	//	}
	//}

	//float move_distance = record->velocity.Length2D() / (record->anim_layers[6].m_flPlaybackRate / csgo.m_globals()->interval_per_tick);

	//auto move_yaw_backup = m_player->m_flPoseParameter()[7];
	//m_player->m_flPoseParameter()[7] = 0.5f;
	//float dist_min = m_player->GetSequenceMoveDist(m_player->GetModelPtr(), anim_layer->m_nSequence);
	//m_player->m_flPoseParameter()[7] = move_yaw_backup;

	//float move_yaw = m_player->m_flPoseParameter()[7];
	//if (move_distance >= dist_min) {
	//	// estiminate server torso yaw/move_yaw pose
	//	float best_delta = FLT_MAX;
	//	float best_move = FLT_MAX;
	//	for (float i = 0.f; i < 1.0f; i += 0.001f) {
	//		auto move_yaw = m_player->m_flPoseParameter()[7];
	//		m_player->m_flPoseParameter()[7] = i;

	//		float rate = CalculatePlaybackRate(m_player, i);
	//		float delta = abs(rate - record->anim_layers[6].m_flPlaybackRate);
	//		if (best_delta > delta) {
	//			best_move = i;
	//			best_delta = delta;
	//		}

	//		m_player->m_flPoseParameter()[7] = move_yaw;
	//	}

	//	if (best_move == FLT_MAX)
	//		return false;

	//	//float serverTorsoYaw = fmod((best_move * 360.0f) + 180.f, 360.f);
	//	//float serverTorsoYaw = best_move;
	//	//if (serverTorsoYaw < 0.f)
	//	//	serverTorsoYaw += 360.f;

	//	float velocityAng = RAD2DEG(atan2f(record->velocity.y, record->velocity.x));
	//	float previousvelocityAng = RAD2DEG(atan2f(previous->velocity.y, previous->velocity.x));

	//	float torso_1 = (best_move);
	//	float torso_2 = (1.0f - best_move);

	//	torso_1 = Math::normalize_angle((torso_1 * 360.0f) + 180.f);
	//	torso_2 = Math::normalize_angle((torso_2 * 360.0f) + 180.f);

	//	float delta_1 = std::fabsf(Math::normalize_angle(torso_1 - velocityAng));
	//	float delta_2 = std::fabsf(Math::normalize_angle(torso_2 - velocityAng));

	//	if (delta_1 > delta_2) {
	//		best_move = torso_2;
	//		m_flLastFlickTime[m_player->entindex()-1] = torso_1;
	//	}
	//	else {
	//		best_move = torso_1;
	//		m_flLastFlickTime[m_player->entindex() - 1] = torso_2;
	//	}

	//	//float serverTorsoYaw = Math::AngleNormalize( ( best_move * 360.0f ) + 180.0f );
	//	m_flLastFlickTime[m_player->entindex() - 1] = Math::normalize_angle((best_move * 360.0f));

	//	float serverTorsoYaw = best_move;

	//	if (serverTorsoYaw < 0.f)
	//		serverTorsoYaw += 360.f;

	//	if (animState->m_move_yaw < 0.f)
	//		animState->m_move_yaw += 360.f;

	//	serverTorsoYaw = Math::normalize_angle(serverTorsoYaw);
	//	animState->m_move_yaw = Math::normalize_angle(animState->m_move_yaw);

	//	float delta = Math::AngleDiff(velocityAng, previousvelocityAng);

	//	//lag_data.m_flServerTorso = serverTorsoYaw;

	//	if (delta > 0.0f)
	//		result = 1;
	//	else
	//		result = 2;
	if ((abs(csgo.m_globals()->realtime - r_log->last_time_changed_direction) > 0.5f || record->anim_layers[7].m_flWeight >= 0.99f) 
		&& int(r_log->resolver_layers[0][6].m_flCycle * 10000.f) == int(record->anim_layers[6].m_flCycle * 10000.f))
	{
		//float delta1 = abs(record->anim_layers[6].m_flPlaybackRate - r_log->resolver_layers[0][6].m_flPlaybackRate);
		//float delta2 = abs(record->anim_layers[6].m_flPlaybackRate - r_log->resolver_layers[1][6].m_flPlaybackRate);
		//float delta3 = abs(record->anim_layers[6].m_flPlaybackRate - r_log->resolver_layers[2][6].m_flPlaybackRate);

		int max_delta = INT_MAX;
		int best_side = -1;

		int left_delta = 0;
		int right_delta = 0;

		for (auto i = 0; i < 3; i++)
		{
			auto resolver_layer = &r_log->resolver_layers[i][6];

			if (resolver_layer->m_nSequence != anim_layer->m_nSequence
				|| resolver_layer->m_nOrder != anim_layer->m_nOrder)
				continue;

			auto delta = int(std::abs(anim_layer->m_flPlaybackRate - resolver_layer->m_flPlaybackRate) * 10000.f);
			//auto wdelta = int(fabsf(anim_layer->m_flWeight - resolver_layer->m_flWeight) * 10000.f);

			if (i == 2)
				left_delta = delta;
			else if (i == 1)
				right_delta = delta;

			if (max_delta > delta) {
				max_delta = delta;
				best_side = i;
			}
		}

		if (best_side == -1)
			return false;

		result = best_side;

		//if (result != 0 && !record->fake_walking)
		//	result = (result == 2 ? 1 : 2);

		return (right_delta != left_delta);
	}

	return false;
}

bool c_resolver::add_shot(const Vector& shotpos, const Vector& shotpoint, C_Tickrecord* record, const int damage, const int hitgroup, const int& enemy_index)
{
	bool result = false;

	static auto last_tick = 0;

	const auto outgoing = ctx.latency[FLOW_OUTGOING];
	const auto incoming = ctx.latency[FLOW_INCOMING];
	const auto serverTickcount = csgo.m_globals()->tickcount + (ctx.fakeducking ? 15 - csgo.m_client_state()->m_iChockedCommands : 0) + TIME_TO_TICKS(outgoing + incoming);

	if (serverTickcount != last_tick)
	{
		shots.emplace_back(shotpos, shotpoint, serverTickcount, csgo.m_globals()->realtime, hitgroup, damage, enemy_index, record);

		result = true;
		last_tick = serverTickcount;
	}

	return result;
}

void c_resolver::update_missed_shots(const ClientFrameStage_t& stage)
{
	if (stage != FRAME_NET_UPDATE_START)
		return;

	auto it = shots.begin();
	while (it != shots.end())
	{
		const auto& shot = *it;
		if (abs(shot.tick - csgo.m_globals()->tickcount) > 40)
		{
			it = shots.erase(it);
		}
		else
		{
			++it;
		}
	}

	auto it2 = current_shots.begin();
	while (it2 != current_shots.end())
	{
		const auto &shot = *it2;
		if (abs(shot.tick - csgo.m_globals()->tickcount) > 40)
		{
			it2 = current_shots.erase(it2);
		}
		else
		{
			++it2;
		}
	}
}

std::deque<shot_t>& c_resolver::get_shots()
{
	return shots;
}

void c_resolver::hurt_listener(IGameEvent* game_event)
{
	if (shots.empty() || !ctx.m_local() || ctx.m_local()->IsDead())
		return;

	//_(attacker_s, "attacker");
	//_(userid_s, "userid");
	//_(hitgroup_s, "hitgroup");
	//_(dmg_health_s, "dmg_health");

	const auto attacker = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("attacker")));
	const auto victim = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));
	const auto hitgroup = game_event->GetInt(sxor("hitgroup"));
	const auto damage = game_event->GetInt(sxor("dmg_health"));

	if (attacker != csgo.m_engine()->GetLocalPlayer())
		return;

	if (victim == csgo.m_engine()->GetLocalPlayer())
		return;

	auto player = csgo.m_entity_list()->GetClientEntity(victim);
	if (!player || player->m_iTeamNum() == ctx.m_local()->m_iTeamNum())
		return;

	if (unapproved_shots.empty())
		return;

	for (auto& shot : unapproved_shots)
	{
		if (!shot.hurt)
		{
			shot.hurt = true;
			shot.hitinfo.victim = victim;
			shot.hitinfo.hitgroup = hitgroup;
			shot.hitinfo.damage = damage;
			return;
		}
	}
}

shot_t* c_resolver::closest_shot(int tickcount)
{
	shot_t* closest_shot = nullptr;
	auto closest_diff = 64;
	for (auto& shot : shots)
	{
		const auto diff = abs(tickcount - shot.tick);
		if (diff <= closest_diff)
		{
			closest_shot = &shot;
			closest_diff = diff;
			continue;
		}


		break;
	}

	return closest_shot;
}

bool c_resolver::is_record_equal(C_Tickrecord* a1, C_Tickrecord* a2)
{
	if (a1->shot_this_tick != a2->shot_this_tick)
		return 0;
	else if (a1->left_side != a2->left_side || a1->right_side != a2->right_side)
		return 0;
	if (a1->origin.x != a2->origin.x
		|| a1->origin.y != a2->origin.y
		|| a1->origin.z != a2->origin.z
		|| a1->eye_angles.x != a2->eye_angles.x
		|| a1->eye_angles.y != a2->eye_angles.y
		|| a1->eye_angles.z != a2->eye_angles.z
		|| a1->entity_flags != a2->entity_flags
		|| a1->animations_updated != a2->animations_updated
		|| a1->object_maxs.z != a2->object_maxs.z)
	{
		return 0;
	}
	return 1;
}

C_Tickrecord* c_resolver::find_shot_record(C_BasePlayer* player, c_player_records* data) {
	resolver_records* r_log = &feature::resolver->player_records[player->entindex()-1];

	if (data->records_count < 1)
		return nullptr;

	C_Tickrecord *previous = 0;

	// iterate records.
	for (auto iter = 0; iter <= min(50, data->records_count); iter++) {

		auto it = &data->tick_records[(data->records_count - iter) & 63];

		if (!it 
			|| !it->data_filled
			|| !it->animated 
			|| !it->valid 
			//|| it->first_after_dormancy
			|| feature::lagcomp->is_time_delta_too_large(it))
			continue;

		if (it->lc_exploit || r_log->breaking_lc && iter >= 1)
			return nullptr;

		if (previous && is_record_equal(previous, it)) {
			previous = it;
			continue;
		}

		if (it->dormant)
			break;

		// try to find a record with a shot, walking or no anti-aim.
		if (it->shot_this_tick || (fabs(it->eye_angles.x) < 25 || it->animations_updated) /*&& (player->get_weapon() == nullptr || fabs(player->get_weapon()->m_flLastShotTime() - it->simulation_time) > TICKS_TO_TIME(6))*/)
			return it;

		previous = it;
		//max_velocity_modifier = it->velocity_modifier;
	}

	// none found above, return the first valid record if possible.
	return nullptr;
}

C_Tickrecord* c_resolver::find_first_available(C_BasePlayer* player, c_player_records* data, bool oldest) {
	resolver_records* r_log = &feature::resolver->player_records[player->entindex() - 1];

	if (data->records_count < 1)
		return nullptr;
	
	if (!oldest) {
		// iterate records.
		for (auto iter = 0; iter <= 1; iter++) {

			auto it = &data->tick_records[(data->records_count - iter) & 63];

			if (!it
				|| !it->data_filled
				|| !it->animated 
				|| !it->valid 
				|| r_log->breaking_lc && iter > 0
				|| feature::lagcomp->is_time_delta_too_large(it) && (csgo.m_client_state()->m_clockdrift_manager.m_nServerTick - it->m_tick) >= TIME_TO_TICKS(0.2f))
				continue;

			if (it->dormant)
				break;

			return it;
		}
	}
	else
	{
		// iterate records.
		for (auto iter = min(50, data->records_count); iter >= 0; --iter) {

			auto it = &data->tick_records[(data->records_count - iter) & 63];

			if (it->breaking_lc && iter >= 1)
				break;

			if (!it
				|| it->dormant 
				|| !it->data_filled 
				|| !it->animated 
				|| !it->valid 
				//|| it->first_after_dormancy 
				|| feature::lagcomp->is_time_delta_too_large(it))
				continue;

			return it;
		}
	}

	return nullptr;
}

void c_resolver::record_shot(IGameEvent* game_event)
{
	//_(userid_s, "userid");

	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));
	auto player = csgo.m_entity_list()->GetClientEntity(userid);

	if (!player)
		return;

	if (player != ctx.m_local())
		return;

	const auto shot = closest_shot(csgo.m_globals()->tickcount);

	if (!shot)
		return;

	current_shots.push_back(*shot);
}

void c_resolver::listener(IGameEvent* game_event)
{
	constexpr auto weap_fire = hash_32_fnv1a_const("weapon_fire");

	if (hash_32_fnv1a_const(game_event->GetName())==weap_fire)
	{
		record_shot(game_event);
		return;
	}

	if (current_shots.empty())
		return;

	const auto userid = csgo.m_engine()->GetPlayerForUserID(game_event->GetInt(sxor("userid")));
	auto player = csgo.m_entity_list()->GetClientEntity(userid);

	if (!player || player != ctx.m_local())
		return;

	const Vector pos(game_event->GetFloat(sxor("x")), game_event->GetFloat(sxor("y")), game_event->GetFloat(sxor("z")));

	//if (vars.misc.impacts.get<bool>())
	//	g_pDebugOverlay->AddBoxOverlay(pos, Vector(-2, -2, -2), Vector(2, 2, 2), QAngle(0, 0, 0), 0, 0, 155, 127, 4);

	auto shot = &current_shots[0];

	static int last_tick = 0;
	static auto last_length = 0.f;
	static auto counter = 0;

	if (last_tick == csgo.m_globals()->tickcount)
		counter++;
	else
	{
		counter = 0;
		last_length = 0.f;
	}

	if (last_tick == csgo.m_globals()->tickcount && shot->shotpos.Distance(pos) < last_length)
		return;

	last_length = shot->shotpos.Distance(pos);

	if (counter)
		unapproved_shots.pop_front();

	shot->hitpos = pos;

	unapproved_shots.push_back(*shot);

	last_tick = csgo.m_globals()->tickcount;
}

bool c_resolver::select_next_side(C_BasePlayer* m_player, C_Tickrecord* record)
{
	//auto apply_if_good_side = [&](const bool is_good, const int new_side) -> bool
	//{
	//	if (is_good)
	//	{
	//		//const auto is_banned = std::find(banned_sides.begin(), banned_sides.end(), new_side) != banned_sides.end();

	//		////alright cool we can just use this side
	//		//if (!is_banned)
	//		//{
	//		record->resolver_index = new_side;
	//		record->resolver_type = 6;

	//		return true;
	//		/*}*/
	//	}

	//	return false;
	//};

	resolver_records* r_log = &player_records[m_player->entindex() - 1];
	c_player_records* log = &feature::lagcomp->records[m_player->entindex() - 1];

	int next_resolving_method = 0;
	int next_resolving_type = 0;

	r_log->next_resolving_index.clear();

	auto was_triggered = false;

	/*if (r_log->resolver_type != 0 && !record->shot_this_tick)
		r_log->next_resolving_index.emplace_back(r_log->resolver_index, 1);*/

	const auto base_attarg = Math::CalcAngle(record->origin, ctx.m_local()->get_abs_origin()).y;
	auto at_target_2 = (int)Math::angle_diff(base_attarg, record->eye_angles.y);
	auto opposite_angle = 0;

	if (record->shot_this_tick)
		//|| csgo.m_globals()->realtime - r_log->anim_time > 15.f && (csgo.m_globals()->realtime - r_log->freestanding_update_time) > 5.f)
	{
		auto left_side_diff = fabsf(Math::AngleDiff(record->left_side, record->original_abs_yaw));
		auto right_side_diff = fabsf(Math::AngleDiff(record->right_side, record->original_abs_yaw));

		//opposite lby detection
		if (fabsf(left_side_diff - right_side_diff) > 30.f/*desync_diff > (record->desync_delta * 1.1f)*/)
		{
			opposite_angle = (left_side_diff > right_side_diff ? 1 : 2);
			was_triggered = true;
		}
	}

	if (was_triggered)
		r_log->next_resolving_index.emplace_back(opposite_angle, record->shot_this_tick ? 3 : 2);

	//if (!record->shot_this_tick/* && record->animstate.m_velocity_length_xy <= 2.f*/)
	//	r_log->next_resolving_index.emplace_back(r_log->last_abs_yaw_side, 2);

	//useless check, doing it since anim resolver might need some work.
	if ((csgo.m_globals()->realtime - r_log->freestanding_update_time) <= 10.f 
		&& r_log->freestanding_side > 0) {
		//next_resolving_method = r_log->freestanding_side;
		//next_resolving_type = 2;
		//if (/*ctx.m_settings.aimbot_extra_scan_aim[2] && */r_log->freestand_right_tick != r_log->freestand_left_tick) {
		//	if (r_log->freestanding_side == 1 && r_log->freestand_left_tick > r_log->freestand_right_tick)
		//		r_log->freestanding_side = 2;
		//	else if (r_log->freestanding_side == 2 && r_log->freestand_left_tick < r_log->freestand_right_tick)
		//		r_log->freestanding_side = 1;
		//}
		r_log->next_resolving_index.emplace_back(r_log->freestanding_side, record->shot_this_tick ? 2 : 3);
	}

	if ((csgo.m_globals()->realtime - r_log->anim_time <= 20.f && r_log->anims_pre_resolving >= 0
		|| record->animations_updated) 
		&& !r_log->missed_in_anim_resolver && !record->shot_this_tick) {
		//next_resolving_method = record->animations_index;
		//next_resolving_type = 3;
		
		r_log->next_resolving_index.emplace_back((record->animations_updated ? record->animations_index : r_log->anims_pre_resolving), 9);
	}

	if (r_log->last_hurt_resolved > 0) {
		//next_resolving_method = r_log->last_hurt_resolved;
		//next_resolving_type = 4;
		r_log->next_resolving_index.emplace_back(r_log->last_hurt_resolved, 5);
	}

	if (r_log->last_hitpos_side > 0 && (csgo.m_globals()->realtime - r_log->last_hitpos_side_correction) <= 2.f && !r_log->missed_in_hitpos_resolver && !record->shot_this_tick/* == r_log->last_hitbox_correction_was_onshot*/) {
		//next_resolving_method = r_log->last_hurt_resolved;
		//next_resolving_type = 4;
		r_log->next_resolving_index.emplace_back(r_log->last_hitpos_side, 7);
	}

	//if (record->velocity.Length2D() > 10 && (csgo.m_globals()->realtime - r_log->last_time_changed_direction) > 2.0f) {
	//	auto vel_angle = ToDegrees(atan2(record->velocity.y, record->velocity.x));
	//
	//	auto delta = Math::AngleDiff(record->eye_angles.y, vel_angle);
	//	//next_resolving_method = r_log->last_hurt_resolved;
	//	//next_resolving_type = 4;
	//	if (abs(delta) > 60.f) {
	//		auto vel_angle_resolver = (delta > 0 ? 2 : 1);
	//
	//		auto local_vel_ang = ToDegrees(atan2(ctx.m_local()->m_vecVelocity().y, ctx.m_local()->m_vecVelocity().x));
	//
	//		auto ldelta = Math::AngleDiff(local_vel_ang, vel_angle);
	//
	//		if (abs(ldelta) > 120)
	//			vel_angle_resolver = (delta > 0 ? 1 : 2);
	//
	//		r_log->next_resolving_index.emplace_back(vel_angle_resolver, 11);
	//	}
	//}

	int shots = ctx.shots_fired[m_player->entindex() - 1];

	if (shots > 0)
	{
		auto shots_brute = ((shots - 1) % 3);

		auto first_shot_resolver = r_log->last_resolving_method;

		if (record->shot_this_tick) {
			if (r_log->missed_shots[R_SHOT] > 0) {
				shots_brute = (r_log->missed_shots[R_SHOT]-1) % 3;
				first_shot_resolver = r_log->missed_side1st[R_SHOT];
			}
			else
				shots_brute = -999;
		}
		else {
			/*if (record->resolver_delta > 41.f) {
				shots_brute = r_log->missed_shots[R_60_DELTA] % 3;
			}
			else {
				shots_brute = r_log->missed_shots[R_40_DELTA] % 3;
			}*/
			if (r_log->missed_shots[R_USUAL] > 0) {
				shots_brute = (r_log->missed_shots[R_USUAL]-1) % 3;
				first_shot_resolver = r_log->missed_side1st[R_USUAL];
			}
			else
				shots_brute = -999;
		}

		if (shots_brute >= 0) {

			if (shots_brute == 0)
				next_resolving_method = (first_shot_resolver != 1 ? 1 : 2);
			else if (shots_brute == 1)
				next_resolving_method = (first_shot_resolver != 2 ? 2 : 1);
			else if (shots_brute == 2)
				next_resolving_method = 0;

			r_log->next_resolving_index.emplace_back(next_resolving_method, 6);
			r_log->brute_side = next_resolving_method;
		}
		else
			shots = 0;
	}

	auto dd = log->records_count <= 1 ? 0.f : abs(Math::angle_diff(record->eye_angles.y, (log->tick_records[(record->record_index - 1) % 63]).eye_angles.y));

	for (auto it = r_log->next_resolving_index.rbegin(); it != r_log->next_resolving_index.rend(); ++it)
	{
		auto& res = *it;

		const auto this_side_abs_yaw = Math::normalize_angle(it->index == 1 ? record->left_side : (it->index == 0 ? record->original_abs_yaw : record->right_side));
		//if (std::find(r_log->brute_banned_sides.begin(), r_log->brute_banned_sides.end(), res.index) != r_log->brute_banned_sides.end())
		//	continue;
		/*if (dd > record->desync_delta && log->records_count > 2 && res.index == 0)
			continue;*/

		if (shots <= 0) {
			if (res.type == 6)
				continue;
		}
		else
		{
			if (res.type != 6)
				continue;
		}

		if (res.type == 5) {
			if ((csgo.m_globals()->realtime - r_log->last_time_hurt) <= 2.f) {
				next_resolving_method = res.index;
				next_resolving_type = res.type;
				break;
			}
			else if ((csgo.m_globals()->realtime - r_log->last_time_hurt) > 10.f)
				continue;
		}

		//{
			/*if (r_log->brute_banned_sides.size() > 0)
			{
				if (res.type > next_resolving_type)
				{
					next_resolving_method = res.index;
					next_resolving_type = res.type;
				}
			}
			else
			{*/
			//if (res.type == 3 && !r_log->missed_in_anim_resolver/* && record->velocity.Length2D() > 2*/)
			//{
			//	/*const auto curr_direction = std::remainderf(RAD2DEG(atan2(record->velocity.y, record->velocity.x)), 360.f);
			//	const auto newest_direction = std::remainderf(RAD2DEG(atan2(r_log->new_velocity.y, r_log->new_velocity.x)), 360.f);*/

			//	//if (fabs(Math::AngleDiff(curr_direction, newest_direction)) < 30.f) {
			//		next_resolving_method = res.index;
			//		next_resolving_type = res.type;
			//		break;
			//	//}
			//}
			//}
		//}
		//else {
			//if (r_log->last_shot_missed && r_log->last_resolved_side == res.index && (res.index != 0 || r_log->last_resolving_method == 0))
			//	continue;

			//if (res.index != 0 && abs(Math::AngleDiff(this_side_abs_yaw, Math::normalize_angle(record->original_abs_yaw))) < 1.f)
			//	continue;
		//}

		if (res.type > next_resolving_type)
		{
			next_resolving_method = res.index;
			next_resolving_type = res.type;
		}
	}

	if ((next_resolving_type == 0 || r_log->next_resolving_index.empty()) && shots < 1)
	{
		// player is using pitch down. let it be our safety check for now.
		if (fabs(record->eye_angles.x) > 45.f)
		{
			if (next_resolving_type == 0)
			{
				if (r_log->anims_pre_resolving >= 0 && csgo.m_globals()->realtime - r_log->anim_time < 15.f)
					next_resolving_method = r_log->anims_pre_resolving;
			}

			record->resolver_index = next_resolving_method;
			record->resolver_type = next_resolving_type;

			return next_resolving_type >= 1 /*|| record->not_desyncing*/ || log->saved_info.fakeplayer || record->shot_this_tick || record->desync_delta < 40.f;
		}
	}

	record->resolver_index = next_resolving_method;
	record->resolver_type = next_resolving_type;

	return true;
}

void c_resolver::approve_shots(const ClientFrameStage_t& stage)
{
	if (stage != FRAME_NET_UPDATE_START)
		return;

	for (auto& shot : unapproved_shots)
	{
		if (!shot.record.data_filled)
		{
			if (shot.hurt)
			{
				const auto player = csgo.m_entity_list()->GetClientEntity(shot.hitinfo.victim);
				if (player)
				{
					//auto origin = player->m_vecOrigin();
					//origin.z = Vector(shot.shotpos + direction * shot.shotpos.Distance(origin)).z;
					//auto hitpos = shot.shotpos + direction * shot.shotpos.Distance(origin);

					//trace_t tr;
					//Ray_t ray;
					//ray.Init(shot.shotpos, shot.hitpos);
					//csgo.m_engine_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr);

					//if (tr.m_pEnt == player)
					//	hitpos = tr.endpos;

					////hitmarker::get().add_hit(hitmarker_t(csgo.m_globals()->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, hitpos));

					//damage_indicators.emplace_back(shot.shotpos, hitpos, shot.hitinfo.damage, shot.hitinfo.hitgroup == HITGROUP_HEAD);

					//if (vars.visuals.beams.local.enabled.get<bool>() && vars.visuals.beams.enabled.get<bool>())
					//	beams::get().add_local_beam(impact_info_t(csgo.m_globals()->curtime, shot.shotpos, hitpos, g_pLocalPlayer(), get_col(vars.visuals.beams.local.color.get<uintptr_t>())));

					continue;
				}

			}

			//if (vars.visuals.beams.local.enabled.get<bool>() && vars.visuals.beams.enabled.get<bool>())
			//	beams::get().add_local_beam(impact_info_t(csgo.m_globals()->curtime, shot.shotpos, shot.hitpos, g_pLocalPlayer(), get_col(vars.visuals.beams.local.color.get<uintptr_t>())));

			continue;
		}

		//const auto angles = Math::CalcAngle(shot.shotpos, shot.hitpos);
		//Vector direction;
		//Math::AngleVectors(angles, &direction);
		//direction.Normalize();

		//const auto hitpos = shot.hitpos;

		//shot.hitpos = shot.shotpos + direction * shot.shotpos.Distance(shot.record.origin) * 1.1f;

		auto player = csgo.m_entity_list()->GetClientEntity(shot.enemy_index);
		if (!player || player->IsDormant() || player->IsDead())
		{
			//if (shot.hurt)
			//{
				//hitmarker::get().add_hit(hitmarker_t(csgo.m_globals()->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.shotpos + direction * shot.shotpos.Dist(shot.record.m_origin)));

				//if (vars.visuals.beams.local.enabled.get<bool>() && vars.visuals.beams.enabled.get<bool>() && !beams::get().beam_exists(g_pLocalPlayer(), csgo.m_globals()->curtime))
				//	beams::get().add_local_beam(impact_info_t(csgo.m_globals()->curtime, shot.shotpos, shot.shotpos + direction * shot.shotpos.Dist(shot.record.m_origin), g_pLocalPlayer(), get_col(vars.visuals.beams.local.color.get<uintptr_t>())));
			//}
			//else if (vars.visuals.beams.local.enabled.get<bool>() && vars.visuals.beams.enabled.get<bool>())
			//	beams::get().add_local_beam(impact_info_t(csgo.m_globals()->curtime, shot.shotpos, hitpos, g_pLocalPlayer(), get_col(vars.visuals.beams.local.color.get<uintptr_t>())));

			continue;
		}

		const auto r_log = &player_records[shot.enemy_index - 1];
		const auto log = &feature::lagcomp->records[shot.enemy_index - 1];

		shot.backup.store(player, true);
		shot.record.apply(player, false);

		bool didhit[4];
		//bool didawall[4] = {};
		auto did_intersect_anything = false;

		bool occulusion_miss = false;

		const auto impact_dist = (shot.hitpos - shot.shotpos).Length();
		const auto hitbox_dist = (shot.shotpoint - shot.shotpos).Length();
		const auto origin_dist = (shot.record.origin - shot.shotpos).Length();

		QAngle shot_angle = Math::CalcAngle(shot.shotpos, shot.shotpoint);
		QAngle impact_angle = Math::CalcAngle(shot.shotpos, shot.hitpos);
		
		auto shot_impact_delta = int(fabs(Math::angle_diff(shot_angle.y, impact_angle.y)) + fabs(Math::angle_diff(shot_angle.x, impact_angle.x)));/*fabs((shot_angle.x + shot_angle.y) - (impact_angle.x + impact_angle.y))*/;

		auto forward = shot.hitpos - shot.shotpos;
		forward.Normalize();

		auto lol = shot.shotpos + (forward * 8192.f);

		for (auto i = 0; i < HITBOX_MAX; i++)
		{
			C_Hitbox box; feature::ragebot->get_hitbox_data(&box, player, i, shot.record.matrixes);
			C_Hitbox box1; feature::ragebot->get_hitbox_data(&box1, player, i, shot.record.leftmatrixes);
			C_Hitbox box2; feature::ragebot->get_hitbox_data(&box2, player, i, shot.record.rightmatrixes);

			if (box.isOBB)
			{
				if (shot.record.resolver_index == 0 || log->saved_info.fakeplayer) {
					Vector delta;
					Math::VectorIRotate((forward * 8192.f), shot.record.matrixes[box.bone], delta);
					didhit[0] = didhit[1] = Math::IntersectBB(box.start_scaled, delta, box.mins, box.maxs);

					Math::VectorIRotate((forward * 8192.f), shot.record.leftmatrixes[box1.bone], delta);
					didhit[2] = Math::IntersectBB(box1.start_scaled, delta, box1.mins, box1.maxs);

					Math::VectorIRotate((forward * 8192.f), shot.record.rightmatrixes[box2.bone], delta);
					didhit[3] = Math::IntersectBB(box2.start_scaled, delta, box2.mins, box2.maxs);
				}
				else if (shot.record.resolver_index == 1) {

					Vector delta;
					Math::VectorIRotate((forward * 8192.f), shot.record.matrixes[box.bone], delta);
					didhit[1] = Math::IntersectBB(box.start_scaled, delta, box.mins, box.maxs);

					Math::VectorIRotate((forward * 8192.f), shot.record.leftmatrixes[box1.bone], delta);
					didhit[0] = didhit[2] = Math::IntersectBB(box1.start_scaled, delta, box1.mins, box1.maxs);

					Math::VectorIRotate((forward * 8192.f), shot.record.rightmatrixes[box2.bone], delta);
					didhit[3] = Math::IntersectBB(box2.start_scaled, delta, box2.mins, box2.maxs);
				}
				else {

					Vector delta;
					Math::VectorIRotate((forward * 8192.f), shot.record.matrixes[box.bone], delta);
					didhit[1] = Math::IntersectBB(box.start_scaled, delta, box.mins, box.maxs);

					Math::VectorIRotate((forward * 8192.f), shot.record.leftmatrixes[box1.bone], delta);
					didhit[2] = Math::IntersectBB(box1.start_scaled, delta, box1.mins, box1.maxs);

					Math::VectorIRotate((forward * 8192.f), shot.record.rightmatrixes[box2.bone], delta);
					didhit[0] = didhit[3] = Math::IntersectBB(box2.start_scaled, delta, box2.mins, box2.maxs);
				}
			}
			else {
				if (shot.record.resolver_index == 0 || log->saved_info.fakeplayer) {
					didhit[0] = didhit[1] = Math::Intersect(shot.shotpos, lol, box.mins, box.maxs, box.radius);
					didhit[2] = Math::Intersect(shot.shotpos, lol, box1.mins, box1.maxs, box1.radius);
					didhit[3] = Math::Intersect(shot.shotpos, lol, box2.mins, box2.maxs, box2.radius);
				}
				else if (shot.record.resolver_index == 1) {
					didhit[1] = Math::Intersect(shot.shotpos, lol, box.mins, box.maxs, box.radius);
					didhit[0] = didhit[2] = Math::Intersect(shot.shotpos, lol, box1.mins, box1.maxs, box1.radius);
					didhit[3] = Math::Intersect(shot.shotpos, lol, box2.mins, box2.maxs, box2.radius);
				}
				else {
					didhit[1] = Math::Intersect(shot.shotpos, lol, box.mins, box.maxs, box.radius);
					didhit[2] = Math::Intersect(shot.shotpos, lol, box1.mins, box1.maxs, box1.radius);
					didhit[0] = didhit[3] = Math::Intersect(shot.shotpos, shot.hitpos, box2.mins, box2.maxs, box2.radius);
				}
			}

			if (r_log->brute_banned_sides.size() > 2)
				r_log->brute_banned_sides.clear();

			if (feature::ragebot->safe_point(player, shot.shotpos, lol, i, &shot.record) > 2)
				shot.shotinfo.safe_point = true;

			if (didhit[0])
				did_intersect_anything = true;
			//else {
				/*if (!shot.shotinfo.safe_point && log->records_count > 3 && shot.record.entity_flags & FL_ONGROUND)
				{
					for (auto i = 1; i < 4; i++) {
						if (shot.hurt && shot.hitinfo.hitgroup != shot.shotinfo.hitgroup && shot.hitinfo.hitgroup == box.hitgroup ? !didhit[i] : didhit[i]) {

							if (r_log->brute_banned_sides.size() > 0 && std::find(r_log->brute_banned_sides.begin(), r_log->brute_banned_sides.end(), i - 1) != r_log->brute_banned_sides.end())
								continue;

							r_log->brute_banned_sides.emplace_back(i - 1);
							_events.emplace_back(std::string("side was " + std::string(shot.hurt ? "prefered" : "banned") + " | R" + std::to_string(i - 1)));
						}
					}
				}*/
			//}

			//if (!shot.hurt)
			//{
			//	memcpy(player->m_CachedBoneData().Base(), shot.record.matrixes, min(128, player->m_bone_count()) * sizeof(matrix3x4_t));

			//	ctx.force_low_quality_autowalling = true;
			//	auto dmg = feature::autowall->CanHit(shot.shotpos, shot.hitpos, ctx.m_local(), player, i);
			//	ctx.force_low_quality_autowalling = false;

			//	//if (dmg >= 1.f && cant_awall) // we did find hitbox that we hit.	
			//	//	cant_awall = false;

			//	memcpy(player->m_CachedBoneData().Base(), shot.record.leftmatrixes, min(128, player->m_bone_count()) * sizeof(matrix3x4_t));

			//	ctx.force_low_quality_autowalling = true;
			//	auto ldmg = feature::autowall->CanHit(shot.shotpos, shot.hitpos, ctx.m_local(), player, i);
			//	ctx.force_low_quality_autowalling = false;

			//	memcpy(player->m_CachedBoneData().Base(), shot.record.rightmatrixes, min(128, player->m_bone_count()) * sizeof(matrix3x4_t));

			//	ctx.force_low_quality_autowalling = true;
			//	auto rdmg = feature::autowall->CanHit(shot.shotpos, shot.hitpos, ctx.m_local(), player, i);
			//	ctx.force_low_quality_autowalling = false;

			//	//if (!didhit[0] && occulusion_miss || didhit[0]) {
			//	if (ldmg >= 1.f && didhit[2]) {// we did find hitbox that we hit.	
			//		r_log->brute_banned_sides[1] = true;

			//		//if (shot.record.resolver_index == 1)
			//		//	cant_awall = false;
			//	}

			//	if (rdmg >= 1.f && didhit[3]) {// we did find hitbox that we hit.	
			//		r_log->brute_banned_sides[2] = true;

			//		//if (shot.record.resolver_index == 2)
			//		//	cant_awall = false;
			//	}

			//	if (dmg >= 1.f && didhit[1]) {// we did find hitbox that we hit.	
			//		r_log->brute_banned_sides[0] = true;

			//		//if (shot.record.resolver_index == 0)
			//		//	cant_awall = false;
			//	}
			//	//}
			//}
		}

		auto occulusion = (fmaxf(origin_dist, hitbox_dist) - 41.f) > impact_dist && int(fabs(hitbox_dist - impact_dist)) > 41;

		if (!occulusion)
			shot.hit = did_intersect_anything;
		else
			shot.hit = false;

		//if (shot_impact_delta > 0) {
		//	//if (shot.hit)
		//		shot.hit = did_intersect_anything && (hitbox_dist <= impact_dist);
		//}
		//else {
		//	shot.hit = (hitbox_dist - 10.f) <= impact_dist || std::fabs<int>(hitbox_dist - impact_dist) < 32;
		//}

		if (shot.hurt)
			shot.hit = true;

		/*csgo.m_debug_overlay()->AddBoxOverlay(shot.shotpoint, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 255, 10.f);
		csgo.m_debug_overlay()->AddBoxOverlay(shot.record.origin, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 0, 255, 255, 255, 10.f);
		csgo.m_debug_overlay()->AddBoxOverlay(shot.hitpos, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 0, 0, 255, 255, 10.f);*/

		//if (vars.visuals.beams.local.enabled.get<bool>() && vars.visuals.beams.enabled.get<bool>())
		//	beams::get().add_local_beam(impact_info_t(csgo.m_globals()->curtime, shot.shotpos, shot.hit ? shot.hitpos : hitpos, g_pLocalPlayer(), get_col(vars.visuals.beams.local.color.get<uintptr_t>())));

		//if (shot.hurt)
		//	damage_indicators.emplace_back(shot.shotpos, /*shot.hit ? */Vector(shot.record.origin.x, shot.record.origin.y, (shot.hit ? shot.hitpos.z : hitpos.z))/* : hitpos*/, shot.hitinfo.damage, shot.hitinfo.hitgroup == HITGROUP_HEAD);

		//	hitmarker::get().add_hit(hitmarker_t(csgo.m_globals()->curtime, shot.hitinfo.victim, shot.hitinfo.damage, shot.hitinfo.hitgroup, shot.hit ? shot.hitpos : shot.shotpos + direction * shot.shotpos.Dist(shot.record.m_origin)));

		//__(weapon_accuracy_nospread, "weapon_accuracy_nospread");
		static auto nospread = csgo.m_engine_cvars()->FindVar(sxor("weapon_accuracy_nospread"));

		// assuming that bot miss is 100% not aimfix/rotation issue.
		std::string missed = std::string(/*(*/shot.hit || nospread->GetBool()/*) && !player->IsBot()*/ ? sxor("missed shot due to resolver") : sxor("missed shot due to spread"));

		if (!shot.hurt) {
			if (ctx.m_settings.misc_notifications[2])
				_events.emplace_back(missed);

			if (shot.hit) {
				if (shot.record.resolver_type == 9)
					r_log->missed_in_anim_resolver = true;
				else if (shot.record.resolver_type == 6)
					r_log->missed_in_hitpos_resolver = true;
			}
		}

		if (!log->saved_info.fakeplayer)
			collect_and_correct_info(&shot);

		shot.backup.apply(player, true);
	}

	current_shots.clear();
	unapproved_shots.clear();
}

bool c_resolver::hurt_resolver(shot_t* shot)
{
	auto record = &shot->record;
	auto player = csgo.m_entity_list()->GetClientEntity(shot->enemy_index);

	if (record == nullptr || player == nullptr || !record->data_filled || record->dormant || ctx.m_local() == nullptr || ctx.m_local()->IsDead() || player->IsDead() || player->IsDormant())
		return false;

	auto start = shot->shotpos;
	auto end = shot->hitpos;

	if (!shot->hurt || !shot->hit) //lol
		return false;

	/*for (auto imp : record->_impacts)
	{
		if (point_in_bbox(imp, record->_tickrecord.origin, record->_tickrecord.object_mins, record->_tickrecord.object_maxs)) {
			end = imp;
			break;
		}
	}*/

	//const auto angle_impact = Math::CalcAngle(start, end);

	C_Tickrecord backup = C_Tickrecord(player, true);
	record->apply(player, false, true);

	//CTraceFilter _filter;
	//_filter.pSkip = ctx.m_local();
	trace_t tr;
	Ray_t ray;

	Vector forward = (end - start).Normalized();
	//Math::AngleVectors(angle_impact, &forward);
	auto endpos = start + forward * 8196.f;

	ray.Init(start, endpos);

	bool failed = false;

	memcpy(player->m_CachedBoneData().Base(), record->leftmatrixes, record->bones_count * sizeof(matrix3x4_t));
	csgo.m_engine_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr);

	const auto left_tr_hg = tr.hitgroup;
	const auto left_tr_endpos = tr.endpos;

	if (tr.m_pEnt != player)
		failed = true;

	memcpy(player->m_CachedBoneData().Base(), record->rightmatrixes, record->bones_count * sizeof(matrix3x4_t));
	csgo.m_engine_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr);

	const auto right_tr_hg = tr.hitgroup;
	const auto right_tr_endpos = tr.endpos;

	if (tr.m_pEnt != player)
		failed = true;

	memcpy(player->m_CachedBoneData().Base(), record->matrixes, record->bones_count * sizeof(matrix3x4_t));
	csgo.m_engine_trace()->ClipRayToEntity(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, player, &tr);

	const auto zero_tr_hg = tr.hitgroup;
	const auto zero_tr_endpos = tr.endpos;

	if (tr.m_pEnt != player)
		failed = true;

	backup.apply(player, true);

	if (failed)
		return false;

	auto r_log = &player_records[player->entindex() - 1];
	const auto prev_resolving = record->resolver_index;

	// safety check if both sides are same hitgroup we should not even try to do any kind of that bs
	if ((left_tr_hg == right_tr_hg || left_tr_hg == zero_tr_hg) && left_tr_hg == shot->hitinfo.hitgroup)
	{
		//const auto zero_diff	= abs((zero_tr_endpos - end).LengthSquared());
		//const auto right_diff	= abs((right_tr_endpos - end).LengthSquared());
		//const auto left_diff = abs((left_tr_endpos - end).LengthSquared());

		//if (right_diff != zero_diff && left_diff != zero_diff && right_diff != left_diff)
		//{
		//	//ctx.shots_fired[player->entindex() - 1] = 0;

		//	if (min(right_diff, left_diff) < zero_diff)
		//	{
		//		if (right_diff < left_diff)
		//			r_log->last_hitpos_side = 2;
		//		else
		//			r_log->last_hitpos_side = 1;
		//	}
		//	else
		//		r_log->last_hitpos_side = 0;

		//	r_log->last_hitbox_correction_was_onshot = record->shot_this_tick;
		//	_events.push_back(_event(std::string("resolving method changed due to hitbox missmatch [i]:[" + std::to_string(prev_resolving) + "]:[" + std::to_string(r_log->last_hitpos_side) + "]")));
		//	return true;
		//}
		//else
		//{
			//_events.push_back(_event("none impacts are right."));
		return false;
	}

		//_events.push_back(_event(std::string("resolving method changed due to hitbox missmatch [i]:[" + std::to_string(prev_resolving) + "]:[" + std::to_string(r_log->last_hitpos_side) + "]")));

		//return true;
	//}

	if (right_tr_hg == shot->hitinfo.hitgroup)
		r_log->last_hitpos_side = 2; //- 60.f
	else if (left_tr_hg == shot->hitinfo.hitgroup)
		r_log->last_hitpos_side = 1; //+ 60.f
	else
		r_log->last_hitpos_side = 0; //+ 0.f

	r_log->last_hitbox_correction_was_onshot = record->shot_this_tick;

	//_events.push_back(_event(std::string("resolving method changed due to hitbox missmatch [" + std::to_string(prev_resolving) + "]:[" + std::to_string(r_log->last_hitpos_side) + "]")));

	return true;
}

void c_resolver::collect_and_correct_info(shot_t* shot)
{
	const auto r_log = &player_records[shot->enemy_index - 1];
	const auto log = &feature::lagcomp->records[shot->enemy_index - 1];
	auto &shots = ctx.shots_fired[shot->enemy_index - 1];

	if (shot->hurt)
	{
		/*if (log->m_nShots > 0)
			log->m_nShots--;
		if (shot->record.m_lby_flick && log->m_nShotsLby > 0)
			log->m_nShotsLby--;*/

		shots--;
		if (shot->record.shot_this_tick) {
			if (r_log->missed_shots[R_SHOT] > 0) {
				r_log->missed_shots[R_SHOT]--;
			}
		}
		else {
			if (shot->record.resolver_delta_multiplier > 0.7f) {
				if (r_log->missed_shots[R_60_DELTA] > 0)
					r_log->missed_shots[R_60_DELTA]--;
			}
			else {
				if (r_log->missed_shots[R_40_DELTA] > 0)
					r_log->missed_shots[R_40_DELTA]--;
			}

			if (r_log->missed_shots[R_USUAL] > 0)
				r_log->missed_shots[R_USUAL]--;
		}

		//_events.push_back({ "corrected shot count " + std::to_string(shots) });

		if (shot->hitinfo.hitgroup == shot->shotinfo.hitgroup/* && abs(shot->hitinfo.damage - shot->shotinfo.damage) < 10.f*/) {
			r_log->last_hurt_resolved = shot->record.resolver_index;
			r_log->last_time_hurt = csgo.m_globals()->realtime;
		}
		else {
			if (/*shot->shotinfo.spread < 0.0015f && */hurt_resolver(shot))
				r_log->last_hitpos_side_correction = csgo.m_globals()->realtime;
		}

		if (shot->shotinfo.hitgroup >= 0 && shot->shotinfo.hitgroup <= 8 && shot->shotinfo.spread <= 0.01f && shot->hitinfo.hitgroup != shot->shotinfo.hitgroup) {
			if (shot->record.shot_this_tick)
				r_log->missed_shots[R_SHOT]++;
			else {
				/*if (shot->record.resolver_delta > 41.f)
					r_log->missed_shots[R_60_DELTA]++;
				else
					r_log->missed_shots[R_40_DELTA]++;*/

				r_log->missed_shots[R_USUAL]++;
			}
			shots++;
		}

		
		if (r_log->shots_missed > 0)
			--r_log->shots_missed;

		return;
	}
	else {
		if (shot->hit) {
			//++shots;
			++r_log->shots_missed;

			r_log->last_shot_missed = true;
			//r_log->brute_banned_sides[std::clamp(shot->record.resolver_index, 0, 2)] = true;
		}
		else
		{
			if (shots > 0) {
				shots--;
			}

			if (shot->record.shot_this_tick) {
				if (r_log->missed_shots[R_SHOT] > 0)
					r_log->missed_shots[R_SHOT]--;
			}
			else {
				if (shot->record.resolver_delta_multiplier > 0.7f) {
					if (r_log->missed_shots[R_60_DELTA] > 0)
					r_log->missed_shots[R_60_DELTA]--;
				}
				else {
					if (r_log->missed_shots[R_40_DELTA] > 0)
					r_log->missed_shots[R_40_DELTA]--;
				}

				if (r_log->missed_shots[R_USUAL] > 0)
				r_log->missed_shots[R_USUAL]--;
			}

			if (r_log->shots_missed > 0)
				--r_log->shots_missed;
		}
	}
}