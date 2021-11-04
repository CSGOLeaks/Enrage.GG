#include "sdk.hpp"
#include "autowall.hpp"
#include "rage_aimbot.hpp"
#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "math.hpp"
#include "displacement.hpp"

#include <thread>

#define SLOBYTE(x)   (*((int8_t*)&(x)))

void c_autowall::TraceLine(Vector& absStart, const Vector& absEnd, unsigned int mask, C_BasePlayer* ignore, CGameTrace* ptr)
{
	Ray_t ray;
	ray.Init(absStart, absEnd);
	CTraceFilter filter;
	filter.pSkip = ignore;

	csgo.m_engine_trace()->TraceRay(ray, mask, &filter, ptr);
}

float c_autowall::ScaleDamage(C_BasePlayer* player, float damage, float armor_ratio, int hitgroup) {
	if (!player || !player->GetClientClass() || !ctx.m_local()->get_weapon() || player->GetClientClass()->m_ClassID != class_ids::CCSPlayer)
		return 0.f;

	auto new_damage = damage;

	const auto is_zeus = ctx.m_local()->get_weapon()->m_iItemDefinitionIndex() == WEAPON_TASER;

	static auto is_armored = [](C_BasePlayer* player, int armor, int hitgroup) {
		if (player && player->m_ArmorValue() > 0)
		{
			if (player->m_bHasHelmet() && hitgroup == HITGROUP_HEAD || (hitgroup >= HITGROUP_CHEST && hitgroup <= HITGROUP_RIGHTARM || hitgroup == 8 || hitgroup == 0))
				return true;
		}
		return false;
	};

	if (!is_zeus) {
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			new_damage *= 4.f;
			break;
		case HITGROUP_STOMACH:
			new_damage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			new_damage *= .75f;
			break;
		default:
			break;
			/*4.0; 1
			1.0; 2
			1.25; 3
			1.0; 4
			1.0; 5
			0.75; 6
			0.75; 7
			1.0; 8*/
		}
	}
	else
		new_damage *= 0.92f;

	if (is_armored(player, player->m_ArmorValue(), hitgroup))
	{
		float flHeavyRatio = 1.0f;
		float flBonusRatio = 0.5f;
		float flRatio = armor_ratio * 0.5f;
		float flNewDamage;

		if (!player->m_bHasHeavyArmor())
		{
			flNewDamage = damage * flRatio;
		}
		else
		{
			flBonusRatio = 0.33f;
			flRatio = armor_ratio * 0.25f;
			flHeavyRatio = 0.33f;
			flNewDamage = (damage * flRatio) * 0.85f;
		}

		int iArmor = player->m_ArmorValue();

		if (((damage - flNewDamage) * (flHeavyRatio * flBonusRatio)) > iArmor)
			flNewDamage = damage - (iArmor / flBonusRatio);

		new_damage = flNewDamage;
	}

	return floorf(new_damage);
}

void c_autowall::ScaleDamage(CGameTrace& enterTrace, weapon_info* weaponData, float& currentDamage)
{
	if (!enterTrace.m_pEnt || !enterTrace.m_pEnt->GetClientClass() || !ctx.m_local()->get_weapon() || enterTrace.m_pEnt->GetClientClass()->m_ClassID != class_ids::CCSPlayer)
		return;

	C_BasePlayer* target = (C_BasePlayer*)enterTrace.m_pEnt;

	auto new_damage = currentDamage;

	const int hitgroup = enterTrace.hitgroup;
	const auto is_zeus = ctx.m_local()->get_weapon()->m_iItemDefinitionIndex() == WEAPON_TASER;

	static auto is_armored = [](C_BasePlayer* player, int armor, int hitgroup) {
		if (player && player->m_ArmorValue() > 0)
		{
			if (player->m_bHasHelmet() && hitgroup == HITGROUP_HEAD || (hitgroup >= HITGROUP_CHEST && hitgroup <= HITGROUP_RIGHTARM))
				return true;
		}
		return false;
	};

	if (!is_zeus) {
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			new_damage *= 4.f;
			break;
		case HITGROUP_STOMACH:
			new_damage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			new_damage *= .75f;
			break;
		default:
			break;
		/*4.0; 1
		1.0; 2
		1.25; 3
		1.0; 4
		1.0; 5
		0.75; 6
		0.75; 7
		1.0; 8*/
		}
	}
	else
		new_damage *= 0.92f;

	if (is_armored(target, target->m_ArmorValue(), hitgroup))
	{
		float flHeavyRatio = 1.0f;
		float flBonusRatio = 0.5f;
		float flRatio = weaponData->armor_ratio * 0.5f;
		float flNewDamage;

		if (!target->m_bHasHeavyArmor())
		{
			flNewDamage = new_damage * flRatio;
		}
		else
		{
			flBonusRatio = 0.33f;
			flRatio = weaponData->armor_ratio * 0.5f;
			flHeavyRatio = 0.33f;
			flNewDamage = (new_damage * (flRatio * 0.5)) * 0.85f;
		}

		int iArmor = target->m_ArmorValue();

		if (((new_damage - flNewDamage) * (flBonusRatio * flHeavyRatio)) > iArmor)
			flNewDamage = new_damage - (iArmor / flBonusRatio);

		new_damage = flNewDamage;
	}

	currentDamage = new_damage;
}

//void c_autowall::ScaleDamage(C_BasePlayer* entity, int hitgroup, weapon_info* weaponData, float& currentDamage)
//{
//	if (!entity || entity->IsDormant() || !entity->GetClientClass() || !ctx.m_local()->get_weapon()) {
//		currentDamage = 0.0f;
//		return;
//	}
//
//	const auto is_zeus = ctx.m_local()->get_weapon()->m_iItemDefinitionIndex() == WEAPON_TASER;
//
//	const auto is_armored = [&]() -> bool
//	{
//		if (entity->m_ArmorValue() > 0)
//		{
//			switch (hitgroup)
//			{
//			case HITGROUP_GENERIC:
//			case HITGROUP_CHEST:
//			case HITGROUP_STOMACH:
//			case HITGROUP_LEFTARM:
//			case HITGROUP_RIGHTARM:
//				return true;
//			case HITGROUP_HEAD:
//				return entity->m_bHasHelmet();
//			default:
//				break;
//			}
//		}
//
//		return false;
//	};
//
//	float new_damage = currentDamage;
//
//	if (!is_zeus) {
//		switch (hitgroup)
//		{
//		case HITGROUP_HEAD:
//			if (entity->m_bHasHeavyArmor())
//				new_damage = currentDamage * 2.0f;
//			else
//				new_damage = currentDamage * 4.0f;
//			break;
//		case HITGROUP_STOMACH:
//			new_damage = currentDamage * 1.25f;
//			break;
//		case HITGROUP_LEFTLEG:
//		case HITGROUP_RIGHTLEG:
//			new_damage = currentDamage * .75f;
//			break;
//		default:
//			new_damage = currentDamage;
//			break;
//			/*4.0; 1
//			1.0; 2
//			1.25; 3
//			1.0; 4
//			1.0; 5
//			0.75; 6
//			0.75; 7
//			1.0; 8*/
//		}
//	}
//	else
//		new_damage = currentDamage * 0.92f;
//
//	if (is_armored())
//	{
//		auto modifier = 1.f, armor_bonus_ratio = .5f, armor_ratio = weaponData->armor_ratio * .5f;
//
//		if (entity->m_bHasHeavyArmor())
//		{
//			armor_bonus_ratio = 0.33f;
//			armor_ratio *= 0.2f;
//			modifier = 0.25f;
//		}
//
//		auto new_damage = currentDamage * armor_ratio;
//
//		if (entity->m_bHasHeavyArmor())
//			new_damage *= 0.85f;
//
//		const auto scaled_armor_ratio = (currentDamage - new_damage) * (modifier * armor_bonus_ratio);
//
//		if (scaled_armor_ratio > entity->m_ArmorValue())
//			new_damage = currentDamage - (entity->m_ArmorValue() / armor_bonus_ratio);
//
//		currentDamage = new_damage;
//	}
//
//	currentDamage = new_damage;
//}

uint32_t c_autowall::get_filter_simple_vtable()
{
	return *reinterpret_cast<uint32_t*>(Engine::Displacement::Signatures[c_signatures::TRACEFILTER_SIMPLE] + 0x3d);
}

bool c_autowall::TraceToExit(const Vector& start, const Vector dir, Vector& out, trace_t* enter_trace, trace_t* exit_trace)
{
	static ConVar* sv_clip_penetration_traces_to_players = csgo.m_engine_cvars()->FindVar(sxor("sv_clip_penetration_traces_to_players"));

	Vector          new_end;
	float           dist = 0.0f;
	int				iterations = 23;
	int				first_contents = 0;
	int             contents;
	Ray_t r{};

	while (1)
	{
		iterations--;

		if (iterations <= 0 || dist > 90.f)
			break;

		dist += 4.0f;
		out = start + (dir * dist);

		contents = csgo.m_engine_trace()->GetPointContents(out, 0x4600400B, nullptr);

		if (first_contents == -1)
			first_contents = contents;

		if (contents & 0x600400B && (!(contents & CONTENTS_HITBOX) || first_contents == contents))
			continue;

		new_end = out - (dir * 4.f);

		//r.Init(out, new_end);
		//CTraceFilter filter;
		//filter.pSkip = nullptr;
		//csgo.m_engine_trace()->TraceRay(r, MASK_SHOT, &filter, exit_trace);
		TraceLine(out, new_end, 0x4600400B, nullptr, exit_trace);

		if (exit_trace->startsolid && (exit_trace->surface.flags & SURF_HITBOX) != 0)
		{
			//r.Init(out, start);
			//filter.pSkip = exit_trace->m_pEnt;
			//filter.pSkip = (exit_trace->m_pEnt);
			//csgo.m_engine_trace()->TraceRay(r, MASK_SHOT_HULL, &filter, exit_trace);
			TraceLine(out, start, MASK_SHOT_HULL, (C_BasePlayer*)exit_trace->m_pEnt, exit_trace);

			if (exit_trace->DidHit() && !exit_trace->startsolid)
			{
				out = exit_trace->endpos;
				return true;
			}
			continue;
		}

		if (!exit_trace->DidHit() || exit_trace->startsolid)
		{
			if (enter_trace->m_pEnt != csgo.m_entity_list()->GetClientEntity(0)) {
				if (exit_trace->m_pEnt && exit_trace->m_pEnt->is_breakable())
				{
					exit_trace->surface.surfaceProps = enter_trace->surface.surfaceProps;
					exit_trace->endpos = start + dir;
					return true;
				}
			}

			continue;
		}

		if ((exit_trace->surface.flags & 0x80u) != 0)
		{
			if (enter_trace->m_pEnt && enter_trace->m_pEnt->is_breakable()
				&& exit_trace->m_pEnt && exit_trace->m_pEnt->is_breakable())
			{
				out = exit_trace->endpos;
				return true; 
			}

			if (!(enter_trace->surface.flags & 0x80u))
				continue;
		}
	
		if (exit_trace->plane.normal.Dot(dir) <= 1.f) // exit nodraw is only valid if our entrace is also nodraw
		{
			out -= dir * (exit_trace->fraction * 4.0f);
			return true;
		}
	}

	return false;
}

bool c_autowall::HandleBulletPenetration(C_BasePlayer* ignore,weapon_info* weaponData, trace_t& enterTrace, Vector& eyePosition, Vector direction, int& possibleHitsRemaining, float& currentDamage, float penetrationPower, float ff_damage_bullet_penetration, bool pskip)
{
	static ConVar* ff_damage_reduction_bullets = csgo.m_engine_cvars()->FindVar(sxor("ff_damage_reduction_bullets"));

	if (possibleHitsRemaining <= 0 || weaponData->penetration <= 0 || currentDamage < 1) {
		//possibleHitsRemaining = 0;
		return false;
	}

	//SafeLocalPlayer() false;
	CGameTrace exitTrace;
	auto* pEnemy = (C_BasePlayer*)enterTrace.m_pEnt;
	auto* const enterSurfaceData = csgo.m_phys_props()->GetSurfaceData(enterTrace.surface.surfaceProps);
	const int enter_material = enterSurfaceData->game.material;
	
	if (!enterSurfaceData || enterSurfaceData->game.penetrationmodifier <= 0)
		return false;

	const auto enter_penetration_modifier = enterSurfaceData->game.penetrationmodifier;
	//float enterDamageModifier = enterSurfaceData->game.damagemodifier;// , modifier, finalDamageModifier, combinedPenetrationModifier;
	const bool isSolidSurf = (enterTrace.contents & CONTENTS_GRATE);
	const bool isLightSurf = (enterTrace.surface.flags & SURF_NODRAW);

	if ((possibleHitsRemaining <= 0 && !isLightSurf && !isSolidSurf && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS)
		|| penetrationPower <= 0)
		return false;

	Vector end;

	if (!TraceToExit(enterTrace.endpos, direction, end, &enterTrace, &exitTrace)) {
		if (!(csgo.m_engine_trace()->GetPointContents(end, 0x600400B) & 0x600400B))
			return false;
	}

	auto* const exitSurfaceData = csgo.m_phys_props()->GetSurfaceData(exitTrace.surface.surfaceProps);
	const int exitMaterial = exitSurfaceData->game.material;
	const float exitSurfPenetrationModifier = exitSurfaceData->game.penetrationmodifier;
	//float exitDamageModifier = exitSurfaceData->game.damagemodifier;

	float combined_damage_modifier = 0.16f;
	float combined_penetration_modifier;

	//Are we using the newer penetration system?
	if (enter_material == CHAR_TEX_GLASS || enter_material == CHAR_TEX_GRATE) {
		combined_damage_modifier = 0.05f;
		combined_penetration_modifier = 3;
	}
	else if (isSolidSurf || isLightSurf) {
		combined_damage_modifier = 0.16f;
		combined_penetration_modifier = 1;
	}
	else if (enter_material == CHAR_TEX_FLESH && ff_damage_reduction_bullets->GetFloat() == 0 && pEnemy != nullptr && pEnemy->IsPlayer() && pEnemy->m_iTeamNum() == ctx.m_local()->m_iTeamNum())
	{
		if (ff_damage_bullet_penetration == 0)
		{
			// don't allow penetrating players when FF is off
			combined_penetration_modifier = 0;
			return false;
		}

		combined_penetration_modifier = ff_damage_bullet_penetration;
	}
	else {
		combined_penetration_modifier = (enter_penetration_modifier + exitSurfPenetrationModifier) / 2;
	}

	if (enter_material == exitMaterial) {
		if (exitMaterial == CHAR_TEX_WOOD || exitMaterial == CHAR_TEX_CARDBOARD)
			combined_penetration_modifier = 3;
		else if (exitMaterial == CHAR_TEX_PLASTIC)
			combined_penetration_modifier = 2;
	}

	/*auto v22 = fmaxf(1.0f / combined_penetration_modifier, 0.0f);
	auto v23 = fmaxf(3.0f / penetrationPower, 0.0f);

	auto penetration_modifier = fmaxf(0.f, 1.f / combined_penetration_modifier);
	auto penetration_distance = (exitTrace.endpos - enterTrace.endpos).Length();

	auto damage_lost = ((currentDamage * combined_damage_modifier) + ((v23 * v22) * 3.0f)) + (((penetration_distance * penetration_distance) * v22) * 0.041666668f);

	auto new_damage = currentDamage - damage_lost;

	currentDamage = new_damage;

	if (new_damage > 0.0f)
	{
		*eyePosition = exitTrace.endpos;
		--possibleHitsRemaining;
		return true;
	}*/

	auto penetration_mod = fmaxf(0.f, (3.f / penetrationPower) * 1.25f);

	float modifier = fmaxf(0, 1.0f / combined_penetration_modifier);

	auto thickness = (exitTrace.endpos - enterTrace.endpos).Length();
	/*thickness *= thickness;
	thickness *= flPenMod;
	thickness /= 24.f;*/

	const auto lost_damage = ((modifier * 3.f) * penetration_mod + (currentDamage * combined_damage_modifier)) + (((thickness * thickness) * modifier) / 24.f);
	/*fmaxf(0, currentDamage * combined_damage_modifier + flPenMod
		* 3.f * fmaxf(0, 3.f / penetrationPower) * 1.25f + thickness);*/

	currentDamage -= std::fmaxf(0.f, lost_damage);

	if (currentDamage < 1.f)
		return false;

	eyePosition = exitTrace.endpos;
	--possibleHitsRemaining;

	return true;
}

void c_autowall::FixTraceRay(Vector end, Vector start, trace_t* oldtrace, C_BasePlayer* ent) {
	if (!ent)
		return;

	const auto mins = ent->OBBMins();
	const auto maxs = ent->OBBMaxs();

	auto dir(end - start);
	auto len = dir.Normalize();

	const auto center = (mins + maxs) / 2;
	const auto pos(ent->m_vecOrigin() + center);

	auto to = pos - start;
	const float range_along = dir.Dot(to);

	float range;
	if (range_along < 0.f) {
		range = -(to).Length();
	}
	else if (range_along > len) {
		range = -(pos - end).Length();
	}
	else {
		auto ray(pos - (start + (dir * range_along)));
		range = ray.Length();
	}

	if (range <= 60.f) {

		Ray_t ray;
		ray.Init(start, end);

		trace_t trace;
		csgo.m_engine_trace()->ClipRayToEntity(ray, 0x4600400B, ent, &trace);

		if (oldtrace->fraction > trace.fraction)
			* oldtrace = trace;
	}
}

void c_autowall::ClipTraceToPlayers(const Vector& start, const Vector& end, uint32_t mask, ITraceFilter* filter, trace_t* tr) {
	Vector     pos, to, dir, on_ray;
	float      len, range_along, range;
	//CGameTrace new_trace;
	
	float smallestFraction = tr->fraction;
	constexpr float maxRange = 60.0f;

	//Vector delta(vecAbsEnd - vecAbsStart);
	//const float delta_length = delta.Normalize();

	dir = start - end;
	len = dir.Normalize();

	Ray_t ray;
	ray.Init(start, end);

	for (int i = 1; i <= 64; ++i) {
		C_BasePlayer* ent = csgo.m_entity_list()->GetClientEntity(i);
		if (!ent || ent->IsDormant() || ent->IsDead())
			continue;

		if (filter && !filter->ShouldHitEntity(ent, mask))
			continue;

		//matrix3x4_t coordinate_frame;
		//Math::AngleMatrix(ent->get_abs_angles(), ent->m_vecOrigin(), coordinate_frame);

		//auto collideble = ent->GetCollideable();
		//auto mins = collideble->OBBMins();
		//auto maxs = collideble->OBBMaxs();

		//auto obb_center = (maxs + mins) * 0.5f;
		//Math::VectorTransform(obb_center, coordinate_frame, obb_center);

		/*auto extend = (obb_center - vecAbsStart);
		auto rangeAlong = delta.Dot(extend);

		float range;
		if (rangeAlong >= 0.0f) {
			if (rangeAlong <= delta_length)
				range = Vector(obb_center - ((delta * rangeAlong) + vecAbsStart)).Length();
			else
				range = -(obb_center - vecAbsEnd).Length();
		}
		else {
			range = -extend.Length();
		}*/

		// set some local vars.
		pos = ent->m_vecOrigin() + ((ent->OBBMins() + ent->OBBMaxs()) * 0.5f);
		to = pos - start;
		range_along = dir.Dot(to);

		// off start point.
		if (range_along < 0.f)
			range = -(to).Length();

		// off end point.
		else if (range_along > len)
			range = -(pos - end).Length();

		// within ray bounds.
		else {
			on_ray = start + (dir * range_along);
			range = (pos - on_ray).Length();
		}

		if (/*range > 0.0f && */range <= maxRange) {
			trace_t playerTrace;

			//ray.Init(start, end);

			csgo.m_engine_trace()->ClipRayToEntity(ray, 0x4600400B, ent, &playerTrace);

			if (playerTrace.fraction < smallestFraction) {
				// we shortened the ray - save off the trace
				*tr = playerTrace;
				smallestFraction = playerTrace.fraction;
			}
		}
	}
}

int c_autowall::HitboxToHitgroup(C_BasePlayer* m_player, int ihitbox)
{/*
	if (ihitbox < 0 || ihitbox > 19) return 0;

	if (!m_player) return 0;

	const auto model = m_player->GetModel();

	if (!model)
		return 0;

	auto pStudioHdr = csgo.m_model_info()->GetStudioModel(model);

	if (!pStudioHdr)
		return 0;

	auto hitbox = pStudioHdr->pHitbox(ihitbox, m_player->m_nHitboxSet());

	if (!hitbox)
		return 0;

	return hitbox->group;*/

	switch (ihitbox)
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
	}
}

bool c_autowall::FireBullet(Vector eyepos, C_WeaponCSBaseGun* pWeapon, Vector& direction, float& currentDamage, C_BasePlayer* ignore, C_BasePlayer* to_who, int hitbox, bool* was_viable, std::vector<float>* power)
{
	if (!pWeapon || !ignore)
		return false;

	//SafeLocalPlayer() false;
	//bool sv_penetration_type;
	//	  Current bullet travel Power to penetrate Distance to penetrate Range               Player bullet reduction convars			  Amount to extend ray by
	float currentDistance = 0;

	static ConVar* damageBulletPenetration = csgo.m_engine_cvars()->FindVar(sxor("ff_damage_bullet_penetration"));

	const float ff_damage_bullet_penetration = damageBulletPenetration->GetFloat();

	weapon_info* weaponData = ctx.latest_weapon_data;
	CGameTrace enterTrace;

	CTraceFilter filter;
	filter.pSkip = ignore;

	if (!weaponData)
		return false;

	//Set our current damage to what our gun's initial damage reports it will do
	currentDamage = float(weaponData->damage);
	auto maxRange = weaponData->range;
	auto penetrationDistance = weaponData->range;
	auto penetrationPower = weaponData->penetration;
	auto RangeModifier = weaponData->range_modifier;

	//This gets set in FX_Firebullets to 4 as a pass-through value.
	//CS:GO has a maximum of 4 surfaces a bullet can pass-through before it 100% stops.
	//Excerpt from Valve: https://steamcommunity.com/sharedfiles/filedetails/?id=275573090
	//"The total number of surfaces any bullet can penetrate in a single flight is capped at 4." -CS:GO Official
	ctx.last_penetrated_count = 4;
	ctx.last_hitgroup = -1;

	if (power)
	{
		maxRange = power->at(0);
		penetrationDistance = power->at(1);
		penetrationPower = power->at(2);
		currentDamage = power->at(3);
		RangeModifier = power->at(4);
	}

	int penetrated = 0;

	//If our damage is greater than (or equal to) 1, and we can shoot, let's shoot.
	while (/*ctx.last_penetrated_count >= 0 &&*/currentDamage > 0)
	{
		//Calculate max bullet range

		//Create endpoint of bullet
		Vector end = eyepos + direction * (maxRange - currentDistance);

		TraceLine(eyepos, end, 0x4600400B/*_HULL | CONTENTS_HITBOX*/, ignore, &enterTrace);

		/*if (enterTrace.startsolid)
		{
			enterTrace.endpos = enterTrace.startpos;
			enterTrace.fraction = 0.0f;
		}*/
		//else
		//if (!(enterTrace.contents & CONTENTS_HITBOX)) {
		if (to_who/* && target_hitbox == HITBOX_HEAD*/ || enterTrace.contents & CONTENTS_HITBOX && enterTrace.m_pEnt) {
			//Pycache/aimware traceray fix for head while players are jumping
			FixTraceRay(eyepos + (direction * 40.f), eyepos, &enterTrace, (to_who != nullptr ? to_who : (C_BasePlayer*)enterTrace.m_pEnt));
		}
		else
			ClipTraceToPlayers(eyepos, eyepos + (direction * 40.f), 0x4600400B, &filter, &enterTrace);
		//}

		if (enterTrace.fraction == 1.0f)
			return false;

		//if (enterTrace.m_pEnt == nullptr || !((C_BasePlayer*)enterTrace.m_pEnt)->IsPlayer())// !hit_entity || !hit_entity->is_player
		//	enterTrace.m_pEnt = nullptr;

		//We have to do this *after* tracing to the player.
		//int enterMaterial = enterSurfaceData->game.material;

		//calculate the damage based on the distance the bullet traveled.
		currentDistance += enterTrace.fraction * (maxRange - currentDistance);

		//Let's make our damage drops off the further away the bullet is.
		currentDamage *= powf(RangeModifier, (currentDistance / 500));

		if (!(enterTrace.contents & CONTENTS_HITBOX))
			enterTrace.hitgroup = 1;

		//This looks gay as fuck if we put it into 1 long line of code.
		const bool canDoDamage = enterTrace.hitgroup > 0 && enterTrace.hitgroup <= 8 || enterTrace.hitgroup == HITGROUP_GEAR;
		const bool isPlayer = enterTrace.m_pEnt != nullptr 
			&& enterTrace.m_pEnt->GetClientClass()
			&& enterTrace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CCSPlayer 
			&& (!ctx.m_local() || ctx.m_local()->IsDead() || ((C_BasePlayer*)enterTrace.m_pEnt)->m_iTeamNum() != ctx.m_local()->m_iTeamNum());

		if (to_who)
		{
			if (enterTrace.m_pEnt && to_who == enterTrace.m_pEnt && canDoDamage && isPlayer) {
				const int group = (pWeapon->m_iItemDefinitionIndex() == WEAPON_TASER) ? HITGROUP_GENERIC : enterTrace.hitgroup;
				ctx.last_hitgroup = group;

				ScaleDamage(enterTrace, weaponData, currentDamage);

				if (was_viable != nullptr)
					*was_viable = (penetrated == 0);

				ctx.force_hitbox_penetration_accuracy = false;

				return true;
			}
		}
		else
		{
			if (enterTrace.m_pEnt && canDoDamage && isPlayer) {
				const int group = (pWeapon->m_iItemDefinitionIndex() == WEAPON_TASER) ? HITGROUP_GENERIC : enterTrace.hitgroup;
				ctx.last_hitgroup = group;

				ScaleDamage(enterTrace, weaponData, currentDamage);

				if (was_viable != nullptr)
					*was_viable = (penetrated == 0);

				ctx.force_hitbox_penetration_accuracy = false;

				return true;
			}
		}

		//Sanity checking / Can we actually shoot through?
		if (currentDistance > maxRange && penetrationPower
			|| csgo.m_phys_props()->GetSurfaceData(enterTrace.surface.surfaceProps)->game.penetrationmodifier < 0.1f) {
			return false;//ctx.last_penetrated_count = 0;
		}

		const auto prev = ctx.last_penetrated_count;

		//Calling HandleBulletPenetration here reduces our penetrationCounter, and if it returns true, we can't shoot through it.
		if (!HandleBulletPenetration(ignore, weaponData, enterTrace, eyepos, direction, ctx.last_penetrated_count, currentDamage, penetrationPower, ff_damage_bullet_penetration)) {
			break;
		}
		if (prev != ctx.last_penetrated_count)
			penetrated++;
	}

	return false;
}

////////////////////////////////////// Usage Calls //////////////////////////////////////
float c_autowall::CanHit(Vector& vecEyePos, Vector& point)
{
	Vector angles, direction;
	Vector tmp = point - vecEyePos;
	float currentDamage = 0;

	Math::VectorAngles(tmp, angles);
	Math::AngleVectors(angles, &direction);
	direction.Normalize();

	if (m_weapon() != nullptr && m_weapon()->IsGun() && FireBullet(vecEyePos, m_weapon(), direction, currentDamage, ctx.m_local()))
		return currentDamage;

	return -1; //That wall is just a bit too thick buddy
}

float c_autowall::CanHit(const Vector& vecEyePos, Vector& point, C_BasePlayer* ignore_ent, C_BasePlayer* to_who, int hitbox, bool* was_viable)
{
	if (ignore_ent == nullptr || to_who == nullptr)
		return 0;

	Vector direction;
	Vector tmp = point - vecEyePos;
	float currentDamage = 0;

	//Math::VectorAngles(tmp, angles);
	//Math::AngleVectors(angles, &direction);
	direction = tmp.Normalized();

	if (m_weapon() != nullptr)
	{
		if (m_weapon()->IsGun() && FireBullet(vecEyePos, m_weapon(), direction, currentDamage, ignore_ent, to_who, hitbox, was_viable))
			return currentDamage;
		else
			return -1;
	}

	return -1; //That wall is just a bit too thick buddy
}

float c_autowall::SimulateShot(Vector& vecEyePos, Vector& point, C_BasePlayer* ignore_ent, C_BasePlayer* to_who, bool* was_viable)
{
	if (ignore_ent == nullptr || to_who == nullptr)
		return 0;

	Vector angles, direction;
	const Vector tmp(point - vecEyePos);
	float currentDamage = 0.f;

	Math::VectorAngles(tmp, angles);
	Math::AngleVectors(angles, &direction);
	direction.Normalize();

	/*
		maxRange = power[0];
		penetrationDistance = power[1];
		penetrationPower = power[2];
		currentDamage = power[3];
	*/

	static std::vector<float> power = { 4000.f, 4000.f, 2.50f, 80.f, 1.f};

	if (m_weapon() != nullptr)
	{
		if (FireBullet(vecEyePos, m_weapon(), direction, currentDamage, ignore_ent, to_who, -1, was_viable, &power))
			return currentDamage;
		else
			return -1;
	}

	return -1; //That wall is just a bit too thick buddy
}