#include "rage_aimbot.hpp"
#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "math.hpp"
#include "displacement.hpp"
#include "lag_comp.hpp"
#include "anti_aimbot.hpp"
#include "resolver.hpp"
#include "visuals.hpp"
#include "menu.hpp"
#include "movement.hpp"
#include "prediction.hpp"
#include "autowall.hpp"
#include "misc.hpp"
#include "usercmd.hpp"
//#include "thread/threading.h"
//#include "thread/shared_mutex.h"

//#include <thread>

static constexpr auto total_seeds = 128;
static constexpr auto autowall_traces = 48;

//std::vector<TargetListing_t> m_entities;

void c_aimbot::get_hitbox_data(C_Hitbox* rtn, C_BasePlayer* ent, int ihitbox, matrix3x4_t* matrix)
{
	if (ihitbox < 0 || ihitbox > 19) return;

	if (!ent) return;

	const model_t* const model = ent->GetModel();

	if (!model)
		return;

	studiohdr_t* const pStudioHdr = csgo.m_model_info()->GetStudioModel(model);

	if (!pStudioHdr)
		return;

	mstudiobbox_t* const hitbox = pStudioHdr->pHitbox(ihitbox, ent->m_nHitboxSet());

	if (!hitbox)
		return;

	const auto is_capsule = hitbox->radius != -1.f;

	Vector min, max;
	if (is_capsule) {
		//QAngle angle;
		//Math::AngleMatrix(hitbox->rotation, matrix[hitbox->bone]);
		Math::VectorTransform(hitbox->bbmin, matrix[hitbox->bone], min);
		//Math::AngleMatrix(hitbox->rotation, matrix[hitbox->bone]);
		Math::VectorTransform(hitbox->bbmax, matrix[hitbox->bone], max);
	}
	else
	{
		min = Math::VectorRotate(hitbox->bbmin, hitbox->rotation);
		max = Math::VectorRotate(hitbox->bbmax, hitbox->rotation);
		//Math::MatrixAngles(matrix[hitbox->bone], hitbox->rotation);
		Math::VectorTransform(min, matrix[hitbox->bone], min);
		Math::VectorTransform(max, matrix[hitbox->bone], max);
	}

	rtn->hitboxID = ihitbox;
	rtn->isOBB = !is_capsule;
	rtn->radius = hitbox->radius;
	rtn->mins = min;
	rtn->maxs = max;
	rtn->hitgroup = hitbox->group;
	rtn->hitbox = hitbox;
	Math::VectorITransform(ctx.m_eye_position, matrix[hitbox->bone], rtn->start_scaled);
	rtn->bone = hitbox->bone;
}

//bool c_aimbot::safe_static_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx)
//{
//	auto resolve_info = &feature::resolver->player_records[entity->entindex() - 1];
//	auto log = &feature::lagcomp->records[entity->entindex() - 1];
//
//	if (!log->best_record->shot_this_tick)
//		return true;
//
//	const auto is_colliding = [entity, hitboxIdx](Vector start, Vector end, C_Hitbox box) -> bool
//	{
//		//cheat::features::lagcomp.apply_record_data(entity, rec);
//		const auto is_intersecting = box.isOBB ? Math::IntersectBB(start, end, box.mins, box.maxs) : Math::Intersect(start, end, box.mins, box.maxs, box.radius);
//		//Source::m_pEngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, entity, &tr);
//		//const auto walldamage = cheat::features::autowall.CanHit(start, end, cheat::main::local(), entity, hitboxIdx);
//		//cheat::features::lagcomp.apply_record_data(entity, orgtc);
//
//		return is_intersecting;
//	};
//
//	/*Vector angles, direction;
//	auto v6 = aim_point - eye_pos;
//	Math::VectorAngles(v6, angles);
//	Math::AngleVectors(angles, &direction);
//	direction.Normalize();
//	const auto end_point = direction * 8092.f + eye_pos;*/
//	const auto angle = Math::CalcAngle(eye_pos, aim_point);
//	Vector forward;
//	Math::AngleVectors(angle, &forward);
//	auto end = eye_pos + forward * 8092.f;
//
//	//Source::m_pDebugOverlay->AddLineOverlay(eye_pos, end, 255, 0, 0, false, Source::m_pGlobalVars->interval_per_tick * 2.f);
//
//	C_Hitbox box1; get_hitbox_data(&box1, entity, hitboxIdx, log->best_record->leftlmatrixes);
//	C_Hitbox box2; get_hitbox_data(&box2, entity, hitboxIdx, log->best_record->rightlmatrixes);
//
//	if (box1.isOBB || box2.isOBB)
//		return true;
//
//	//C_Tickrecord rec;
//	//cheat::features::lagcomp.store_record_data(entity, &rec);
//
//	if (is_colliding(eye_pos, end, box2))
//		return true;
//	if (is_colliding(eye_pos, end, box1))//is_colliding(eye_pos, end_point, &resolve_info->leftlrec, &rec))
//		return true;
//
//	return false;
//}

Vector c_aimbot::get_hitbox(C_BasePlayer* ent, int ihitbox, matrix3x4_t mat[])
{
	if (ihitbox < 0 || ihitbox > 19) return Vector::Zero;

	if (!ent) return Vector::Zero;

	if (!ent->GetClientRenderable())
		return Vector::Zero;

	const model_t* const model = ent->GetModel();

	if (!model)
		return Vector::Zero;

	studiohdr_t* const pStudioHdr = csgo.m_model_info()->GetStudioModel(model);

	if (!pStudioHdr)
		return Vector::Zero;

	mstudiobbox_t* const hitbox = pStudioHdr->pHitbox(ihitbox, ent->m_nHitboxSet());

	if (!hitbox)
		return Vector::Zero;

	if (hitbox->bone > 128 || hitbox->bone < 0)
		return Vector::Zero;

	Vector min, max;
	Vector top_point;
	constexpr float rotation = 0.70710678f;
	Math::VectorTransform(hitbox->bbmin, mat[hitbox->bone], min);
	Math::VectorTransform(hitbox->bbmin, mat[hitbox->bone], max);
	//Math::VectorTransform((hitbox->bbmax + hitbox->bbmin) * 0.5f, mat[hitbox->bone], top_point);

	auto center = (min + max) / 2.f;

	return center;
}

//float c_aimbot::can_hit(int hitbox, C_BasePlayer* Entity, Vector position, matrix3x4_t mx[], bool check_center, bool predict)
//{
//	static auto is_visible = [](C_BasePlayer* thisptr, Vector& Start, Vector& End) -> bool
//	{
//		if (!thisptr) return NULL;
//
//		CGameTrace tr;
//		Ray_t ray;
//		static CTraceFilter traceFilter;
//		traceFilter.pSkip = ctx.m_local();
//
//		ray.Init(Start, End);
//
//		Source::m_pEngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &traceFilter, &tr);
//		cheat::features::autowall.FixTraceRay(Start, End, &tr, thisptr);
//
//		return (tr.m_pEnt == thisptr || tr.fraction > 0.99f);
//	};
//
//	auto local_weapon = (C_WeaponCSBaseGun*)(csgo.m_entity_list()->GetClientEntityFromHandle(ctx.m_local()->m_hActiveWeapon()));
//
//	if (!local_weapon->GetCSWeaponData())
//		return 0;
//
//	auto eyepos = ctx.m_eye_position;
//
//	auto cdmg = cheat::features::autowall.CanHit(eyepos, position, ctx.m_local(), Entity, hitbox);
//
//	auto nob = (Entity->m_iHealth() < cheat::Cvars.RageBot_MinDamage.GetValue() && cheat::Cvars.RageBot_ScaledmgOnHp.GetValue() ? Entity->m_iHealth() : ((cheat::Cvars.RageBot_MinDmgKey.GetValue() && cheat::game::pressed_keys[(int)cheat::Cvars.RageBot_MinDmgKey.GetValue()]) ? cheat::Cvars.RageBot_MinDamage.GetValue() : cheat::Cvars.RageBot_MinDmgKey_val.GetValue()));
//
//	if (check_center) {
//		if (cdmg >= nob)
//			return cdmg;
//
//		return 0.f;
//	}
//
//	static std::vector<Vector> points;
//
//	if (Entity->get_multipoints(hitbox, points, mx) && !points.empty())
//	{
//		cheat::main::points[Entity->entindex() - 1][hitbox] = points;
//
//		//if (cdmg >= nob)
//		//	return cdmg;
//
//		if (hitbox != 11 && hitbox != 12)
//		{
//
//			//auto dmg = cheat::features::autowall.CanHit(eyepos, points.at(1), ctx.m_local(), Entity, hitbox);
//			//auto dmg2 = cheat::features::autowall.CanHit(eyepos, points.at(2), ctx.m_local(), Entity, hitbox);
//			//auto dmg3 = cheat::features::autowall.CanHit(eyepos, points.at(3), ctx.m_local(), Entity, hitbox);
//
//			if (hitbox == 0) {
//				auto dmg = cheat::features::autowall.CanHit(eyepos, points.at(1), ctx.m_local(), Entity, hitbox);
//				auto dmg2 = cheat::features::autowall.CanHit(eyepos, points.at(4), ctx.m_local(), Entity, hitbox);
//				auto dmg3 = cheat::features::autowall.CanHit(eyepos, points.at(3), ctx.m_local(), Entity, hitbox);
//
//				if (max(dmg, max(dmg2, dmg3)) >= nob)
//					return dmg;
//			}
//			else
//			{
//				auto dmg = cheat::features::autowall.CanHit(eyepos, points.at(1), ctx.m_local(), Entity, hitbox);
//				auto dmg2 = cheat::features::autowall.CanHit(eyepos, points.at(6), ctx.m_local(), Entity, hitbox);
//				auto dmg3 = cheat::features::autowall.CanHit(eyepos, points.at(4), ctx.m_local(), Entity, hitbox);
//
//				//auto dmg = cheat::features::autowall.CanHit(eyepos, points.at(1), ctx.m_local(), Entity, hitbox);
//				//auto dmg2 = cheat::features::autowall.CanHit(eyepos, points.at(2), ctx.m_local(), Entity, hitbox);
//				//auto dmg3 = cheat::features::autowall.CanHit(eyepos, points.at(3), ctx.m_local(), Entity, hitbox);
//
//				if (max(dmg, max(dmg2, dmg3)) >= nob)
//					return dmg;
//			}
//		}
//		else
//		{
//			auto dmg1 = cheat::features::autowall.CanHit(eyepos, points.at(0), ctx.m_local(), Entity, hitbox);
//			auto dmg2 = cheat::features::autowall.CanHit(eyepos, points.at(1), ctx.m_local(), Entity, hitbox);
//
//			if (max(dmg1, dmg2) >= nob)
//				return max(dmg1, dmg2);
//		}
//	}
//
//	if (cdmg >= nob)
//		return cdmg;
//
//	return 0;
//}

int low_count = 0;
int medium_count = 0;

void c_aimbot::build_seed_table()
{
	//static DWORD get_shotgun_spread = Memory::Scan("client.dll", "55 8B EC 83 EC 10 56 8B 75 08 8D");
	//static auto weapon_accuracy_shotgun_spread_patterns = csgo.m_engine_cvars()->FindVar("weapon_accuracy_shotgun_spread_patterns");

	if (seeds_filled >= total_seeds)
		return;

	seeds_filled = 0;

	for (auto i = 0; i < total_seeds; i++) {

		//if (precomputed_seeds.size() >= total_seeds)
		//	break;
		if (seeds_filled >= 128)
			break;

		RandomSeed(seeds[i]);

		float a = RandomFloat(0.0f, 6.2831855f);
		float c = RandomFloat(0.0f, 1.0f);
		float b = RandomFloat(0.0f, 6.2831855f);

		/*if (c < 0.3f)
			++low_count;
		else if (c < 0.55f && c > 0.3f)
			++medium_count;

		if (low_count > int(total_seeds / 3) && c < 0.3f) {
			if (i % 10 != 0)
				c += RandomFloat(0.15f, Math::clamp(1.f - c, 0.f, 1.f));
			else
				continue;
		}

		if (medium_count > int(total_seeds / 3) && c < 0.55f && c > 0.3f) {
			if (i % 10 != 0)
				c += RandomFloat(0.15f, Math::clamp(1.f - c, 0.f, 1.f));
			else
				continue;
		}*/

		/*int id = m_weapon()->m_iItemDefinitionIndex();
		auto recoil_index = m_weapon()->m_flRecoilIndex();

		if (id == 28 && recoil_index < 3.0f)
		{
			for (int i = 3; i > recoil_index; i--)
				c *= c;

			c = 1.0f - c;
		}

		if (m_weapon()->IsShotgun() && weapon_accuracy_shotgun_spread_patterns->GetInt())
		{
			auto _get_shotgun_spread = reinterpret_cast<void(__stdcall*)(int, int, unsigned int, float*, float*)>(get_shotgun_spread);

			if (_get_shotgun_spread)
				_get_shotgun_spread(m_weapon()->m_iItemDefinitionIndex(), 0, i + m_weapon()->GetCSWeaponData()->max_speed * recoil_index, &c, &a);
		}*/

		precomputed_seeds[seeds_filled++] = std::tuple<float, float, float, float, float>(c,
			sin(a), cos(b), sin(b), cos(a));
	}

	//r8_seeds_filled = 0;

	//for (auto i = 0; i < total_seeds; i++) {

	//	//if (precomputed_seeds.size() >= total_seeds)
	//	//	break;
	//	if (r8_seeds_filled >= 128)
	//		break;

	//	RandomSeed(seeds[i]);

	//	float a = RandomFloat(0.0f, 6.2831855f);
	//	float c = RandomFloat(0.0f, 1.0f);
	//	float b = RandomFloat(0.0f, 6.2831855f);

	//	precomputed_r8_seeds[r8_seeds_filled++] = std::tuple<float, float, float, float, float>(c,
	//		sin(a), cos(b), sin(b), cos(a));
	//}
}

int mini_low_count = 0;

void c_aimbot::build_mini_hc_table()
{
	for (auto i = 0; i < 64; i++) 
	{
		RandomSeed(i);///*seeds[*/(i * 255) - (min(1,i) / 2)/*]*/);

		float a = RandomFloat(0.0f, 6.2831855f);
		float c = RandomFloat(0.0f, 1.0f);
		float b = RandomFloat(0.0f, 6.2831855f);

		/*if (c <= 0.5f) {
			if (mini_low_count >= (total_seeds / 6))
				continue;
			else
				mini_low_count++;
		}*/

		precomputed_mini_seeds[i] = std::tuple<float, float, float, float, float>(c,
			sin(a), cos(b), sin(b), cos(a));
	}
}

bool c_aimbot::mini_hit_chance(Vector vhitbox, C_BasePlayer* ent, int hitbox, int& hc)
{
	//if (precomputed_mini_seeds.size() > 100)// {
	//	precomputed_mini_seeds.resize(99);
	//	return false;
	//}

	build_mini_hc_table();

	if (!ctx.latest_weapon_data)
		return false;
	
	C_Hitbox ht;
	get_hitbox_data(&ht, ent, hitbox, ent->m_CachedBoneData().Base());

	auto traces_hit = 0;
	//auto awalls_hit = 0;

	auto const ang = Math::CalcAngle(ctx.m_eye_position, vhitbox);

	//Vector right, up;
	//Math::AngleVectors(, forward, right, up);

	Vector forward, right, up;
	Math::AngleVectors(ang, &forward, &right, &up); // maybe add an option to not account for punch.

	m_weapon()->UpdateAccuracyPenalty();
	const float weap_inaccuracy = m_weapon()->GetInaccuracy();

	if (int(weap_inaccuracy * 1000.f) == 0) {
		hc = 100;
		return true;
	}

	const auto weap_spread = m_weapon()->GetSpread();

	// performance optimization.
	if ((ctx.m_eye_position - ent->m_vecOrigin()).Length() >= (ctx.latest_weapon_data->range * 1.01f))
		return false;

	// setup calculation parameters.
	//const auto round_acc = [](const float accuracy) { return roundf(accuracy * 1000.f) / 1000.f; };
	//const auto sniper = weapon->m_iItemDefinitionIndex() == WEAPON_AWP || weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1
	//	|| weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || weapon->m_iItemDefinitionIndex() == WEAPON_SSG08;
	//const auto crouched = ctx.m_local()->m_fFlags() & FL_DUCKING;

	//if (weapon->m_iItemDefinitionIndex() == 64)
	//	return weap_inaccuracy < (crouched ? .0020f : .0055f);

	// no need for hitchance, if we can't increase it anyway.
	/*if (crouched)
	{
		if (round_acc(weap_inaccuracy) <= round_acc(sniper ? weapon->GetCSWeaponData()->flInaccuracyCrouchAlt : weapon->GetCSWeaponData()->flInaccuracyCrouch))
			return true;
	}
	else if (ctx.m_local()->m_vecVelocity().Length2D() < 1.f)
	{
		if (round_acc(weap_inaccuracy) <= round_acc(sniper ? weapon->GetCSWeaponData()->flInaccuracyStandAlt : weapon->GetCSWeaponData()->flInaccuracyStand))
			return true;
	}*/

	if (precomputed_mini_seeds.empty())
		return false;

	static std::tuple<float, float, float, float, float>* seed;
	static float c, spread_val, inaccuracy_val;
	static Vector v_spread, dir;
	
	Ray_t ray;

	for (int i = 0; i < 64; i++)
	{
		// get seed.
		seed = (&precomputed_mini_seeds[i]);

		//// calculate spread.
		//inaccuracy = std::get<0>(*seed) * weap_inaccuracy;
		//spread_x = std::get<2>(*seed) * inaccuracy;
		//spread_y = std::get<1>(*seed) * inaccuracy;
		//total_spread = (forward + right * spread_x + up * spread_y);

		//// calculate angle with spread applied.
		//Math::VectorAngles(total_spread, spread_angle);

		//// calculate end point of trace.
		////Math::AngleVectors(spread_angle, &end);
		//Math::AngleVectors(spread_angle, &direction);
		//end = eye_position + direction * 8092.f;

		c = std::get<0>(*seed);

		spread_val = c * weap_spread;
		inaccuracy_val = c * weap_inaccuracy;

		v_spread.Set((std::get<2>(*seed) * spread_val) + (std::get<4>(*seed) * inaccuracy_val), (std::get<3>(*seed) * spread_val) + (std::get<1>(*seed) * inaccuracy_val), 0);

		//dir.x = forward.x + (right.x * v_spread.x) + (up.x * v_spread.y);
		//dir.y = forward.y + (right.y * v_spread.x) + (up.y * v_spread.y);
		//dir.z = forward.z + (right.z * v_spread.x) + (up.z * v_spread.y);

		//Vector spread_view;
		//Math::VectorAngles(dir, spread_view);
		////spread_view.Normalize();
		//Math::AngleVectors(spread_view, &end);

		dir.Set(forward.x + (v_spread.x * right.x) + (v_spread.y * up.x),
		forward.y + (v_spread.x * right.y) + (v_spread.y * up.y),
		forward.z + (v_spread.x * right.z) + (v_spread.y * up.z));
		dir.NormalizeInPlace();
		//proper
		auto const end = ctx.m_eye_position + (dir * 8192.f);

		trace_t tr;
		ray.Init(ctx.m_eye_position, end);

		tr.fraction = 1.0;
		tr.startsolid = false;

		//csgo.m_engine_trace()->ClipRayToEntity(ray, CONTENTS_HITBOX, entity, &tr);
		auto intersect = Math::ClipRayToHitbox(ray, ht.hitbox, ent->m_CachedBoneData().Base()[ht.bone], tr) >= 0;

		//bool intersect;

		//if (ht.isOBB) {
		//	Vector delta;
		//	Math::VectorIRotate((dir * 8192.f), ent->m_CachedBoneData().Base()[ht.bone], delta);

		//	intersect = Math::IntersectBB(ht.start_scaled, delta, ht.mins, ht.maxs);
		//}
		//else
		//{
		//	intersect = Math::Intersect(ctx.m_eye_position, end, ht.mins, ht.maxs, ht.radius);

		//	//if (intersect && i % 2 == 0)
		//	//	intersect = feature::autowall->CanHit(eye_position, end, ctx.m_local(), ent, hitbox) > 1;
		//}

		/*Color color = Color::Red();

		switch (hitbox)
		{
		case 0:
			color = Color::Red();
			break;
		case 1:
			color = Color::Blue();
			break;
		case 2:
			color = Color::Grey();
			break;
		case 3:
			color = Color::Green();
			break;
		case 4:
			color = Color::Purple();
			break;
		case 5:
			color = Color::White();
			break;
		case 6:
			color = Color(255,130, 0);
			break;
		case 7:
			color = Color(100, 255, 0);
			break;
		}

		csgo.m_debug_overlay()->AddLineOverlay(eye_position, end, color.r(), color.g(), color.b(), true, csgo.m_globals()->interval_per_tick * 2.f);*/

		if (intersect)
			++traces_hit;
	}

	hc = /*int(float(float(*/traces_hit * 1.5625f;//) * 1.5625f));

	return true;
}

int c_aimbot::hitbox2hitgroup(C_BasePlayer* m_player, int ihitbox)
{
	if (ihitbox < 0 || ihitbox > 19) return 0;

	if (!m_player) return 0;

	const model_t* const model = m_player->GetModel();

	if (!model)
		return 0;

	studiohdr_t* const pStudioHdr = csgo.m_model_info()->GetStudioModel(model);

	if (!pStudioHdr)
		return 0;

	mstudiobbox_t* const hitbox = pStudioHdr->pHitbox(ihitbox, m_player->m_nHitboxSet());

	if (!hitbox)
		return 0;

	return hitbox->group;

	/*switch (ihitbox)
	{
	case HITBOX_HEAD:
	case HITBOX_NECK:
		return HITGROUP_HEAD;
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
	case HITBOX_THORAX:
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_RIGHT_UPPER_ARM:
		return HITGROUP_CHEST;
	case HITBOX_PELVIS:
	case HITBOX_LEFT_THIGH:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_BODY:
		return HITGROUP_STOMACH;
	case HITBOX_LEFT_CALF:
	case HITBOX_LEFT_FOOT:
		return HITGROUP_LEFTLEG;
	case HITBOX_RIGHT_CALF:
	case HITBOX_RIGHT_FOOT:
		return HITGROUP_RIGHTLEG;
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_HAND:
		return HITGROUP_LEFTARM;
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_RIGHT_HAND:
		return HITGROUP_RIGHTARM;
	default:
		return HITGROUP_STOMACH;
	}*/
}

int c_aimbot::safe_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx, C_Tickrecord* record)
{
	resolver_records* resolve_info = &feature::resolver->player_records[entity->entindex() - 1];
	c_player_records* log = &feature::lagcomp->records[entity->entindex() - 1];

	//auto baimkey_shit = (cheat::Cvars.RageBot_SafePointsBaimKey.GetValue() && (cheat::game::pressed_keys[(int)cheat::Cvars.RageBot_BaimKey.GetValue()] && cheat::Cvars.RageBot_BaimKey.GetValue() > 0.f));

	//auto can_safepoint = log->best_record != nullptr && !log->best_record->shot_this_tick && hitboxIdx < 2;

	//if (!can_safepoint && !(baimkey_shit && cheat::Cvars.RageBot_SafePointsBaimKey.GetValue() != 0.f))
	//	return true;

	if (!record || !record->data_filled || !record->valid)
		return 0;

	const auto is_colliding = [entity, hitboxIdx](Vector start, Vector end, C_Hitbox* hbox_data, matrix3x4_t *mx) -> bool
	{
		/*Ray_t ray;
		trace_t tr;
		ray.Init(start, end);

		tr.fraction = 1.0f;
		tr.startsolid = false;*/
		if (hbox_data->isOBB)
		{ 
			auto dir = end - start;
			dir.NormalizeInPlace();
			Vector delta;
			Math::VectorIRotate((dir * 8192.f), mx[hbox_data->bone], delta);

			return Math::IntersectBB(hbox_data->start_scaled, delta, hbox_data->mins, hbox_data->maxs);
		}
		else
		{
			if (Math::Intersect(ctx.m_eye_position, end, hbox_data->mins, hbox_data->maxs, hbox_data->radius))
				return true;
		}

		return false;
	};

	auto forward = aim_point - eye_pos;
	auto end = eye_pos + (forward * 8192.f);

	C_Hitbox box1; get_hitbox_data(&box1, entity, hitboxIdx, record->leftmatrixes);
	C_Hitbox box2; get_hitbox_data(&box2, entity, hitboxIdx, record->rightmatrixes);
	C_Hitbox box3; get_hitbox_data(&box3, entity, hitboxIdx, record->matrixes);

	int hits = 0;

	if (is_colliding(eye_pos, end, &box1, record->leftmatrixes)) ++hits;

	if (is_colliding(eye_pos, end, &box2, record->rightmatrixes)) ++hits;

	if (is_colliding(eye_pos, end, &box3, record->matrixes)) ++hits;

	return hits;
}

bool c_aimbot::safe_side_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx, C_Tickrecord* record)
{
	resolver_records* resolve_info = &feature::resolver->player_records[entity->entindex() - 1];
	c_player_records* log = &feature::lagcomp->records[entity->entindex() - 1];

	//auto baimkey_shit = (cheat::Cvars.RageBot_SafePointsBaimKey.GetValue() && (cheat::game::pressed_keys[(int)cheat::Cvars.RageBot_BaimKey.GetValue()] && cheat::Cvars.RageBot_BaimKey.GetValue() > 0.f));
	//auto can_safepoint = log->best_record != nullptr && !log->best_record->shot_this_tick && hitboxIdx < 2;

	//if (!can_safepoint && !(baimkey_shit && cheat::Cvars.RageBot_SafePointsBaimKey.GetValue() != 0.f))
	//	return true;

	if (!record || !record->data_filled || !record->valid)
		return false;
	/*const auto is_colliding = [entity, hitboxIdx, maxdamage](Vector start, Vector end, C_Tickrecord *rec, C_Tickrecord* orgtc) -> bool
	{
		Ray_t ray;
		trace_t tr;
		ray.Init(start, end);

		cheat::features::lagcomp.apply_record_data(entity, rec);
		Source::m_pEngineTrace->ClipRayToEntity(ray, MASK_SHOT | CONTENTS_GRATE, entity, &tr);
		const auto walldamage = cheat::features::autowall.CanHit(start, end, cheat::main::local(), entity, hitboxIdx);
		cheat::features::lagcomp.apply_record_data(entity, orgtc);

		return (tr.m_pEnt == entity && walldamage > 1.f);
	};*/

	const auto angle = Math::CalcAngle(eye_pos, aim_point);
	Vector forward;
	Math::AngleVectors(angle, &forward);
	auto const end(eye_pos + forward * 8192.f)/*(eye_pos.DistanceSquared(aim_point) * 1.01f)*/;

	C_Hitbox box1; get_hitbox_data(&box1, entity, hitboxIdx, record->leftmatrixes);
	C_Hitbox box2; get_hitbox_data(&box2, entity, hitboxIdx, record->rightmatrixes);
	C_Hitbox box3; get_hitbox_data(&box3, entity, hitboxIdx, record->matrixes);

	//C_Tickrecord rec;
	//cheat::features::lagcomp.store_record_data(entity, &rec);

	auto hits = 0;

	//if (pizdets ? Math::IntersectBB(eye_pos, end, box1.mins, box1.maxs) : Math::Intersect(eye_pos, end, box1.mins, box1.maxs, box1.radius))//if (is_colliding(eye_pos, end_point, &resolve_info->leftrec, &rec))
	//	++hits;
	//if (pizdets ? Math::IntersectBB(eye_pos, end, box2.mins, box2.maxs) : Math::Intersect(eye_pos, end, box2.mins, box2.maxs, box2.radius))//if (is_colliding(eye_pos, end_point, &resolve_info->rightrec, &rec))
	//	++hits;
	//if (pizdets ? Math::IntersectBB(eye_pos, end, box3.mins, box3.maxs) : Math::Intersect(eye_pos, end, box3.mins, box3.maxs, box3.radius))//if (is_colliding(eye_pos, end_point, &resolve_info->norec, &rec))
	//	++hits;

	//bool ok = false;
	if (box2.isOBB)
	{
		Vector delta1;
		Math::VectorIRotate((forward * 8192.f), record->leftmatrixes[box1.bone], delta1);

		Vector delta2;
		Math::VectorIRotate((forward * 8192.f), record->rightmatrixes[box2.bone], delta2);

		Vector delta3;
		Math::VectorIRotate((forward * 8192.f), record->matrixes[box3.bone], delta3);

		if (Math::IntersectBB(box1.start_scaled, delta1, box1.mins, box1.maxs))
			++hits;
		if (Math::IntersectBB(box2.start_scaled, delta2, box2.mins, box2.maxs))
			++hits;
		if (/*hitboxIdx <= 5 && hits < 2 && */Math::IntersectBB(box3.start_scaled, delta3, box3.mins, box3.maxs))
			++hits;

		/*trace_t ll;
		Ray_t rr;
		rr.Init(eye_pos, end);

		for (auto i = 0; i < 2; i++)
		{
			rr.Init(eye_pos, get_hitbox(entity, hitboxIdx, (i == 2 ? record->rightmatrixes : (i == 1 ? record->leftmatrixes : record->matrixes))));

			csgo.m_engine_trace()->ClipRayToEntity(rr, 0x4600400B, entity, &ll);

			bool can_damage = (ll.hitgroup >= 0 && ll.hitgroup <= 8);
			bool is_required_player = (ll.m_pEnt == entity);

			if (can_damage && is_required_player)
				++hits;
		}*/
	}
	else
	{
		if (Math::Intersect(eye_pos, end, box1.mins, box1.maxs, box1.radius))
			++hits;
		if (Math::Intersect(eye_pos, end, box2.mins, box2.maxs, box2.radius))
			++hits;
		if (Math::Intersect(eye_pos, end, box3.mins, box3.maxs, box3.radius))
			++hits;
	}

	//if (ok)
	//	++hits;

	return (hits >= 2);
}

bool c_aimbot::hit_chance(QAngle angle, Vector point, C_BasePlayer* ent, float chance, int hitbox, float damage, float* hc)
{
	//static float last_innacc = 0.f;

	if (chance < 1.f)
		return true;

	if (ctx.latest_weapon_data == nullptr || !m_weapon())
		return false;

	build_seed_table();

	C_Hitbox ht;
	get_hitbox_data(&ht, ent, hitbox, ent->m_CachedBoneData().Base());

	int traces_hit = 0;
	int awalls_hit = 0;
	int awall_traces_done = 0;

	static Vector forward, right, up;

	// performance optimization.
	if ((ctx.m_eye_position - ent->m_vecOrigin()).Length() > (ctx.latest_weapon_data->range * 1.02f))
		return false;

	Math::AngleVectors(angle, &forward, &right, &up);

	/*
	weapon->m_weaponMode() == 0
		? *(float*)(uintptr_t(weapon->GetCSWeaponData()) + 0x13C)
		: *(float*)(uintptr_t(weapon->GetCSWeaponData()) + 0x140);
	*/

	//m_weapon()->UpdateAccuracyPenalty();

	const float& weap_inaccuracy = Engine::Prediction::Instance()->GetInaccuracy();

	if (int(weap_inaccuracy * 10000.f) == 0) {
		hc[0] = 13;
		hc[1] = 37;
		return true;
	}

	const auto weap_spread = Engine::Prediction::Instance()->GetSpread();

	// setup calculation parameters.
	// auto round_acc = [](const float& accuracy) -> int { return int(accuracy * 10000.f); };

	//const auto sniper = weapon->m_iItemDefinitionIndex() == WEAPON_AWP || weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1
	//	|| weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || weapon->m_iItemDefinitionIndex() == WEAPON_SSG08;

	/*auto pressed_move_key = !ctx.last_usercmd || (ctx.last_usercmd->buttons & IN_MOVELEFT
		|| ctx.last_usercmd->buttons & IN_MOVERIGHT
		|| ctx.last_usercmd->buttons & IN_BACK
		|| ctx.last_usercmd->buttons & IN_FORWARD);*/

	//if (!pressed_move_key || fmaxf(ctx.m_local()->m_vecVelocity().Length(), Engine::Prediction::Instance()->m_vecVelocity.Length()) < 4.f)
	//{
	auto is_special_weapon = m_weapon()->m_iItemDefinitionIndex() == 9
		|| m_weapon()->m_iItemDefinitionIndex() == 11
		|| m_weapon()->m_iItemDefinitionIndex() == 38
		|| m_weapon()->m_iItemDefinitionIndex() == 40;

	//if (ctx.m_local()->m_fFlags() & FL_ONGROUND && round_acc(Engine::Prediction::Instance()->m_flCalculatedInaccuracy) > round_acc(weap_inaccuracy)) {
	//	hc[0] = 13;
	//	hc[1] = 37;
	//	return true;/*round_acc(weap_inaccuracy) <= round_acc(Engine::Prediction::Instance()->m_flCalculatedInaccuracy)*/;
	//}
		//}
	//}
	//	return weap_inaccuracy < (crouched ? .0020f : .0055f);

	//if (ctx.m_local()->get_animation_state() != nullptr && ctx.m_local()->get_animation_state()->t_since_stopped_moving > 0.25f) 
	//{
	//	// no need for hitchance, if we can't increase it anyway.
	//	if (crouched)
	//	{
	//		if (round_acc(weap_inaccuracy) <= round_acc(sniper && ctx.m_local()->m_bIsScoped() ? weapon->GetCSWeaponData()->flInaccuracyCrouchAlt : weapon->GetCSWeaponData()->flInaccuracyCrouch))
	//			return true;
	//	}
	//	else if (feature::anti_aim->animation_speed < 20.f)
	//	{
	//		if (round_acc(weap_inaccuracy) <= round_acc(sniper && ctx.m_local()->m_bIsScoped() ? weapon->GetCSWeaponData()->flInaccuracyStandAlt : weapon->GetCSWeaponData()->flInaccuracyStand))
	//			return true;
	//	}
	//}
	//if (ctx.m_local()->m_iShotsFired() > 0 && (weap_inaccuracy - last_innacc) > 0.0015f)
	//	return false;

	if (precomputed_seeds.empty())
		return false;

	/*if (!(ctx.m_local()->m_fFlags() & FL_ONGROUND && Engine::Prediction::Instance()->GetFlags() & FL_ONGROUND) && static_cast<int>(weap_spread * 100.f) > 1)
		return false;*/

	//float avg_damage = 0.f;
	//int damage_count = 0;
	//
	//int awall_traces_done = 0;

	static std::tuple<float, float, float, float, float>* seed;
	static float c, spread_val, inaccuracy_val;
	static Vector v_spread, dir, end;
	Ray_t ray;
	float average_spread = 0;

	//std::deque<Vector> hit_rays;
	for (auto i = 0; i < total_seeds; i++)
	{
		// get seed.
		seed = &precomputed_seeds[i];

		c = std::get<0>(*seed);

		spread_val = c * weap_spread;
		inaccuracy_val = c * weap_inaccuracy;

		v_spread = Vector((std::get<2>(*seed) * spread_val) + (std::get<4>(*seed) * inaccuracy_val), (std::get<3>(*seed) * spread_val) + (std::get<1>(*seed) * inaccuracy_val), 0);
		//dir.x = forward.x + (right.x * v_spread.x) + (up.x * v_spread.y);
		//dir.y = forward.y + (right.y * v_spread.x) + (up.y * v_spread.y);
		//dir.z = forward.z + (right.z * v_spread.x) + (up.z * v_spread.y);

		//Vector spread_view;
		//Math::VectorAngles(dir, spread_view);
		////spread_view.Normalize();
		//Math::AngleVectors(spread_view, &end);

		dir.x = forward.x + (v_spread.x * right.x) + (v_spread.y * up.x);
		dir.y = forward.y + (v_spread.x * right.y) + (v_spread.y * up.y);
		dir.z = forward.z + (v_spread.x * right.z) + (v_spread.y * up.z);

		dir.NormalizeInPlace();
		//proper
		end = ctx.m_eye_position + (dir * 8192.f);

		//trace_t tr;
		//ray.Init(ctx.m_eye_position, end);

		//tr.fraction = 1.0f;
		//tr.startsolid = false;

		//csgo.m_engine_trace()->ClipRayToEntity(ray, 0x4600400B, ent, &tr);
		//auto intersect = Math::ClipRayToHitbox(ray, ht.hitbox, ent->m_CachedBoneData().Base()[ht.bone], tr) >= 0;

		bool intersect = false;

		if (ht.isOBB) 
		{
			Vector delta;
			Math::VectorIRotate((dir * 8192.f), ent->m_CachedBoneData().Base()[ht.bone], delta);

			intersect = Math::IntersectBB(ht.start_scaled, delta, ht.mins, ht.maxs);
			//trace_t ll;
			//Ray_t rr;
			//rr.Init(ctx.m_eye_position, end);

			//csgo.m_engine_trace()->ClipRayToEntity(rr, 0x4600400B, ent, &ll);

			//const bool can_damage = (ll.hitgroup-1) <= 7;
			//const bool is_required_player = (ll.m_pEnt == ent);
			//intersect = can_damage && is_required_player;// && ll.hitgroup == hitbox2hitgroup(ent, hitbox);

			//csgo.m_debug_overlay()->AddLineOverlay(ctx.m_eye_position, end, 255, intersect ? 255 : 0, 0, false, csgo.m_globals()->interval_per_tick * 2.f);
		}
		else
		{
			intersect = Math::Intersect(ctx.m_eye_position, end, ht.mins, ht.maxs, ht.radius);

			//csgo.m_debug_overlay()->AddLineOverlay(ctx.m_eye_position, end, intersect ? 0 : 255, intersect ? 255 : 0, 0, false, csgo.m_globals()->interval_per_tick * 2.f);
			//csgo.m_debug_overlay()->AddBoxOverlay((ht.mins + ht.maxs) * 0.5f, Vector(-2,-2,-2), Vector(2,2,2), Vector(0,0,0), intersect ? 0 : 255, intersect ? 255 : 0, 0, 255, csgo.m_globals()->interval_per_tick * 2.f);
		}

		if (intersect)
		{
			++traces_hit;

			if (m_weapon()->m_iItemDefinitionIndex() != 64 && !(is_special_weapon && !ctx.m_local()->m_bIsScoped())) {

				average_spread += inaccuracy_val;

				if (autowall_traces > awall_traces_done && (i < 5 || (average_spread / i) <= inaccuracy_val))
				{
					//ctx.force_hitbox_penetrate_accuracy = hitbox == 0;
					auto dmg = feature::autowall->CanHit(ctx.m_eye_position, end, ctx.m_local(), ent, hitbox);
					//ctx.force_hitbox_penetrate_accuracy = false;
					/*if (Engine::Prediction::Instance()->m_flFrameTime > csgo.m_globals()->interval_per_tick)
						dmg = (i % 3 == 0 ? ((awalls_hit > total_seeds / 2.5) ? 1.f : 0.f) : feature::autowall->CanHit(eye_position, end, ctx.m_local(), ent, hitbox, &viable));
					else
						dmg = feature::autowall->CanHit(eye_position, end, ctx.m_local(), ent, hitbox, &viable);*/

					const auto dmg_fine = dmg >= damage;

					if (dmg_fine) {
						++awalls_hit;
						//hit_rays.push_back(end);
					}

					++awall_traces_done;
				}
			}
			else
			{
				//#TODO: fix
				if (autowall_traces > awalls_hit)
					++awalls_hit;
			}
		}

		//csgo.m_debug_overlay()->AddLineOverlay(ctx.m_eye_position, end, 255, intersect ? 255 : 0, 0, false, csgo.m_globals()->interval_per_tick * 2.f);
		//
		//csgo.m_debug_overlay()->AddLineOverlay(eye_position, end, 255, (i <= 64 ? 0 : 255), 0, false, csgo.m_globals()->interval_per_tick * 2.f);
		//
		//if (((static_cast<float>(traces_hit) / static_cast<float>(total_seeds)) >= (chance / 100.f)) && traces_hit > 0)
		//{
		//	if (((static_cast<float>(awalls_hit) / static_cast<float>(total_seeds)) >= float(ctx.m_settings.aimbot_accuracy_boost / 100.f)) || ctx.m_settings.aimbot_accuracy_boost <= 1)
		//		return true;
		//
		// abort if we can no longer reach hitchance.
		/*if (((static_cast<float>(traces_hit + total_seeds - i) / static_cast<float>(total_seeds)) < (chance / 100.f)))
			return false;*/
		//}
	}

	//auto meme = float(float(traces_hit) / float(total_seeds));
	//auto accurate = meme * (int)roundf(total_seeds / 3);

	//if (weapon->can_shoot() || ctx.m_local()->m_iShotsFired() <= 0)
	//	last_innacc = weap_inaccuracy;

	/*if (ctx.m_local()->m_fFlags() & FL_ONGROUND && round_acc(Engine::Prediction::Instance()->m_flCalculatedInaccuracy) >= round_acc(weap_inaccuracy))
	{
		const auto final_chance = (float(awalls_hit) / float(autowall_traces)) * 100.f;

		if (chance <= final_chance) {
			hc[0] = final_chance;
			hc[1] = 100;
			return true;
		}

		return false;
	}*/

	const auto trace_chance = ((float(traces_hit) / float(total_seeds)) * 100.f);

	if (trace_chance >= 15.f && ctx.m_local()->m_fFlags() & FL_ONGROUND && is_special_weapon && !ctx.m_local()->m_bIsScoped() && m_weapon()->can_shoot()) {
		if (Engine::Prediction::Instance()->m_flCalculatedInaccuracy >= (m_weapon()->m_flAccuracyPenalty() * 0.02f))
			return true;
	}

	if (trace_chance < min(100.f, (chance + 5.f)))
		return false;

	//if (weapon->m_iItemDefinitionIndex() == WEAPON_AWP || weapon->m_iItemDefinitionIndex() == WEAPON_SSG08)
	//	return autowall_traces == awalls_hit;

	const auto final_chance = (float(awalls_hit) / float(autowall_traces)) * 100.f;

	if (chance <= final_chance) {
		hc[0] = final_chance;
		hc[1] = trace_chance;
		return true;
	}
	
	return false;
}


void c_aimbot::visualize_hitboxes(C_BasePlayer* entity, matrix3x4_t* mx, Color color, float time)
{
	const model_t* model = entity->GetModel();

	if (!model)
		return;

	const studiohdr_t* studioHdr = csgo.m_model_info()->GetStudioModel(model);

	if (!studioHdr)
		return;

	const mstudiohitboxset_t* set = studioHdr->pHitboxSet(entity->m_nHitboxSet());

	if (!set)
		return;

	for (int i = 0; i < set->numhitboxes; i++)
	{
		mstudiobbox_t* hitbox = set->pHitbox(i);

		if (!hitbox)
			continue;

		Vector min, max/*, center*/;
		Math::VectorTransform(hitbox->bbmin, mx[hitbox->bone], min);
		Math::VectorTransform(hitbox->bbmax, mx[hitbox->bone], max);

		if (hitbox->radius != -1)
			csgo.m_debug_overlay()->AddCapsuleOverlay(min, max, hitbox->radius, color.r(), color.g(), color.b(), color.a(), time, 0, 1);

	}
}

//void Autostop(CUserCmd* cmd)
//{
//	//auto v6 = m_weapon()->GetCSWeaponData();
//	//if (v6)
//	//{
//	//	cmd->buttons |= IN_SPEED;
//	//	auto minimal_speed = 0.f;
//	//
//	//	if (ctx.m_local()->m_bIsScoped() || m_weapon()->m_weaponMode() != 0)                   // isScoped
//	//		minimal_speed = v6->max_speed_alt;
//	//	else
//	//		minimal_speed = v6->max_speed;
//	//
//	//	auto ForwardMove = cmd->forwardmove;
//	//	const auto chocked_ticks = abs(ctx.last_sent_tick - csgo.m_globals()->tickcount);
//	//	auto MinimalSpeed = minimal_speed * min(0.32f, (0.32f - float(0.005f * chocked_ticks)));
//	//	auto SideMove = cmd->sidemove;
//	//	auto CmdMove = ((cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove));
//	//	CmdMove = std::sqrtf(CmdMove);
//	//
//	//	auto velocity_per_tick = (ctx.m_local()->m_vecVelocity() - Engine::Prediction::Instance()->GetVelocity()) * csgo.m_globals()->interval_per_tick;
//	//	auto speed2d = (ctx.m_local()->m_vecVelocity() + velocity_per_tick).Length2D();
//	//
//	//	if (CmdMove >= MinimalSpeed)
//	//	{
//	//		auto forwardNorm = ForwardMove / CmdMove;
//	//		auto sideNorm = SideMove / CmdMove;
//	//
//	//		if ((MinimalSpeed + 1.0f) <= speed2d)
//	//		{
//	//			cmd->forwardmove = 0.0f;
//	//			cmd->sidemove = 0.0f;
//	//		}
//	//		else
//	//		{
//	//			cmd->sidemove = MinimalSpeed * sideNorm;
//	//			cmd->forwardmove = MinimalSpeed * forwardNorm;
//	//		}
//	//	}
//	//}
//
//	auto v81 = !ctx.m_local()->m_bIsScoped();
//	auto v90 = 0.f;
//
//	if (v81)
//		v90 = m_weapon()->GetCSWeaponData()->max_speed;
//	else
//		v90 = m_weapon()->GetCSWeaponData()->max_speed_alt;
//
//	const auto chocked_ticks = fabsf(ctx.last_sent_tick - csgo.m_globals()->tickcount);
//	auto v234 = v90 * /*min(*/0.33000001f;//, (0.32f - float(0.005f * chocked_ticks)));
//	auto velocity = Engine::Prediction::Instance()->GetVelocity();
//	auto v98 = ((velocity.x * velocity.x) + (velocity.y * velocity.y));
//	v98 = sqrtf(v98);
//
//	auto CmdMove = sqrtf((cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove));
//
//	//if (feature::anti_aim->animation_speed <= 6.f && !ctx.did_stop_before) //accurate enough not to stop.
//	//	return;
//
//	auto v73 = cmd->sidemove;
//	auto v74 = cmd->forwardmove;
//
//	csgo.m_engine()->GetViewAngles(Engine::Movement::Instance()->m_qRealAngles);
//
//	bool did_stop = false;
//
//	if (v98 <= (v234 + 1.0f))
//	{
//		auto v110 = (v73 * v73) + (v74 * v74);
//		if (((cmd->upmove * cmd->upmove) + v110) > 0.0f)
//		{
//			cmd->buttons |= 0x20000u;
//
//			//csgo.m_engine()->GetViewAngles(Engine::Movement::Instance()->m_qRealAngles);
//
//			if (v98 <= 0.1f)
//			{
//				v73 = cmd->sidemove * v234;
//				v74 = cmd->forwardmove * v234;
//				did_stop = false;
//			}
//			else
//			{
//				v74 = (cmd->forwardmove / CmdMove) * v234;
//				v73 = (cmd->sidemove / CmdMove) * v234;
//				did_stop = true;
//			}
//		}
//	}
//	else
//	{
//		//Engine::Movement::Instance()->m_qAnglesView.y = RAD2DEG(std::atan2(ctx.m_local()->m_vecVelocity().y, ctx.m_local()->m_vecVelocity().x)) - 180.f;
//
//		//if (ctx.m_local()->m_vecVelocity().Length2D() > 10.f) {
//
//		/*	Vector v247 = Vector::Zero;
//			auto v266 = Engine::Prediction::Instance()->GetVelocity() * -1.0f;
//			v247.x = RAD2DEG(std::atan2(v266.z * -1.f, sqrtf((v266.x * v266.x) + (v266.y * v266.y))));
//			v247.y = RAD2DEG(std::atan2(v266.y, v266.x));
//			v247.z = 0;
//			auto v100 = cmd->viewangles.y - v247.y;
//			auto v101 = v100 * 0.017453292f;
//			auto v103 = cos(v101);
//			auto v105 = sin(v101);
//			auto v107 = (v247.x * 0.017453292f);
//			v107 = sin(v107);*/
//		/*auto neg_velocity = Engine::Prediction::Instance()->GetVelocity() * -1.0f;
//		neg_velocity.z = 0.f;
//		auto direction = RAD2DEG(std::atan2(neg_velocity.y, neg_velocity.x));
//		auto delta = (cmd->viewangles.y - direction) * 0.017453292f;
//
//		v73 = sin(delta) * 450.0f;
//		v74 = cos(delta) * 450.0f;*/
//		Engine::Movement::Instance()->Quick_stop(cmd);
//		did_stop = true;
//
//		//	v73 = (v107 * v103) * 450.f;
//		//	v74 = (v107 * v105) * 450.f;
//		//}
//		/*else
//		{
//			v73 = 0;
//			v74 = 0;
//		}*/
//
//		v73 = cmd->sidemove;
//		v74 = cmd->forwardmove;
//	}
//
//	auto v112 = fminf(450.f, v74);
//
//	if (v112 <= -450.0f)
//		cmd->forwardmove = -450.0f;
//	else
//		cmd->forwardmove = v112;
//
//	auto v113 = fminf(450.f, v73);
//
//	if (v113 <= -450.0f)
//		cmd->sidemove = -450.0f;
//	else
//		cmd->sidemove = v113;
//
//	Engine::Movement::Instance()->forcemovement = Vector(v112, v113, 0);
//	Engine::Movement::Instance()->did_force = did_stop;
//
//	Engine::Prediction::Instance()->prev_cmd_command_num = 0;
//	//Engine::Movement::Instance()->Fix_Movement(cmd, Engine::Movement::Instance()->m_qRealAngles);
//
//	ctx.did_stop_before = true;
//}

void c_aimbot::autostop(CUserCmd* cmd, C_WeaponCSBaseGun* local_weapon)
{
	static auto accel = csgo.m_engine_cvars()->FindVar(sxor("sv_accelerate"));
	//static float last_time_stopped = csgo.m_globals()->realtime;

	static bool was_onground = ctx.m_local()->m_fFlags() & FL_ONGROUND;

	//ctx.did_stop_before = false;

	/*if (csgo.m_client_state()->m_iChockedCommands > 0 && ctx.do_autostop && abs(last_time_stopped - csgo.m_globals()->realtime) > 0.5f)
	{
		for (auto i = 0; i < max(1,(int)feature::usercmd->command_numbers.size()); i++)
		{
			auto ucmd = feature::usercmd->command_numbers[i];
			auto cmd = csgo.m_input()->GetUserCmd(ucmd.command_number);
			auto vcmd = csgo.m_input()->GetVerifiedUserCmd(ucmd.command_number);

			auto v73 = cmd->sidemove;
			auto v74 = cmd->forwardmove;

			auto sqr = sqrtf((v73 * v73) + (v74 * v74));

			auto v81 = !ctx.m_local()->m_bIsScoped();
			auto v90 = 0.f;

			if (v81)
				v90 = m_weapon()->GetCSWeaponData()->max_speed;
			else
				v90 = m_weapon()->GetCSWeaponData()->max_speed_alt;

			const auto chocked_ticks = fabsf(ctx.last_sent_tick - csgo.m_globals()->tickcount);
			auto v234 = v90 * 0.32f;

			if (v234 <= (sqr + 1.0f))
			{
				auto v110 = (v73 * v73) + (v74 * v74);
				if (((cmd->upmove * cmd->upmove) + v110) > 0.0f)
				{
					cmd->buttons |= 0x20000u;
					if (v234 <= 0.1f)
					{
						v73 = cmd->sidemove * v234;
						v74 = cmd->forwardmove * v234;
					}
					else
					{
						auto v111 = sqrtf(v110);
						v74 = (cmd->forwardmove / v111) * v234;
						v73 = (cmd->sidemove / v111) * v234;
					}
				}
			}

			auto v112 = fminf(450.f, v74);

			if (v112 <= -450.0f)
				cmd->forwardmove = -450.0f;
			else
				cmd->forwardmove = v112;

			auto v113 = fminf(450.f, v73);

			if (v113 <= -450.0f)
				cmd->sidemove = -450.0f;
			else
				cmd->sidemove = v113;

			vcmd->m_cmd = *cmd;
			vcmd->m_crc = cmd->GetChecksum();
		}

		*(int*)((DWORD)csgo.m_prediction() + 0xC) = -1;
		*(int*)((DWORD)csgo.m_prediction() + 0x1C) = 0;
	}*/

	if (ctx.m_settings.autostop_only_when_shooting && (!local_weapon->can_shoot() || m_weapon()->m_iItemDefinitionIndex() == 64)) {
		ctx.did_stop_before = false;
		ctx.do_autostop = false;
		return;
	}

	Engine::Prediction::Instance()->m_autostop_velocity_to_validate = 0.f;

	if (ctx.m_settings.aimbot_autostop && local_weapon && local_weapon->m_iItemDefinitionIndex() != WEAPON_TASER && ctx.m_local()->m_fFlags() & FL_ONGROUND && was_onground && ctx.latest_weapon_data/* && !(cmd->buttons & IN_JUMP)*/)
	{
		auto v10 = cmd->buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD | IN_JUMP | IN_SPEED);
		cmd->buttons = v10;

		const auto chocked_ticks = (cmd->command_number % 3) == 0 ? (14 - csgo.m_client_state()->m_iChockedCommands) : ((14 - csgo.m_client_state()->m_iChockedCommands) / 2);
		const auto max_speed = (local_weapon->GetMaxSpeed() * 0.33f) - 1.f - (float(chocked_ticks) * (m_weapon()->m_iItemDefinitionIndex() == WEAPON_AWP ? 1.35f : 1.65f) * + ctx.m_local()->m_iShotsFired()/* * 1.2f*/);//, (0.32f - float(0.005f * chocked_ticks)));

		auto velocity = ctx.m_local()->m_vecVelocity();
		velocity.z = 0;
		auto current_speed = ((velocity.x * velocity.x) + (velocity.y * velocity.y));
		current_speed = sqrtf(current_speed);

		const auto cmd_speed = sqrtf((cmd->sidemove * cmd->sidemove) + (cmd->forwardmove * cmd->forwardmove));

		//if (feature::anti_aim->animation_speed <= 6.f && !ctx.did_stop_before) //accurate enough not to stop.
		//	return;

		auto new_sidemove = cmd->sidemove;
		auto new_forwardmove = cmd->forwardmove;

		if (current_speed >= 28.f) {
			if (current_speed <= max_speed && ctx.m_settings.autostop_type == 0)
			{
				if (current_speed > 0.0f)
				{
					//cmd->buttons |= IN_SPEED;
					if (current_speed <= 0.1f)
					{
						new_sidemove = cmd->sidemove * fminf(current_speed, max_speed);
						new_forwardmove = cmd->forwardmove * fminf(current_speed, max_speed);
					}
					else
					{
						new_forwardmove = (cmd->forwardmove / cmd_speed) * fminf(current_speed, max_speed);
						new_sidemove = (cmd->sidemove / cmd_speed) * fminf(current_speed, max_speed);
					}
				}
			}
			else
			{
				QAngle angle;
				Math::VectorAngles(velocity, angle);

				// fix direction by factoring in where we are looking.
				angle.y = ctx.cmd_original_angles.y - angle.y;

				// convert corrected angle back to a direction.
				Vector direction;
				Math::AngleVectors(angle, &direction);

				if (current_speed > 5.f) {
					auto stop = direction * -current_speed;

					new_forwardmove = stop.x;
					new_sidemove = stop.y;
				}
				else
				{
					new_forwardmove = 0;
					new_sidemove = 0;
				}
			}
		}

		if (ctx.m_local()->m_bDucking()
			|| ctx.m_local()->m_fFlags() & FL_DUCKING) {
			new_forwardmove = new_forwardmove / (((ctx.m_local()->m_flDuckAmount() * 0.34f) + 1.0f) - ctx.m_local()->m_flDuckAmount());
			new_sidemove = new_sidemove / (((ctx.m_local()->m_flDuckAmount() * 0.34f) + 1.0f) - ctx.m_local()->m_flDuckAmount());
		}

		cmd->sidemove = Math::clamp(new_sidemove, -450.f, 450.f);
		cmd->forwardmove = Math::clamp(new_forwardmove, -450.f, 450.f);

		//Engine::Movement::Instance()->m_oldsidemove = cmd->sidemove;
		//Engine::Movement::Instance()->m_oldforward = cmd->forwardmove;

		//Engine::Movement::Instance()->FixMove(cmd, Engine::Movement::Instance()->m_qRealAngles);

		ctx.did_stop_before = true;
		ctx.last_autostop_tick = cmd->command_number/*csgo.m_client_state()->m_clockdrift_manager.m_nServerTick*/;

		//Engine::Prediction::Instance()->prev_cmd_command_num = 0;
		//Engine::Prediction::Instance()->Predict(cmd);

		//last_time_stopped = csgo.m_globals()->realtime;
		ctx.do_autostop = false;
	}

	was_onground = (ctx.m_local()->m_fFlags() & FL_ONGROUND);
}

std::string hitbox_to_string(int h)
{
	switch (h)
	{
	case 0:
		return "head";
		break;
	case 1:
		return "neck";
		break;
	case HITBOX_RIGHT_FOOT:
	case HITBOX_RIGHT_CALF:
	case HITBOX_RIGHT_THIGH:
		return "right leg";
		break;
	case HITBOX_LEFT_FOOT:
	case HITBOX_LEFT_CALF:
	case HITBOX_LEFT_THIGH:
		return "left leg";
		break;
	case HITBOX_RIGHT_HAND:
	case HITBOX_RIGHT_UPPER_ARM:
	case HITBOX_RIGHT_FOREARM:
		return "right hand";
		break;
	case HITBOX_LEFT_HAND:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_UPPER_ARM:
		return "left hand";
		break;
	case HITBOX_CHEST:
		return "lower chest";
	case HITBOX_UPPER_CHEST:
		return "upper chest";
		break;
	default:
		return "body";
		break;
	}
}

Vector get_bone(int bone, matrix3x4_t mx[])
{
	return Vector(mx[bone][0][3], mx[bone][1][3], mx[bone][2][3]);
}

//bool c_aimbot::work(CUserCmd* cmd, bool* send_packet)
//{
//	/*auto fill_players_list = [](void* _data) {
//		lagcomp_mt* data = (lagcomp_mt*)_data;
//
//		data->job_done = false;
//
//		
//
//		data->job_done = true;
//	};*/
//
//	best_player = nullptr;
//	best_hitbox = Vector::Zero;
//	best_hitboxid = -1;
//	m_entities.clear();
//	will_shoot_2nd_wit_r8 = false;
//
//	if (!ctx.m_local() || ctx.m_local()->IsDead() || cmd->weaponselect != 0 || ctx.m_eye_position.IsZero()) return false;
//
//	C_WeaponCSBaseGun* local_weapon = m_weapon();
//
//	if (!local_weapon /*|| !ctx.pressed_keys[6]*/) return false;
//
//	ctx.latest_weapon_data = local_weapon->GetCSWeaponData();
//
//	const auto bodyaim = ctx.get_key_press(ctx.m_settings.aimbot_bodyaim_key);
//
//	if (bodyaim)
//		ctx.active_keybinds[5] = ctx.m_settings.aimbot_bodyaim_key.mode + 1;
//
//	const auto dmg_override = ctx.get_key_press(ctx.m_settings.aimbot_min_damage_override);
//
//	if (dmg_override)
//		ctx.active_keybinds[6] = ctx.m_settings.aimbot_min_damage_override.mode + 1;
//
//	/*auto IsGrenade = [](int item)
//	{
//		if (item == weapon_flashbang
//			|| item == weapon_hegrenade
//			|| item == weapon_smokegrenade
//			|| item == weapon_molotov
//			|| item == weapon_decoy
//			|| item == weapon_incgrenade
//			|| item == weapon_tagrenade)
//			return true;
//		else
//			return false;
//	};*/
//
//	/*if ((Source::m_pGlobalVars->realtime - last_shoot_time) > 0.25f) {
//		cheat::features::antiaimbot.flip_side = false;
//
//		if (cheat::Cvars.anti_aim_desync_extend_limit_on_shot.GetValue())
//			cheat::features::antiaimbot.extend = false;
//	}*/
//
//	//if (low_fps_ticks > 2) {
//	//	fps_dropped = true;
//	//	low_fps = ctx.fps;
//	//}
//
//	//if (fps_dropped)
//	//{
//	//	const auto cur_fps_amt = ctx.fps / prefered_fps;
//	//	const auto low_fps_amt = low_fps / prefered_fps;
//	//
//	//	fps_dropped = (cur_fps_amt - low_fps_amt) >= 0.5f || Engine::Prediction::Instance()->m_flFrameTime >= csgo.m_globals()->interval_per_tick;
//	//}
//
//	//ctx.boost_fps = fps_dropped;
//
//	const auto is_zeus = (local_weapon->m_iItemDefinitionIndex() == WEAPON_TASER) || local_weapon->is_knife();
//
//	if (!ctx.m_settings.aimbot_enabled || (!local_weapon->IsGun() || local_weapon->m_iClip1() <= 0) && !local_weapon->is_knife() || is_zeus && !ctx.m_settings.aimbot_allow_taser)
//		return false;
//
//	//if (local_weapon->m_iItemDefinitionIndex() == 64 && ctx.m_settings.aimbot_auto_revolver)
//	//{
//	//	auto curtime = TICKS_TO_TIME(ctx.m_local()->m_nTickBase());
//	//
//	//	if (local_weapon->m_flPostponeFireReadyTime() > curtime)
//	//	{
//	//		cmd->buttons |= IN_ATTACK;
//	//	}
//	//	// COCK EXTENDER
//	//	else if (local_weapon->m_flNextSecondaryAttack() > curtime)
//	//	{
//	//		cmd->buttons |= IN_ATTACK2;
//	//	}
//	//}
//
//	if (ctx.m_settings.aimbot_auto_revolver && !is_zeus)// && fabs(Engine::Prediction::Instance()->m_flFrameTime - csgo.m_globals()->interval_per_tick) > (csgo.m_globals()->interval_per_tick / 4))
//	{
//		/*if (local_weapon->m_iItemDefinitionIndex() == 64)
//		{
//			auto v7 = Source::m_pGlobalVars->curtime;
//			if (r8cock_time <= (Source::m_pGlobalVars->frametime + v7))
//				r8cock_time = v7 + 0.249f;
//			else
//				cmd->buttons |= IN_ATTACK;
//		}
//		else
//		{
//			r8cock_time = 0.0;
//		}
//
//		local_weapon->m_flPostponeFireReadyTime() = r8cock_time;*/
//
//		if (local_weapon->m_iItemDefinitionIndex() == 64)
//		{
//			auto v33 = csgo.m_globals()->curtime;
//			is_cocking = true;
//
//			if (!ctx.pressed_keys[1] && !ctx.pressed_keys[2]) {
//				cmd->buttons &= ~IN_ATTACK;
//				cmd->buttons &= ~IN_ATTACK2;
//			}
//
//			if (local_weapon->can_cock())
//			{
//				if (r8cock_time <= v33)
//				{
//					if (local_weapon->m_flNextSecondaryAttack() <= v33)
//						r8cock_time = v33 + 0.234375f;
//					else {
//						cmd->buttons |= IN_ATTACK2;
//						//will_shoot_2nd_wit_r8 = true;
//					}
//				}
//				else
//					cmd->buttons |= IN_ATTACK;
//
//				is_cocking = v33 > r8cock_time;
//			}
//			else
//			{
//				is_cocking = false;
//				r8cock_time = v33 + 0.234375f;
//				cmd->buttons &= ~IN_ATTACK;
//			}
//		}
//	}
//
//	//////////////////////////////////////////////////
//	/*lagcomp_mt adata;
//	Threading::QueueJobRef(fill_players_list, &adata);
//	Threading::FinishQueue();*/
//
//	int ppl = 0;
//	//int players_with_damage = 0; int damageable_player = -1;
//	//auto predicted_eyepos_bt = ctx.m_eye_position + Engine::Prediction::Instance()->GetVelocity() * csgo.m_globals()->interval_per_tick;
//
//	//const auto prev_shot_ang = ctx.shot_angles.x;
//	//ctx.shot_angles.clear();
//	static bool baim = false;
//
//	if (m_entities.empty())
//	{
//		static ConVar* sv_maxunlag = csgo.m_engine_cvars()->FindVar(sxor("sv_maxunlag"));
//		for (auto idx = 1; idx < 64; idx++)
//		{
//			C_BasePlayer* entity = csgo.m_entity_list()->GetClientEntity(idx);
//
//			if (!entity ||
//				entity->IsDormant() ||
//				!entity->IsPlayer() ||
//				entity->m_iHealth() <= 0 ||
//				entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum() ||
//				entity->m_bGunGameImmunity()
//				) continue;
//
//			c_player_records* log = &feature::lagcomp->records[idx - 1];
//			resolver_records* resolver_info = &feature::resolver->player_records[idx - 1];
//
//			//log.head_position.clear();
//			log->best_record = nullptr;
//
//			if (!log->player || log->player != entity || log->records_count < 1)
//				continue;
//
//			log->restore_record.data_filled = false;
//			log->restore_record.store(entity, true);
//
//			//must_baim_player[idx - 1] = false;
//
//			//int best_index = 0;
//			float best_damage = 0.1f;
//			float best_distance = 8196;
//			/*float best_delta = 60.f;
//			Vector last_hitbox_pos = Vector::Zero;*/
//
//			int passed_records = 0;
//
//			log->hitboxes_damage[HITBOX_HEAD] = 0;
//			log->hitboxes_damage[HITBOX_BODY] = 0;
//			log->hitboxes_damage[HITBOX_PELVIS] = 0;
//			log->hitboxes_damage[HITBOX_UPPER_CHEST] = 0;
//			log->hitboxes_damage[HITBOX_LEFT_FOOT] = 0;
//			log->hitboxes_damage[HITBOX_RIGHT_FOOT] = 0;
//
//			auto newest_record = (log->tick_records[(log->records_count - 1) & 63].simulation_time <= log->tick_records[(log->records_count) & 63].simulation_time && log->tick_records[(log->records_count) & 63].animated ? &log->tick_records[(log->records_count) & 63] : &log->tick_records[(log->records_count - 1) & 63]);
//
//			/*resolve_info.aimbot_resolve_method = resolve_info.resolving_method;
//
//			if (resolve_info.resolving_method <= 0 && ctx.shots_fired[idx] == 0 && fabs(entity->m_angEyeAngles().x) > 45.f)
//				resolve_info.aimbot_resolve_method = 1;
//
//			if (ctx.shots_fired[idx] > 0)
//			{
//				const auto shots = ((ctx.shots_fired[idx] - 1) % 3);
//
//				switch (shots)
//				{
//				case 0:
//					resolve_info.aimbot_resolve_method = (int(resolve_info.prev_resolving_method != 2) + 1);
//					break;
//				case 1:
//					resolve_info.aimbot_resolve_method = resolve_info.prev_resolving_method;
//					break;
//				case 2:
//					resolve_info.aimbot_resolve_method = 0;
//					break;
//				}
//			}
//			else {
//				if (resolve_info.last_hurt_resolved >= 0)
//					resolve_info.aimbot_resolve_method = resolve_info.last_hurt_resolved;
//			}*/
//
//			bool force_backtrack = false;
//
//			if (newest_record->valid && !newest_record->dormant && newest_record->data_filled && newest_record->animated) {
//				if (!feature::lagcomp->is_time_delta_too_large(newest_record)/* && !ctx.fakeducking*/) {
//					log->best_record = newest_record;
//				}
//				else
//					force_backtrack = true;
//
//				if (!newest_record->shot_this_tick && newest_record->animated && !entity->IsBot())
//				{
//					auto was_viable_r = false;
//					auto was_viable_l = false;
//
//					newest_record->apply(entity, false);
//					//const auto bonecount = entity->GetBoneCount();
//					//entity->GetBoneAccessor().m_ReadableBones = log->tick_records[0]->bones_count;
//					//ctx.force_hitbox_penetrate_accuracy = true;
//					//ctx.force_low_quality_autowalling = true;
//					memcpy(entity->m_CachedBoneData().Base(), newest_record->rightmatrixes, min(128, newest_record->bones_count) * sizeof(matrix3x4_t));
//					//entity->GetBoneAccessor().m_ReadableBones = log->tick_records[0]->bones_count;
//					//entity->GetBoneAccessor().m_WritableBones = 0;
//					ctx.points[entity->entindex() - 1][HITBOX_HEAD].clear();
//					const auto r_damage = /*ctx.current_tickcount % 6 != 5 ? feature::autowall->CanHit(ctx.m_eye_position, feature::ragebot->get_hitbox(entity, HITBOX_HEAD, entity->m_CachedBoneData().Base()), ctx.m_local(), entity, HITBOX_HEAD, &was_viable_r) : */can_hit(HITBOX_HEAD, entity, newest_record->rightmatrixes, &was_viable_r, true);
//					memcpy(entity->m_CachedBoneData().Base(), newest_record->leftmatrixes, min(128, newest_record->bones_count) * sizeof(matrix3x4_t));
//					//entity->GetBoneAccessor().m_ReadableBones = log->tick_records[0]->bones_count;
//					//entity->GetBoneAccessor().m_WritableBones = 0;
//					ctx.points[entity->entindex() - 1][HITBOX_HEAD].clear();
//					const auto l_damage = /*ctx.current_tickcount % 6 != 5 ? feature::autowall->CanHit(ctx.m_eye_position, feature::ragebot->get_hitbox(entity, HITBOX_HEAD, entity->m_CachedBoneData().Base()), ctx.m_local(), entity, HITBOX_HEAD, &was_viable_l) : */can_hit(HITBOX_HEAD, entity, newest_record->leftmatrixes, &was_viable_l, true);
//					//ctx.force_low_quality_autowalling = false;
//					//entity->GetBoneCount() = bonecount;
//					//ctx.force_hitbox_penetrate_accuracy = false;
//
//					log->restore_record.apply(entity, true);
//					//entity->GetBoneAccessor().m_ReadableBones = bonecount;
//					//entity->GetBoneAccessor().m_WritableBones = 0;
//
//					const auto prev_freestanding_res = log->records_count > 0 ? newest_record->freestanding_index : 0;
//
//					if ((r_damage <= 0 || l_damage <= 0) && !(l_damage <= 0 && r_damage <= 0)) {
//
//						int new_freestand_resolver = prev_freestanding_res;
//
//						if (l_damage < r_damage)
//							new_freestand_resolver = 1;
//						else
//							new_freestand_resolver = 2;
//
//						//log->resolver_indexes[csgo.m_globals()->tickcount & 63] = new_freestand_resolver;
//						//if (abs(cmd->tick_count - log->last_freestand_use[idx]) > 7)
//						//{
//						newest_record->freestanding_index = new_freestand_resolver;
//						resolver_info->freestanding_side = new_freestand_resolver;
//						resolver_info->freestanding_update_time = csgo.m_globals()->realtime;
//
//						///	log->last_freestand_use[idx] = cmd->tick_count;
//						log->checked_freestand = true;
//						//}
//					}
//					else
//						log->checked_freestand = false;
//				}
//			}
//
//			auto start = min(int(log->records_count - 1), int(sv_maxunlag->GetFloat() * 200));
//
//			auto first_valid_record = start + 1;
//
//			auto hp = static_cast<float>(min(100, entity->m_iHealth()));
//
//			//int changed_angles = 0;
//
//			//bool did_remove = false;
//			//bool best_was_viable = false;
//
//			//bool changed_pitch_suddenly = false;
//			//int record_pitch_change = 0;
//
//			//bool latest_no_damage = false;
//
//			//float best_pred_dmg = 0.1f;
//
//			//auto local_vel = int(min(1.f, ((m_weapon()->GetMaxWeaponSpeed() * 0.33f) / ctx.m_local()->m_vecVelocity().Length2D())) * 4.f);
//			//const auto epic_pred_eyepos = Engine::Prediction::Instance()->m_vecOrigin + (Engine::Prediction::Instance()->GetVelocity() * TICKS_TO_TIME(4));
//
//			const auto new_resolver_idx = !entity || !newest_record || !newest_record->data_filled || entity->IsBot() ? 0 : feature::resolver->select_next_side(entity, newest_record);
//
//			//resolver_info->last_resolving_method = new_resolver_idx;
//
//			//ctx.points[entity->entindex() - 1].fill(Vector::Zero);
//			//ctx.points[entity->entindex() - 1][0].clear();
//			//ctx.points[entity->entindex() - 1][0].clear();
//
//			auto had_shot_rec = false;
//			auto had_low_delta = false;
//			auto had_anims_update = false;
//
//			ctx.optimized_point_search = false;
//
//			//for (auto& record : log.tick_records)
//			for (auto k = start; k > 0; k--)
//			{
//				if (k > (log->records_count - 1))
//					continue;
//
//				//if (newest_record->breaking_lc && k > 0)
//				//	break;
//
//				auto record = &log->tick_records[(log->records_count - k) & 63];
//
//				if (!record 
//					|| !record->valid 
//					|| record->dormant 
//					|| !record->animated 
//					//|| record->breaking_lc && k > 0 && (local_weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 || local_weapon->m_iItemDefinitionIndex() == WEAPON_AWP) 
//					|| record->exploit)
//					continue;
//
//				if (feature::lagcomp->is_time_delta_too_large(record))
//					continue;
//
//				if (first_valid_record >= start)
//					first_valid_record = k;
//
//				record->resolver_index = new_resolver_idx;
//				record->resolver_type = newest_record->resolver_type;
//
//				//// fov check.
//				//if (g_menu.main.aimbot.fov.get()) {
//				//	// if out of fov, retn false.
//				//	if (Math::GetFOV(g_cl.m_view_angles, g_cl.m_shoot_pos, aim) > g_menu.main.aimbot.fov_amount.get())
//				//		return false;
//				//}
//
//				//if (ppl > 2)
//				//{
//				//	//if (ctx.exploit_allowed && ctx.m_settings.aimbot_tickbase_exploit > 0 || !latest_no_damage) {
//				//		if (k < (first_valid_record - 1) && k > 1 && (!record.shot_this_tick && !record.animations_updated && abs(record.desync_delta) > 35.f))
//				//			continue;
//				//	//}
//				//	//else
//				//	//{
//				//	//	if (latest_no_damage && k > 1 && !record.shot_this_tick)
//				//	//		continue;
//				//	//}
//				//}
//				//else
//				//{
//					//if (k > 0 && k % 2 == 0 && (log->records_count-1) > 2)
//					//	continue;
//				//}
//
//				/*if (resolve_info->resolving_method != record.rtype && resolve_info.resolving_method > 0)
//					continue;*/
//
//					//if (resolver_info->resolving_method <= 0 && resolver_info->prev_resolving_method < 0 && ctx.shots_fired[idx - 1] < 1 && abs(record.eye_angles.x) > 45.f/* && abs(previous->eye_angles.x) > 45.f*/)
//					//	resolver_info->resolving_method = 1;
//
//					/*if (log->missed_at_onshot > 0 && !record.shot_this_tick && ctx.shots_fired[idx - 1] > 0 && !did_remove) {
//						ctx.shots_fired[idx - 1] -= 1;
//						log->missed_at_onshot -= 1;
//						did_remove = true;
//					}*/
//
//					//const auto shots = (ctx.shots_fired[idx - 1] % 4);
//
//				C_Tickrecord* prev = log->records_count > 1
//					? &log->tick_records[(log->records_count - 1 - k) & 63]
//					: nullptr;
//
//				//const auto prev_resolver_type = prev ? prev->resolver_type : resolver_info->resolver_type;
//
//				//auto next_resolving_method = 0;
//				//auto next_resolver_type = 0;
//
//				//if (resolver_info->last_abs_yaw_side > 0) {
//				//	next_resolving_method = resolver_info->last_abs_yaw_side;
//				//	next_resolver_type = 1;
//				//}
//
//				//if (record->freestanding_index > 0 && !record->shot_this_tick) {
//				//	next_resolving_method = record->freestanding_index;
//				//	next_resolver_type = 3;
//				//}
//
//				//if (resolver_info->last_hurt_resolved >= 0 && (abs(csgo.m_globals()->realtime - last_shoot_time) < 0.5f || next_resolver_type == 0)) {
//				//	next_resolving_method = resolver_info->last_hurt_resolved;
//				//	next_resolver_type = 2;
//				//}
//
//				//if (record->animations_index > 0 /*&& !record->shot_this_tick*/ && record->animations_updated) {
//				//	next_resolving_method = record->animations_index;
//				//	next_resolver_type = 4;
//				//}
//
//				//const auto prev_resolver_index = prev ? prev->resolver_index : resolver_info->last_resolving_method;
//
//				//if (ctx.shots_fired[idx - 1] > 0)
//				//{
//				//	if (resolver_info->brute_banned_sides.empty())
//				//	{
//				//		if ((csgo.m_globals()->realtime - last_shoot_time) < 0.7f)
//				//		{
//				//			//how did this happen?
//				//			if (resolver_info->last_hurt_resolved >= 0) {
//				//				next_resolving_method = resolver_info->last_hurt_resolved;
//				//				next_resolver_type = 2;
//				//			}
//				//		}
//				//		else
//				//		{
//				//			const auto is_banned = std::find(resolver_info->brute_banned_sides.begin(), resolver_info->brute_banned_sides.end(), next_resolving_method) != resolver_info->brute_banned_sides.end();
//
//				//			//if (is_banned && )
//				//		}
//				//	}
//				//	else
//				//	{
//				//		const auto is_banned = std::find(resolver_info->brute_banned_sides.begin(), resolver_info->brute_banned_sides.end(), next_resolving_method) != resolver_info->brute_banned_sides.end();
//
//				//		if (is_banned && (csgo.m_globals()->realtime - resolver_info->last_time_shot) < 0.5f)
//				//		{
//				//			//select new best side.
//				//		}
//				//		else
//				//		{
//				//			//should we change index if we did
//				//		}
//				//	}
//				//}
//
//				//switch (shots)
//				//{
//				//case 0:
//				//	/*if (history.size() > 2 && abs(diff) > (feature::resolver->get_delta(m_player->get_animation_state()) * 0.95f) && !record->animations_updated && history[2].eye_angles.y == previous->eye_angles.y && record->anim_layers[6].m_flWeight <= 0.01f && !record->shot_this_tick)
//				//	{
//				//		resolver_info->resolving_method = (diff <= 0.f ? 2 : 1);
//				//		resolver_info->resolver_type = 3;
//				//	}*/
//				//	break;
//				//case 1:
//				//	next_resolving_method = (int(prev_resolver_index != 2) + 1);
//				//	next_resolver_type = 5;
//				//	break;
//				//case 2:
//				//	//if (ctx.shots_fired[idx - 1] <= 3)
//				//	next_resolving_method = (int(prev_resolver_index == 2) + 1);
//				//	next_resolver_type = 5;
//				//	break;
//				//case 3:
//				//	next_resolving_method = 0;
//				//	next_resolver_type = 5;
//				//	break;
//				//}
//
//				//auto lol = abs(Math::AngleDiff(record->eye_angles.y, record->lower_body_yaw));
//				//if (lol > 30.f && record->lag < 14 && ctx.shots_fired[idx - 1] < 1 && !record->shot_this_tick)
//				//{
//				//	/*if (resolver_info->change)
//				//		resolver_info->resolving_method = (int(resolver_info->prev_resolving_method != 2) + 1);
//
//				//	resolver_info->change = !resolver_info->change;*/
//
//				//	//auto delta_1 = abs(Math::AngleDiff(previous->eye_angles.y, resolver_info->simulated_animstates[0].abs_yaw));
//				//	auto delta_2 = abs(Math::AngleDiff(record->eye_angles.y, resolver_info->simulated_animstates[1].m_abs_yaw));
//				//	auto delta_3 = abs(Math::AngleDiff(record->eye_angles.y, resolver_info->simulated_animstates[2].m_abs_yaw));
//
//				//	//auto max_delta = max(delta_1, max(delta_2, delta_3));
//
//				//	/*if (max_delta == delta_1)
//				//		resolver_info->resolving_method = 0;
//				//	else
//				//	{*/
//				//	if (delta_2 > delta_3)
//				//		next_resolving_method = 2;
//				//	else
//				//		next_resolving_method = 1;
//				//	//}
//
//				//	/*if (max_delta > max(delta_2, delta_3))
//				//	{
//				//		resolver_info->resolving_method = 0;
//				//	}
//				//	else if (max_delta > max(delta_2, delta_1))
//				//	{
//				//		resolver_info->resolving_method = 2;
//				//	}
//				//	else
//				//	{
//				//		resolver_info->resolving_method = 1;
//				//	}*/
//				//	record->animations_updated = true;
//				//	next_resolver_type = 6;
//				//	resolver_info->prev_resolving_method = next_resolving_method;
//				//}
//
//				/*if (log->tick_records.size() > 1 && changed_angles > 1 && abs(Math::AngleDiff(log->tick_records[max(k - 1, 0)].eye_angles.y, record.eye_angles.y)) > 165.f && log->tick_records[max(k - 1, 0)].eye_angles.x == record.eye_angles.x && !record.shot_this_tick && !log->tick_records[max(k - 1, 0)].shot_this_tick)
//				{
//					changed_angles++;
//
//					if (resolver_info->change)
//						next_resolving_method = (int(next_resolving_method != 2) + 1);
//
//					resolver_info->change = !resolver_info->change;
//
//					next_resolver_type = 7;
//					resolver_info->prev_resolving_method = next_resolving_method;
//				}*/
//
//				//float at_target = ToDegrees(atan2(record.origin.y - ctx.m_local()->m_vecOrigin().y,
//				//	record.origin.x - ctx.m_local()->m_vecOrigin().x));
//
//				//auto some_delta = std::remainderf(at_target - record.eye_angles.y, 360.0f);
//
//				//if (!(some_delta >= 0.0f) && ctx.shots_fired[idx - 1] <= 2) {
//				//	/*left = -1;
//				//	right = 1;*/
//				//	next_resolving_method = (int(next_resolving_method != 2) + 1);
//				//	next_resolver_type = 7;
//				//	resolver_info->prev_resolving_method = next_resolving_method;
//				//}
//
//				/*if (abs(Math::AngleDiff(record.eye_angles.x,log->tick_records[k + 1].eye_angles.x)) > 20.f && !record.shot_this_tick)
//				{
//					record_pitch_change = k-1;
//					changed_pitch_suddenly = true;
//				}*/
//
//				//resolver_info->resolving_method = next_resolving_method;
//				//resolver_info->resolver_type = next_resolver_type;
//
//				/*if (log->best_record != nullptr && ctx.shots_fired[idx-1] == 1 && log->best_record->shot_this_tick && !resolver_info->did_backtrack_onshot && resolver_info->resolver_type >= 3) {
//					resolver_info->resolving_method = (int(resolver_info->resolving_method != 2) + 1);
//					resolver_info->resolver_type = 8;
//					resolver_info->prev_resolving_method = (int(resolver_info->resolving_method != 2) + 1);
//				}*/
//
//				log->hitboxes_positions[HITBOX_HEAD] = feature::ragebot->get_hitbox(entity, HITBOX_HEAD, entity->m_CachedBoneData().Base());
//				//auto safe_to_hit_head = safe_point(entity, ctx.m_eye_position, log->hitboxes_positions[HITBOX_HEAD], HITBOX_HEAD, record);
//
//				ctx.shot_angles = Math::CalcAngle(ctx.m_eye_position, log->hitboxes_positions[HITBOX_HEAD]).Clamped();
//
//				/*if (!entity->IsBot())
//					record->desync_delta = entity->get_bone_pos(8, record->leftmatrixes).DistanceSquared(entity->get_bone_pos(8, record->matrixes));*/
//				//else
//				//	record->desync_delta = 58.f;
//				//record->desync_delta = 
//
//				if (record->animstate.m_last_update_time > record->simulation_time || prev && record->simulation_time == prev->simulation_time)
//					continue;
//
//				if (k > ((ctx.m_settings.aimbot_tickbase_exploit > 0 && ctx.exploit_allowed && !ctx.fakeducking || force_backtrack) ? 6 : 0) && passed_records > 2)
//				{
//					if (prev != nullptr && k > 1 && ((get_bone(8, record->matrixes) - get_bone(8, prev->matrixes)).LengthSquared() < 1.f && !force_backtrack))
//						continue;
//				}
//
//				auto shot_around = (record->shot_time > record->simulation_time_old || record->shot_this_tick);
//
//				if (had_shot_rec && !shot_around && abs(record->desync_delta) >= 38 && !is_zeus)
//					continue;
//
//				if (had_low_delta && !shot_around && abs(record->desync_delta) >= 38 && !is_zeus)
//					continue;
//
//				//if (ctx.shots_fired[idx - 1] < 1)
//				//	resolver_info->prev_resolving_method = resolver_info->resolving_method;
//
//				//if (resolver_info->anims_resolving > 0 /*&& !record->shot_this_tick*/ && record->animations_updated && ctx.shots_fired[idx] < 1) {
//				//	resolver_info->resolving_method = resolver_info->anims_resolving;
//				//	resolver_info->resolver_type = 4;
//				//}
//
//				//const auto lolz = resolver_info->resolving_method;
//
//				//auto attarget = Math::CalcAngle(record->abs_origin, ctx.m_eye_position);
//
//				//if (abs(Math::AngleDiff(record->eye_angles.y, attarget.y)) < 45.f && lolz > 0 && ctx.shots_fired[idx] <= 1) {
//				//	resolver_info->resolving_method = ((resolver_info->resolver_type == 5 ? resolver_info->resolving_method == 1 : prev_resolver_index == 1) ? 2 : 1);// (int(resolver_log->resolving_method != 2) + 1);
//				//	resolver_info->resolver_type = 9;
//				//}
//
//				//record->resolver_type = resolver_info->resolving_method;
//
//				log->hitboxes_positions[HITBOX_BODY] = feature::ragebot->get_hitbox(entity, HITBOX_BODY, entity->m_CachedBoneData().Base());
//
//				record->apply(entity, false);
//
//				passed_records++;
//
//				//auto was_viable = false;
//
//				ctx.points[entity->entindex() - 1][HITBOX_BODY].clear();
//				ctx.points[entity->entindex() - 1][HITBOX_UPPER_CHEST].clear();
//				ctx.points[entity->entindex() - 1][HITBOX_HEAD].clear();
//				ctx.optimized_point_search = k > 1 && (passed_records > 2 || m_entities.size() > 1 && (ctx.m_settings.aimbot_tickbase_exploit > 0 && ctx.exploit_allowed || ctx.fakeducking)/* || ctx.fps <= (1.f/csgo.m_globals()->interval_per_tick)*/);
//
//				log->hitboxes_damage[HITBOX_BODY] = local_weapon->is_knife() ? 0 : feature::ragebot->can_hit(HITBOX_BODY, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_BODY]);
//				log->hitboxes_damage[HITBOX_HEAD] = is_zeus ? 0 : feature::ragebot->can_hit(HITBOX_HEAD, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_HEAD]);
//
//				if (best_damage >= hp)
//				{
//					if (log->hitboxes_damage[HITBOX_BODY] <= hp && log->hitboxes_damage[HITBOX_HEAD] < hp) {
//						ctx.optimized_point_search = false;
//						continue;
//					}
//				}
//
//				//ctx.force_hitbox_penetrate_accuracy = false;
//				if ((ctx.current_tickcount % 2) == 1) {
//					ctx.points[entity->entindex() - 1][HITBOX_LEFT_FOOT].clear();
//					ctx.points[entity->entindex() - 1][HITBOX_RIGHT_FOOT].clear();
//					log->hitboxes_damage[HITBOX_LEFT_FOOT] = is_zeus ? 0 : feature::ragebot->can_hit(HITBOX_LEFT_FOOT, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_LEFT_FOOT]);
//					log->hitboxes_damage[HITBOX_RIGHT_FOOT] = is_zeus ? 0 : feature::ragebot->can_hit(HITBOX_RIGHT_FOOT, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_RIGHT_FOOT]);
//				}
//
//				auto max_damage = max(log->hitboxes_damage[HITBOX_HEAD], max(log->hitboxes_damage[HITBOX_BODY], max(log->hitboxes_damage[HITBOX_PELVIS], max(log->hitboxes_damage[HITBOX_UPPER_CHEST], max(log->hitboxes_damage[HITBOX_LEFT_FOOT], log->hitboxes_damage[HITBOX_RIGHT_FOOT])))));
//
//				if (max_damage <= 0) {
//					ctx.optimized_point_search = false;
//					continue;
//				}
//
//				ctx.optimized_point_search = false;
//				auto dist = log->hitboxes_positions[HITBOX_BODY].Distance(ctx.m_eye_position);//ctx.m_settings.aimbot_low_fps_optimizations
//
//				//last_hitbox_pos = record->head_pos;
//				//if (k <= first_valid_record && k > (first_valid_record - 1) && best_damage < 1.f)
//				//	latest_no_damage = true;
//
//				//int hitbox_with_dmg = log->hitboxes_damage[HITBOX_HEAD] > log->hitboxes_damage[HITBOX_BODY] ? HITBOX_HEAD : (log->hitboxes_damage[HITBOX_BODY] < log->hitboxes_damage[HITBOX_UPPER_CHEST] ? HITBOX_UPPER_CHEST : HITBOX_BODY);
//
//				//if (hitbox_with_dmg == HITBOX_UPPER_CHEST && max(log->hitboxes_damage[HITBOX_LEFT_FOOT], log->hitboxes_damage[HITBOX_RIGHT_FOOT]) < log->hitboxes_damage[HITBOX_UPPER_CHEST])
//				//	hitbox_with_dmg = log->hitboxes_damage[HITBOX_LEFT_FOOT] > log->hitboxes_damage[HITBOX_RIGHT_FOOT] ? HITBOX_LEFT_FOOT : HITBOX_RIGHT_FOOT;
//
//				if (max_damage > best_damage || (is_zeus && best_distance > dist))// /*|| (point_damage >= best_damage && log->best_record != nullptr && record.simulation_time > log->best_record->simulation_time && !had_anims_update && !had_shot_rec)*/) || ((max_damage >= entity->m_iHealth() || max_damage >= best_damage) && best_delta > record->desync_delta) || (is_zeus && best_distance < dist))
//				{
//					best_damage = max_damage;
//					//const auto eye = ctx.m_eye_position;
//					//ctx.m_eye_position = epic_pred_eyepos;
//					//best_pred_dmg = feature::ragebot->can_hit(hitbox_with_dmg, entity, entity->m_CachedBoneData().Base(), 0, true);
//					//ctx.m_eye_position = eye;
//					best_distance = dist;
//					//best_delta = record->desync_delta;
//					log->best_record = record;
//					log->best_record->resolver_index = new_resolver_idx;
//					//best_index = k;
//
//					if (record->shot_this_tick)
//						had_shot_rec = true;
//					else if (abs(record->desync_delta) < 41)
//						had_low_delta = true;
//					else if (/*(k < start && ) || */record->animations_updated)
//						had_anims_update = true;
//
//					if (max_damage > hp/*&& (record->shot_this_tick || record->desync_delta < 40.f)*/)
//						break;
//				}
//			}
//
//			//if (ctx.m_settings.aimbot_fakelag_prediction && log->tick_records.size() > 3 && best_damage <= 1.f && log->best_record != nullptr && log->best_record->velocity.Length() > 30.f && log->best_record->simulation_time == log->tick_records[0].simulation_time && !log->best_record->exploit && !log->tick_records[1].exploit && !log->tick_records[2].exploit)
//			//{
//			//	auto pre_previous_record = log->tick_records[2];
//			//
//			//	// setup simulation data
//			//	C_Simulationdata data;
//			//	data.velocity = log->best_record->velocity;
//			//	data.origin = log->best_record->origin;
//			//	data.flags = log->best_record->entity_flags;
//			//	data.simtime = log->best_record->simulation_time;
//			//	data.data_filled = true;
//			//	data.entity = entity;
//			//
//			//	data.jumped = true;
//			//	if (log->best_record->entity_flags & FL_ONGROUND) {
//			//		if (log->tick_records[1].entity_flags & FL_ONGROUND)
//			//			data.jumped = false;
//			//	}
//			//
//			//	// estimate time when new record will arrive
//			//	float realtime = TICKS_TO_TIME(csgo.m_globals()->tickcount);
//			//	int time_ticks = TIME_TO_TICKS((realtime + log->best_record->latency) - TICKS_TO_TIME(log->best_record->tickcount));
//			//
//			//	// clamp between zero and tickrate
//			//	time_ticks = Math::clamp(time_ticks, 0, (int)(1.0f / csgo.m_globals()->interval_per_tick));
//			//
//			//	float time_to_simulate = log->best_record->simulation_time - log->tick_records[1].simulation_time;
//			//	time_to_simulate = Math::clamp(time_to_simulate, csgo.m_globals()->interval_per_tick, 1.0f);
//			//
//			//	int ticks_to_simulate = TIME_TO_TICKS(time_to_simulate);
//			//	ticks_to_simulate = Math::clamp(ticks_to_simulate, 1, 17);
//			//
//			//	/*if (log->tick_records[0].breaking_lc && ticks_to_simulate > 3 || ticks_to_simulate >= 14)
//			//	{
//			//
//			//		for (auto i = 0; i <= ticks_to_simulate; i++)
//			//		{
//			//
//			//		}
//			//	}*/
//			//
//			//	if (time_ticks - ticks_to_simulate > 0) {
//			//
//			//		float velocityDirection = atan2(log->best_record->velocity.y, log->best_record->velocity.x);
//			//		velocityDirection = RAD2DEG(velocityDirection);
//			//
//			//		float prevVelocityDirection = atan2(log->tick_records[1].velocity.y, log->tick_records[1].velocity.x);
//			//		prevVelocityDirection = RAD2DEG(prevVelocityDirection);
//			//
//			//		// estimate new velocity direction
//			//		float deltaDirection = Math::normalize_angle(velocityDirection - prevVelocityDirection);
//			//		float directionPerTick = deltaDirection / time_to_simulate;
//			//		float currentSpeed = data.velocity.Length2D();
//			//
//			//		float penultimateChoke = log->tick_records[1].simulation_time - pre_previous_record.simulation_time;
//			//		penultimateChoke = Math::clamp(penultimateChoke, csgo.m_globals()->interval_per_tick, 1.0f);
//			//
//			//		// FIXME: this calculations will be correct only for spinbots and jitters with big delta
//			//		float yawDeltaPrevious = Math::normalize_angle(log->best_record->eye_angles.y - log->tick_records[1].eye_angles.y) / float(ticks_to_simulate);
//			//		float yawDeltaPenultimate = Math::normalize_angle(log->tick_records[1].eye_angles.y - pre_previous_record.eye_angles.y) / float(TIME_TO_TICKS(penultimateChoke));
//			//		float yawPerTick = (yawDeltaPrevious + yawDeltaPenultimate) * 0.5f;
//			//
//			//		// useless extrapolation
//			//		if (currentSpeed > 0.0f)
//			//		{
//			//
//			//			float curtime = csgo.m_globals()->curtime;
//			//			float frametime = csgo.m_globals()->frametime;
//			//
//			//			// calculate animations based on ticks aka server frames instead of render frames
//			//			csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
//			//
//			//			QAngle backup_angles = entity->m_angEyeAngles();
//			//			const auto oldsim = entity->m_flOldSimulationTime();
//			//			const auto absvel = entity->m_vecAbsVelocity();
//			//
//			//			CCSGOPlayerAnimState animState = *entity->get_animation_state();
//			//
//			//			C_AnimationLayer layers[13];
//			//			std::memcpy(layers, entity->animation_layers_ptr(), min(13, entity->get_animation_layers_count()) * sizeof(C_AnimationLayer));
//			//
//			//			auto total_predicted = 0;
//			//
//			//			int i = 0;
//			//			while (true)
//			//			{
//			//				total_predicted++;
//			//				i++;
//			//
//			//				float velocitySin = sin(DEG2RAD(velocityDirection)), velocityCos = cos(DEG2RAD(velocityDirection));
//			//
//			//				// TODO: extrapolate speed too
//			//				data.velocity.y = velocitySin * currentSpeed;
//			//				data.velocity.x = velocityCos * currentSpeed;
//			//
//			//				velocityDirection += csgo.m_globals()->interval_per_tick * directionPerTick;
//			//
//			//				feature::lagcomp->simulate_movement(data);
//			//
//			//				if (data.flags & FL_ONGROUND)
//			//					entity->m_fFlags() |= FL_ONGROUND;
//			//				else
//			//					entity->m_fFlags() &= ~FL_ONGROUND;
//			//
//			//				entity->m_vecVelocity() = data.velocity;
//			//				entity->m_vecAbsVelocity() = data.velocity;
//			//				entity->set_abs_origin(data.origin);
//			//
//			//				//entity->m_angEyeAngles().y = Math::normalize_angle(entity->m_angEyeAngles().y + yawPerTick);
//			//
//			//				entity->m_flOldSimulationTime() = entity->m_flSimulationTime();
//			//				data.simtime += csgo.m_globals()->interval_per_tick;
//			//				
//			//				const auto dword_3CF22C70 = csgo.m_globals()->realtime;
//			//				const auto dword_3CF22C74 = csgo.m_globals()->curtime;
//			//				const auto dword_3CF22C78 = csgo.m_globals()->frametime;
//			//				const auto dword_3CF22C7C = csgo.m_globals()->absoluteframetime;
//			//				const auto dword_3CF22C80 = csgo.m_globals()->interpolation_amount;
//			//				const auto dword_3CF22C84 = csgo.m_globals()->framecount;
//			//				const auto dword_3CF22C88 = csgo.m_globals()->tickcount;
//			//
//			//				const auto simulated_ticks = TIME_TO_TICKS(data.simtime);
//			//
//			//				csgo.m_globals()->realtime = data.simtime;
//			//				csgo.m_globals()->curtime = data.simtime;
//			//				csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
//			//				csgo.m_globals()->absoluteframetime = csgo.m_globals()->interval_per_tick;
//			//				csgo.m_globals()->framecount = simulated_ticks;
//			//				csgo.m_globals()->tickcount = simulated_ticks;
//			//				csgo.m_globals()->interpolation_amount = 0.0f;
//			//
//			//				// force to use correct abs origin and velocity ( no CalcAbsolutePosition and CalcAbsoluteVelocity calls )
//			//				entity->m_iEFlags() &= ~(EFL_DIRTY_ABSTRANSFORM | EFL_DIRTY_ABSVELOCITY);
//			//
//			//				if (i != ticks_to_simulate && resolver_info->resolving_method != 0)
//			//					entity->get_animation_state()->abs_yaw = log->best_record->eye_angles.y + (resolver_info->resolving_method == 1 ? 60 : -60.f);
//			//
//			//				ctx.updating_resolver = true;
//			//				// Update pose parameters and abs rotation
//			//				entity->update_clientside_animations();
//			//				ctx.updating_resolver = false;
//			//
//			//				csgo.m_globals()->realtime = dword_3CF22C70;
//			//				csgo.m_globals()->curtime = dword_3CF22C74;
//			//				csgo.m_globals()->frametime = dword_3CF22C78;
//			//				csgo.m_globals()->absoluteframetime = dword_3CF22C7C;
//			//				csgo.m_globals()->interpolation_amount = dword_3CF22C80;
//			//				csgo.m_globals()->framecount = dword_3CF22C84;
//			//				csgo.m_globals()->tickcount = dword_3CF22C88;
//			//
//			//				if (i >= ticks_to_simulate)
//			//				{
//			//					//save_data();
//			//					break;
//			//				}
//			//
//			//				auto delta_origin = log->tick_records[0].origin.DistanceSquared(data.origin);
//			//
//			//				if (total_predicted >= time_ticks && delta_origin < 4096.f /*&& log->best_record->breaking_lc*/)
//			//					break;
//			//			}
//			//
//			//			// build valid bones matrix
//			//			entity->force_bone_rebuild();
//			//			entity->SetupBonesEx();
//			//
//			//			// restore server anim overlays
//			//			std::memcpy(entity->animation_layers_ptr(), layers, min(13, entity->get_animation_layers_count()) * sizeof(C_AnimationLayer));
//			//
//			//			entity->m_angEyeAngles() = backup_angles;
//			//			entity->m_vecAbsVelocity() = absvel;
//			//			entity->m_flOldSimulationTime() = oldsim;
//			//
//			//			// setup extrapolated recorrd
//			//			log->best_record->simulation_time = entity->m_flSimulationTime();
//			//			log->best_record->origin = data.origin;
//			//			log->best_record->velocity = data.velocity;
//			//			log->best_record->extrapolated = true;
//			//
//			//			std::memcpy((resolver_info->resolving_method == 0 ? log->best_record->matrixes : (resolver_info->resolving_method == 1 ? log->best_record->leftmatrixes : log->best_record->rightmatrixes)),
//			//				entity->m_CachedBoneData().Base(),
//			//				entity->GetBoneCount() * sizeof(matrix3x4_t));
//			//
//			//			// restore animstate
//			//			*entity->get_animation_state() = animState;
//			//
//			//			csgo.m_globals()->curtime = curtime;
//			//			csgo.m_globals()->frametime = frametime;
//			//		}
//			//	}
//			//}
//
//			//const auto can_stop = first_valid_record > 4 && best_index > 4;
//
//			log->hitboxes_positions[HITBOX_HEAD] = feature::ragebot->get_hitbox(entity, HITBOX_HEAD, entity->m_CachedBoneData().Base());
//			ctx.shot_angles = Math::CalcAngle(ctx.m_eye_position, log->hitboxes_positions[HITBOX_HEAD]).Clamped();
//
//			//ctx.shot_angles.x += (ctx.shot_angles.x - prev_shot_ang) * csgo.m_globals()->interval_per_tick;
//
//			//if (ctx.m_settings.aimbot_position_adjustment && feature::lagcomp->StartPrediction(entity, log))
//			//{
//			//	if (!entity->IsBot())
//			//		log->tick_records[0]->desync_delta = entity->get_bone_pos(8, log->tick_records[0]->leftmatrixes).DistanceSquared(entity->get_bone_pos(8, log->tick_records[0]->matrixes));
//			//	else
//			//		log->tick_records[0]->desync_delta = 58.f;
//
//			//	log->hitboxes_positions[HITBOX_BODY] = feature::ragebot->get_hitbox(entity, HITBOX_BODY, entity->m_CachedBoneData().Base());
//
//			//	log->tick_records[0]->rtype = resolver_info->resolving_method;
//
//			//	log->tick_records[0]->apply(entity, false);
//
//			//	bool was_viable = false;
//
//			//	log->hitboxes_damage[HITBOX_BODY] = feature::ragebot->can_hit(HITBOX_BODY, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_BODY], true);
//			//	//log->hitboxes_damage[HITBOX_PELVIS] = feature::ragebot->can_hit(HITBOX_PELVIS, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_PELVIS], true);
//			//	//if (ctx.current_tickcount % 2 == 1 && ppl <= 1)
//			//	//ctx.force_hitbox_penetrate_accuracy = true;
//			//	log->hitboxes_damage[HITBOX_UPPER_CHEST] = feature::ragebot->can_hit(HITBOX_UPPER_CHEST, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_UPPER_CHEST], true);
//			//	log->hitboxes_damage[HITBOX_HEAD] = feature::ragebot->can_hit(HITBOX_HEAD, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_HEAD], true);
//
//			//	//ctx.force_hitbox_penetrate_accuracy = false;
//			//	if (ctx.current_tickcount % 2 == 0 && ppl <= 2) {
//			//		log->hitboxes_damage[HITBOX_LEFT_FOOT] = feature::ragebot->can_hit(HITBOX_LEFT_FOOT, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_LEFT_FOOT], true);
//			//		log->hitboxes_damage[HITBOX_RIGHT_FOOT] = feature::ragebot->can_hit(HITBOX_RIGHT_FOOT, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_RIGHT_FOOT], true);
//			//	}
//
//			//	auto dist = log->hitboxes_positions[HITBOX_BODY].DistanceSquared(ctx.m_eye_position);//ctx.m_settings.aimbot_low_fps_optimizations
//
//			//	last_hitbox_pos = log->tick_records[0]->head_pos;
//
//			//	auto max_damage = max(log->hitboxes_damage[HITBOX_HEAD], max(log->hitboxes_damage[HITBOX_BODY], max(log->hitboxes_damage[HITBOX_PELVIS], max(log->hitboxes_damage[HITBOX_UPPER_CHEST], max(log->hitboxes_damage[HITBOX_LEFT_FOOT], log->hitboxes_damage[HITBOX_RIGHT_FOOT])))));
//
//			//	if ((max_damage > best_damage /*|| (point_damage >= best_damage && log->best_record != nullptr && record.simulation_time > log->best_record->simulation_time && !had_anims_update && !had_shot_rec)*/) || ((max_damage >= entity->m_iHealth() || max_damage >= best_damage) && best_delta > record->desync_delta) || (is_zeus && best_distance < dist))
//			//	{
//			//		best_damage = max_damage;
//			//		best_distance = dist;
//			//		best_delta = record->desync_delta;
//			//		log->best_record = record;
//			//	}
//			//	else {
//			//		if (log->restore_record)
//			//			log->restore_record->apply(entity, true);
//
//			//		continue;
//			//	}
//			//}
//
//			if (force_backtrack && (!log->best_record || best_damage < 1 && !is_zeus || best_distance > 200.f && is_zeus)) {
//				log->restore_record.apply(entity, true);
//				continue;
//			}
//
//			//auto can_stop = best_pred_dmg > 1.f/*!(abs(best_index - first_valid_record) <= local_vel && (best_index > 4)) && local_vel > 0 || local_vel <= 0*/;
//
//			auto viable_dmg = min(ctx.m_settings.aimbot_min_damage_viable, hp);
//			auto wall_dmg = min(ctx.m_settings.aimbot_min_damage, hp);
//
//			if (dmg_override) {
//				viable_dmg = min(hp, ctx.m_settings.aimbot_min_damage_override_val);
//				wall_dmg = min(hp, ctx.m_settings.aimbot_min_damage_override_val);
//			}
//
//			if (best_damage >= 1)
//			{
//				if (ctx.ticks_allowed < 13)
//					ctx.last_speedhack_time = csgo.m_globals()->realtime;
//			}
//
//			//log->only_old_shit_viable = max((int)log->tick_records.size() - 4, 6) < k && ;
//
//			if (best_damage >= min(viable_dmg, wall_dmg) && !is_zeus || best_distance < 200.f && is_zeus) {
//				if (!is_zeus)
//					ctx.do_autostop = (!ctx.m_settings.aimbot_autostop_options[1] && ctx.m_settings.aimbot_autostop) || (ctx.m_settings.aimbot_autostop && (local_weapon->can_shoot() /*|| !local_weapon->can_exploit(10)*/ || local_weapon->m_iItemDefinitionIndex() == 64));
//
//				m_entities.push_back(TargetListing_t{ entity, best_damage });
//			}
//
//			//if (log->restore_record)
//			log->restore_record.apply(entity, true);
//
//			////entity passed all checks and now we can aim at it.
//			//if (log->best_record != nullptr) {
//
//			//	/*if ((log->best_record->abs_origin + log->best_record->object_maxs).z < entity->get_bone_pos(8).z) {
//			//		must_baim_player[idx] = true;
//			//	}*/
//
//			//	m_entities.push_back(TargetListing_t(entity));
//			ppl++;
//			//}
//		}
//
//		if (!m_entities.empty() && m_entities.size() > 0) {
//			std::sort(m_entities.begin(), m_entities.end(), [&](const TargetListing_t& a, const TargetListing_t& b)
//				{
//					//if (fabs(previous_player - a.idx) < fabs(previous_player - b.idx))
//					//	return true;
//
//					switch (ctx.m_settings.aimbot_target_selection)
//					{
//					case 0:
//						return (a.distance < b.distance);
//						break;
//					case 1:
//						return (a.fov < b.fov);
//						break;
//					case 2:
//						return (a.damage < b.damage) || (a.damage * 1.1f <= b.damage && a.distance < b.distance);
//						break;
//					case 3:
//						return (a.hp < b.hp) || (a.hp * 1.1f >= b.hp && a.distance < b.distance);
//						break;
//					case 4:
//						return (a.height < b.height);
//						break;
//					default:
//						return (a.hp < b.hp) || (a.hp * 1.1f >= b.hp && a.distance < b.distance);
//						break;
//					}
//				}
//			);
//		}
//	}
//
//	previous_player = -1;
//
//	if (!m_entities.empty())
//		ctx.do_autostop = (!ctx.m_settings.aimbot_autostop_options[1] && ctx.m_settings.aimbot_autostop) || (ctx.m_settings.aimbot_autostop && (local_weapon->can_shoot() /*|| !local_weapon->can_exploit(10)*/ || local_weapon->m_iItemDefinitionIndex() == 64));
//
//	if (ctx.do_autostop && local_weapon && (!local_weapon->IsGun() || is_zeus))
//		ctx.do_autostop = false;
//
//	autostop(cmd, *send_packet, local_weapon);
//
//	if (ctx.allow_shooting > cmd->command_number)
//		return false;
//
//	if (local_weapon->is_knife())
//		return knife(send_packet, m_entities, cmd);
//
//	if (m_entities.empty() || m_entities.size() < 1/* || ctx.local_spawntime != ctx.m_local()->m_flSpawnTime()*/) {
//		//feature::lagcomp->finish_position_adjustment();
//		return false;
//	}
//
//	static int hitboxesLoop[] =
//	{
//		HITBOX_HEAD,
//		HITBOX_NECK,
//		HITBOX_PELVIS,
//		HITBOX_BODY,
//		HITBOX_THORAX,
//		HITBOX_CHEST,
//		HITBOX_UPPER_CHEST,
//		//HITBOX_RIGHT_THIGH,
//		//HITBOX_LEFT_THIGH,
//		//HITBOX_LEFT_CALF,
//		//HITBOX_RIGHT_CALF,
//		HITBOX_LEFT_FOOT,
//		HITBOX_RIGHT_FOOT,
//		HITBOX_LEFT_HAND,
//		HITBOX_RIGHT_HAND,
//		HITBOX_LEFT_UPPER_ARM,
//		HITBOX_LEFT_FOREARM,
//		HITBOX_RIGHT_UPPER_ARM,
//		HITBOX_RIGHT_FOREARM
//	};
//
//	float max_damage = 0.1f;
//
//	const auto loh = ctx.m_settings.aimbot_hitboxes[0] && (!ctx.m_settings.aimbot_hitboxes[1]
//		&& !ctx.m_settings.aimbot_hitboxes[2]
//		&& !ctx.m_settings.aimbot_hitboxes[3]
//		&& !ctx.m_settings.aimbot_hitboxes[4]
//		&& !ctx.m_settings.aimbot_hitboxes[5]);
//
//	baim = false;
//
//	
//	//for (auto k = 1; k < 64; k++)
//	for (const auto& target : m_entities)
//	{
//		//auto entity = (C_BasePlayer*)csgo.m_entity_list()->GetClientEntity(k);
//		auto* entity = target.entity;
//
//		if (!entity ||
//			entity->IsDormant() ||
//			!entity->IsPlayer() ||
//			entity->m_bGunGameImmunity() ||
//			entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum() ||
//			entity->IsDead() ||
//			!entity->GetClientClass())
//		{
//			continue;
//		}
//
//		/*if (players_with_damage == 1)
//		{
//			auto was_skipped = &skip_player[damageable_player];
//
//			if (was_skipped)
//			{
//				--players_skipped;
//				was_skipped = 0;
//			}
//		}*/
//
//		//if (ctx.m_settings.aimbot_low_fps_optimizations[0] && m_entities.size() > 1 && players_skipped >= (m_entities.size() - 1)) {
//		//	memset(skip_player, 0, sizeof(int) * 64);
//		//	players_skipped = 0;
//		//}
//
//		const auto idx = entity->entindex() - 1;
//
//		if (!entity->get_animation_state())
//			continue;
//
//		//if (!ctx.double_tapped || ctx.m_settings.aimbot_tickbase_exploit != 2) {
//		//if (ctx.m_settings.aimbot_low_fps_optimizations[0] && m_entities.size() > 1 && players_skipped > 0 && skip_player[idx] != 0)
//		//	continue;
//		//}
//
//		int loopsize = ARRAYSIZE(hitboxesLoop) - 1;
//
//		feature::lagcomp->records[idx].tick_count = -1;
//
//		const float maxRange = ctx.latest_weapon_data ? ctx.latest_weapon_data->range : 3000;
//		const float hp = min(100, entity->m_iHealth());
//
//		//if (is_zeus)
//		//	max_damage = (hp - 0.1f);
//
//		resolver_records* resolver_log = &feature::resolver->player_records[idx];
//		c_player_records* log = &feature::lagcomp->records[idx];
//
//		if (!log || log->records_count < 1 || !log->best_record || log->player != entity) {
//			//if (log.restore_record)
//				log->restore_record.apply(entity, true);
//			continue;
//		}
//
//		const auto distance = log->best_record->origin.Distance(ctx.m_local()->m_vecOrigin());
//
//		//auto left_bone_pos = entity->get_bone_pos(8, log.best_record->leftmatrixes);
//
//		//auto attarget_diff = Math::normalize_angle(Math::AngleDiff(Math::CalcAngle(log.best_record->abs_origin, ctx.m_local()->get_abs_origin()).y, log.best_record->eye_angles.y));
//		//auto delta1 = !is_point_in_bbox(entity, left_bone_pos, log.best_record->abs_origin, log.best_record->object_maxs) && (abs(attarget_diff) <= 15.f || abs(attarget_diff) >= 165.f);
//
//		/*if (log.best_record->shot_this_tick && log.tick_records.size() > 2)
//		{
//			resolver_log->resolving_method = (int(resolver_log->resolving_method != 2) + 1);
//			resolver_log->resolver_type = 99;
//		}*/
//
//		//log.best_record->resolver_index = resolver_log->resolving_method;
//
//		if (distance > maxRange || is_zeus && distance > (180.0f - (float(hp) / 10))) {
//			//feature::lagcomp->finish_position_adjustment(entity);
//			//if (log.restore_record)
//			log->restore_record.apply(entity, true);
//			
//			continue;
//		}
//
//		if (ctx.shots_fired[idx] < 3 && log->best_record->resolver_type != 4 && !log->best_record->shot_this_tick)
//		{
//			float at_target = Math::normalize_angle(Math::CalcAngle(log->best_record->origin, ctx.m_eye_position).y);
//
//			if (at_target < 0.f)
//				at_target += 360.f;
//
//			auto some_delta = Math::AngleDiff(at_target, log->best_record->eye_angles.y);
//
//			if (some_delta <= 60.f && some_delta >= -60.f)
//			{
//				log->best_record->resolver_index = (int(log->best_record->resolver_index != 2) + 1);
//				log->best_record->resolver_type = 9;
//			}
//		}
//
//		log->best_record->apply(entity, false);
//
//		log->tick_count = TIME_TO_TICKS(log->best_record->simulation_time + ctx.lerp_time);
//		log->backtrack_ticks = (csgo.m_globals()->tickcount - TIME_TO_TICKS(log->best_record->simulation_time));
//		//log->head_position = get_hitbox(entity, HITBOX_HEAD, entity->m_CachedBoneData().Base());
//
//		if (is_zeus)
//			loopsize = 5;
//
//		ctx.points[entity->entindex() - 1][HITBOX_CHEST].clear();
//		ctx.points[entity->entindex() - 1][HITBOX_UPPER_CHEST].clear();
//		ctx.points[entity->entindex() - 1][HITBOX_PELVIS].clear();
//		
//		ctx.optimized_point_search = true;
//		log->hitboxes_damage[HITBOX_UPPER_CHEST] = feature::ragebot->can_hit(HITBOX_UPPER_CHEST, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_UPPER_CHEST]);
//		log->hitboxes_damage[HITBOX_PELVIS] = feature::ragebot->can_hit(HITBOX_PELVIS, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_PELVIS]);
//		ctx.optimized_point_search = false; 
//		log->hitboxes_damage[HITBOX_CHEST] = feature::ragebot->can_hit(HITBOX_CHEST, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_CHEST]);
//
//		//if (ctx.current_tickcount % 2 == 1)
//		//log->hitboxes_damage[HITBOX_UPPER_CHEST] = feature::ragebot->can_hit(HITBOX_UPPER_CHEST, entity, entity->m_CachedBoneData().Base(), &log->hitboxes_viable[HITBOX_UPPER_CHEST], true);
//
//		const auto baim_dmg = max(log->hitboxes_damage[HITBOX_UPPER_CHEST], max(max(log->hitboxes_damage[HITBOX_BODY], log->hitboxes_damage[HITBOX_CHEST]), log->hitboxes_damage[HITBOX_PELVIS]));
//
//		/*auto bhitgroup = feature::autowall->HitboxToHitgroup(entity, (log->hitboxes_damage[HITBOX_UPPER_CHEST] == baim_dmg) ? HITBOX_UPPER_CHEST 
//			: ((log->hitboxes_damage[HITBOX_BODY] == baim_dmg) ? HITBOX_BODY 
//			: ((log->hitboxes_damage[HITBOX_CHEST] == baim_dmg) ? HITBOX_CHEST : HITBOX_PELVIS)));*/
//
//		//auto prefered_hitbox = HITBOX_HEAD;
//
//		/*auto pred = (ctx.m_eye_position + (ctx.m_local()->m_vecVelocity() * max(csgo.m_globals()->interval_per_tick, TICKS_TO_TIME(1))));
//		bool can_baim_next_tick = false;*/
//		//float pred_dmg = 0.f;
//
//		//if (ctx.current_tickcount % 2 == 1) {
//		//ctx.force_low_quality_autowalling = true;
//		//pred_dmg = feature::autowall->CanHit(pred, log->hitboxes_positions[HITBOX_BODY], ctx.m_local(), entity, HITBOX_BODY);
//		//ctx.force_low_quality_autowalling = false;
//		//can_baim_next_tick = (pred_dmg >= hp || (pred_dmg > (local_weapon->m_iItemDefinitionIndex() != WEAPON_SSG08 ? hp * 0.5 : hp * 0.7))) && ctx.m_settings.aimbot_prefer_body/*&& hp < 80.f*/;
//		//}
//
//		/*if ((can_baim_next_tick && max(log->hitboxes_damage[HITBOX_BODY], log->hitboxes_damage[HITBOX_PELVIS]) >= (hp * 0.7)) && ctx.m_settings.aimbot_prefer_body)
//			prefered_hitbox = HITBOX_BODY;*/
//
//		int chest_chance = 0;
//		mini_hit_chance(log->hitboxes_positions[HITBOX_CHEST], entity, HITBOX_CHEST, chest_chance);
//
//		int hs_chance = 0;
//		mini_hit_chance(log->hitboxes_positions[HITBOX_HEAD], entity, HITBOX_HEAD, hs_chance);
//
//		//if (ctx.m_settings.aimbot_headaim[0] && log->best_record->shot_this_tick /*&& log->best_record->shot_time > log->best_record->simulation_time_ol*/)
//		//	prefered_hitbox = HITBOX_HEAD;
//		//if (ctx.m_settings.aimbot_headaim[1] && (log->best_record->velocity.Length2D() > 150.0 || ctx.m_settings.aimbot_position_adjustment_old && log->best_record->velocity.Length2D() > 10.0))
//		//	prefered_hitbox = HITBOX_HEAD;
//		//if (ctx.m_settings.aimbot_headaim[2] && ((log->best_record->desync_delta < 35.0 && log->best_record->resolver_index != 0 || ctx.m_settings.aimbot_position_adjustment_old && log->best_record->lag < 1) || entity->IsBot()))
//		//	prefered_hitbox = HITBOX_HEAD;
//		//if (ctx.m_settings.aimbot_headaim[3] && ((baim_dmg * 1.4) <= log->hitboxes_damage[HITBOX_HEAD] || hp <= log->hitboxes_damage[HITBOX_HEAD] && (chest_chance >= hs_chance)))
//		//	prefered_hitbox = HITBOX_HEAD;
//		//if (ctx.m_settings.aimbot_headaim[4] && (log->best_record->animations_updated || log->best_record->desync_delta <= 35.0 || log->best_record->resolver_index == resolver_log->last_hurt_resolved))
//		//	prefered_hitbox = HITBOX_HEAD;
//
//		//C_Hitbox box; feature::ragebot->get_hitbox_data(&box, entity, HITBOX_HEAD, entity->m_CachedBoneData().Base());
//
//		//const auto angle = Math::CalcAngle(ctx.m_eye_position, hitbox_neck);
//		//Vector forward;
//		//Math::AngleVectors(angle, &forward);
//		//auto end = ctx.m_eye_position + forward * 8096.f;
//
//		//if (safe_point(entity, ctx.m_eye_position, hitbox_neck, HITBOX_HEAD, log->best_record))//Math::Intersect(ctx.m_eye_position, end, box.mins, box.maxs, (box.radius * 0.60f)) && !loh && !(can_baim_next_tick && ctx.m_settings.aimbot_prefer_body) /*&& !log->best_record->shot_this_tick*/ && !bodyaim)
//		//	prefered_hitbox = HITBOX_NECK;
//
//		//if (max(log->hitboxes_damage[HITBOX_BODY], log->hitboxes_damage[HITBOX_PELVIS]) >= hp && !loh)
//		//	prefered_hitbox = HITBOX_BODY;
//
//		//if (!loh && (ctx.shots_fired[idx] > 1 || abs(ctx.shots_fired[idx] - ctx.shots_total[idx]) > 1))
//		//	prefered_hitbox = (max(log->hitboxes_damage[HITBOX_BODY], log->hitboxes_damage[HITBOX_PELVIS]) ? HITBOX_BODY : HITBOX_PELVIS);
//
//		//if (loh)
//		//	prefered_hitbox = HITBOX_HEAD;
//
//		//if (is_zeus ||/* max(stomach_damage, pelvis_damage) < chest_damage ||*/ (can_baim_next_tick && pred_dmg >= hp || max(log->hitboxes_damage[HITBOX_BODY], log->hitboxes_damage[HITBOX_PELVIS]) >= hp) && ctx.m_settings.aimbot_prefer_body /*|| ctx.m_settings.aimbot_ignore_head*/ || bodyaim)
//		//	prefered_hitbox = (log->hitboxes_damage[HITBOX_BODY] < log->hitboxes_damage[HITBOX_PELVIS] ? HITBOX_PELVIS : HITBOX_BODY);
//
//		/*if (must_baim_player[idx])
//		{
//			if (!loh)
//				prefered_hitbox = HITBOX_BODY;
//			else {
//				log->best_record->leftmatrixes[8][2][3] -= 4.f;
//				log->best_record->rightmatrixes[8][2][3] -= 4.f;
//				log->best_record->matrixes[8][2][3] -= 4.f;
//				entity->m_CachedBoneData().Base()[8][2][3] -= 4.f;
//			}
//		}*/
//
//		//auto phitbox = get_hitbox(entity, prefered_hitbox, entity->m_CachedBoneData().Base());
//		//float pdmg = 0.f;
//
//		//bool prior_was_viable = false;
//
//		//switch (prefered_hitbox)
//		//{
//		//case HITBOX_HEAD:
//		//	pdmg = head_damage;
//		//	prior_was_viable = head_was_viable;
//		//	break;
//		//case HITBOX_NECK:
//		//	pdmg = neck_damage;
//		//	prior_was_viable = neck_was_viable;
//		//	break;*/
//		//case HITBOX_BODY:
//		//	pdmg = stomach_damage;
//		//	prior_was_viable = stomach_was_viable;
//		//	break;
//		//case HITBOX_PELVIS:
//		//	pdmg = pelvis_damage;
//		//	prior_was_viable = pelvis_was_viable;
//		//	break;
//		//case HITBOX_CHEST:
//		//	pdmg = chest_damage;
//		//	prior_was_viable = chest_was_viable;
//		//	break;
//		//default:
//		//	pdmg = can_hit(prefered_hitbox, entity, phitbox, entity->m_CachedBoneData().Base(), &prior_was_viable, true);
//		//	break;
//		//}
//
//		auto viable_dmg = (hp > ctx.m_settings.aimbot_min_damage_viable || !ctx.m_settings.aimbot_scale_damage_on_hp) ? ctx.m_settings.aimbot_min_damage_viable : hp;
//		const auto wall_dmg = (hp > ctx.m_settings.aimbot_min_damage || !ctx.m_settings.aimbot_wall_scale_damage_on_hp) ? ctx.m_settings.aimbot_min_damage : hp;
//		
//		//if (dmg_override)
//		//	viable_dmg = min(hp, ctx.m_settings.aimbot_min_damage_override_val);
//		//auto good_prior_dmg = (prior_was_viable ? (pdmg > max_damage && pdmg > viable_dmg) : (pdmg > max_damage && pdmg > wall_dmg && ctx.m_settings.aimbot_autowall));
//		//
//		//if (dmg_override)
//		//	good_prior_dmg = pdmg > min(hp, ctx.m_settings.aimbot_min_damage_override_val);	
//		//
//		//int phc = 0;
//		//
//		//if (!mini_hit_chance(phitbox, entity, prefered_hitbox, phc)) {
//		//	log->restore_record.apply(entity, true);
//		//	continue;
//		//}
//		//
//		//if (!phitbox.IsZero() && good_prior_dmg && phc > 80)
//		//{
//		//	max_damage = pdmg;
//		//	best_player = entity;
//		//	best_hitbox = phitbox;
//		//	best_hitboxid = prefered_hitbox;
//		//}
//		//else
//		//{
//
//		//C_Hitbox box; feature::ragebot->get_hitbox_data(&box, entity, HITBOX_UPPER_CHEST, entity->m_CachedBoneData().Base());
//		//auto intersect = Math::Intersect(ctx.m_eye_position, get_hitbox(entity, HITBOX_NECK, entity->m_CachedBoneData().Base()), box.mins, box.maxs, box.radius);
//
//		//if (ctx.m_settings.aimbot_low_fps_optimizations[2] && m_entities.size() > 2)
//		//	loopsize = 6;
//
//		//feature::autowall->ScaleDamage(entity, bhitgroup, ctx.latest_weapon_data->armor_ratio, ctx.latest_weapon_data->damage);
//
//		const auto can_not_baim = (log->best_record->shot_this_tick || entity->get_weapon() && !entity->get_weapon()->can_shoot() || log->best_record->desync_delta < 45.f && (int(baim_dmg) < min(hp, log->hitboxes_damage[HITBOX_HEAD]) && ctx.m_settings.aimbot_prefer_body)) && baim_dmg < (min(viable_dmg, min(wall_dmg, hp))) /*&& hs_chance >= 0.83f */&& log->hitboxes_damage[HITBOX_HEAD] > (min(viable_dmg, min(wall_dmg, hp)));
//
//		auto should_baim = !loh && (ctx.shots_fired[idx] > 2 && !can_not_baim
//			//|| /*abs(ctx.shots_fired[idx] - */ctx.shots_total[idx]/*)*/ > 2 && !can_not_baim
//			|| bodyaim
//			|| (baim_dmg >= hp || baim_dmg >= (hp*0.9f)/*(baim_dmg * 1.4f) > log->hitboxes_damage[HITBOX_HEAD])*/) && ctx.m_settings.aimbot_extra_scan_aim[0]
//			|| hs_chance <= (chest_chance * 0.8f) && !can_not_baim && ctx.m_settings.aimbot_extra_scan_aim[1]
//			|| min(91, int(baim_dmg * 1.09f)) >= min(hp, log->hitboxes_damage[HITBOX_HEAD]) && ctx.m_settings.aimbot_prefer_body
//			|| log->hitboxes_damage[HITBOX_HEAD] <= 0 && ctx.m_settings.aimbot_prefer_body);
//
//		if (dmg_override && !bodyaim)
//			should_baim = baim_dmg >= hp;
//
//		int max_hc = 0;
//		bool find_anything = false;
//		for (auto i = loopsize; i >= 0 ; i--)
//		{
//			if (i > loopsize || i < 0)
//				continue;
//			
//			if (!ctx.m_settings.aimbot_hitboxes[0] && hitboxesLoop[i] <= HITBOX_NECK
//				|| !ctx.m_settings.aimbot_hitboxes[1] && (HITBOX_CHEST == hitboxesLoop[i] || HITBOX_UPPER_CHEST == hitboxesLoop[i])
//				|| !ctx.m_settings.aimbot_hitboxes[2] && (hitboxesLoop[i] >= HITBOX_PELVIS && hitboxesLoop[i] <= HITBOX_THORAX)
//				|| !ctx.m_settings.aimbot_hitboxes[3] && hitboxesLoop[i] >= HITBOX_LEFT_HAND
//				|| !ctx.m_settings.aimbot_hitboxes[4] && (hitboxesLoop[i] >= HITBOX_RIGHT_THIGH && hitboxesLoop[i] <= HITBOX_LEFT_CALF)
//				|| !ctx.m_settings.aimbot_hitboxes[5] && (hitboxesLoop[i] == HITBOX_RIGHT_FOOT || hitboxesLoop[i] == HITBOX_LEFT_FOOT))
//				continue;
//
//			if (is_zeus && (hitboxesLoop[i] <= 1 || hitboxesLoop[i] >= 11))
//				continue;
//
//			if (loh && hitboxesLoop[i] != 0 && !is_zeus)
//				continue;
//			//if (!resolver_log->has_been_resolved && agressive_baim && (baim_dmg > hp || baim_dmg > (hp * 0.5f)) && (hitboxesLoop[i] <= 1/* || hitboxesLoop[i] == HITBOX_UPPER_CHEST*/))
//			//	continue;
//
//			/*if ((should_baim || bodyaim) && hitboxesLoop[i] < 1)
//				continue;*/
//
//			//if (hitboxesLoop[i] == 1 && (ctx.m_local()->m_vecOrigin().z < entity->m_vecOrigin().z || intersect))
//			//	continue;
//
//			//{ "head", "chest", "body", "arms", "legs", "foot" }
//
//			auto hitbox = get_hitbox(entity, hitboxesLoop[i], entity->m_CachedBoneData().Base());
//
//			if (hitbox.IsZero())
//				continue;
//
//			auto dmg = 0.f;
//			bool was_viable = false;
//			int hc = 0;
//
//			if (!mini_hit_chance(hitbox, entity, hitboxesLoop[i], hc))
//				continue;
//
//			//switch (hitboxesLoop[i])
//			//{
//			//case HITBOX_HEAD:
//			//	dmg = head_damage;
//			//	was_viable = head_was_viable;
//			//	break;
//			//case HITBOX_NECK:
//			//	dmg = neck_damage;
//			//	was_viable = neck_was_viable;
//			//	break;
//			//case HITBOX_BODY:
//			//	dmg = stomach_damage;
//			//	was_viable = stomach_was_viable;
//			//	break;
//			//case HITBOX_PELVIS:
//			//	dmg = pelvis_damage;
//			//	was_viable = pelvis_was_viable;
//			//	break;
//			//case HITBOX_CHEST:
//			//	dmg = chest_damage;
//			//	was_viable = chest_was_viable;
//			//	break;
//			//default:
//			//	dmg = can_hit(hitboxesLoop[i], entity, hitbox, entity->m_CachedBoneData().Base(), &was_viable, true);
//			//	break;
//			//}
//
//			if (hitboxesLoop[i] == HITBOX_HEAD && log->hitboxes_damage[HITBOX_HEAD] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_HEAD];
//				was_viable = log->hitboxes_viable[HITBOX_HEAD];
//			}
//			else if (hitboxesLoop[i] == HITBOX_BODY && log->hitboxes_damage[HITBOX_BODY] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_BODY];
//				was_viable = log->hitboxes_viable[HITBOX_BODY];
//			}
//			else if (hitboxesLoop[i] == HITBOX_PELVIS && log->hitboxes_damage[HITBOX_PELVIS] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_PELVIS];
//				was_viable = log->hitboxes_viable[HITBOX_PELVIS];
//			}
//			else if (hitboxesLoop[i] == HITBOX_LEFT_FOOT && log->hitboxes_damage[HITBOX_LEFT_FOOT] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_LEFT_FOOT];
//				was_viable = log->hitboxes_viable[HITBOX_LEFT_FOOT];
//			}
//			else if (hitboxesLoop[i] == HITBOX_RIGHT_FOOT && log->hitboxes_damage[HITBOX_RIGHT_FOOT] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_RIGHT_FOOT];
//				was_viable = log->hitboxes_viable[HITBOX_RIGHT_FOOT];
//			}
//			else if (hitboxesLoop[i] == HITBOX_CHEST && log->hitboxes_damage[HITBOX_CHEST] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_CHEST];
//				was_viable = log->hitboxes_viable[HITBOX_CHEST];
//			}
//			else if (hitboxesLoop[i] == HITBOX_UPPER_CHEST && log->hitboxes_damage[HITBOX_UPPER_CHEST] > 0)
//			{
//				dmg = log->hitboxes_damage[HITBOX_UPPER_CHEST];
//				was_viable = log->hitboxes_viable[HITBOX_UPPER_CHEST];
//			}
//			else
//			{
//				if (m_entities.size() > 1 || ctx.fps < 40)
//					continue;
//
//				ctx.points[entity->entindex() - 1][hitboxesLoop[i]].clear();
//
//				/*if (log->hitboxes_damage[HITBOX_CHEST] > 0.f && hitboxesLoop[i] == HITBOX_CHEST)
//				{
//					dmg = log->hitboxes_damage[HITBOX_CHEST];
//					was_viable = log->hitboxes_viable[HITBOX_CHEST];
//				}
//				else*/
//				dmg = can_hit(hitboxesLoop[i], entity, entity->m_CachedBoneData().Base(), &was_viable);
//			}
//
//			auto good_dmg = (was_viable ? (dmg > max_damage && dmg >= viable_dmg) : (dmg > max_damage && dmg >= wall_dmg && ctx.m_settings.aimbot_autowall));
//
//			if (dmg_override)
//				good_dmg = dmg > min(hp, ctx.m_settings.aimbot_min_damage_override_val) && dmg > max_damage;
//
//			//if (good_dmg)
//			//	csgo.m_engine_cvars()->ConsoleColorPrintf(Color(255,130,0), "//*DBG*// [%d] : [%.0f] (%.0f) : [%d] (%d)\n", hitboxesLoop[i], dmg, max_damage, hc, max_hc);
//
//			if (good_dmg || (dmg >= min(hp, max_damage) && hc >= int(float(max_hc) * 0.91f) && hitboxesLoop[i] > 0 && hitboxesLoop[i] < 4) && ctx.m_settings.aimbot_extra_scan_aim[1])
//			{
//				max_damage = dmg;
//				best_player = entity;
//				best_hitbox = hitbox;
//				best_hitboxid = hitboxesLoop[i];
//				max_hc = hc;
//				baim = (should_baim || bodyaim);
//				find_anything = true;
//
//				if ((should_baim || bodyaim || ((dmg > hp || ctx.shots_total[idx] > 2) && ctx.m_settings.aimbot_extra_scan_aim[0])
//					|| (dmg > hp || ((dmg * 2.f) > hp && ctx.exploit_allowed && !ctx.fakeducking && ctx.m_settings.aimbot_tickbase_exploit == 3) || !(log->best_record->entity_flags & FL_ONGROUND)) && ctx.m_settings.aimbot_extra_scan_aim[0])
//					&& hitboxesLoop[i] > 1 && (log->hitboxes_damage[HITBOX_BODY] >= hp ? hitboxesLoop[i] < 4 : hitboxesLoop[i] < 6))
//					break;
//			}
//		}
//
//		log->restore_record.apply(entity, true);
//
//		if (find_anything)
//			break;
//	}
//
//	float max_dmg = 0.1;
//	//int walls_penetrated = 0;
//	//bool delay = false;
//
//	//Vector origin = ctx.m_local()->m_vecOrigin();
//
//	/*Vector vDuckHullMin = csgo.m_movement()->GetPlayerMins(true);
//	Vector vStandHullMin = csgo.m_movement()->GetPlayerMins(false);
//
//	float fMore = (vDuckHullMin.z - vStandHullMin.z);
//
//	Vector vecDuckViewOffset = csgo.m_movement()->GetPlayerViewOffset(true);
//	Vector vecStandViewOffset = csgo.m_movement()->GetPlayerViewOffset(false);
//
//	auto delta = (ctx.m_local()->m_flDuckAmount() - Engine::Prediction::Instance()->GetDuckAmount()) * csgo.m_globals()->interval_per_tick;
//
//	float duckFraction = min(1.f, ctx.m_local()->m_flDuckAmount() + delta);
//
//	float tempz = ((vecDuckViewOffset.z - fMore) * duckFraction) +
//		(vecStandViewOffset.z * (1 - duckFraction));*/
//
//	//origin.z += tempz;
//
//	//factor.z = 0.f;
//
//
//	//if (delta > 0.f/* && !ctx.fakeducking*/)
//	//	pred.z = tempz;
//
//	if (best_player != nullptr && best_hitboxid != -1/* && m_entities.size() > 0*/)
//	{
//		c_player_records* log = &feature::lagcomp->records[best_player->entindex() - 1];
//
//		//if (m_entities.size() <= 0 || !log || log->player == nullptr || log->tick_records.empty() || !log->best_record || log->player != best_player/* || feature::lagcomp->is_time_delta_too_large(log->best_record)*/)
//		//	return false;
//		
//		log->best_record->apply(best_player, false);
//
//		bool center = false;
//
//		if (best_player->get_multipoints(best_hitboxid, ctx.points[best_player->entindex() - 1][best_hitboxid], best_player->m_CachedBoneData().Base(), center))
//		{
//			best_hitbox.clear();
//
//			//auto resolve_info = &feature::resolver->player_records[best_player->entindex() - 1];
//
//			auto points = &ctx.points[best_player->entindex() - 1][best_hitboxid];
//			//bool center_was_viable = false;
//			//auto cwall = feature::autowall->CanHit(ctx.m_eye_position, points.front(), ctx.m_local(), best_player, best_hitboxid, &center_was_viable);
//			const auto hp = float(min(100, best_player->m_iHealth()));
//
//			best_hitbox = points->front();
//
//			//auto angle = Math::CalcAngle(ctx.m_eye_position, points.front());
//			//angle.Clamp();
//
//			//int cchance = 0;
//
//			auto viable_dmg = (hp > ctx.m_settings.aimbot_min_damage_viable || !ctx.m_settings.aimbot_scale_damage_on_hp) ? ctx.m_settings.aimbot_min_damage_viable : hp;
//			const auto wall_dmg = (hp > ctx.m_settings.aimbot_min_damage || !ctx.m_settings.aimbot_wall_scale_damage_on_hp) ? ctx.m_settings.aimbot_min_damage : hp;
//
//			//float best_non_safepoint_dmg = 0.f;
//
//			//ctx.multi_points[best_player->entindex() - 1][best_hitboxid].clear();
//
//			//if (((cwall >= hp || (cwall * 2.f) >= hp))
//			//	&& (ctx.last_penetrated_count >= 3 || center_was_viable) 
//			//	&& (best_hitboxid == HITBOX_BODY || best_hitboxid == HITBOX_CHEST)
//			//	&& (safe_point(best_player, ctx.m_eye_position, points.front(), best_hitboxid, log->best_record) || safe_side_point(best_player, ctx.m_eye_position, points.front(), best_hitboxid, log->best_record)))
//			//{
//			//	//ctx.multi_points[best_player->entindex() - 1][best_hitboxid].push_back({ points.back(), (int)cwall });
//			//	
//			//	best_hitbox = points.front();
//			//	max_dmg = cwall;
//			//	walls_penetrated = abs(4 - ctx.last_penetrated_count);
//			//}
//			//else
//			//{
//				//int max_hc = 1;
//				float dist = 9999;
//				//float hdist = 9999.f;
//				bool had_safe_point = false;
//				const auto best_player_eyepos = best_player->GetEyePosition();
//
//				//bool force_safepoints = log->best_record->desync_delta <= 37.f || baim && best_hitboxid >= 0 && best_hitboxid <= HITBOX_UPPER_CHEST && ctx.m_settings.aimbot_extra_scan_aim[2] || best_hitboxid > HITBOX_UPPER_CHEST && ctx.m_settings.aimbot_extra_scan_aim[3];
//
//				for (auto i = 0; i < (int)points->size(); i++)
//				{
//					Vector &point = points->at(i);
//					auto hc = 0;
//
//					//if (!mini_hit_chance(point, best_player, best_hitboxid, hc))
//					//	continue;
//
//					bool was_viable = false;
//					//ctx.force_hitbox_penetration_accuracy = (best_hitboxid == HITBOX_UPPER_CHEST|| best_hitboxid == HITBOX_CHEST);
//					//ctx.force_hitbox_penetration_accuracy = true;
//					const auto dmg = feature::autowall->CanHit(ctx.m_eye_position, point, ctx.m_local(), best_player, best_hitboxid, &was_viable);
//					//ctx.force_hitbox_penetration_accuracy = false;
//					//ctx.force_hitbox_penetration_accuracy = false;
//
//					//auto angle = Math::CalcAngle(ctx.m_eye_position, point);
//					//angle.Clamp();
//
//					//if (best_hitboxid == HITBOX_HEAD && ctx.last_hitgroup != HITGROUP_HEAD || ctx.last_hitgroup == HITGROUP_HEAD && best_hitboxid > 1/* && !is_safe_point*/)
//					//	continue;
//
//					//const auto hdist = point.DistanceSquared(log.head_position);
//						
//					//if ((hdist > (dist * 2)) && best_hitboxid == 1 && had_points)
//					//	continue;
//
//					auto is_safe_side_point = ctx.m_settings.aimbot_extra_scan_aim[4] ? false : safe_side_point(best_player, ctx.m_eye_position, point, best_hitboxid, log->best_record);
//					auto is_safe_point = safe_point(best_player, ctx.m_eye_position, point, best_hitboxid, log->best_record);
//
//					auto eyedist = best_player_eyepos.DistanceSquared(point);
//					//auto headdist = log->hitboxes_positions[HITBOX_HEAD].DistanceSquared(point);
//
//					/*if (!is_safe_point && ctx.m_settings.aimbot_low_fps_optimizations[1] && (i + 1) < points.size())
//					{
//						const auto hdist = point.DistanceSquared(points[i+1]);
//
//						if (hdist < 1.f)
//							continue;
//					}*/
//
//					//ctx.multi_points[best_player->entindex() - 1][best_hitboxid].push_back({ point, (int)dmg });
//
//					/*if (is_safe_point)
//					{
//						if (!delay)
//							delay = (feature::autowall->CanHit(pred, point, ctx.m_local(), best_player, best_hitboxid, &center_was_viable) > hp) || center_was_viable;
//					}*/
//
//					auto good_dmg = (was_viable ? (dmg > max_dmg && dmg >= viable_dmg) : (dmg > max_dmg && dmg >= wall_dmg) && ctx.m_settings.aimbot_autowall);
//
//					if (dmg_override)
//						good_dmg = dmg > min(hp, ctx.m_settings.aimbot_min_damage_override_val) && dmg > max_dmg;
//
//					//if (good_dmg && best_hitboxid == HITBOX_NECK && hdist > headdist && !is_safe_point && !is_safe_side_point)
//					//	continue;
//
//					if (good_dmg && best_hitboxid <= HITBOX_NECK && !log->best_record->shot_this_tick && log->best_record->shot_time > log->best_record->simulation_time_old && dist > eyedist && !is_safe_point)
//						continue;
//
//					if (good_dmg /*&& i != 0 */&& had_safe_point && !is_safe_point && !is_safe_side_point && (max_dmg * 0.8f) > dmg /*|| force_safepoints && !is_safe_point*/)
//						continue;
//
//					if (good_dmg)
//					{
//						max_dmg = dmg;
//						//if (!is_safe_point)
//						//	best_non_safepoint_dmg = dmg;
//						best_hitbox = point;
//						dist = eyedist;
//						//hdist = headdist;
//						//max_hc = chance;
//						//walls_penetrated = (4 - ctx.last_penetrated_count);
//						had_safe_point = (is_safe_point || is_safe_side_point);
//
//						/*if (is_safe_point && dmg > hp)
//							break;*/
//					}
//				}
//			//}
//		}
//	}
//
//	auto hitchance = false;
//	auto aim_angles = QAngle(0, 0, 0);
//	int hc = 0;
//
//	if (best_player != nullptr && !best_hitbox.IsZero()) {
//		aim_angles = Math::CalcAngle(ctx.m_eye_position, best_hitbox);
//		aim_angles.Clamp();
//
//		//ctx.force_low_quality_autowalling = true;
//		hitchance = hit_chance(aim_angles, 
//		best_player, 
//		float(ctx.m_settings.aimbot_tickbase_exploit == 2 ? 
//			(!local_weapon->can_exploit(13) && ctx.double_tapped > 0) : ((fabs(csgo.m_globals()->realtime - ctx.last_speedhack_time) < 1.f  
//			&& ctx.allow_shooting > 0 
//			&& ctx.ticks_allowed < 13
//			&& ctx.m_local()->m_iShotsFired() <= 1
//			&& ctx.m_settings.aimbot_tickbase_exploit >= 2 
//			&& !ctx.fakeducking) ? 
//		ctx.m_settings.aimbot_doubletap_hitchance_val : 
//		ctx.m_settings.aimbot_hitchance_val)),
//		best_hitboxid, 
//		max_dmg * 0.3f, 
//		hc);
//		//ctx.force_low_quality_autowalling = false;
//	}
//
//	//ctx.shot_angles.clear();
//	//const auto old = ctx.shot_angles;
//	//ctx.shot_angles.clear();
//
//	if (!best_player || best_hitbox.IsZero() || aim_angles.IsZero() || best_hitboxid == -1)
//		return false;
//
//	const auto entity_index = best_player->entindex() - 1;
//
//	c_player_records* log = &feature::lagcomp->records[entity_index];
//	resolver_records* r_log = &feature::resolver->player_records[entity_index];
//
//	if (log->player != best_player)
//		return false;
//
//	//ctx.shot_angles = aim_angles;
//
//	//C_Hitbox box; feature::ragebot->get_hitbox_data(&box, best_player, best_hitboxid, best_player->m_CachedBoneData().Base());
//	//auto hit = Math::Intersect(ctx.m_eye_position, best_hitbox, box.mins, box.maxs, box.radius);
//
//	/*if (!hit && !box.isOBB) {
//		log->restore_record.apply(best_player, true);
//		return false;
//	}*/
//
//	//auto had_shot_ang = abs(ctx.shot_angles.x - old.x) < 1.f;
//
//	//if (hitchance)
//
//	const auto cur_mul = float(min(100, best_player->m_iHealth())) / max_dmg;
//	float dmg = 0.1;
//
//	auto velocity = ctx.m_local()->m_vecVelocity();
//	auto pred = ctx.m_eye_position;
//
//	if (cur_mul > 1.f &&
//		velocity.Length2D() > 6
//		&& cmd->buttons & (IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD))
//	{
//		velocity.z = 0.f;
//
//		Ray_t ray;
//		ray.Init(ctx.m_eye_position, ctx.m_eye_position + (velocity * 0.2f), ctx.m_local()->OBBMins(), ctx.m_local()->OBBMaxs());
//
//		CTraceFilter filter;
//		filter.pSkip = ctx.m_local();
//		trace_t tr;
//
//		csgo.m_engine_trace()->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);
//
//		if (tr.DidHit())
//		{
//			const auto frac = (tr.fraction * 0.2f) * 0.95f;
//			pred = ctx.m_eye_position + (velocity * frac);
//		}
//
//		ray.Init(ctx.m_eye_position, ctx.m_eye_position - Vector(0, 0, 500.f), Vector(-2, -2, -2), Vector(2, 2, 2));
//		csgo.m_engine_trace()->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);
//
//		pred.z = ctx.m_eye_position.z;
//
//		if (tr.DidHit())
//		{
//			//const auto frac = (tr.fraction * 0.2f) * 0.94999999f;
//			//pred.z = tr.endpos.z * frac;
//			//pred.z += ctx.m_eye_position.z - ctx.m_local()->m_vecOrigin().z;
//			pred.z = tr.endpos.z + (ctx.m_eye_position.z - ctx.m_local()->m_vecOrigin().z);
//		}
//
//		//csgo.m_debug_overlay()->AddBoxOverlay(pred, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 255, csgo.m_globals()->frametime * 2.f);
//	}
//
//	if (cur_mul >= 1.f) {
//		ctx.force_low_quality_autowalling = true;
//		dmg = feature::autowall->CanHit(pred, best_hitbox, ctx.m_local(), best_player, best_hitboxid);
//		ctx.force_low_quality_autowalling = false;
//	}
//
//	const auto new_mul = float(min(100, best_player->m_iHealth())) / (dmg * 0.9f);
//
//	//csgo.m_debug_overlay()->AddBoxOverlay(best_hitbox, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, !hitchance ? 255 : 0, 0, 255, csgo.m_globals()->frametime * 2.f);
//
//	//const auto weapon_mul = float(max(100.f, (float)local_weapon->GetCSWeaponData()->damage) / max_dmg);
//	//if (log->restore_record)
//	log->restore_record.apply(best_player, true);
//
//	auto is_zoomable_weapon = (local_weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 || local_weapon->m_iItemDefinitionIndex() == WEAPON_AWP || local_weapon->m_iItemDefinitionIndex() == WEAPON_SCAR20 || local_weapon->m_iItemDefinitionIndex() == WEAPON_G3SG1);
//	auto sniper = (local_weapon->m_iItemDefinitionIndex() == WEAPON_SSG08 || local_weapon->m_iItemDefinitionIndex() == WEAPON_AWP);
//
//	if (ctx.m_settings.aimbot_autoscope && !hitchance && is_zoomable_weapon && !ctx.m_local()->m_bIsScoped() && local_weapon->m_zoomLevel() == 0 && !ctx.m_local()->m_bResumeZoom())
//	{
//		cmd->buttons |= IN_ATTACK2;
//		cmd->buttons &= ~IN_ATTACK;
//		return false;
//	}
//
//	//auto vecHeadPos = ctx.m_local()->get_bone_pos(8);
//	//vecHeadPos.z += 1.7f;
//
//	//const auto shoot_origin = ctx.m_local()->m_vecAbsOrigin().z + 55.f;
//
//	auto can_shoot_while_fd = !ctx.fakeducking || ctx.fakeducking && !feature::anti_aim->did_shot_in_chocked_cycle;//(/*ctx.shots_total[entity_index] <= 2 || */fabs(csgo.m_globals()->realtime - ctx.last_shot_time_fakeduck) >= TICKS_TO_TIME(5));
//
//	/*if (max_dmg < min(92, best_player->m_iHealth()) && max(cur_mul, new_mul) < 2 ? (cur_mul < 2) : false)
//		dmg = 0;*/
//
//	if (local_weapon->IsGun() && local_weapon->can_shoot() && (cur_mul <= new_mul /*|| dmg < 1 || cur_mul < 1.1f*//*|| (cur_mul <= 1.4f && (log->best_record->shot_this_tick || log->best_record->shot_time >= log->best_record->simulation_time_old))*//* || weapon_mul < 1.f*/) && (!ctx.m_settings.extended_silent_shot || !*send_packet || feature::anti_aim->unchocking || ctx.fakeducking /*|| sniper*/) && cmd->weaponselect <= 0)
//	{
//		/*if (ctx.m_settings.aimbot_autoscope != 0 && (ctx.m_settings.aimbot_autoscope == 1 || ctx.m_settings.aimbot_autoscope == 2 && !hitchance) && local_weapon->m_zoomLevel() <= 0 && is_zoomable_weapon && !(cmd->buttons & IN_JUMP))
//		{
//			cmd->buttons |= IN_ATTACK2;
//			cmd->buttons &= ~IN_ATTACK;
//			return false;
//		}*/
//
//		const auto is_tick_valid = log->tick_count != -1;
//
//		if (hitchance && can_shoot_while_fd && (ctx.can_aimbot || ctx.m_settings.aimbot_tickbase_exploit == 0 || !ctx.exploit_allowed) && (ctx.m_settings.fake_lag_shooting && csgo.m_client_state()->m_iChockedCommands < ctx.m_settings.fake_lag_value || !ctx.m_settings.fake_lag_shooting)/*(!ctx.m_local()->m_bResumeZoom() || ctx.m_settings.aimbot_autoscope == 0 || !is_zoomable_weapon)*/)
//		{
//			if (is_tick_valid)
//				cmd->tick_count = log->tick_count;
//
//			//if (sniper)
//			//	cmd->buttons |= IN_SPEED;
//
//			cmd->viewangles = aim_angles;
//			cmd->buttons |= IN_ATTACK;
//
//			ctx.allow_shooting = 0;
//
//			if (ctx.shots_fired[best_player->entindex() - 1] <= 0)
//				r_log->last_resolving_method = log->best_record->resolver_index;
//
//			if (!is_zeus) 
//				ctx.shots_fired[entity_index] += 1;
//				//ctx.shots_total[entity_index] += 1;
//			
//			if (log->best_record) {
//				const auto did_shot = feature::resolver->add_shot(ctx.m_eye_position, log->best_record, feature::misc->hitbox_to_hitgroup(best_hitboxid), best_player->entindex());
//
//				if (did_shot && ctx.m_settings.misc_notifications[3]) {
//					static auto _f = std::string(sxor("fired shot at "));
//					static auto _s = std::string(sxor("'s "));
//					static auto _b = std::string(sxor(" ["));
//					static auto _hc = std::string(sxor("] | hc: "));
//					static auto _bt = std::string(sxor(" | bt: "));
//					static auto _tp = std::string(sxor("t | type: "));
//					static auto _dmg = std::string(sxor(" | dmg: "));
//					static auto _db = std::string(sxor("]:["));
//					static auto _p = std::string(sxor("+"));
//					static auto _m = std::string(sxor("-"));
//					static auto _sh = std::string(sxor("] | shots: "));
//					static auto _r = std::string(sxor(" | R:"));
//
//					_events.emplace_back(_f + best_player->m_szNickName() 
//						+ _s + hitbox_to_string(best_hitboxid) 
//						+ _b + std::to_string(best_hitboxid) 
//						+ _hc + std::to_string(hc)
//						+ _bt + std::to_string(log->backtrack_ticks) 
//						+ _tp + std::to_string(log->best_record->shot_this_tick ? 2 : (log->best_record->animations_updated ? 3 : 1)) 
//						+ _dmg + std::to_string(static_cast<int>(max_dmg)) + _r + std::to_string(log->best_record->resolver_index) 
//						+ std::to_string(log->best_record->resolver_type)
//						+ _b + std::to_string(log->best_record->lag) 
//						+ _db + std::string(send_packet ? _p : _m) 
//						+ _sh + std::to_string(ctx.shots_fired[entity_index]));
//				}
//			}
//			//if (!ctx.fakeducking && (!*send_packet && !ctx.m_settings.fake_lag_shooting && (!ctx.m_settings.extended_silent_shot || !feature::anti_aim->unchocking) || (ctx.m_settings.aimbot_tickbase_exploit && ctx.exploit_allowed))) {
//			//	*send_packet = true;
//				//ctx.send_next_tick = PACKET_SEND;
//
//			//	ctx.accurate_max_previous_chocked_amt = csgo.m_client_state()->m_iChockedCommands + 1;
//
//				//if (feature::anti_aim->animation_speed <= 6.f && ctx.m_local()->get_animation_state()->t_since_stopped_moving >= 0.1f)
//				//	ctx.send_next_tick = PACKET_CHOKE;
//			//}
//
//			r_log->did_backtrack_onshot = log->best_record->shot_this_tick;
//
//			//if (ctx.m_settings.fake_lag_shooting)
//			//	*send_packet = false;
//
//			//previous_player = best_player->entindex();
//
//			//csgo.m_debug_overlay()->AddLineOverlay(ctx.m_eye_position, best_hitbox, 255,0,0,true, 4.f);
//
//			//if (ctx.m_settings.visuals_draw_local_impacts) {
//			//	csgo.m_debug_overlay()->AddBoxOverlay(best_hitbox, Vector(-2.f, -2.f, -2.f), Vector(2.f, 2.f, 2.f), Vector(0.f, 0.f, 0.f), 255, 0, 0, 127, 4.f);
//				//ctx.last_shot_time_clientside = -1.f;
//			//}
//
//			//if (log->best_record->shot_this_tick)
//			//	log->missed_at_onshot++;
//
//			//if (auto net = csgo.m_engine()->GetNetChannelInfo(); net != nullptr && (abs(csgo.m_globals()->realtime - last_shoot_time) > (cmd->buttons & IN_ATTACK2 ? ctx.latest_weapon_data->flCycleTimeAlt : ctx.latest_weapon_data->flCycleTime) || ctx.exploit_allowed)) {
//				//auto impact_time = csgo.m_globals()->tickcount + (send_packet ? 1 : (14 - csgo.m_client_state()->m_iChockedCommands)) + TIME_TO_TICKS(net->GetLatency(FLOW_OUTGOING) + csgo.m_globals()->interval_per_tick);
//
//				//const auto &matrixes = (r_log->resolving_method != 0 && !best_player->IsBot() ? (r_log->resolving_method == 1 ? log->best_record->leftmatrixes : log->best_record->rightmatrixes) : log->best_record->matrixes);
//
//				//ctx.fired_shot.emplace_back(best_player, matrixes, ctx.m_eye_position, best_hitbox, log, local_weapon->GetInaccuracy(), ctx.m_local()->m_flVelocityModifier(), best_hitboxid, (int)max_dmg, feature::misc->hitbox_to_hitgroup(best_hitboxid), walls_penetrated, hc, csgo.m_globals()->realtime);
//			//}
//
//			if (!ctx.m_settings.aimbot_silent_aim)
//				csgo.m_engine()->SetViewAngles(aim_angles);
//
//			//Engine::Prediction::Instance()->Begin(cmd, true);
//
//			if (ctx.m_settings.misc_visuals_hitboxes[0] && (abs(csgo.m_globals()->realtime - last_shoot_time) > (cmd->buttons & IN_ATTACK2 ? ctx.latest_weapon_data->flCycleTimeAlt : ctx.latest_weapon_data->flCycleTime) || ctx.exploit_allowed))
//				visualize_hitboxes(best_player, (log->best_record->resolver_index != 0 && !best_player->IsBot() ? (log->best_record->resolver_index == 1 ? log->best_record->leftmatrixes : log->best_record->rightmatrixes) : log->best_record->matrixes), ctx.m_settings.misc_visuals_hitboxes_color, (float)ctx.m_settings.misc_visuals_hitboxes_time);
//
//			ctx.left_side = !ctx.left_side;
//
//			last_shoot_time = csgo.m_globals()->realtime;
//
//			if (ctx.fakeducking)
//				ctx.last_shot_time_fakeduck = csgo.m_globals()->realtime;
//
//			ctx.did_shot = true;
//
//			//Engine::Prediction::Instance()->Predict(cmd);
//
//			return true;
//		}
//	}
//
//	return false;
//}
//
TargetListing_t::TargetListing_t(C_BasePlayer* ent)
{
	entity = ent;

	hp = min(100, entity->m_iHealth());
	//damage = (hp / max(1.f, max_dmg)) * 100.f;//entity->m_iHealth();
	//distance = ctx.m_local()->get_abs_origin().Distance(entity->get_abs_origin());
	//height = entity->m_vecOrigin().z - ctx.m_local()->m_vecOrigin().z;
	//Vector lol;
	//if (Drawing::WorldToScreen(entity->get_abs_origin(), lol))
	//fov = Math::GetFov(Engine::Movement::Instance()->m_qRealAngles,
	//                   Math::CalcAngle(ctx.m_eye_position, entity->get_abs_origin()));
	//float(Vector(ctx.screen_size.x * 0.5f, ctx.screen_size.y * 0.5f, 0).DistanceSquared(lol));
	//else
	//	fov = 9999.f;
	idx = entity->entindex();
	//auto& log = feature::lagcomp->records[idx - 1];

	//if (log.best_record)
	//	last_delta = log.best_record->desync_delta;
}

void c_aimbot::OnRoundStart(C_BasePlayer* player) {
	m_target = nullptr;
	m_hitboxes.clear();

	// IMPORTANT: DO NOT CLEAR LAST HIT SHIT.
}

void c_aimbot::SetupHitboxes(C_BasePlayer* ent, C_Tickrecord* record, bool history) {

	// reset hitboxes.
	m_hitboxes.clear();

	if (!record)
		return;


	if (m_weapon()->m_iItemDefinitionIndex() == WEAPON_TASER) {
		// hitboxes for the zeus.
		m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::NORMAL });
		m_hitboxes.push_back({ HITBOX_PELVIS, HitscanMode::NORMAL });
		m_hitboxes.push_back({ HITBOX_CHEST, HitscanMode::NORMAL });
		return;
	}

	// prefer, always.
	if (ctx.m_settings.aimbot_prefer_body)
		m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER});

	auto can_baim = ctx.m_settings.aimbot_hitboxes[3];

	// prefer, lethal.
	if (can_baim)
		m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::LETHAL});

	// prefer, lethal x2.
	if (can_baim && ctx.exploit_allowed && ctx.has_exploit_toggled && ctx.main_exploit >= 2 && (ctx.ticks_allowed > 12 || ctx.force_aimbot > 0) && !ctx.fakeducking)
		m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::LETHAL2});

	// prefer, fake.
	//if (ctx.m_settings.aimbot_extra_scan_aim[2] && record && (!record->shot_this_tick || record->desync_delta > 35.f))
	//	m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER});

	// prefer, in air.
	//if (g_menu.main.aimbot.baim1.get(4) && !(record->m_pred_flags & FL_ONGROUND))
	//	m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER }});

	//bool only{ false };

	//// only, always.
	//if (g_menu.main.aimbot.baim2.get(0)) {
	//	only = true;
	//	m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER }});
	//}

	//// only, health.
	//if (g_menu.main.aimbot.baim2.get(1) && ent->m_iHealth() <= (int)g_menu.main.aimbot.baim_hp.get()) {
	//	only = true;
	//	m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER }});
	//}

	//// only, fake.
	//if (g_menu.main.aimbot.baim2.get(2) && record->m_mode != Resolver::Modes::RESOLVE_NONE && record->m_mode != Resolver::Modes::RESOLVE_WALK) {
	//	only = true;
	//	m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER }});
	//}

	//// only, in air.
	//if (g_menu.main.aimbot.baim2.get(3) && !(record->m_pred_flags & FL_ONGROUND)) {
	//	only = true;
	//	m_hitboxes.push_back({ HITBOX_BODY, HitscanMode::PREFER }});
	//}

	// only baim conditions have been met.
	// do not insert more hitboxes.
	//if (only)
	//	return;

	//std::vector< size_t > hitbox{ ctx.m_settings.aimbot_hitboxes/*history ? g_menu.main.aimbot.hitbox_history.GetActiveIndices() : g_menu.main.aimbot.hitbox.GetActiveIndices()*/ };
	//if (hitbox.empty())
	//	return;

	for (auto h = 0; h < 6; h++) {
		// head.
		if (ctx.m_settings.aimbot_hitboxes[0] && h == 0)
			m_hitboxes.push_back({ HITBOX_HEAD, HitscanMode::NORMAL});

		// stomach.
		if (ctx.m_settings.aimbot_hitboxes[3] && h == 3)
			m_hitboxes.push_back({ HITBOX_BODY,   HitscanMode::NORMAL });

		// chest.
		if (ctx.m_settings.aimbot_hitboxes[1] && h == 1)
			m_hitboxes.push_back({ HITBOX_CHEST,       HitscanMode::NORMAL });

		if (ctx.m_settings.aimbot_hitboxes[2] && h == 2)
			m_hitboxes.push_back({ HITBOX_UPPER_CHEST, HitscanMode::NORMAL });

		if (ctx.m_settings.aimbot_hitboxes[5] && h == 5) {
			m_hitboxes.push_back({ HITBOX_LEFT_FOOT,  HitscanMode::NORMAL });
			m_hitboxes.push_back({ HITBOX_RIGHT_FOOT,  HitscanMode::NORMAL });
		}

		if (Engine::Prediction::Instance()->m_flFrameTime < csgo.m_globals()->interval_per_tick) {

			if (ctx.m_settings.aimbot_hitboxes[1] && h == 1)
				m_hitboxes.push_back({ HITBOX_THORAX,      HitscanMode::NORMAL });

			//if (ctx.m_settings.aimbot_hitboxes[3] && h == 3)
			//	m_hitboxes.push_back({ HITBOX_PELVIS, HitscanMode::NORMAL });

			// arms.
			if (ctx.m_settings.aimbot_hitboxes[4] && h == 4) {
				m_hitboxes.push_back({ HITBOX_LEFT_UPPER_ARM, HitscanMode::NORMAL });
				m_hitboxes.push_back({ HITBOX_RIGHT_UPPER_ARM, HitscanMode::NORMAL });
			}

			// legs.
			if (ctx.m_settings.aimbot_hitboxes[5] && h == 5) {
				m_hitboxes.push_back({ HITBOX_LEFT_THIGH, HitscanMode::NORMAL });
				m_hitboxes.push_back({ HITBOX_RIGHT_THIGH, HitscanMode::NORMAL });
				m_hitboxes.push_back({ HITBOX_LEFT_CALF,  HitscanMode::NORMAL });
				m_hitboxes.push_back({ HITBOX_RIGHT_CALF,  HitscanMode::NORMAL });
			}
		}
		else
			continue;
	}
}

void c_aimbot::init() {
	// clear old targets.
	m_targets[0].entity = nullptr;
	m_targets_count = 0;

	m_target = nullptr;
	m_aim = Vector{};
	m_angle = QAngle{};
	m_damage = 0.f;
	//ctx.do_autostop = false;

	m_baim_key = false;
	m_damage_key = false;

	m_best_dist = FLT_MAX;
	m_best_fov = 180.f + 1.f;
	m_best_damage = 0.f;
	m_best_hp = 100 + 1;
	m_best_lag = FLT_MAX;
	m_best_height = FLT_MAX;
}

void c_aimbot::StripAttack(CUserCmd* cmd) {
	/*if (g_cl.m_weapon_id == 64)
		g_cl.m_cmd->m_buttons &= ~IN_ATTACK2;

	else*/
		//g_cl.m_cmd->m_buttons &= ~IN_ATTACK;
}

bool c_aimbot::think(CUserCmd* cmd, bool* send_packet) {
	// do all startup routines.
	init();

	/*if (ctx.m_local()->m_vecViewOffset().z >= 46.1f)
	{
		if (ctx.m_local()->m_vecViewOffset().z > 64.0f)
			ctx.m_eye_position.z = (ctx.m_eye_position.z - ctx.m_local()->m_vecViewOffset().z) + 64.0f;
	}
	else
		ctx.m_eye_position.z = (ctx.m_eye_position.z - ctx.m_local()->m_vecViewOffset().z) + 46.0f;*/

	m_baim_key = ctx.get_key_press(ctx.m_settings.aimbot_bodyaim_key);

	if (m_baim_key)
		ctx.active_keybinds[5].mode = ctx.m_settings.aimbot_bodyaim_key.mode + 1;

	m_damage_key = ctx.get_key_press(ctx.m_settings.aimbot_min_damage_override);

	if (m_damage_key)
		ctx.active_keybinds[6].mode = ctx.m_settings.aimbot_min_damage_override.mode + 1;

	
	// we have no aimbot enabled.
	if (!ctx.m_settings.aimbot_enabled)
		return false;
	
	// sanity.
	if (!m_weapon() || cmd->weaponselect > 0) {
		ctx.m_last_shot_index = -1;
		return false;
	}

	if (abs(cmd->command_number - ctx.m_ragebot_shot_nr) > 40)
		ctx.m_last_shot_index = -1;

	// no grenades or bomb.
	if (!m_weapon()->IsGun() || ctx.air_stuck) {
		ctx.m_last_shot_index = -1;
		return false;
	}

	//if (!ctx.m_local()
	//	StripAttack();

	// no point in aimbotting if we cannot fire this tick.
	if (/*!m_weapon()->can_shoot() && */(ctx.fakeducking || ctx.m_settings.fake_lag_between_shots) && feature::anti_aim->did_shot_in_chocked_cycle)
		return false;

	if (!m_weapon()->can_shoot())
	{
		if (m_weapon()->m_reloadState() != 0) {
			return false;
		}

		ctx.shot_angles.clear();
	}

	// run knifebot.
	if (m_weapon()->is_knife() && m_weapon()->m_iItemDefinitionIndex() != WEAPON_TASER) {

		//if (g_menu.main.aimbot.knifebot.get())
		//	knife();

		return false;
	}

	// setup bones for all valid targets.
	for (int i{ 1 }; i <= 64; ++i) {
		auto player = csgo.m_entity_list()->GetClientEntity(i);
		auto r_log = &feature::resolver->player_records[i - 1];

		if (!player ||
			player->IsDormant() ||
			!player->IsPlayer() ||
			player->m_iHealth() <= 0 ||
			player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() ||
			player->m_bGunGameImmunity()) {
				continue;
		}

		auto data = &feature::lagcomp->records[i - 1];

		if (!data || data->player != player || data->records_count < 1) {
			continue;
		}

		if (ctx.shots_fired[i - 1] < 1)
			r_log->last_shot_missed = false;

		// store player as potential target this tick.
		m_targets[m_targets_count++] = player;
	}

	/*const Vector old_move = Vector(cmd->forwardmove, cmd->sidemove, cmd->upmove);
	const bool pressed_b4 = cmd->buttons & 0x20000u;

	if (!ctx.m_settings.aimbot_autostop_options[1] || m_weapon()->can_shoot() || m_weapon()->m_iItemDefinitionIndex() == 64)
		autostop(cmd, m_weapon());*/

	ctx.do_autostop = true;

	// scan available targets... if we even have any.
	find(cmd);

	/*if (cmd->buttons & IN_ATTACK && m_weapon()->can_shoot() && ctx.main_exploit == 0 && ctx.m_settings.anti_aim_typeyfake_shot != 0) {	
		static bool invert = true;

		if (ctx.onshot_desync != 2 && csgo.m_client_state()->m_iChockedCommands < 14)
			ctx.onshot_desync = 1;
		else
			ctx.onshot_desync = 0;

		if (ctx.onshot_desync == 1 && csgo.m_client_state()->m_iChockedCommands < 14) {
			cmd->buttons &= ~IN_ATTACK;

			ctx.fside *= -1;

			*send_packet = false;
			
			ctx.onshot_aa_cmd = cmd->command_number;
			ctx.onshot_desync = 2;
			ctx.do_autostop = true;
		}
	}*/

	if (ctx.m_settings.fake_lag_shooting && feature::anti_aim->did_shot_in_chocked_cycle) {
		cmd->buttons &= ~IN_ATTACK;
		ctx.do_autostop = false;
	}

	apply(cmd, send_packet);

	if (!m_target || m_damage == 0)
		ctx.do_autostop = false;

	return m_target && m_damage > 0 /*&& ctx.m_ragebot_shot_nr == cmd->command_number*/ && cmd->buttons & IN_ATTACK;
}

void c_aimbot::find(CUserCmd* cmd) {
	struct BestTarget_t { C_BasePlayer* player; Vector pos; float damage; int hitbox; };

	Vector       tmp_pos;
	float        tmp_damage;
	int				tmp_hitbox;
	BestTarget_t best;
	best.player = nullptr;
	best.damage = -1.f;
	best.pos = Vector{};
	best.hitbox = -1;

	int players_iterated = 0;

	if (m_targets[0].entity == nullptr || m_targets_count < 1)
		return;

	//if (g_cl.m_weapon_id == ZEUS && !g_menu.main.aimbot.zeusbot.get())
	//	return;

	// iterate all targets.
	for (auto i = 0; i < m_targets_count; i++) {
		auto& target = m_targets[i];
		auto t = m_targets[i].entity;

		if (!t ||
			t->IsDormant() ||
			!t->IsPlayer() ||
			t->m_iHealth() <= 0 ||
			t->m_iTeamNum() == ctx.m_local()->m_iTeamNum() ||
			t->m_bGunGameImmunity()
			) continue;

		auto data = &feature::lagcomp->records[target.idx - 1];
		auto rdata = &feature::resolver->player_records[target.idx - 1];

		if (!data || data->records_count < 1 || data->player != t)
			continue;

		data->best_record = nullptr;

		C_Tickrecord* last = feature::resolver->find_first_available(t, data, false);

		if (!last || !last->data_filled || last->dormant)
			continue;

		if (last->animated && !data->saved_info.fakeplayer)
		{
			float right_dmg;
			float left_dmg;

			last->apply(t, false, true);
			memcpy(t->m_CachedBoneData().Base(), last->rightmatrixes, last->bones_count * sizeof(matrix3x4_t));
			t->m_bone_count() = last->bones_count;
			t->force_bone_cache();
			auto r_head = feature::ragebot->get_hitbox(t, HITBOX_HEAD, last->rightmatrixes);
			right_dmg = feature::autowall->CanHit(ctx.m_eye_position, r_head, ctx.m_local(), t, HITBOX_HEAD);
			memcpy(t->m_CachedBoneData().Base(), last->leftmatrixes, last->bones_count * sizeof(matrix3x4_t));
			t->force_bone_cache();
			auto l_head = feature::ragebot->get_hitbox(t, HITBOX_HEAD, last->leftmatrixes);

			left_dmg = feature::autowall->CanHit(ctx.m_eye_position, l_head, ctx.m_local(), t, HITBOX_HEAD);

			/*if (TIME_TO_TICKS(csgo.m_globals()->realtime - rdata->freestanding_update_time) >= 25)
			{
				rdata->freestand_left_tick = 0;
				rdata->freestand_right_tick = 0;
				rdata->freestanding_updates = 0;
			}*/

			if (left_dmg > 0)
				rdata->freestand_left_tick++;

			if (right_dmg > 0)
				rdata->freestand_right_tick++;

			if (fmaxf(left_dmg, right_dmg) >= 1 && abs(left_dmg - right_dmg) > 10.f)//bool(right_dmg >= 1) != bool(left_dmg >= 1))
			{
				int new_freestand_resolver = 0;

				if (right_dmg > left_dmg)
					new_freestand_resolver = 1;
				else
					new_freestand_resolver = 2;

				last->freestanding_index = new_freestand_resolver;
				rdata->freestanding_update_time = csgo.m_globals()->realtime;
				rdata->last_tick_damageable = ctx.current_tickcount;

				//if (rdata->freestanding_side != new_freestand_resolver)
					rdata->freestanding_updates++;

				rdata->freestanding_side = new_freestand_resolver;
			}

			/*if (right_dmg < 1 && left_dmg < 1)
			{
				rdata->freestand_left_tick = 0;
				rdata->freestand_right_tick = 0;
			}*/
		}

		C_Tickrecord* ideal = (last && last->shot_this_tick ? nullptr : feature::resolver->find_shot_record(t, data));

		if (ideal && ideal->data_filled && !ideal->breaking_lc)
		{
			SetupHitboxes(t, ideal, false);

			if (m_hitboxes.empty())
				continue;

			// try to select best record as target.
			if (GetBestAimPosition(t, tmp_pos, tmp_damage, tmp_hitbox, ideal, players_iterated) && SelectTarget(t, ideal, tmp_pos, tmp_damage)) {
				// if we made it so far, set shit.
				best.player = t;
				best.pos = tmp_pos;
				best.damage = tmp_damage;
				data->best_record = ideal;
				best.hitbox = tmp_hitbox;
				break;
			}
		}

		SetupHitboxes(t, last, false);

		if (m_hitboxes.empty())
			continue;

		players_iterated++;

		if (GetBestAimPosition(t, tmp_pos, tmp_damage, tmp_hitbox, last, players_iterated) && SelectTarget(t, last, tmp_pos, tmp_damage)) {
			// if we made it so far, set shit.
			best.player = t;
			best.pos = tmp_pos;
			best.damage = tmp_damage;
			data->best_record = last;
			best.hitbox = tmp_hitbox;
			break;
		}

		if (Engine::Prediction::Instance()->m_flFrameTime > csgo.m_globals()->interval_per_tick && ideal && ideal->data_filled && !ideal->breaking_lc || !m_weapon()->can_shoot())
			continue;

		C_Tickrecord* old = feature::resolver->find_first_available(t, data, true);

		if (!old || !old->data_filled || old->dormant || !old->breaking_lc)
			continue;

		SetupHitboxes(t, old, false);

		if (m_hitboxes.empty())
			continue;

		if (GetBestAimPosition(t, tmp_pos, tmp_damage, tmp_hitbox, old, players_iterated) && SelectTarget(t, old, tmp_pos, tmp_damage)) {
			// if we made it so far, set shit.
			best.player = t;
			best.pos = tmp_pos;
			best.damage = tmp_damage;
			data->best_record = old;
			best.hitbox = tmp_hitbox;
			break;
		}

		//// this player broke lagcomp.
		//// his bones have been resetup by our lagcomp.
		//// therfore now only the front record is valid.
		//if (g_menu.main.aimbot.lagfix.get() && g_lagcomp.StartPrediction(t)) {
		//	auto front = ;

		//	t->get_multipoints(front, false);
		//	if (t->m_hitboxes.empty())
		//		continue;

		//	// rip something went wrong..
		//	if (t->GetBestAimPosition(tmp_pos, tmp_damage, front) && SelectTarget(front, tmp_pos, tmp_damage)) {

		//		// if we made it so far, set shit.
		//		best.player = t->m_player;
		//		best.pos = tmp_pos;
		//		best.damage = tmp_damage;
		//		best.record = front;
		//	}
		//}
		
		// player did not break lagcomp.
		// history aim is possible at this point.
		//else {
		//}
	}

	// verify our target and set needed data.
	if (best.player && ctx.m_local()) {
		auto data = &feature::lagcomp->records[best.player->entindex() - 1];
		auto r_data = &feature::resolver->player_records[best.player->entindex() - 1];

		// calculate aim angle.
		Math::VectorAngles(best.pos - ctx.m_eye_position, m_angle);

		// set member vars.
		m_target = best.player;
		m_aim = best.pos;
		m_damage = best.damage;
		m_hitbox = best.hitbox;
		//m_record = best.record;

		//csgo.m_debug_overlay()->AddBoxOverlay(m_aim, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 255, 2.f * csgo.m_globals()->frametime);

		if (m_damage > 0) {
			// write data, needed for traces / etc.
			data->best_record->apply(m_target, false);

			const auto cur_mul = float(min(100, m_target->m_iHealth())) / m_damage;
			float ndmg = 1;

			auto velocity = Engine::Prediction::Instance()->GetVelocity();

			//ctx.last_aim_state = 1;
			
			if (auto animstate = ctx.m_local()->get_animation_state(); animstate != nullptr && animstate->m_player && m_weapon()->can_shoot()) {
				if (ctx.m_local()->should_fix_modify_eye_pos()) {
					
					const auto oldposeparam = *(float*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48));
					auto eye_pitch = Math::normalize_angle(m_angle.x + ctx.m_local()->m_viewPunchAngle().x);

					auto angles = QAngle(0.f, ctx.m_local()->get_animation_state()->m_abs_yaw, 0);
					ctx.m_local()->set_abs_angles(angles);

					if (eye_pitch > 180.f)
						eye_pitch = eye_pitch - 360.f;

					eye_pitch = Math::clamp(eye_pitch, -89, 89);
					*(float*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = Math::clamp((eye_pitch + 89.f) / 176.f, 0.0f, 1.0f);

					ctx.m_local()->force_bone_rebuild();

					const auto absorg = ctx.m_local()->get_abs_origin();
					ctx.m_local()->set_abs_origin(ctx.m_local()->m_vecOrigin());
					ctx.m_local()->SetupBonesEx(0x100);
					ctx.m_local()->set_abs_origin(absorg);

					ctx.m_local()->force_bone_cache();

					ctx.m_eye_position = ctx.m_local()->GetEyePosition(); //call weapon_shootpos
					*(float*)(uintptr_t(ctx.m_local()) + (Engine::Displacement::DT_CSPlayer::m_flPoseParameter + 48)) = oldposeparam;
					
					Math::VectorAngles(best.pos - ctx.m_eye_position, m_angle);
					//m_angle = Math::CalcAngle(ctx.m_eye_position, m_aim);
					//}
				}
			}

			if (ctx.m_settings.aimbot_extra_scan_aim[5] && cur_mul >= 1.0f &&
				velocity.Length2D() > 0
				&& cmd->buttons & (IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD) && !(cmd->buttons & IN_DUCK))
			{
				velocity.z = 0.f;
				auto pred = ctx.m_eye_position + (velocity * 0.2f);

				Ray_t ray;
				ray.Init(ctx.m_eye_position, pred, ctx.m_local()->OBBMins(), ctx.m_local()->OBBMaxs());

				CTraceFilter filter;
				filter.pSkip = ctx.m_local();
				trace_t tr;

				csgo.m_engine_trace()->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

				if (tr.DidHit())
				{
					const auto frac = (tr.fraction * 0.2f) * 0.95f;
					pred = ctx.m_eye_position + (velocity * frac);
				}

				ray.Init(ctx.m_eye_position, ctx.m_eye_position - Vector(0, 0, 500.f), Vector(-2, -2, -2), Vector(2, 2, 2));
				csgo.m_engine_trace()->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

				pred.z = ctx.m_eye_position.z;

				if (tr.DidHit())
				{
					//const auto frac = (tr.fraction * 0.2f) * 0.94999999f;
					//pred.z = tr.endpos.z * frac;
					//pred.z += ctx.m_eye_position.z - ctx.m_local()->m_vecOrigin().z;
					pred.z = tr.endpos.z + (ctx.m_eye_position.z - ctx.m_local()->m_vecOrigin().z);
				}

				ctx.force_low_quality_autowalling = true;
				ndmg = feature::autowall->CanHit(pred, m_aim, ctx.m_local(), m_target, m_hitbox);

				ctx.force_low_quality_autowalling = false;
				//csgo.m_debug_overlay()->AddBoxOverlay(pred, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 255, csgo.m_globals()->frametime * 2.f);
			}

			const auto new_mul = float(min(100, m_target->m_iHealth())) / ndmg;

			//if (cur_mul <= new_mul || cur_mul <= 1.0f || ctx.exploit_allowed && ctx.main_exploit >= 2 && cur_mul < 2.f)
			//	ctx.last_aim_state = 2;

			//csgo.m_debug_overlay()->AddBoxOverlay(ctx.m_eye_position + (best.pos - ctx.m_eye_position).Normalized() * 8192.f, Vector(-2, -2, -2), Vector(2, 2, 2), Vector(0, 0, 0), 255, 0, 0, 255, 2.f);

			if (ctx.ticks_allowed < 13)
				ctx.last_speedhack_time = csgo.m_globals()->realtime;

			m_best_hc[0] = 0;
			m_best_hc[1] = 0;

			auto hc_result = !ctx.m_settings.aimbot_hitchance 
				|| ctx.m_settings.aimbot_hitchance && hit_chance(m_angle, m_aim, m_target, ctx.force_aimbot > 0 ? ctx.m_settings.aimbot_hitchance_val / 2 : ctx.m_settings.aimbot_hitchance_val, best.hitbox, 1/*max(min(5, m_damage * 0.2f), 1)*/, m_best_hc);

			bool hit = !ctx.m_settings.aimbot_extra_scan_aim[5] 
				|| ctx.force_aimbot > 0
				|| cur_mul <= new_mul 
				|| cur_mul <= 1.0f 
				|| ctx.exploit_allowed && ctx.main_exploit >= 2 && cur_mul <= 2.f;

			ctx.do_autostop = hit;
			ctx.shot_angles = m_angle;

			auto is_zoomable_weapon = (m_weapon()->m_iItemDefinitionIndex() == WEAPON_SSG08 
				|| m_weapon()->m_iItemDefinitionIndex() == WEAPON_AWP 
				|| m_weapon()->m_iItemDefinitionIndex() == WEAPON_SCAR20 
				|| m_weapon()->m_iItemDefinitionIndex() == WEAPON_G3SG1);

			auto sniper = (m_weapon()->m_iItemDefinitionIndex() == WEAPON_SSG08 || m_weapon()->m_iItemDefinitionIndex() == WEAPON_AWP);

			if (ctx.m_settings.aimbot_autoscope 
				&& is_zoomable_weapon 
				&& !ctx.m_local()->m_bIsScoped() 
				&& !ctx.m_local()->m_bResumeZoom())
			{
				cmd->buttons |= IN_ATTACK2;
				cmd->buttons &= ~IN_ATTACK;
				return;
			}

			if (hit && hc_result)
			{
				if (!m_weapon()->m_iClip1())
				{
					if (m_weapon()->m_iPrimaryReserveAmmoCount() > 0)
						cmd->buttons = cmd->buttons & ~1 | 0x2000;
					else
						ctx.do_autostop = false;

					return;
				}

				ctx.last_aim_state = 3;
				// right click attack.
				//if (g_menu.main.config.mode.get() == 1 && g_cl.m_weapon_id == REVOLVER)
				//	cmd->m_buttons |= IN_ATTACK2;
				//// left click attack.
				//else

				if (!(cmd->buttons & 0x2000) 
					&& (!ctx.m_settings.autostop_force_accuracy
						|| ctx.m_settings.autostop_force_accuracy && (ctx.m_local()->m_vecVelocity().Length2D() + 1) < (ctx.max_weapon_speed * 0.34f)))
				{
					if (!ctx.m_local()->m_bWaitForNoAttack() && m_weapon()->can_shoot()) {
						cmd->buttons |= IN_ATTACK;

						//auto v10 = cmd->buttons & ~(IN_MOVERIGHT | IN_MOVELEFT | IN_BACK | IN_FORWARD | IN_JUMP);
						//cmd->buttons = v10;
					}
				}
			}
		}
	}
	else
		ctx.shot_angles.clear();
}

bool c_aimbot::GetBestAimPosition(C_BasePlayer* player, Vector& aim, float& damage, int& hitbox, C_Tickrecord* record, int players_iterated) {
	bool                  done, pen;
	float                 dmg, pendmg;
	HitscanData_t         scan;
	scan.m_hitchance = 0;
	std::vector< Vector > points;

	//memset(&scan.m_safepoint[0], false, sizeof(bool) * 19);

	// get player hp.
	int hp = min(100, player->m_iHealth());

	scan.m_damage = 0;

	auto data = &feature::lagcomp->records[player->entindex() - 1];
	auto rdata = &feature::resolver->player_records[player->entindex() - 1];

	if (m_weapon()->m_iItemDefinitionIndex() == WEAPON_TASER) {
		dmg = pendmg = hp;
		pen = false;
	}
	else {
		dmg = min(hp, ctx.m_settings.aimbot_min_damage_viable);
		
		pendmg = min(hp, ctx.m_settings.aimbot_min_damage);
		
		if (m_damage_key)
			pendmg = dmg = min(ctx.m_settings.aimbot_min_damage_override_val, hp);

		pen = ctx.m_settings.aimbot_autowall;
	}

	auto resolved = feature::resolver->select_next_side(player, record);
	
	if (rdata->resolver_type != 0) {
		rdata->resolver_index = record->resolver_index;
		rdata->resolver_type = record->resolver_index;
	}
	/*if (record->shot_this_tick && ctx.m_settings.aimbot_extra_scan_aim[2])
	{
		if (record->desync_delta > 35.f) {
			if (record->resolver_index != 0)
				record->resolver_index = (record->resolver_index == 1) ? 2 : 1;
			else
				record->resolver_index = 2;
		}
		else
			record->resolver_index = 0;

		record->resolver_type = 9;
	}*/

	/*if (record->resolver_type == 2 && !record->shot_this_tick && max(rdata->freestand_left_tick, rdata->freestand_right_tick) <= 1)
		return false;*/

	//if (/*(record->not_desyncing || */record->had_use_key && record->resolver_index != 0 && data->records_count > 3) {
	//	record->resolver_index = 0;
	//	record->resolver_type = 8;
	//}

	/*if (rdata->desync_swap && data->records_count > 2)
	{

	}*/

	/*if (!record->shot_this_tick && record->resolver_index != 0 && record->resolver_type != 4 && record->resolver_type != 3)
    {
        float at_target = Math::CalcAngle(record->origin, ctx.m_eye_position).y;

        auto some_delta = Math::normalize_angle(Math::AngleDiff(at_target, record->eye_angles.y));

        if (abs(some_delta) <= 60.f && ctx.shots_fired[player->entindex() - 1] <= 2) {
            record->resolver_index = (int(record->resolver_index != 2) + 1);
            rdata->resolver_type = 8;
        }
    }*/

	/*if (ctx.shots_fired[player->entindex() - 1] < 3 && record->resolver_type != 4 && !record->shot_this_tick && record->resolver_index != 0)
	{
		float at_target = Math::normalize_angle(Math::CalcAngle(record->origin, ctx.m_eye_position).y);

		if (at_target < 0.f)
			at_target += 360.f;

		auto some_delta = Math::AngleDiff(at_target, record->eye_angles.y);

		if (some_delta <= 60.f && some_delta >= -60.f)
		{
			record->resolver_index = (int(record->resolver_index != 2) + 1);
			record->resolver_type = 8;
		}
	}*/

	//else
	//{
	//	record->resolver_index = (int(record->resolver_index != 2) + 1);
	//	rdata->resolver_type = 9;
	//}

	// apply
	record->apply(player, false);
	data->is_restored = false;

	bool had_any_dmg = false;

	// find hitboxes.
	for (const auto& it : m_hitboxes) {
		done = false;

		bool retard;

		if (it.m_index == HITBOX_HEAD) {

			// nope we did not hit head..
			if (m_baim_key)
				continue;
		}

		// setup points on hitbox.
		if (!player->get_multipoints(it.m_index, points, player->m_CachedBoneData().Base(), retard))
			continue;

		int points_this_hitbox = 0;

		scan.m_safepoint[it.m_index] = false;

		// iterate points on hitbox.
		for (auto& point : points) {
			int safepoints = safe_point(player, ctx.m_eye_position, point, it.m_index, record);

			if (safepoints == 0)// || (!resolved || ctx.shots_fired[player->entindex()-1] > 2) && it.m_index == 0 && !record->shot_this_tick && record->desync_delta > 40.f && safepoints < 2/*((m_weapon()->m_iItemDefinitionIndex() != WEAPON_SCAR20 && m_weapon()->m_iItemDefinitionIndex() != WEAPON_G3SG1) ? safepoints < 2 : safepoints <= 2)*//*|| ctx.shots_fired[player->entindex() - 1] > 1 && safepoints <= 2 && ctx.m_settings.aimbot_extra_scan_aim[2]*/)
				continue;

			float wall_dmg = pendmg;
			float just_dmg = dmg;

			if (m_damage_key)
				just_dmg = wall_dmg = min(hp, ctx.m_settings.aimbot_min_damage_override_val);

			/*if (ctx.latest_weapon_data && hp < (min(100, ctx.latest_weapon_data->damage) * 0.92f))
			{
				if ((it.m_index == 6 || it.m_index == 5 || it.m_index == 11 || it.m_index == 12) && safepoints < 2)
					continue;
				else {
					if ((it.m_index <= 1 || it.m_index > 6) && safepoints < 3)
						continue;
				}
			}*/

			// ignore mindmg.
			//if (it.m_mode == HitscanMode::LETHAL || it.m_mode == HitscanMode::LETHAL2)
			//	just_dmg = wall_dmg = 1.f;

			//int point_chance = 0;

			//if (m_weapon()->can_shoot() && !mini_hit_chance(point, player, it.m_index, point_chance))
			//	continue;

			//penetration::PenetrationOutput_t out;

			bool was_viable;

			//++points_this_hitbox;

			//if (scan.m_hitbox != it.m_index && points_this_hitbox > 4 && safepoints < 2)
			//	continue;
			float best_damage_per_hitbox = 0;
			float distance_to_center = 0;

			// we can hit p!
			if (float m_damage = feature::autowall->CanHit(ctx.m_eye_position, point, ctx.m_local(), player, it.m_index, &was_viable); m_damage > 0)
			{
				if (it.m_index == HITBOX_HEAD && ctx.last_hitgroup != HITGROUP_HEAD
					/*|| it.m_index != HITBOX_HEAD && ctx.last_hitgroup == HITGROUP_HEAD*/)
					continue;

				//if (rdata->damage_ticks < 2 && it.m_index == HITBOX_HEAD && safepoints < 3)
				//	continue;

				if (it.m_index == 3 && m_damage >= (hp * 0.5f))
					rdata->baim_tick = ctx.current_tickcount;

				if (!was_viable)
				{
					if (!pen || m_damage < wall_dmg)
						continue;
				}
				else
				{
					if (m_damage < just_dmg)
						continue;
				}

				auto good_for_safepoint = (!ctx.m_settings.aimbot_extra_scan_aim[4]
					|| !scan.m_safepoint[it.m_index]
					|| scan.m_safepoint[it.m_index] && safepoints > 2);

				//// prefered hitbox, just stop now.
				//if (it.m_mode == HitscanMode::PREFER && (m_damage * 2.f) > player->m_iHealth() && scan.m_damage <= m_damage)
				//	done = true;
				////// this hitbox requires lethality to get selected, if that is the case.
				////// we are done, stop now.
				//else if (it.m_mode == HitscanMode::LETHAL && m_damage >= player->m_iHealth() && scan.m_damage <= m_damage) {
				//	done = true;
				//}
				//// 2 shots will be sufficient to kill.
				//else if (it.m_mode == HitscanMode::LETHAL2 && (m_damage * 2.f) >= player->m_iHealth() && scan.m_damage <= m_damage) {
				//	done = true;
				//}
				//// this hitbox has normal selection, it needs to have more damage.
				//else if (it.m_mode == HitscanMode::NORMAL) {
					// we did more damage.

				bool good_for_baim = false;

				if (it.m_mode == HitscanMode::PREFER && (m_damage * 2.f) >= player->m_iHealth())
					good_for_baim = true;
				else if ((it.m_mode == HitscanMode::LETHAL || it.m_index == 3) && m_damage >= player->m_iHealth()) {
					good_for_baim = true;
				}
				// 2 shots will be sufficient to kill.
				else if ((it.m_mode == HitscanMode::LETHAL2 || ctx.m_local()->m_iShotsFired() >= 2 && it.m_index == 3) && (m_damage * 2.f) >= player->m_iHealth()) {
					good_for_baim = true;
				}

				if (good_for_baim) {
					done = true;//m_damage > scan.m_damage && good_for_safepoint || m_weapon()->m_iItemDefinitionIndex() != WEAPON_SSG08;
					rdata->baim_tick = ctx.current_tickcount;
				}

				auto distance = point.DistanceSquared(points.front());

				if (best_damage_per_hitbox > 0 && (m_damage >= best_damage_per_hitbox || (m_damage >= (best_damage_per_hitbox - 15)) && distance < distance_to_center) && good_for_safepoint)
				{
					scan.m_damage = m_damage;
					scan.m_pos = point;
					scan.m_hitbox = it.m_index;
					scan.m_safepoint[it.m_index] = safepoints > 2;
					best_damage_per_hitbox = m_damage;
					distance_to_center = distance;
					//if (it.m_index == 3)
					//printf("damage: %d\n", m_damage);
				}
				else {
					if (((m_damage > scan.m_damage || (m_damage >= (scan.m_damage - 15)) && distance < distance_to_center) || done && good_for_baim)/* && (scan.m_hitchance * 0.9f) < point_chance */ && good_for_safepoint)
					{
						// save new best data.
						scan.m_damage = m_damage;
						scan.m_pos = point;
						scan.m_hitbox = it.m_index;
						scan.m_safepoint[it.m_index] = safepoints > 2;
						best_damage_per_hitbox = m_damage;
						distance_to_center = distance;
						//scan.m_hitchance = point_chance;

						// if the first point is lethal
						// screw the other ones.
						/*if ((point == points.front() && it.m_index == HITBOX_BODY || it.m_index < 5 && it.m_index > 1 && safepoints > 2) && m_damage > player->m_iHealth()) {
							done = true;
						}*/
					}
				}
				//}

				/*if ((Engine::Prediction::Instance()->m_flInaccuracy - Engine::Prediction::Instance()->m_flCalculatedInaccuracy) >= 0.011f
					&& it.m_index == 3
					&& scan.m_damage > min(wall_dmg, just_dmg)
					&& ctx.m_settings.aimbot_extra_scan_aim[2])
				{
					done = true;
				}*/

				if ((ctx.m_settings.aimbot_hitboxes[3] || ctx.main_exploit >= 2)
					&& (scan.m_safepoint[it.m_index] 
						|| scan.m_pos == points.front() && scan.m_hitbox == 3 
						|| scan.m_hitbox != 0 && distance <= distance_to_center)
					&& (scan.m_damage >= (hp * 0.5f)
						&& ctx.exploit_allowed
						&& ctx.main_exploit >= 2
						&& !ctx.fakeducking
						&& (ctx.ticks_allowed > 13 || ctx.force_aimbot > 1)
						|| scan.m_damage >= hp))
					done = true;

				//// we found a preferred / lethal hitbox.
				//if (done && good_for_safepoint) {
				//	// save new best data.
				//	scan.m_damage = m_damage;
				//	scan.m_pos = point;
				//	scan.m_safepoint[it.m_index] = safepoints > 2;
				//	scan.m_hitbox = it.m_index;
				//	//scan.m_hitchance = point_chance;
				//	//break;
				//}
				//else if (done)
				//	done = false;

				//if (done && points.size() > 0 && point == points.front())
				//	break;
			}
		}

		// ghetto break out of outer loop.
		if (done)
			break;
	}

	// we found something that we can damage.
	// set out vars.
	if (scan.m_damage > 0.f) {
		aim = scan.m_pos;
		damage = scan.m_damage;
		hitbox = scan.m_hitbox;
		return true;
	}
	else
		rdata->damage_ticks = 0;

	return false;
}

float GetFOV(const QAngle& view_angles, const Vector& start, const Vector& end) {
	Vector dir, fw;

	// get direction and normalize.
	dir = (end - start).Normalized();

	// get the forward direction vector of the view angles.
	Math::AngleVectors(view_angles, &fw);

	// get the angle between the view angles forward directional vector and the target location.
	return max(RAD2DEG(std::acos(fw.Dot(dir))), 0.f);
}

bool c_aimbot::SelectTarget(C_BasePlayer* player, C_Tickrecord* record, const Vector& aim, float damage) {
	float dist, fov, height;
	int   hp;

	// fov check.
	if (ctx.m_settings.aimbot_fov_limit > 0 && ctx.m_settings.aimbot_fov_limit < 180) {
		// if out of fov, retn false.
		if (GetFOV(ctx.cmd_original_angles, ctx.m_eye_position, aim) > ctx.m_settings.aimbot_fov_limit)
			return false;
	}

	if (ctx.m_last_shot_index == player->entindex() && damage > 0)
		return true;

	switch (ctx.m_settings.aimbot_target_selection) {

		// distance.
	case 0:
		dist = (record->origin - ctx.m_eye_position).Length();

		if (dist < m_best_dist) {
			m_best_dist = dist;
			return true;
		}

		break;

		// crosshair.
	case 1:
		fov = Math::GetFov(Engine::Movement::Instance()->m_qRealAngles, Math::CalcAngle(ctx.m_eye_position, aim));

		if (fov < m_best_fov) {
			m_best_fov = fov;
			return true;
		}

		break;

		// damage.
	case 2:
		if (damage > m_best_damage) {
			m_best_damage = damage;
			return true;
		}

		break;

		// lowest hp.
	case 3:
		// fix for retarded servers?
		hp = min(100, player->m_iHealth());

		if (hp < m_best_hp) {
			m_best_hp = hp;
			return true;
		}

		break;

		// least lag.
	case 4:
		if (record->lag < m_best_lag) {
			m_best_lag = record->lag;
			return true;
		}

		break;

		// height.
	case 5:
		height = record->origin.z - ctx.m_local()->m_vecOrigin().z;

		if (height < m_best_height) {
			m_best_height = height;
			return true;
		}

		break;

	default:
		return false;
	}

	return false;
}

void c_aimbot::apply(CUserCmd* cmd, bool* send_packet) {
	bool attack;

	// attack states.
	attack = cmd->buttons & IN_ATTACK;
	// ensure we're attacking.
	if (attack) {
		// choke every shot.

		auto v89 = csgo.m_client_state()->m_iChockedCommands;

		if (ctx.m_settings.fake_lag_shooting)
		{
			if (v89 < 14)
				*send_packet = false;
		}
		else if (!ctx.fakeducking)
			*send_packet = true;

		//if (ctx.main_exploit == 2 && ctx.force_aimbot < 1 && ctx.ticks_allowed > 13 && ctx.exploit_allowed/* && ctx.m_local()->m_iShotsFired() <= 1*/)
		//	*send_packet = false;

		if (m_target) {
			c_player_records* log = &feature::lagcomp->records[m_target->entindex()-1];
			auto r_log = &feature::resolver->player_records[m_target->entindex()-1];
			// make sure to aim at un-interpolated data.
			// do this so BacktrackEntity selects the exact record.
			if (log->best_record && !log->best_record->exploit && !log->best_record->lc_exploit)
				cmd->tick_count = TIME_TO_TICKS(log->best_record->simulation_time + ctx.lerp_time);

			// set angles to target.
			cmd->viewangles = m_angle;

			ctx.fside *= -1;

			//csgo.m_debug_overlay()->AddLineOverlay(ctx.m_eye_position, m_aim, 255, 0, 0, true, 10.f);

			if (ctx.m_settings.misc_visuals_hitboxes)
				visualize_hitboxes(m_target, (log->best_record->resolver_index != 0 && !log->saved_info.fakeplayer ? (log->best_record->resolver_index == 1 ? log->best_record->leftmatrixes : log->best_record->rightmatrixes) : log->best_record->matrixes), ctx.flt2color(ctx.m_settings.misc_visuals_hitboxes_color), (float)ctx.m_settings.misc_visuals_hitboxes_time);

			if (log->best_record != nullptr) {
				if (ctx.shots_fired[m_target->entindex() - 1] < 1) {
					r_log->last_resolving_method = log->best_record->resolver_index;
					r_log->last_resolved_side = log->best_record->resolver_index;
				}
				else
					r_log->last_resolved_side = log->best_record->resolver_index;

				if (log->best_record->shot_this_tick) {

					if (r_log->missed_shots[R_SHOT] == 0)
						r_log->missed_side1st[R_SHOT] = log->best_record->resolver_index;

					r_log->missed_shots[R_SHOT]++;
				}
				else {
					if (log->best_record->resolver_delta_multiplier > .7f)
						r_log->missed_shots[R_60_DELTA]++;
					else
						r_log->missed_shots[R_40_DELTA]++;

					if (r_log->missed_shots[R_USUAL] == 0)
						r_log->missed_side1st[R_USUAL] = log->best_record->resolver_index;

					r_log->missed_shots[R_USUAL]++;
				}
			}

			if (!log->best_record)
				r_log->missed_shots[R_USUAL]++;

			ctx.shots_fired[m_target->entindex() - 1]++;


			r_log->last_shot_missed = false;
			ctx.autopeek_back = true;

			// if not silent aim, apply the viewangles.
			if (!ctx.m_settings.aimbot_silent_aim)
				csgo.m_engine()->SetViewAngles(m_angle);

			const auto did_shot = feature::resolver->add_shot(ctx.m_eye_position, m_aim, log->best_record, feature::misc->hitbox_to_hitgroup(m_hitbox), m_damage, m_target->entindex());

			if (did_shot && ctx.m_settings.misc_notifications[3]) {

				char text[255];
				auto new_nick = m_target->m_szNickName(); new_nick.resize(31);

				if (log->best_record) {
					sprintf_s(text, sxor("SHOT %s (%s) | client DMG: [%.0f] | HC: [%.0f] | BT: [%dt|%d] | LAG: [%d%d|%ds] | A: [%.0f|%.2f]\0"),
						new_nick.c_str(),
						hitbox_to_string(m_hitbox).c_str(),
						m_damage,
						fminf(m_best_hc[0], m_best_hc[1]),
						max(0, (csgo.m_globals()->tickcount - TIME_TO_TICKS(log->best_record->simulation_time))),
						log->best_record->lag,
						log->best_record->resolver_index,
						log->best_record->resolver_type,
						ctx.shots_fired[m_target->entindex() - 1] - 1,
						log->best_record->velocity.Length2D(),
						log->best_record->resolver_delta_multiplier);
				}
				else
				{
					sprintf_s(text, sxor("(BAD) SHOT %s (%s) | client DMG: [%.0f] | HC: [%.0f] | BT: [%dt][%ds]\0"),
						new_nick.c_str(),
						hitbox_to_string(m_hitbox).c_str(),
						m_damage,
						fminf(m_best_hc[0], m_best_hc[1]),
						max(0, (csgo.m_globals()->tickcount - TIME_TO_TICKS(log->best_record->simulation_time))),
						ctx.shots_fired[m_target->entindex() - 1] - 1);
				}

				_events.emplace_back(text);
				/*_events.emplace_back(_f + new_nick
					+ _s + hitbox_to_string(m_hitbox)
					+ _b + std::to_string(m_hitbox)
					+ _hc + std::to_string(0)
					+ _bt + std::to_string(log->backtrack_ticks)
					+ _tp + std::to_string(log->best_record->shot_this_tick ? 2 : (log->best_record->animations_updated ? 3 : 1))
					+ _dmg + std::to_string(static_cast<int>(m_best_damage)) + _r + std::to_string(log->best_record->resolver_index)
					+ std::to_string(log->best_record->resolver_type)
					+ _b + std::to_string(log->best_record->lag)
					+ _db + std::string(send_packet ? _p : _m)
					+ _sh + std::to_string(ctx.shots_fired[m_target->entindex()-1]));*/
			}

			// set that we fired.
			ctx.m_ragebot_shot_nr = cmd->command_number;
			ctx.m_last_shot_index = m_target->entindex();
		}
	}
}