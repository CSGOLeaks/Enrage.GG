#include "source.hpp"
#include "sdk.hpp"
#include "grenades.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "visuals.hpp"
#include "hooked.hpp"
#include "autowall.hpp"
#include "prediction.hpp"
#include "movement.hpp"

std::vector<IClientUnknown*> BrokenEntities;

static std::vector<Breakable> Breakables;

void dispatch_effect(const char* name, const CEffectData& data)
{
	// 55 8B EC 83 E4 F8 83 EC 20 56 57 8B F9 C7 44 24
	static auto dispatch_effect_fn
		= reinterpret_cast<int(__fastcall*)(const char* n, const CEffectData & d)>(
			Memory::Scan("client.dll", "55 8B EC 83 E4 F8 83 EC 20 56 57 8B F9 C7 44 24")
			);

	dispatch_effect_fn(name, data);
}

void c_grenade_tracer::clear_broken()
{
	BrokenEntities.clear();
}

void c_grenade_tracer::add_broken(const Breakable& Breakable, IClientUnknown* Entity)
{
	static auto FindEntityByModel = [](const char* Model) -> C_BaseEntity *
	{
		auto Hash = hash_32_fnv1a_const(Model);
		for (auto i = csgo.m_globals()->maxClients + 1; i <= csgo.m_entity_list()->GetHighestEntityIndex(); i++)
		{
			auto Entity = csgo.m_entity_list()->GetClientEntity(i);
			if (!Entity)
				continue;

			auto CurrentModel = csgo.m_model_info()->GetModelName(Entity->GetModel());
			if (hash_32_fnv1a_const(CurrentModel) == Hash)
				return Entity;
		}

		return nullptr;
	};

	if (find(BrokenEntities.begin(), BrokenEntities.end(), Entity) != BrokenEntities.end())
		return; // Should ONLY fix infinite loops caused by "co-breaking" entities and not cause any bugs

	BrokenEntities.emplace_back(Entity);

	for (const auto& Broken : Breakable.Breakables)
	{
		if (auto BrokenEntity = FindEntityByModel(Broken->Model.c_str()))
			add_broken(*Broken, BrokenEntity);
	}
}

void c_grenade_tracer::add_broken(IClientUnknown* Entity)
{
	BrokenEntities.emplace_back(Entity);
}

bool c_grenade_tracer::is_broken(IClientUnknown* Entity)
{
	return find(BrokenEntities.begin(), BrokenEntities.end(), Entity) != BrokenEntities.end();
}

void c_grenade_tracer::reset() {
	m_start.clear();
	m_move.clear();
	m_velocity.clear();
	m_vel = 0.f;
	m_power = 0.f;
	m_collision_group = COLLISION_GROUP_PROJECTILE;
	m_collision_ent = nullptr;

	m_path.clear();
	m_bounces.clear();
}

void c_grenade_tracer::paint() {
	//static CTraceFilterSimple_game filter{};
	CGameTrace	                   trace;
	std::pair< float, C_BasePlayer* >    target{ 0.f, nullptr };
	std::vector<std::pair< C_BasePlayer*, float >>    damages;

	if (!ctx.m_settings.misc_grenade_preview)
		return;

	// we dont want to do this if dead.
	if (!ctx.m_local() || ctx.m_local()->IsDead() || !m_weapon() || !m_weapon()->IsGrenade())
		return;

	// aww man...
	// we need some points at least.
	if (m_path.size() < 2)
		return;

	// setup trace filter for later.
	//filter.SetPassEntity(g_cl.m_local);

	uint32_t filter[4] = { feature::autowall->get_filter_simple_vtable(), uint32_t(ctx.m_local()), 0, 0 };

	// previous point, set to last point.
	// or actually.. the first point, we are drawing in reverse.
	Vector prev = m_path.front();

	// iterate and draw path.
	for (const auto& cur : m_path) {
		Vector screen0, screen1;

		if (Drawing::WorldToScreen(prev, screen0) && Drawing::WorldToScreen(cur, screen1))
			Drawing::DrawLine(screen0.x, screen0.y, screen1.x, screen1.y, { 255, 255, 255 });

		// store point for next iteration.
		prev = cur;
	}

	// iterate all players.
	for (int i{ 1 }; i <= csgo.m_globals()->maxClients; ++i) {
		C_BasePlayer* player = (C_BasePlayer * )csgo.m_entity_list()->GetClientEntity(i);
		if (!player || player->IsDormant() || player->IsDead())
			continue;

		// get center of mass for player.
		Vector center = player->get_abs_origin();
		player->GetWorldSpaceCenter(center);

		// get delta between center of mass and final nade pos.
		Vector delta = center - prev;

		if (m_id == WEAPON_HEGRENADE) {
			// pGrenade->m_flDamage = 100;
			// pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;

			// is within damage radius?
			if (delta.Length() > 350.f)
				continue;

			Ray_t l;
			l.Init(prev, center);

			// check if our path was obstructed by anything using a trace.
			csgo.m_engine_trace()->TraceRay(l, MASK_SHOT, (ITraceFilter*)& filter, &trace);

			// something went wrong here.
			if (!trace.m_pEnt || trace.m_pEnt != player)
				continue;

			// rather 'interesting' formula by valve to compute damage.
			float d = (delta.Length() - 25.f) / 140.f;
			float damage = 105.f * std::exp(-d * d);

			// scale damage.
			damage = feature::autowall->ScaleDamage(player, damage, 1.f, HITGROUP_CHEST);

			// clip max damage.
			damage = min(damage, (player->m_ArmorValue() > 0) ? 57.f : 98.f);

			//if (player->m_iTeamNum() != ctx.m_local()->m_iTeamNum())
			damages.emplace_back(player, damage);

			// better target?
			if (damage > target.first) {
				target.first = damage;
				target.second = player;
			}
		}
		else if (m_id == WEAPON_MOLOTOV || m_id == WEAPON_INCGRENADE)
		{
			// pGrenade->m_flDamage = 100;
			// pGrenade->m_DmgRadius = pGrenade->m_flDamage * 3.5f;

			// is within damage radius?
			if (delta.Length() > 150.f)
				continue;

			Ray_t l;
			l.Init(prev, center);

			// check if our path was obstructed by anything using a trace.
			csgo.m_engine_trace()->TraceRay(l, MASK_SHOT, (ITraceFilter*)& filter, &trace);

			// something went wrong here.
			if (!trace.m_pEnt || trace.m_pEnt != player)
				continue;

			//if (player->m_iTeamNum() != ctx.m_local()->m_iTeamNum())
			damages.emplace_back(player, float(delta.Length() * 0.0254f) + 1337.0f);
		}
	}

	// we have a target for damage.
	//if (target.second) {
	//	Vector screen;

		// replace the last bounce with green.
		//if (!m_bounces.empty())
		//	m_bounces.back().color = { 0, 255, 0, 255 };

		//if (Drawing::WorldToScreen(prev, screen))
		//	Drawing::DrawString(F::ESPInfo,screen.x, screen.y + 5, Color{ 255, 255, 255, 0xb4 }, FONT_CENTER,sxor("%i"), (int)target.first);
	//}

	//if (damages.size() > 0)
	//{
	//	// replace the last bounce with green.
	//	if (!m_bounces.empty())
	//		m_bounces.back().color = { 0, 255, 0, 255 };

	//	for (auto idmg : damages)
	//	{
	//		if (!idmg.first || idmg.first->IsDormant() || !idmg.first->GetClientClass())
	//			continue;

	//		Vector screen;

	//		if (Drawing::WorldToScreen(idmg.first->get_abs_origin(), screen)) {

	//			auto& info = idmg.second;

	//			if (info < 100.f)
	//				Drawing::DrawString(F::ESPInfo, screen.x, screen.y + 15, info >= idmg.first->m_iHealth() ? Color{ 255, 0, 0, 250 } : Color{ 225, 225, 225, 240 }, FONT_CENTER, sxor("-%i"), (int)idmg.second);
	//			else
	//				Drawing::DrawString(F::ESPInfo, screen.x, screen.y + 15, ctx.m_settings.menu_color.alpha(250), FONT_CENTER, sxor("%.1fm"), float(idmg.second - 1337.f));
	//		}
	//	}
	//}

	// render bounces.
	for (auto &b : m_bounces) {
		Vector screen;

		if (b.point == m_bounces.back().point)
			break;

		if (Drawing::WorldToScreen(b.point, screen)) {
			Drawing::DrawRect(screen.x - 2, screen.y - 2, 4, 4, b.color);
			Drawing::DrawOutlinedRect(screen.x - 3, screen.y - 3, 6, 6, Color{58,58,58, 0xb4});
		}
	}

	if (m_bounces.size() > 0)
	{

		/*if (m_id == WEAPON_MOLOTOV || m_id == WEAPON_INCGRENADE)
		{
			CEffectData data;
			data.hitBox = iEffectIndex;
			data.origin = m_bounces.back().point;
			data.otherEntIndex = 0;
			data.damageType = 2;

			dispatch_effect("weapon_molotov_held", data);
		}
		else *///if (m_id == WEAPON_SMOKEGRENADE)
		//	last_smoke_endpos = m_bounces.back().point;

		const float step = (M_PI * 2.f) / 24.0f;
		auto radius = 24.0f;
		static bool lol = true;
		static auto start = 0.f;
		static auto max_a = ((M_PI * 2.f) - step);
		static float prev_a = 0.f;
		Vector w2sCenter;
		if (Drawing::WorldToScreen(m_bounces.back().point, w2sCenter)) {
			Drawing::DrawBox(w2sCenter.x, w2sCenter.y, 10.f, 10.f, Color::Red().alpha(240));
		}
	}
}

void c_grenade_tracer::think(CUserCmd* cmd) {
	bool attack, attack2;

	// reset some data.
	reset();

	if (!ctx.m_settings.misc_grenade_preview)
		return;

	if (!ctx.m_local() || ctx.m_local()->IsDead())
		return;

	// validate nade.
	if (!m_weapon() || !m_weapon()->IsGrenade())
		return;

	attack = (cmd->buttons & IN_ATTACK);
	attack2 = (cmd->buttons & IN_ATTACK2);

	if (!attack && !attack2)
		return;

	m_id = m_weapon()->m_iItemDefinitionIndex();
	m_power = m_weapon()->m_flThrowStrength();
	m_vel = ctx.latest_weapon_data->throw_velocity;

	if (m_id != WEAPON_SMOKEGRENADE)
		last_smoke_endpos.clear();

	simulate();
}

void c_grenade_tracer::simulate() {
	// init member variables
	// that will be used during the simulation.
	setup();

	// log positions 20 times per second.
	size_t step = (size_t)TIME_TO_TICKS(0.05f), timer{ 0u };

	// iterate until the container is full, should never happen.
	for (size_t i{ 0u }; i < 4096u; ++i) {

		// the timer was reset, insert new point.
		if (!timer)
			m_path.emplace_back(m_start);

		// advance object to this frame.
		size_t flags = advance(i);

		// if we detonated, we are done.
		// our path is complete.
		if ((flags & DETONATE))
			break;

		// reset or bounced.
		// add a new point when bounced, and one every step.
		if ((flags & BOUNCE) || timer >= step)
			timer = 0;

		// increment timer.
		else
			++timer;

		if (m_velocity == Vector{})
			break;
	}

	// fire grenades can extend to the ground.
	// this happens if their endpoint is within range of the floor.
	// 131 units to be exact.
	if (m_id == WEAPON_MOLOTOV || m_id == WEAPON_FIREBOMB) {
		CGameTrace trace;
		PhysicsPushEntity(m_start, { 0.f, 0.f, -131.f }, trace, ctx.m_local());

		if (trace.fraction < 0.9f)
			m_start = trace.endpos;
	}

	// store final point.
	// likely the point of detonation.
	m_collision_ent = nullptr;
	m_path.emplace_back(m_start);
	m_bounces.emplace_back(m_start, Color::Red(250));
}

void c_grenade_tracer::setup() {
	// get the last CreateMove angles.
	QAngle angle = ctx.cmd_original_angles;

	//csgo.m_engine()->GetViewAngles(angle);

	// grab the pitch from these angles.
	float pitch = angle.x;

	// correct the pitch.
	if (pitch < -90.f)
		pitch += 360.f;

	else if (pitch > 90.f)
		pitch -= 360.f;

	// a rather 'interesting' approach at the approximation of some angle.
	// lets keep it on a pitch 'correction'.
	angle.x = pitch - (90.f - std::abs(pitch)) * 10.f / 90.f;

	// get ThrowVelocity from weapon files.
	float vel = m_vel * 0.9f;

	// clipped to [ 15, 750 ]
	Math::clamp(vel, 15.f, 750.f);

	// apply throw power to velocity.
	// this is set depending on mouse states:
	// m1=1  m1+m2=0.5  m2=0
	vel *= ((m_power * 0.7f) + 0.3f);

	// convert throw angle into forward direction.
	Vector forward;
	Math::AngleVectors(angle, &forward);

	// set start point to our shoot position.
	m_start = ctx.m_local()->GetEyePosition();

	// adjust starting point based on throw power.
	m_start.z += (m_power * 12.f) - 12.f;

	// create end point from start point.
	// and move it 22 units along the forward axis.
	Vector end = m_start + (forward * 22.f);

	CGameTrace trace;
	TraceHull(m_start, end, trace, ctx.m_local());

	// we now have 'endpoint', set in our gametrace object.

	// move back start point 6 units along forward axis.
	m_start = trace.endpos - (forward * 6.f);

	// finally, calculate the velocity where we will start off with.
	// weird formula, valve..
	m_velocity = ctx.m_local()->m_vecAbsVelocity();
	m_velocity *= 1.25f;
	m_velocity += (forward * vel);
}

size_t c_grenade_tracer::advance(size_t tick) {
	size_t     flags{ NONE };
	CGameTrace trace;

	// apply gravity.
	PhysicsAddGravityMove(m_move);

	// move object.
	PhysicsPushEntity(m_start, m_move, trace, ctx.m_local());

	// check if the object would detonate at this point.
	// if so stop simulating further and endthe path here.
	if (detonate(tick, trace))
		flags |= DETONATE;

	// fix collisions/bounces.
	if (trace.fraction != 1.f) {
		// mark as bounced.
		flags |= BOUNCE;

		// adjust velocity.
		ResolveFlyCollisionBounce(trace, &tick, &flags);
	}

	// take new start point.
	m_start = trace.endpos;

	return flags;
}

bool c_grenade_tracer::detonate(size_t tick, CGameTrace& trace) {

	static auto weapon_molotov_maxdetonateslope = csgo.m_engine_cvars()->FindVar(sxor("weapon_molotov_maxdetonateslope"));
	static auto molotov_throw_detonate_time = csgo.m_engine_cvars()->FindVar(sxor("molotov_throw_detonate_time"));

	// convert current simulation tick to time.
	float time = TICKS_TO_TIME(tick);

	// CSmokeGrenadeProjectile::Think_Detonate
	// speed <= 0.1
	// checked every 0.2s

	// CDecoyProjectile::Think_Detonate
	// speed <= 0.2
	// checked every 0.2s

	// CBaseCSGrenadeProjectile::SetDetonateTimerLength
	// auto detonate at 1.5s
	// checked every 0.2s

	switch (m_id) {
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return time >= 1.5f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_SMOKEGRENADE:
		return m_velocity.Length() <= 0.01f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_DECOY:
		return m_velocity.LengthSquared() <= 0.04f && !(tick % TIME_TO_TICKS(0.2f));

	case WEAPON_MOLOTOV:
	case WEAPON_FIREBOMB:
		// detonate when hitting the floor.
		if ((trace.fraction != 1.f || trace.m_pEnt && ((C_BasePlayer*)trace.m_pEnt)->GetClientClass() && !(((C_BasePlayer*)trace.m_pEnt)->GetClientClass()->m_ClassID != class_ids::CCSPlayer)) && (std::cos(DEG2RAD(weapon_molotov_maxdetonateslope->GetFloat())) <= trace.plane.normal.z))
			return true;

		// detonate if we have traveled for too long.
		// checked every 0.1s
		return time >= molotov_throw_detonate_time->GetFloat() && !(tick % TIME_TO_TICKS(0.2f));

	default:
		return false;
	}

	return false;
}

//void c_grenade_tracer::OnNewMap(const char* Map) // TO DO: Maybe handle more events
//{
//	struct EventData
//	{
//		EventData(const std::string& Targetname, const std::vector<std::string>& OnBreak) :
//			Targetname(Targetname),
//			OnBreak(OnBreak)
//		{ }
//
//		std::string Targetname;
//		std::vector<std::string> OnBreak;
//	};
//	std::vector<EventData> Events;
//	Breakables.clear();
//
//	static auto FindEntitiesByTargetname = [](const std::vector<EventData>& Events, const std::string& Targetname) // See https://github.com/LestaD/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/server/entitylist.cpp#L620 as well
//	{
//		std::vector<Breakable*> Entities;
//
//		for (size_t i = 0; i < Events.size(); ++i)
//		{
//			auto it1 = Events[i].Targetname.begin();
//			auto it2 = Targetname.begin();
//
//			while (it1 != Events[i].Targetname.end() && it2 != Targetname.end())
//			{
//				if (*it1 != *it2 && tolower(*it1) != tolower(*it2))
//					break;
//
//				++it1;
//				++it2;
//			}
//
//			if (it2 == Targetname.end())
//			{
//				if (it1 == Events[i].Targetname.end())
//					Entities.emplace_back(&Breakables[i]);
//			}
//			else if (*it2 == '*')
//				Entities.emplace_back(&Breakables[i]);
//		}
//
//		return Entities;
//	};
//
//	auto Entities = GetMapEntities(Map); // It's up to you to parse the BSP file
//	for (const auto& Entity : Entities)
//	{
//		auto Classname = Entity.GetValue(sxor("classname"));
//
//		BreakableType Type;
//		if (Classname == sxor("func_breakable"))
//			Type = BreakableType::func_breakable;
//		else if (Classname == sxor("prop_dynamic") || Classname == sxor("prop_dynamic_override"))
//			Type = BreakableType::prop_dynamic;
//		else if (Classname == sxor("func_breakable_surf"))
//			Type = BreakableType::func_breakable_surf;
//		/*else if ( Classname == XorStringC( "prop_physics_multiplayer" ) ) // This never seems to break like I want it to so let's just ignore it, works fine in all official maps 10/4/17
//			Type = BreakableType::prop_physics_multiplayer;*/
//		else
//			continue;
//
//		if (Type == BreakableType::prop_dynamic && atoi(Entity.GetValue(sxor("solid")).c_str()) == solid_none) // TO DO: Figure out how to filter out bs better to save some iterations inside IsBreakableEntity
//			continue;
//
//		Breakable Breakable;
//		Breakable.Model = Entity.GetValue(sxor("model"));
//		Breakable.Health = atoi(Entity.GetValue(sxor("health")).c_str());
//		Breakable.Type = Type;
//		Breakables.emplace_back(Breakable);
//
//		Events.emplace_back(Entity.GetValue(sxor("targetname")), Entity.GetValues(sxor("OnBreak")));
//	}
//
//	for (size_t i = 0; i < Events.size(); ++i)
//	{
//		for (const auto& OnBreak : Events[i].OnBreak)
//		{
//			auto Event = ParseEvent(OnBreak); // Just split the string with '\x1B'
//
//			if (Event.TargetInput != sxor("Break"))
//				continue;
//
//			auto Found = FindEntitiesByTargetname(Events, Event.Target); // TO DO: Maybe use the delay, keeping in mind that it is completely useless in the official maps
//			if (Found.empty())
//				continue; // Maybe error?
//
//			Breakables[i].Breakables.insert(Breakables[i].Breakables.end(), Found.begin(), Found.end());
//		}
//	}
//}

//Breakable* IsBreakableEntity(C_BaseEntity* Entity)
//{
//	auto Model = Entity->GetModel();
//	if (!Model)
//		return nullptr;
//
//	auto ModelName = csgo.m_model_info()->GetModelName(Model);
//	for (auto& Breakable : Breakables)
//	{
//		if (Breakable.Model == ModelName)
//		{
//			if (Breakable.Type == BreakableType::prop_dynamic || Breakable.Type == BreakableType::prop_physics_multiplayer)
//			{
//				auto Breakables = 0;
//				if (auto Collide = csgo.m_model_info()->GetVCollide(Model))
//				{
//					auto Parser = csgo.m_physcollision()->VPhysicsKeyParserCreate(Collide->pKeyValues);
//					while (!Parser->Finished())
//					{
//						if (!_strcmpi(Parser->GetCurrentBlockName(), sxor("break")))
//							++Breakables;
//
//						Parser->SkipBlock();
//					}
//					csgo.m_physcollision()->VPhysicsKeyParserDestroy(Parser);
//				}
//
//				if (!Breakables)
//					return nullptr;
//
//				// TO DO: Properly look for "health" https://developer.valvesoftware.com/wiki/Prop_data, https://github.com/LestaD/SourceEngine2007/blob/master/se2007/game/shared/props_shared.cpp
//				if (auto KeyValueText = csgo.m_model_info()->GetModelKeyValueText(Model))
//				{
//					if (auto HealthLocation = strstr(KeyValueText, sxor("health")))
//					{
//						Breakable.Health = atoi(HealthLocation + 9);
//
//						return &Breakable;
//					}
//
//					if (!strstr(KeyValueText, sxor("Window")))
//						return nullptr;
//				}
//				else
//					return nullptr;
//			}
//
//			return &Breakable;
//		}
//	}
//
//	return nullptr;
//}

//void c_grenade_tracer::Touch(C_BaseEntity* pOther, trace_t& g_TouchTrace)
//{
//
//	if (m_WeaponID == WEAPON_MOLOTOV || m_WeaponID == WEAPON_INCGRENADE)
//	{
//		if (!pOther->IsPlayer() && g_TouchTrace.plane.normal.z >= cos(weapon_molotov_maxdetonateslope->GetFloat()))
//			Detonate(true);
//	}
//	else if (m_WeaponID == WEAPON_TAGRENADE)
//	{
//		if (!pOther->IsPlayer())
//			Detonate(true);
//	}
//}

void c_grenade_tracer::ResolveFlyCollisionBounce(CGameTrace& trace, size_t* tick, size_t *flags) {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1341

	// assume all surfaces have the same elasticity
	float surface = 1.f;

	//if (trace.m_pEnt) {
	//	if (trace.m_pEnt->is_breakable()) {
	//		if (trace.m_pEnt->GetClientClass() != nullptr &&
	//			(trace.m_pEnt->GetClientClass()->m_ClassID != class_ids::CFuncBrush ||
	//				trace.m_pEnt->GetClientClass()->m_ClassID != class_ids::CBaseDoor ||
	//				trace.m_pEnt->GetClientClass()->m_ClassID != class_ids::CCSPlayer ||
	//				trace.m_pEnt->GetClientClass()->m_ClassID != class_ids::CBaseEntity))
	//		{
	//
	//			//!trace.m_entity->is(sxor("CFuncBrush")) &&
	//			//!trace.m_entity->is(sxor("CBaseDoor")) &&
	//			//!trace.m_entity->is(sxor("CCSPlayer")) &&
	//			//!trace.m_entity->is(sxor("CBaseEntity"))) {
	//
	//			// move object.
	//			PhysicsPushEntity(m_start, m_move, trace, (C_BasePlayer*)trace.m_pEnt);
	//
	//			// deduct velocity penalty.
	//			m_velocity *= 0.4f;
	//			return;
	//		}
	//	}
	//}
	//if (trace.m_pEnt)
	//{
	//	//PhysicsPushEntity(m_start, m_move, trace, (C_BasePlayer*)trace.m_pEnt);
	//
	//	if (!is_broken(trace.m_pEnt))
	//	{
	//		if (trace.m_pEnt->GetClientClass() && trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CChicken)
	//			add_broken(trace.m_pEnt);
	//		if (trace.m_pEnt->is_breakable())
	//		{
	//			const auto breakable = ((C_BasePlayer*)trace.m_pEnt)->m_iHealth();
	//			add_broken(trace.m_pEnt);
	//
	//			m_velocity *= 0.4f;
	//		}
	//	}
	//	else
	//	{
	//		if (trace.m_pEnt->is_breakable())
	//			m_velocity *= 0.4f;
	//	}	
	//
	//	if (trace.m_pEnt && trace.m_pEnt->GetClientClass() && ((C_BaseEntity*)trace.m_pEnt)->IsPlayer())
	//		surface = 0.3f;
	//
	//	if (!trace.DidHitWorld())
	//	{
	//		if (m_collision_ent == trace.m_pEnt)
	//		{
	//			if (((C_BaseEntity*)trace.m_pEnt)->IsPlayer() 
	//				|| trace.m_pEnt->GetClientClass() && (trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CHostage 
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CMolotovProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CDecoyProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CSmokeGrenadeProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CSnowballProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CBreachChargeProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CBumpMineProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CBaseCSGrenadeProjectile
	//					|| trace.m_pEnt->GetClientClass()->m_ClassID == class_ids::CSensorGrenadeProjectile
	//				))
	//			{
	//				m_collision_group = COLLISION_GROUP_DEBRIS;
	//
	//				//	if ( IsGrenadeProjectile )
	//				//		trace.m_pEnt->m_CollisionGroup = COLLISION_GROUP_DEBRIS;
	//				m_velocity *= 0.4f;
	//			}
	//		}
	//
	//		m_collision_ent = trace.m_pEnt;
	//	}
	//
	//	//PhysicsPushEntity(m_start, m_move, trace, (C_BasePlayer*)trace.m_pEnt);
	//}

	if (trace.m_pEnt)
	{
		if (trace.m_pEnt->entindex() != 0)
		{
			m_velocity *= 0.3;
			m_collision_ent = trace.m_pEnt;
		}

		if (trace.m_pEnt->is_breakable())
			if (((C_BasePlayer*)trace.m_pEnt)->m_iHealth() <= 0)
			{
				m_velocity *= 0.4;
				m_collision_ent = trace.m_pEnt;
				return;
			}
	}

	// combine elasticities together.
	float elasticity = 0.45f * surface;

	// clipped to [ 0, 0.9 ]
	Math::clamp(elasticity, 0.f, 0.9f);

	Vector velocity;
	PhysicsClipVelocity(m_velocity, trace.plane.normal, velocity, 2.f);
	velocity *= elasticity;

	if (trace.plane.normal.z > 0.7f) {
		float speed = velocity.LengthSquared();

		// hit surface with insane speed.
		if (speed > 96000.f) {

			// weird formula to slow down by normal angle?
			float len = velocity.Normalized().Dot(trace.plane.normal);
			if (len > 0.5f)
				velocity *= 1.5f - len;
		}

		// are we going too slow?
		// just stop completely.
		if (speed < 400.f)
			m_velocity = Vector{};

		else {
			// set velocity.
			m_velocity = velocity;

			// compute friction left.
			float left = 1.f - trace.fraction;

			// advance forward.
			PhysicsPushEntity(trace.endpos, velocity * (left * csgo.m_globals()->interval_per_tick), trace, ctx.m_local());
		}
	}

	else {
		// set velocity.
		m_velocity = velocity;

		// compute friction left.
		float left = 1.f - trace.fraction;

		// advance forward.
		PhysicsPushEntity(trace.endpos, velocity * (left * csgo.m_globals()->interval_per_tick), trace, ctx.m_local());
	}

	m_bounces.emplace_back(trace.endpos, ctx.m_settings.menu_color.alpha(250));
}

void c_grenade_tracer::PhysicsPushEntity(Vector& start, const Vector& move, CGameTrace& trace, C_BasePlayer* ent, size_t *tick, size_t* flags) {
	// compute end point.
	Vector end = start + move;

	// trace through world.
	TraceHull(start, end, trace, ent);

	if (trace.m_pEnt && tick && flags)
	{
		if (detonate(*tick, trace))
			*flags |= DETONATE;
	}
}

void c_grenade_tracer::TraceHull(const Vector& start, const Vector& end, CGameTrace& trace, C_BasePlayer* ent) {
	uint32_t filter[4] = { feature::autowall->get_filter_simple_vtable(), uint32_t(ent), 0, 0 };

	//filter.SetPassEntity(ent);
	Ray_t t; t.Init(start, end, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f });

	ctx.force_low_quality_autowalling = true;

	csgo.m_engine_trace()->TraceRay(t, MASK_SOLID, (ITraceFilter*)& filter, &trace);
	ctx.force_low_quality_autowalling = false;
}

void c_grenade_tracer::PhysicsAddGravityMove(Vector& move) {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1264

	// gravity for grenades.
	float gravity = 800.f * 0.4f;

	// move one tick using current velocity.
	move.x = m_velocity.x * csgo.m_globals()->interval_per_tick;
	move.y = m_velocity.y * csgo.m_globals()->interval_per_tick;

	// apply linear acceleration due to gravity.
	// calculate new z velocity.
	float z = m_velocity.z - (gravity * csgo.m_globals()->interval_per_tick);

	// apply velocity to move, the average of the new and the old.
	move.z = ((m_velocity.z + z) / 2.f) * csgo.m_globals()->interval_per_tick;

	// write back new gravity corrected z-velocity.
	m_velocity.z = z;
}

void c_grenade_tracer::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce) {
	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1294
	constexpr float STOP_EPSILON = 0.1f;

	// https://github.com/VSES/SourceEngine2007/blob/master/se2007/game/shared/physics_main_shared.cpp#L1303

	float backoff = in.Dot(normal) * overbounce;

	for (int i{}; i < 3; ++i) {
		out[i] = in[i] - (normal[i] * backoff);

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.f;
	}
}