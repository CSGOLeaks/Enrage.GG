#include "visuals.hpp"
#include "source.hpp"
#include "entity.hpp"
#include "player.hpp"
#include "weapon.hpp"
#include "hooked.hpp"
#include "displacement.hpp"
#include "anti_aimbot.hpp"
#include "resolver.hpp"
#include "prop_manager.hpp"
#include <algorithm>
#include "movement.hpp"
#include "menu.hpp"
#include <time.h>
#include "prediction.hpp"
#include "rage_aimbot.hpp"
#include "lag_comp.hpp"
#include "chams.hpp"
#include "sound_parser.hpp"
#include "autowall.hpp"
#include "music_player.hpp"
#include "grenades.hpp"

#include <thread>
#include <cctype>
#include <map>

std::vector<_event> _events;
std::vector<c_bullet_tracer> bullet_tracers;
std::vector<c_damage_indicator> damage_indicators;
//std::vector<std::string> m_esp_info;

constexpr auto _hk = LIT(("HK"));
constexpr auto _k = LIT(("K"));
constexpr auto _h = LIT(("H"));
constexpr auto _r8 = LIT(("revolver"));
constexpr auto _z = LIT(("ZOOM"));
constexpr auto _pin = LIT(("PIN"));
constexpr auto _c4 = LIT(("C4"));
constexpr auto _vip = LIT(("VIP"));
constexpr auto _hs = LIT(("HEADSHOT"));

constexpr auto _m = LIT(("read tutorial on forums"));

bool c_visuals::get_espbox(C_BasePlayer* entity, int& x, int& y, int& w, int& h)
{
	if (!entity || !entity->GetClientClass())
		return false;

	if (entity->IsPlayer() && entity->GetCollideable())
	{
		//Vector origin, mins, maxs;
		//Vector bottom, top;

		//if (ctx.m_settings.esp_box_type == 1)
		//{
		//auto log = entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum() ? nullptr : &feature::lagcomp->records[entity->entindex() - 1];

		/*Vector maxs = entity->GetCollideable()->OBBMaxs();
		Vector pos, pos3d;
		Vector top, top3d;
		pos3d = entity->get_abs_origin() + (log && log->tick_records.size() > 0 && !ctx.m_local()->IsDead() ? log->tick_records[0].abs_velocity : entity->m_vecVelocity()) * csgo.m_globals()->interval_per_tick;
		top3d = pos3d + Vector(0, 0, maxs.z);

		if (!Drawing::WorldToScreen(pos3d, pos) || !Drawing::WorldToScreen(top3d, top))
			return false;

		int iMiddle = (pos.y - top.y) + 4;
		int iWidth = iMiddle / 3.5;
		int iCornerWidth = iWidth / 4;*/

		//// get interpolated origin.
		//origin = entity->get_abs_origin();

		//// get hitbox bounds.
		//entity->ComputeHitboxSurroundingBox(entity->m_CachedBoneData().Base(), &mins, &maxs);

		//// correct x and y coordinates.
		//mins = { origin.x, origin.y, mins.z };
		//maxs = { origin.x, origin.y, maxs.z + 8.f };

		//if (!Drawing::WorldToScreen(mins, bottom) || !Drawing::WorldToScreen(maxs, top))
		//	return false;

		//h = bottom.y - top.y;
		//w = h / 2.f;
		//x = bottom.x - (w / 2.f);
		//y = bottom.y - h;

		//	return true;
		//}
		//else 
		//{
		auto log = &feature::lagcomp->records[entity->entindex() - 1];

			auto min = entity->GetCollideable()->OBBMins();
			auto max = entity->GetCollideable()->OBBMaxs();

			Vector dir, vF, vR, vU;

			csgo.m_engine()->GetViewAngles(dir);
			dir.x = 0;
			dir.z = 0;

			Math::AngleVectors(dir, &vF, &vR, &vU);

			auto zh = vU * max.z + vF * max.y + vR * min.x; // = Front left front
			auto e = vU * max.z + vF * max.y + vR * max.x; //  = Front right front
			auto d = vU * max.z + vF * min.y + vR * min.x; //  = Front left back
			auto c = vU * max.z + vF * min.y + vR * max.x; //  = Front right back

			auto g = vU * min.z + vF * max.y + vR * min.x; //  = Bottom left front
			auto f = vU * min.z + vF * max.y + vR * max.x; //  = Bottom right front
			auto a = vU * min.z + vF * min.y + vR * min.x; //  = Bottom left back
			auto b = vU * min.z + vF * min.y + vR * max.x; //  = Bottom right back*-

			Vector pointList[] = {
				a,
				b,
				c,
				d,
				e,
				f,
				g,
				zh,
			};

			Vector transformed[ARRAYSIZE(pointList)];

			for (int i = 0; i < ARRAYSIZE(pointList); i++)
			{	
				auto origin = !entity->IsDormant() ? entity->get_abs_origin() : entity->m_vecOrigin();
				
				if (log && log->player == entity && log->records_count > 0 && !entity->IsDormant() && ctx.m_local() && !ctx.m_local()->IsDead()) {
					auto last = &log->tick_records[log->records_count & 63];
					origin = entity->get_bone_pos(8) + (last->origin - last->head_pos);
				}



				pointList[i] += origin;
			//	pointList[i] += Math::interpolate(entity->m_vecOldOrigin(), entity->m_vecOrigin(), 1.f - csgo.m_globals()->interval_per_tick);

				if (!Drawing::WorldToScreen(pointList[i], transformed[i]))
					return false;
			}

			float left = FLT_MAX;
			float top = -FLT_MAX;
			float right = -FLT_MAX;
			float bottom = FLT_MAX;

			for (int i = 0; i < ARRAYSIZE(pointList); i++) {
				if (left > transformed[i].x)
					left = transformed[i].x;
				if (top < transformed[i].y)
					top = transformed[i].y;
				if (right < transformed[i].x)
					right = transformed[i].x;
				if (bottom > transformed[i].y)
					bottom = transformed[i].y;
			}

			x = left;
			y = bottom;
			w = right - left;
			h = top - bottom;
		//}

		return true;
	}
	else
	{
		Vector /*vOrigin, */min, max, flb, brt, blb, frt, frb, brb, blt, flt;
		//float left, top, right, bottom;

		auto* collideable = entity->GetCollideable();

		if (!collideable)
			return false;

		min = collideable->OBBMins();
		max = collideable->OBBMaxs();

		matrix3x4_t& trans = entity->GetCollisionBoundTrans();

		Vector points[] =
		{
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(max.x, max.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(min.x, min.y, max.z),
			Vector(max.x, min.y, max.z)
		};

		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			Math::VectorTransform(points[i], trans, pointsTransformed[i]);
		}

		Vector pos = entity->get_abs_origin();

		if (!Drawing::WorldToScreen(pointsTransformed[3], flb) || !Drawing::WorldToScreen(pointsTransformed[5], brt)
			|| !Drawing::WorldToScreen(pointsTransformed[0], blb) || !Drawing::WorldToScreen(pointsTransformed[4], frt)
			|| !Drawing::WorldToScreen(pointsTransformed[2], frb) || !Drawing::WorldToScreen(pointsTransformed[1], brb)
			|| !Drawing::WorldToScreen(pointsTransformed[6], blt) || !Drawing::WorldToScreen(pointsTransformed[7], flt))
			return false;

		Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
		//+1 for each cuz of borders at the original box
		float left = flb.x;        // left
		float top = flb.y;        // top
		float right = flb.x;    // right
		float bottom = flb.y;    // bottom

		for (int i = 1; i < 8; i++)
		{
			if (left > arr[i].x)
				left = arr[i].x;
			if (bottom < arr[i].y)
				bottom = arr[i].y;
			if (right < arr[i].x)
				right = arr[i].x;
			if (top > arr[i].y)
				top = arr[i].y;
		}

		x = (int)left;
		y = (int)top;
		w = (int)(right - left);
		h = (int)(bottom - top);

		return true;
	}
	return false;
}

constexpr int dlinesize = 3, dlinedec = 5;
constexpr int linesize = 5, linedec = 11;

void c_visuals::damage_esp()
{
	if (damage_indicators.empty() || damage_indicators.size() > 20) return;

	int last_tick;
	Vector last_pos;

	for (auto it = damage_indicators.begin(); it != damage_indicators.end();)
	{
		auto record = &(*it);

		if (it == damage_indicators.end() || !record)
			break;

		if (csgo.m_globals()->realtime >= record->_time) {
			it = damage_indicators.erase(it);
			continue;
		}

		if (damage_indicators.size() > 10 && record->_time != damage_indicators.front()._time)
		{
			int skip = float(0.5f * (int)damage_indicators.size());

			auto prev = *(damage_indicators.begin() + skip);

			if (record->_time < prev._time) {
				++it;
				continue;
			}
		}

		if (!Drawing::WorldToScreen(record->_spot, record->w2s))
		{ 
			++it;
			continue;
		}

		const auto was_same = record->_tick == csgo.m_globals()->tickcount;

		if (!record->_headshot)
			last_tick = csgo.m_globals()->tickcount;

		auto alpha = min(245, 90 * (record->_time - csgo.m_globals()->realtime));

		//if (record._headshot && was_same) {
		//	Drawing::DrawString(F::ESPInfo, last_pos.x, last_pos.y - 2.f - 10.f * (2.f - ((record._time - csgo.m_globals()->realtime) * 0.4f)), ctx.m_settings.menu_color.alpha(alpha), FONT_CENTER, _hs.operator std::string().c_str());
		//
		//}
		//else {
			//Drawing::DrawString(F::ESPInfo, record.w2s.x, record.w2s.y - 1.f * (2.f - ((record._time - csgo.m_globals()->realtime) * 0.4f)), Color::White(alpha), FONT_CENTER, "%d", record._damage);

			//if (alpha > 0) {
				Drawing::DrawLine(record->w2s.x - dlinesize, record->w2s.y - dlinesize, record->w2s.x - dlinedec, record->w2s.y - dlinedec, Color(200, 200, 200, alpha));
				Drawing::DrawLine(record->w2s.x - dlinesize, record->w2s.y + dlinesize, record->w2s.x - dlinedec, record->w2s.y + dlinedec, Color(200, 200, 200, alpha));
				Drawing::DrawLine(record->w2s.x + dlinesize, record->w2s.y + dlinesize, record->w2s.x + dlinedec, record->w2s.y + dlinedec, Color(200, 200, 200, alpha));
				Drawing::DrawLine(record->w2s.x + dlinesize, record->w2s.y - dlinesize, record->w2s.x + dlinedec, record->w2s.y - dlinedec, Color(200, 200, 200, alpha));
			//}
			
			last_pos = record->w2s;
		//}

		++it;
	}
}

void c_visuals::logs()
{
	int bottom = 0;

	//if (ctx.m_local() && !ctx.m_local()->IsDead() && m_weapon() && ctx.latest_weapon_data) {
	//	if (!ctx.fired_shot.empty()) {
	//		for (auto it = ctx.fired_shot.begin(); it != ctx.fired_shot.end();)
	//		{
	//			if (it == ctx.fired_shot.end())
	//				break;

	//			const auto &record = &*it;

	//			if (record->_printed) {
	//				it = ctx.fired_shot.erase(it);
	//				continue;
	//			}

	//			if (record->_impact_called && record->_hurt_called) {
	//				//hit_chams.push_back(c_hit_chams(record._tickrecord, record._target->entindex()));
	//				//damage_indicators.push_back(c_damage_indicator(record._eyepos, record._hitbox, record._final_damage, record._final_hitgroup == HITGROUP_HEAD));

	//				it = ctx.fired_shot.erase(it);
	//				continue;
	//			}

	//			if (!record->_impact_called) {
	//				if ((csgo.m_globals()->realtime - record->_time) > 5.f)
	//					it = ctx.fired_shot.erase(it);
	//				else
	//					++it;

	//				continue;
	//			}

	//			const auto &entity = record->_target;
	//			auto start = record->_eyepos;
	//			auto end = record->_impacts.back();
	//			const auto hitbox = record->_hitbox;
	//			const auto &precord = record->_record;
	//			//const auto rtype = precord.type;
	//			const auto bticks = precord->backtrack_ticks;

	//			if (entity != nullptr && !entity->IsDead() && entity->GetClientClass() && entity->GetModelPtr() && entity->IsPlayer())
	//			{
	//				bool had_impact_in_bbox = false;

	//				const auto &tickrecord = record->_tickrecord;

	//				//const auto angle = Math::CalcAngle(start, end);
	//				//Vector forward;
	//				//Math::AngleVectors(angle, &forward);
	//				end = record->_impacts.back();

	//				/*float dist = 10000000.f;
	//				for (auto imp : record->_impacts)
	//				{
	//					auto ndist = (end - imp).LengthSquared();

	//					if (dist > ndist)
	//					{
	//						end = imp;
	//						dist = ndist;
	//					}
	//				}*/

	//				int type = 1; // resolver
	//				bool didhit[4] = {};
	//				bool didawall[4] = {};
	//				auto did_intersect_anything = false;

	//				//const auto bonecount = min(128, entity->GetBoneCount());

	//				bool cant_awall = true;

	//				//const auto impact_dist = (start - end).LengthSquared();

	//				auto most_far = 0.1f;

	//				record->_restore.store(entity, true);

	//				auto resolve_info = &feature::resolver->player_records[entity->entindex() - 1];

	//				tickrecord->apply(entity, false, true);

	//				memcpy(entity->m_CachedBoneData().Base(), ((tickrecord->resolver_index == 0 || entity->IsBot()) ? tickrecord->matrixes : (tickrecord->resolver_index == 1 ? tickrecord->leftmatrixes : tickrecord->rightmatrixes)), min(128, entity->m_bone_count()) * sizeof(matrix3x4_t));

	//				for (auto i = 0; i < HITBOX_MAX; i++)
	//				{
	//					C_Hitbox box; feature::ragebot->get_hitbox_data(&box, entity, i, tickrecord->matrixes);
	//					C_Hitbox box1; feature::ragebot->get_hitbox_data(&box, entity, i, tickrecord->leftmatrixes);
	//					C_Hitbox box2; feature::ragebot->get_hitbox_data(&box, entity, i, tickrecord->rightmatrixes);

	//					if (box.isOBB)
	//					{
	//						trace_t ll;
	//						Ray_t rr;
	//						rr.Init(start, end);

	//						csgo.m_engine_trace()->ClipRayToEntity(rr, 0x4600400B, entity, &ll);

	//						bool can_damage = (ll.hitgroup - 1) <= 7;
	//						bool is_required_player = (ll.m_pEnt == entity);
	//						didhit[0] = can_damage && is_required_player;// && ll.hitgroup == hitbox2hitgroup(ent, hitbox);
	//					}
	//					else {

	//						if (tickrecord->resolver_index == 0) {
	//							didhit[0] = didhit[1] = Math::Intersect(start, end, box.mins, box.maxs, box.radius * 0.93f);
	//							didhit[2] = Math::Intersect(start, end, box1.mins, box1.maxs, box1.radius * 0.93f);
	//							didhit[3] = Math::Intersect(start, end, box2.mins, box2.maxs, box2.radius * 0.93f);
	//						}
	//						else if (tickrecord->resolver_index == 1) {
	//							didhit[1] = Math::Intersect(start, end, box.mins, box.maxs, box.radius * 0.93f);
	//							didhit[0] = didhit[2] = Math::Intersect(start, end, box1.mins, box1.maxs, box1.radius * 0.93f);
	//							didhit[3] = Math::Intersect(start, end, box2.mins, box2.maxs, box2.radius * 0.93f);
	//						}
	//						else {
	//							didhit[1] = Math::Intersect(start, end, box.mins, box.maxs, box.radius * 0.93f);
	//							didhit[2] = Math::Intersect(start, end, box1.mins, box1.maxs, box1.radius * 0.93f);
	//							didhit[0] = didhit[3] = Math::Intersect(start, end, box2.mins, box2.maxs, box2.radius * 0.93f);
	//						}
	//					}

	//					if (didhit[0])
	//						did_intersect_anything = true;

	//					memcpy(entity->m_CachedBoneData().Base(), tickrecord->matrixes, min(128, entity->m_bone_count()) * sizeof(matrix3x4_t));

	//					ctx.force_low_quality_autowalling = true;
	//					auto dmg = feature::autowall->CanHit(start, end, ctx.m_local(), entity, i);
	//					ctx.force_low_quality_autowalling = false;

	//					//if (dmg >= 1.f && cant_awall) // we did find hitbox that we hit.	
	//					//	cant_awall = false;

	//					memcpy(entity->m_CachedBoneData().Base(), tickrecord->leftmatrixes, min(128, entity->m_bone_count()) * sizeof(matrix3x4_t));

	//					ctx.force_low_quality_autowalling = true;
	//					auto ldmg = feature::autowall->CanHit(start, end, ctx.m_local(), entity, i);
	//					ctx.force_low_quality_autowalling = false;

	//					memcpy(entity->m_CachedBoneData().Base(), tickrecord->rightmatrixes, min(128, entity->m_bone_count()) * sizeof(matrix3x4_t));

	//					ctx.force_low_quality_autowalling = true;
	//					auto rdmg = feature::autowall->CanHit(start, end, ctx.m_local(), entity, i);
	//					ctx.force_low_quality_autowalling = false;

	//					if (ldmg >= 1.f && didhit[2]) {// we did find hitbox that we hit.	
	//						resolve_info->brute_banned_sides.push_back(1);

	//						if (tickrecord->resolver_index == 1)
	//							cant_awall = false;
	//					}

	//					if (rdmg >= 1.f && didhit[3]) {// we did find hitbox that we hit.	
	//						resolve_info->brute_banned_sides.push_back(2);

	//						if (tickrecord->resolver_index == 2)
	//							cant_awall = false;
	//					}

	//					if (dmg >= 1.f && didhit[1]) {// we did find hitbox that we hit.	
	//						resolve_info->brute_banned_sides.push_back(0);

	//						if (tickrecord->resolver_index == 0)
	//							cant_awall = false;
	//					}
	//				}

	//				if (record->_restore.data_filled)
	//					record->_restore.apply(entity, true);
	//				//memcpy(entity->m_CachedBoneData().Base(), bones_backup, min(128, entity->GetBoneCount()) * sizeof(matrix3x4_t));

	//				//const auto hitbox_dist = (start - hitbox).LengthSquared() - 32.0f;

	//				//if (most_far < hitbox_dist)
	//				//	most_far = hitbox_dist;

	//				//entity->GetBoneCount() = bonecount;
	//				//entity->GetBoneAccessor().m_ReadableBones = bonecount;
	//				//entity->GetBoneAccessor().m_WritableBones = 0;
	//				//didhit = (box.isOBB ? Math::IntersectBB(start, end, box.mins, box.maxs) : Math::Intersect(start, end, box.mins, box.maxs, (box.radius * 0.95f)));
	//				//Source::m_pDebugOverlay->AddLineOverlay(start, tr.endpos, 255, 0, 0, false, 4.f);

	//				//const auto aimpoint_delta = start.Distance(hitbox);
	//				//const auto impact_delta = start.Distance(end);
	//	
	//				if (did_intersect_anything && !cant_awall/* && had_impact_in_bbox*/)
	//					type = 1; // resolve issue
	//				else
	//					type = 0;

	//				//if (aimpoint_delta > impact_delta)
	//				//	type = 0;

	//				//auto v15 = start.y;
	//				//auto v16 = start.x - hitbox.x;
	//				//auto v17 = start.x - end.x;
	//				//auto v18 = v15 - end.y;
	//				//auto v19 = start.z;
	//				//auto v20 = v19 - end.z;
	//				//auto ap_d = ((((v15 - hitbox.y) * (v15 - hitbox.y)) + (v16 * v16))
	//				//	+ ((v19 - hitbox.z) * (v19 - hitbox.z)));
	//				//ap_d = sqrtf(*&ap_d);
	//				//ap_d = ap_d;
	//				//auto aimpoint_delta = ap_d - 32.0;
	//				//auto impact_delta2 = (((v18 * v18) + (v17 * v17)) + (v20 * v20));
	//				//impact_delta2 = sqrtf(impact_delta2);

	//				if (type != 0)
	//				{
	//					if (ctx.m_settings.misc_notifications[2]) {
	//						//if (type == 1)
	//						_events.emplace_back(sxor("missed shot due to resolver"));
	//						//else
	//						//	_events.push_back(_event("missed shot due to obstruction"));
	//					}
	//				}
	//				else
	//				{
	//					if (ctx.shots_fired[entity->entindex() - 1] > 0)
	//						ctx.shots_fired[entity->entindex() - 1]--;

	//					if (ctx.m_settings.misc_notifications[2]) {
	//						//if (type == 2)
	//						//	_events.push_back(_event("missed shot due to obstruction"));
	//						//else

	//						//csgo.m_debug_overlay()->AddLineOverlay(start, end, 255, 0, 0, true, 20.f);
	//						//	feature::ragebot->visualize_hitboxes(entity, ((tickrecord->rtype == 0 || entity->IsBot()) ? tickrecord->matrixes : (tickrecord->rtype == 1 ? tickrecord->leftmatrixes : tickrecord->rightmatrixes)), Color::White(), 20.f);

	//						_events.emplace_back(sxor("missed shot due to spread"));
	//					}
	//				}

	//			}

	//			record->_printed = true;

	//			++it;
	//		}
	//	}
	//}

	if (_events.empty())
		return;

	int x = 8;
	int y = 5;
	auto count = 0;
	const int fontTall = csgo.m_surface()->GetFontTall(F::Events) + 1;

	for (auto& event : _events)
	{
		if (_events.back()._time < csgo.m_globals()->realtime)
			_events.pop_back();

		if (event._time < csgo.m_globals()->realtime && event._displayticks > 0)
			continue;

		if (count > 10)
			break;

		if (!event._msg.empty())
		{
			if (!event._displayed)
			{
				csgo.m_engine_cvars()->ConsoleColorPrintf(Color::White(), sxor("["));
				csgo.m_engine_cvars()->ConsoleColorPrintf(Color::LightBlue()/*Color(175, 255, 0, 255)*/, sxor("enrage.gg"));
				csgo.m_engine_cvars()->ConsoleColorPrintf(Color::White(), sxor("] %s\n"), event._msg.c_str());

				//if (event._secretmsg.size() > 0)
				//	csgo.m_engine_cvars()->ConsoleColorPrintf((Color::Blue()), sxor(" %s\n"), event._secretmsg.c_str());

				event._displayed = true;
			}
			//fired shot at 
			if (event._msg[0] == 'f' && event._msg[2] == 'r' && event._msg[6] == 's')
				continue;
			
			Color clr = Color::White();

			const float timeleft = fabs(event._time - csgo.m_globals()->realtime);

			if (timeleft < .5f)
			{
				float f = Math::clamp(timeleft, 0.0f, .5f) / .5f;

				clr[3] = (int)(f * 255);

				if (count == 0 && f < 0.2f)
				{
					y -= (1.0f - f / 0.2f) * fontTall;
				}
			}
			else
			{
				clr[3] = 255;
			}

			Drawing::DrawString(F::Events, x, y, clr, FONT_LEFT, "%s", event._msg.c_str());

			y += fontTall;

			count++;
		}
	}
}

void c_visuals::skeleton(C_BasePlayer* Entity, Color color, matrix3x4_t* pBoneToWorldOut)
{
	auto model = Entity->GetModel();
	if (!model) return;

	auto studio_model = csgo.m_model_info()->GetStudioModel(Entity->GetModel());
	if (!studio_model) return;

	for (int i = 0; i < studio_model->numbones; i++)
	{
		auto pBone = studio_model->pBone(i);

		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;

		if (Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]).IsZero() || Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]).IsZero())
			continue;

		Vector vBonePos1;
		if (!Drawing::WorldToScreen(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;

		Vector vBonePos2;
		if (!Drawing::WorldToScreen(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;

		Drawing::DrawLine((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, color);
	}
}

//void c_visuals::offscreen_esp(C_BasePlayer* player, float alpha) {
//
//	Vector view_origin, target_pos, delta;
//	Vector screen_pos, offscreen_pos;
//	float  leeway_x, leeway_y, radius, offscreen_rotation;
//	bool   is_on_screen;
//	Vertex_t verts[3], verts_outline[3];
//	Color  color;
//
//	// todo - dex; move this?
//	static auto get_offscreen_data = [](const Vector& delta, float radius, Vector& out_offscreen_pos, float& out_rotation) {
//		QAngle  view_angles; csgo.m_engine()->GetViewAngles(view_angles);
//		view_angles.x = 0;
//		view_angles.z = 0;
//		Vector fwd, right, up(0.f, 0.f, 1.f);
//		float  front, side, yaw_rad, sa, ca;
//
//		// get viewport angles forward directional vector.
//		Math::AngleVectors(view_angles, &fwd);
//
//		// convert viewangles forward directional vector to a unit vector.
//		fwd.z = 0.f;
//		//fwd.Normalize();
//
//		// calculate front / side positions.
//		right = up.Cross(fwd);
//		front = delta.Dot(fwd);
//		side = delta.Dot(right);
//
//		// setup offscreen position.
//		out_offscreen_pos.x = radius * -side;
//		out_offscreen_pos.y = radius * -front;
//
//		// get the rotation ( yaw, 0 - 360 ).
//		out_rotation = RAD2DEG(std::atan2(out_offscreen_pos.x, out_offscreen_pos.y) + M_PI);
//
//		if (out_rotation < 0.f)
//			out_rotation += 360.f;
//		if (out_rotation > 360.f)
//			out_rotation -= 360.f;
//
//		// get needed sine / cosine values.
//		yaw_rad = DEG2RAD(-out_rotation);
//		sa = std::sin(yaw_rad);
//		ca = std::cos(yaw_rad);
//
//		// rotate offscreen position around.
//		out_offscreen_pos.x = (int)((ctx.screen_size.x / 2.f) + (radius * sa));
//		out_offscreen_pos.y = (int)((ctx.screen_size.y / 2.f) - (radius * ca));
//	};
//
//	//if (!g_menu.main.players.offscreen.get())
//	//	return;
//
//	//if (!g_cl.m_processing || !g_cl.m_local->enemy(player))
//	//	return;
//
//	// get the player's center screen position.
//	target_pos = player->get_abs_origin();
//	is_on_screen = Drawing::WorldToScreen(target_pos, screen_pos);
//
//	// give some extra room for screen position to be off screen.
//	leeway_x = ctx.screen_size.x / 18.f;
//	leeway_y = ctx.screen_size.y / 18.f;
//
//	// origin is not on the screen at all, get offscreen position data and start rendering.
//	if (!is_on_screen
//		|| screen_pos.x < -leeway_x
//		|| screen_pos.x >(ctx.screen_size.x + leeway_x)
//		|| screen_pos.y < -leeway_y
//		|| screen_pos.y >(ctx.screen_size.y + leeway_y)) {
//
//		// get viewport origin.
//		view_origin = ctx.m_local()->get_abs_origin();
//
//		// get direction to target.
//		delta = (target_pos - view_origin);
//
//		// note - dex; this is the 'YRES' macro from the source sdk.
//		radius = 200.f * (ctx.screen_size.y / 480.f);
//
//		// get the data we need for rendering.
//		get_offscreen_data(delta, radius, offscreen_pos, offscreen_rotation);
//
//		// bring rotation back into range... before rotating verts, sine and cosine needs this value inverted.
//		// note - dex; reference: 
//		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/game/client/tf/tf_hud_damageindicator.cpp#L182
//		offscreen_rotation = -offscreen_rotation;
//
//		// setup vertices for the triangle.
//		verts[0] = { offscreen_pos.x, offscreen_pos.y };        // 0,  0
//		verts[1] = { offscreen_pos.x - 12.f, offscreen_pos.y + 24.f }; // -1, 1
//		verts[2] = { offscreen_pos.x + 12.f, offscreen_pos.y + 24.f }; // 1,  1
//
//		
//
//		// setup verts for the triangle's outline.
//		verts_outline[0] = { verts[0].m_Position.x, verts[0].m_Position.y };
//		verts_outline[1] = { verts[1].m_Position.x, verts[1].m_Position.y };
//		verts_outline[2] = { verts[2].m_Position.x, verts[2].m_Position.y };
//
//		// rotate all vertices to point towards our target.
//		verts[0] = Math::RotateVertex(offscreen_pos, verts[0], offscreen_rotation);
//		verts[1] = Math::RotateVertex(offscreen_pos, verts[1], offscreen_rotation);
//		verts[2] = Math::RotateVertex(offscreen_pos, verts[2], offscreen_rotation);
//		verts_outline[ 0 ] = Math::RotateVertex( offscreen_pos, verts_outline[ 0 ], offscreen_rotation );
//		verts_outline[ 1 ] = Math::RotateVertex( offscreen_pos, verts_outline[ 1 ], offscreen_rotation );
//		verts_outline[ 2 ] = Math::RotateVertex( offscreen_pos, verts_outline[ 2 ], offscreen_rotation );
//
//		// todo - dex; finish this, i want it.
//		// auto &damage_data = m_offscreen_damage[ player->index( ) ];
//		// 
//		// // the local player was damaged by another player recently.
//		// if( damage_data.m_time > 0.f ) {
//		//     // // only a small amount of time left, start fading into white again.
//		//     // if( damage_data.m_time < 1.f ) {
//		//     //     // calculate step needed to reach 255 in 1 second.
//		//     //     // float step = UINT8_MAX / ( 1000.f * g_csgo.m_globals->m_frametime );
//		//     //     float step = ( 1.f / g_csgo.m_globals->m_frametime ) / UINT8_MAX;
//		//     //     
//		//     //     // increment the new value for the color.
//		//     //     // if( damage_data.m_color_step < 255.f )
//		//     //         damage_data.m_color_step += step;
//		//     // 
//		//     //     math::clamp( damage_data.m_color_step, 0.f, 255.f );
//		//     // 
//		//     //     damage_data.m_color.g( ) = (uint8_t)damage_data.m_color_step;
//		//     //     damage_data.m_color.b( ) = (uint8_t)damage_data.m_color_step;
//		//     // }
//		//     // 
//		//     // g_cl.print( "%f %f %u %u %u\n", damage_data.m_time, damage_data.m_color_step, damage_data.m_color.r( ), damage_data.m_color.g( ), damage_data.m_color.b( ) );
//		//     
//		//     // decrement timer.
//		//     damage_data.m_time -= g_csgo.m_globals->m_frametime;
//		// }
//		// 
//		// else
//		//     damage_data.m_color = colors::white;
//
//		//Drawing::DrawLine(ctx.screen_size.x * 0.5f, ctx.screen_size.y * 0.5f, offscreen_pos.x, offscreen_pos.y, ctx.m_settings.colors_esp_offscreen);
//
//		// render!
//		color = ctx.m_settings.colors_esp_offscreen; // damage_data.m_color;
//
//		csgo.m_surface()->DrawSetColor(color);
//		csgo.m_surface()->DrawTexturedPolygon(3, verts,false);
//
//		csgo.m_surface()->DrawSetColor( Color::Black(150) );
//		csgo.m_surface()->DrawTexturedPolyLine(verts_outline, 3);
//	}
//}

void c_visuals::offscreen_esp(C_BasePlayer* entity, float alpha)
{
	//auto idx = entity->entindex() - 1;

	Vector vEnemyOrigin = entity->get_abs_origin();
	Vector vLocalOrigin = ctx.m_local()->get_abs_origin();

	auto is_on_screen = [](Vector origin, Vector& screen) -> bool
	{
		if (!Drawing::WorldToScreen(origin, screen))
			return false;

		return (screen.x > 0 && screen.x < ctx.screen_size.x) && (ctx.screen_size.y > screen.y && screen.y > 0);
	};

	Vector screenPos;

	if (!entity->IsDormant())
	{
		if (entity->m_bone_count() <= 10 || is_on_screen(entity->get_bone_pos(6), screenPos)) //TODO (?): maybe a combo/checkbox to turn this on/off
			return;
	}
	else
	{
		if (is_on_screen(vEnemyOrigin, screenPos))
			return;
	}

	Vector dir;

	csgo.m_engine()->GetViewAngles(dir);

	float view_angle = dir.y;

	if (view_angle < 0)
		view_angle += 360;
	
	view_angle = DEG2RAD(view_angle);

	auto entity_angle = Math::CalcAngle(vLocalOrigin, vEnemyOrigin);
	entity_angle.Normalize();

	if (entity_angle.y < 0.f)
		entity_angle.y += 360.f;
	
	entity_angle.y = DEG2RAD(entity_angle.y);
	entity_angle.y -= view_angle;

	const float wm = ctx.screen_size.x / 2, hm = ctx.screen_size.y / 2;

	auto position = Vector2D(wm, hm);
	position.x -= ctx.m_settings.esp_arrows_distance * 5.25f;

	Drawing::rotate_point(position, Vector2D(wm, hm), false, entity_angle.y);

	const auto size = static_cast<int>(ctx.m_settings.esp_arrows_size);//std::clamp(100 - int(vEnemyOrigin.Distance(vLocalOrigin) / 6), 10, 25);

	static float old_fade_alpha = 1;
	static bool sw = false;

	if (old_fade_alpha <= 0)
		sw = true;
	else if (old_fade_alpha >= 1)
		sw = false;

	if (sw)
		old_fade_alpha = Math::clamp(old_fade_alpha + csgo.m_globals()->frametime * 1.2f, 0, 1);
	else
		old_fade_alpha = Math::clamp(old_fade_alpha - csgo.m_globals()->frametime * 1.2f, 0, 1);

	float fade_alpha = old_fade_alpha;

	if (alpha < fade_alpha)
		fade_alpha = alpha;



	Drawing::filled_tilted_triangle(position, Vector2D(size - 1, size), position, true, -entity_angle.y,
		ctx.flt2color(ctx.m_settings.colors_esp_offscreen).malpha(fade_alpha * 0.6), true,
		ctx.flt2color(ctx.m_settings.colors_esp_offscreen).malpha(alpha));
}

int indicators_count = 0;

class c_indicator
{
public:
	c_indicator()
	{
		text = "";
		max_value = 0.f;
	};

	c_indicator(const char* _text, const float _max_value)
	{
		text = _text;
		max_value = _max_value;
	};

	float get_max_value(const float meme = FLT_MAX)
	{
		if (meme < FLT_MAX)
			max_value = meme;
		
		return max_value;
	}

	void draw(const float factor, Color color = Color(123, 194, 21, 250), const float max_factor = -1.f)
	{
		const auto max = max_factor > 0.f ? max_factor : max_value;

		if (csgo.m_client()->IsChatRaised())
			color._a() = 10;

		const auto text_size = Drawing::GetTextSize(F::LBY, text);
		Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count, Color(30, 30, 30, csgo.m_client()->IsChatRaised() ? 10 : 250), FONT_LEFT, text);
		const auto draw_factor = Math::clamp((factor / max), 0, 1);
		*reinterpret_cast<bool*>(uintptr_t(csgo.m_surface()) + 0x280) = true;
		int x, y, x1, y1;
		Drawing::GetDrawingArea(x, y, x1, y1);
		Drawing::LimitDrawingArea(0, ctx.screen_size.y - 88 - 26 * indicators_count, int((text_size.right + 15) * draw_factor), (int)text_size.bottom);
		Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, color, FONT_LEFT, text);
		Drawing::LimitDrawingArea(x, y, x1, y1);
		*reinterpret_cast<bool*>(uintptr_t(csgo.m_surface()) + 0x280) = false;
	};
private:
	const char* text = "";
	float max_value = 0.f;
};

int g_Math(float num, float max, int arrsize)
{
	if (num > max)
		num = max;

	auto tmp = max / num;
	auto i = (arrsize / tmp);

	//i = (i >= 0 && floor(i + 0.5) || ceil(i - 0.5));

	if (i >= 0)
		i = floor(i + 0.5f);
	else
		i = ceil(i - 0.5);

	return i;
}

Color g_ColorByInt(float number, float max) {
	static Color Colors[] = {
		{ 255, 0, 0 },
		{ 237, 27, 3 },
		{ 235, 63, 6 },
		{ 229, 104, 8 },
		{ 228, 126, 10 },
		{ 220, 169, 16 },
		{ 213, 201, 19 },
		{ 176, 205, 10 },
		{ 124, 195, 13 }
	};

	auto i = g_Math(number, max, ARRAYSIZE(Colors)-1);
	return Colors[Math::clamp(i, 0, (int)ARRAYSIZE(Colors)-1)];
}

std::string _name[15] = { (std::string)sxor("fakeduck"), (std::string)sxor("doubletap"), (std::string)sxor("voice player"), (std::string)sxor("thirdperson"), (std::string)sxor("slow walk"), (std::string)sxor("force body aim"), (std::string)sxor("min damage override"), (std::string)sxor("side control"), (std::string)sxor("manual backward"), (std::string)sxor("manual right"), (std::string)sxor("manual left"), (std::string)sxor("auto peek"), (std::string)sxor("freestanding"), (std::string)sxor("suppress shot"), (std::string)sxor("time stop") };
std::string _type[5] = { (std::string)sxor("toggle"), (std::string)sxor("on key"), (std::string)sxor("off key"), (std::string)sxor("always on"), (std::string)sxor("pressed") };
float keybinds_time[15];
constexpr int size_info = 8;

void DECLSPEC_NOINLINE c_visuals::render(bool reset)
{
	//int indicators = 0;

	//if (!m_entities.empty())
	//	m_entities.clear();

	//if (!m_worldentities.empty())
	//	m_worldentities.clear();

	VIRTUALIZER_MUTATE_ONLY_START;

	logs();

	auto get_name_by_paint_kit = [](int wpn, int id) -> const char*
	{
		if (parser::weapons.list[wpn].skins.list.empty())
			return "";

		auto weapon = -1;

		for (auto i = 0; i < parser::weapons.list.size(); i++) {
			auto& cwpn = parser::weapons.list[i];
			if (cwpn.id == wpn) {
				weapon = i;
			}
		}

		if (weapon == -1)
			return "";

		for (auto i = 0; i < parser::weapons.list[weapon].skins.list.size(); i++) {
			auto& skin = parser::weapons.list[weapon].skins.list[i];

			if (skin.id == id) {

				if (skin.translated_name.size() <= 1)
					continue;

				auto strname = std::string(skin.translated_name.begin(), skin.translated_name.end());

				char name[128] = "";
				sprintf_s(name, " | %s", strname.c_str());

				return name;
			}
		}

		return "";
	};

	auto get_spectators = []() -> std::list<int>
	{
		std::list<int> list = {};

		if (!csgo.m_engine()->IsInGame() || !ctx.m_local() || ctx.m_local()->IsDead()) {
			return list;
		}

		for (int i = 1; i < 64; i++)
		{
			auto ent = csgo.m_entity_list()->GetClientEntity(i);

			if (ent == nullptr)
				continue;

			if (ent->IsDormant() || !ent->GetClientClass())
				continue;

			if (ent->m_hObserverTarget() == INVALID_HANDLE_VALUE)
				continue;

			auto target = csgo.m_entity_list()->GetClientEntityFromHandle(*ent->m_hObserverTarget());

			if (!target || ctx.m_local() != target)
				continue;

			list.push_back(i);
		}

		return list;
	};

	auto get_keybinds = [&](int& working_keybinds) -> std::list<c_keybindinfo>
	{

		std::list<c_keybindinfo> list = {};

		if (!csgo.m_engine()->IsInGame() || !ctx.m_local() || ctx.m_local()->IsDead())
			return list;



		for (auto i = 0; i < 15; i++)
		{
			const auto w = working_keybinds;

			ctx.active_keybinds[i].index = i;

			if (ctx.active_keybinds[i].mode > 0)
			{ //(csgo.m_globals()->realtime - keybinds_time[i]) >= csgo.m_globals()->interval_per_tick)
				keybinds_time[i] = csgo.m_globals()->realtime;
				++working_keybinds;
			}
			else if (((csgo.m_globals()->realtime - keybinds_time[i]) * 2.0f) < 0.9f)
				++working_keybinds;
			else
				ctx.active_keybinds[i].sort_index = 0;
			
			if (w != working_keybinds && ctx.active_keybinds[i].mode > 0 && !ctx.active_keybinds[i].prev_state)
				ctx.active_keybinds[i].sort_index = ctx.active_keybinds_visible++;
			else
			{
				if (ctx.active_keybinds[i].prev_state && ctx.active_keybinds[i].mode < 0)
					ctx.active_keybinds_visible--;
			}

			list.push_back(c_keybindinfo{ ctx.active_keybinds[i].index, _name[i].c_str(), _type[Math::clamp(ctx.active_keybinds[i].mode - 1, 0, 4)].c_str(), ctx.active_keybinds[i].sort_index });

			ctx.active_keybinds[i].prev_state = w != working_keybinds;
		}

		list.sort(
			[](const c_keybindinfo& a, const c_keybindinfo& b) {
				return a.sort_index < b.sort_index;
			});
		/*
		c_keybind anti_aim_fake_switch = c_keybind{ 0, 1 , false, 0 }; //key is: Z
		c_keybind anti_aim_fakeduck_key = c_keybind{ 0, 1 , false, 0 };
		c_keybind anti_aim_yaw_left_switch = c_keybind{ 0, 1, false, 0 };
		c_keybind anti_aim_yaw_right_switch = c_keybind{ 0, 1, false, 0 };
		c_keybind anti_aim_yaw_backward_switch = c_keybind{ 0, 1 , false, 0 };
		c_keybind anti_aim_slowwalk_key = c_keybind{ 0, 1 , false, 0 };
		c_keybind aimbot_tickbase_exploit_toggle = c_keybind{ 0, 1, false, 0 };
		c_keybind aimbot_bodyaim_key = c_keybind{ 0, 1 , false, 0 };
		c_keybind aimbot_min_damage_override = c_keybind{ 0, 1, false, 0 };
		c_keybind visuals_tp_key = c_keybind{ 0, 1, false, 0 };
		*/

		return list;
	};

	static auto prev_frame = csgo.m_globals()->framecount;

	static char buffer[255];
	static char timebuff[80];
	//static float rtt = 0.0;

	if (abs(prev_frame - csgo.m_globals()->framecount) > 1) {
		time_t now = time(0);   // get time now
		/*struct*/ tm  tstruct;
		tstruct = *localtime(&now);

		/*rtt = 0;

		if (csgo.m_engine()->IsInGame())
			rtt = ctx.latency[FLOW_INCOMING] * 1000.f;
		else {
			ctx.latency[FLOW_OUTGOING] = 0;
			ctx.latency[FLOW_INCOMING] = 0;
			ctx.lerp_time = 0;
		}*/

		strftime(timebuff, sizeof(timebuff), "%X", &tstruct);

		prev_frame = csgo.m_globals()->framecount;
	}

	static float piska = 0.f;

	if (!ctx.auto_peek_spot.IsZero()) {
		const float step = (M_PI * 2.0f) / 24;
		float radius = 24;
		static bool lol = true;
		static auto start = 0.f;
		static auto max_a = ((M_PI * 2.0f) - step);
		static float prev_a = 0.f;
		Vector w2sCenter;
		if (Drawing::WorldToScreen(ctx.auto_peek_spot, w2sCenter)) {
			for (float a = 0; a < ((M_PI * 2.0f) - step); a += step) {
				float cos_end, sin_end;
				sin_end = sin(a + step);
				cos_end = cos(a + step);

				Vector end(radius * cos_end + ctx.auto_peek_spot.x, radius * sin_end + ctx.auto_peek_spot.y, ctx.auto_peek_spot.z);

				Vector end2d;

				if (!Drawing::WorldToScreen(end, end2d))
					break;

				if (a < prev_a)
					max_a = prev_a;

				if (start >= (max_a + (step * 0.5f)))
					lol = false;
				if (start <= -(step * 0.5f))
					lol = true;

				start += (lol ? (min(max(csgo.m_globals()->frametime * 0.1f, max_a - start), csgo.m_globals()->frametime * 0.1f)) : (max(-max(csgo.m_globals()->frametime * 0.1f, start), csgo.m_globals()->frametime * -0.1f)));

				start = Math::clamp(start, -(step * 0.5f), (max_a + (step * 0.5f)));

				//if (start > 1.f)
				//	start /= round(start);

				//auto delta = abs(start - a);

				bool nrender = start > a;

				Drawing::DrawRect(end2d.x, end2d.y, 5.f, 5.f, ctx.m_settings.menu_color.alpha(nrender ? 10 : 230));
				Drawing::DrawOutlinedRect(end2d.x - 1, end2d.y - 1, 7.f, 7.f, Color(38, 38, 38, 240));

				prev_a = a;
				//Drawing::DrawBox(start2d.x, start2d.y, 10.f, 10.f, ctx.m_settings.menu_color.alpha(240));
			}
		}
	}

	/*static auto progress = 0.f;

	if (progress < 1.f)
		progress += min(1.f - progress, 0.01f);
	else
		progress = 0;

	Drawing::DrawCircularProgressBar({ ctx.screen_size.x / 2,ctx.screen_size.y / 2 }, 9, 2, 50, progress, Color::LightBlue());*/

	indicators_count = 0;

	if (ctx.m_settings.visuals_extra_windows[0] && ctx.screen_size.x > 0.0 && ctx.screen_size.y > 0.0)
	{
		static double prev_alpha = 0;

		auto spectators = 1;

		static auto d = Drawing::GetTextSize(F::ESP, "H");

		static auto lol = get_spectators();

		static auto tick = csgo.m_globals()->tickcount;

		if (tick != csgo.m_globals()->tickcount) {
			lol = get_spectators();
			tick = csgo.m_globals()->tickcount;
		}

		const Vector2D spectator_size = { 150, int(lol.size()) * float(d.bottom) + 20 };

		if ((ctx.m_settings.visuals_spectators_pos + spectator_size) > ctx.screen_size)
		{
			if ((ctx.m_settings.visuals_spectators_pos.x + spectator_size.x) >= ctx.screen_size.x)
				ctx.m_settings.visuals_spectators_pos.x = ctx.screen_size.x - 150.0;

			if ((ctx.m_settings.visuals_spectators_pos.y + spectator_size.y) >= ctx.screen_size.y)
				ctx.m_settings.visuals_spectators_pos.y = ctx.screen_size.y - (int(lol.size()) * int(d.bottom) + 15);
		}
		else if (ctx.m_settings.visuals_spectators_pos < Vector2D(0, 0)) {
			if (ctx.m_settings.visuals_spectators_pos.x < 0.0)
				ctx.m_settings.visuals_spectators_pos.x = 0.0;

			if (ctx.m_settings.visuals_spectators_pos.y < 0.0)
				ctx.m_settings.visuals_spectators_pos.y = 0.0;
		}

		if (feature::menu->_menu_opened)
			prev_alpha = 255.f;

		if (csgo.m_client()->IsChatRaised())
			prev_alpha = 10.0;

		//Drawing::DrawRect(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y, spectator_size.x, 15.f, Color(31.f, 33.f, 35.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 100.f))); // 31.f, 33.f, 35.f
		////Drawing::DrawString(Drawing::Eagle, ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y, Color(255.f, 255.f, 255.f, prev_alpha * float(cheat::Cvars.Visuals_spectators_alpha.GetValue() / 80.f)), FONT_LEFT, "r");
		//Drawing::DrawRect(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y + 15.f, spectator_size.x, spectator_size.y - 13.f, Color(43.f, 44.f, 46.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 100.f)));

		Drawing::DrawRect		(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y + 1.0, spectator_size.x, spectator_size.y + 2.0, Color(35, 35, 35, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 130.0)));
		Drawing::DrawOutlinedRect(ctx.m_settings.visuals_spectators_pos.x - 1.0, ctx.m_settings.visuals_spectators_pos.y, spectator_size.x + 2.0, spectator_size.y + 4.0, Color(10, 10, 10, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 130.0)));
		Drawing::DrawRect		(ctx.m_settings.visuals_spectators_pos.x - 1.0, ctx.m_settings.visuals_spectators_pos.y, spectator_size.x + 2.0, 2.0, ctx.m_settings.menu_color.alpha(prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 100.f)));
		Drawing::DrawString(F::ESP, ctx.m_settings.visuals_spectators_pos.x + spectator_size.x / 2.0, ctx.m_settings.visuals_spectators_pos.y + 3.0, Color(255.0, 255.0, 255.0, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 100.f)), FONT_CENTER, "%s [%d]", sxor("spectators"), (int)lol.size());

		//Drawing::DrawLine(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y + (d.bottom + 4.f), ctx.m_settings.visuals_spectators_pos.x + spectator_size.x-1.f, ctx.m_settings.visuals_spectators_pos.y + (d.bottom + 4.f), Color(80.f, 80.f, 80.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 130.f)));

		Drawing::DrawRectGradientHorizontal(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y + (d.bottom + 4.0), spectator_size.x / 2, 2.0, Color(35, 35, 35, prev_alpha * double(ctx.m_settings.visuals_spectators_alpha / 130.0)), ctx.m_settings.menu_color.alpha(prev_alpha* double(ctx.m_settings.visuals_spectators_alpha / 130.0)));
		Drawing::DrawRectGradientHorizontal(ctx.m_settings.visuals_spectators_pos.x + spectator_size.x / 2, ctx.m_settings.visuals_spectators_pos.y + (d.bottom + 4.0), spectator_size.x / 2 - 1.0, 2.0, ctx.m_settings.menu_color.alpha(prev_alpha * double(ctx.m_settings.visuals_spectators_alpha / 130.0)), Color(35, 35, 35, prev_alpha * double(ctx.m_settings.visuals_spectators_alpha / 130.0)));

		if (!lol.empty())
		{
			if (prev_alpha < 255.0 && !csgo.m_client()->IsChatRaised())
				prev_alpha += min(5.0, 255.0 - prev_alpha);

			for (int spec : lol)
			{
				player_info spec_inf;
				csgo.m_engine()->GetPlayerInfo(spec, &spec_inf);

				auto size = Drawing::GetTextSize(F::ESP, spec_inf.name);
				//Drawing::DrawRect(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y + d.bottom * spectators, size.right + 2, 13, Color(0.f, 0.f, 0.f, prev_alpha * float(cheat::Cvars.Visuals_spectators_alpha.GetValue() / 100.f)));

				//char new_name[34];

				//memcpy(new_name, spec_inf.name, sizeof(char) * 33);

				//if (spectators > 1)
				//	Drawing::DrawLine(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y + 5.f + (d.bottom + 1.f) * spectators, ctx.m_settings.visuals_spectators_pos.x + 149.f, ctx.m_settings.visuals_spectators_pos.y + 5.f + (d.bottom + 1.f) * spectators, Color(80.f, 80.f, 80.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 80.f)));

				std::string name(spec_inf.name);

				//const auto wstr = std::wstring(std::string(spec_inf.name).begin(), std::string(spec_inf.name).end());

				if (name.length() > 20)
					name.resize(20);

				Drawing::DrawString(F::ESP, ctx.m_settings.visuals_spectators_pos.x + 3.0f, ctx.m_settings.visuals_spectators_pos.y + 6.0f + 2.0f + (d.bottom * spectators), Color(255, 255, 255, prev_alpha), FONT_LEFT, name.c_str());

				spectators++;
			}
		}
		else
		{
			if (prev_alpha > 0.f && !feature::menu->_menu_opened)
				prev_alpha -= min(5, prev_alpha);
		}

		if (feature::menu->_menu_opened && !was_moved_hotkeys)
		{
			if (ctx.pressed_keys[1] && feature::menu->mouse_in_pos(Vector(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y, 0), Vector(ctx.m_settings.visuals_spectators_pos.x + spectator_size.x, ctx.m_settings.visuals_spectators_pos.y + 15.0, 0)) || was_moved)
			{
				if (save_pos == false)
				{
					saved_x = feature::menu->_cursor_position.x - ctx.m_settings.visuals_spectators_pos.x;
					saved_y = feature::menu->_cursor_position.y - ctx.m_settings.visuals_spectators_pos.y;
					save_pos = true;
				}
				ctx.m_settings.visuals_spectators_pos.x = feature::menu->_cursor_position.x;
				ctx.m_settings.visuals_spectators_pos.y = feature::menu->_cursor_position.y;
				ctx.m_settings.visuals_spectators_pos.x = ctx.m_settings.visuals_spectators_pos.x - saved_x;
				ctx.m_settings.visuals_spectators_pos.y = ctx.m_settings.visuals_spectators_pos.y - saved_y;
			}
			else
				save_pos = was_moved = false;

			if (!was_moved)
				was_moved = ctx.pressed_keys[1] && feature::menu->mouse_in_pos(Vector(ctx.m_settings.visuals_spectators_pos.x, ctx.m_settings.visuals_spectators_pos.y, 0), Vector(ctx.m_settings.visuals_spectators_pos.x + spectator_size.x, ctx.m_settings.visuals_spectators_pos.y + 15.0, 0));
			else
				was_moved = ctx.pressed_keys[1];
		}
		else
			was_moved = false;
	}

	if (ctx.m_settings.visuals_extra_windows[1] && ctx.screen_size.x > 0 && ctx.screen_size.y > 0)
	{
		static float prev_alpha = 0;

		auto spectators = 1;

		static auto d = Drawing::GetTextSize(F::ESP, "H");

		int working_keybinds = 0;

		auto lol = get_keybinds(working_keybinds);

		Vector2D spectator_size = { 150.0f, working_keybinds * d.bottom + 20.0f };

		if ((ctx.m_settings.visuals_keybinds_pos + spectator_size) > ctx.screen_size)
		{
			if ((ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x) >= ctx.screen_size.x)
				ctx.m_settings.visuals_keybinds_pos.x = ctx.screen_size.x - 150.0;

			if ((ctx.m_settings.visuals_keybinds_pos.y + spectator_size.y) >= ctx.screen_size.y)
				ctx.m_settings.visuals_keybinds_pos.y = ctx.screen_size.y - (float(working_keybinds * d.bottom) + 15.0f);
		}
		else if (ctx.m_settings.visuals_keybinds_pos < Vector2D(0, 0)) {
			if (ctx.m_settings.visuals_keybinds_pos.x < 0)
				ctx.m_settings.visuals_keybinds_pos.x = 0;

			if (ctx.m_settings.visuals_keybinds_pos.y < 0)
				ctx.m_settings.visuals_keybinds_pos.y = 0;
		}

		if (feature::menu->_menu_opened)
			prev_alpha = 255;

		if (csgo.m_client()->IsChatRaised())
			prev_alpha = 10;

		//Drawing::DrawRect(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y, spectator_size.x, 15.f, Color(31.f, 33.f, 35.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 100.f))); // 31.f, 33.f, 35.f
		////Drawing::DrawString(Drawing::Eagle, ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y, Color(255.f, 255.f, 255.f, prev_alpha * float(cheat::Cvars.Visuals_spectators_alpha.GetValue() / 80.f)), FONT_LEFT, "r");
		//Drawing::DrawRect(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y + 15.f, spectator_size.x, spectator_size.y - 13.f, Color(43.f, 44.f, 46.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 100.f)));

		if (ctx.m_settings.visuals_keybinds_alpha > 2) {
			Drawing::DrawRect(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y + 1.0, spectator_size.x, spectator_size.y + 2.0, Color(35, 35, 35, prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.0)));
			Drawing::DrawOutlinedRect(ctx.m_settings.visuals_keybinds_pos.x - 1.0, ctx.m_settings.visuals_keybinds_pos.y, spectator_size.x + 2.0, spectator_size.y + 4.0, Color(10, 10, 10, prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.0)));
			Drawing::DrawRect(ctx.m_settings.visuals_keybinds_pos.x - 1.0, ctx.m_settings.visuals_keybinds_pos.y, spectator_size.x + 2.0, 2, ctx.m_settings.menu_color.alpha(prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 100.0)));
			Drawing::DrawString(F::ESP, ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x / 2.0, ctx.m_settings.visuals_keybinds_pos.y + 3.0, Color(255.0, 255.0, 255.0, prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 100.0)), FONT_CENTER, sxor("active keys"));

			//Drawing::DrawRectGradientVertical(ctx.screen_size.x - watermark_size.right - 21.f + watermark_size.right + 10.f, 9.f, 2, 17, ctx.m_settings.menu_color.alpha(220), Color(35, 35, 35, 200));
			//Drawing::DrawRectGradientVertical(ctx.screen_size.x - watermark_size.right - 21.f, 9.f, 2, 17, ctx.m_settings.menu_color.alpha(220), Color(35, 35, 35, 200));

			Drawing::DrawRectGradientHorizontal(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y + (d.bottom + 4.0), spectator_size.x / 2, 2.0, Color(35, 35, 35, prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.0)), ctx.m_settings.menu_color.alpha(prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.0)));
			Drawing::DrawRectGradientHorizontal(ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x / 2, ctx.m_settings.visuals_keybinds_pos.y + (d.bottom + 4.0), spectator_size.x / 2 - 1.0, 2.0, ctx.m_settings.menu_color.alpha(prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.0)), Color(35, 35, 35, prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.0)));
		}
		//Drawing::DrawLine(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y + (d.bottom + 4.f), ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x - 1.f, ctx.m_settings.visuals_keybinds_pos.y + (d.bottom + 4.f), Color(80.f, 80.f, 80.f, prev_alpha * float(ctx.m_settings.visuals_keybinds_alpha / 130.f)));

		if (working_keybinds > 0)
		{
			if (prev_alpha < 255 && !csgo.m_client()->IsChatRaised())
				prev_alpha += min(5, 255 - prev_alpha);

			for (auto bind : lol)
			{
				if (((csgo.m_globals()->realtime - keybinds_time[bind.index]) * 2.f) > 0.89f)
					continue;

				auto size = Drawing::GetTextSize(F::ESP, bind.name);

				//Drawing::DrawRect(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y + d.bottom * spectators, size.right + 2, 13, Color(0.f, 0.f, 0.f, prev_alpha * float(cheat::Cvars.Visuals_spectators_alpha.GetValue() / 100.f)));

				//char new_name[34];

				//memcpy(new_name, spec_inf.name, sizeof(char) * 33);

				//if (spectators > 1)
				//	Drawing::DrawLine(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y + 5.f + (d.bottom + 1.f) * spectators, ctx.m_settings.visuals_keybinds_pos.x + 149.f, ctx.m_settings.visuals_keybinds_pos.y + 5.f + (d.bottom + 1.f) * spectators, Color(80.f, 80.f, 80.f, prev_alpha * float(ctx.m_settings.visuals_spectators_alpha / 80.f)));

				//const auto wstr = std::wstring(std::string(spec_inf.name).begin(), std::string(spec_inf.name).end());

				auto new_alpha = min(prev_alpha, 250 * min(1.0f, 1.0f - abs(csgo.m_globals()->realtime - keybinds_time[bind.index]) * 2.0f));


				Drawing::DrawString(F::ESP, ctx.m_settings.visuals_keybinds_pos.x + 3.0f + (ctx.m_settings.visuals_keybinds_alpha == 1 ? spectator_size.x / 2 : 0), ctx.m_settings.visuals_keybinds_pos.y + 6.0f + 2.0f + float(d.bottom * spectators), Color(255, 255, 255, new_alpha), ctx.m_settings.visuals_keybinds_alpha == 1 ? FONT_CENTER : FONT_LEFT, "%s", bind.name);

				if (ctx.m_settings.visuals_keybinds_alpha > 1) {
					if (ctx.active_keybinds[bind.index].mode > 0 || bind.index >= 7 && bind.index < 15)
						Drawing::DrawString(F::ESP, ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x - 3.0f, ctx.m_settings.visuals_keybinds_pos.y + 6.0f + 2.0f + float(d.bottom * spectators), Color(255, 255, 255, new_alpha), FONT_RIGHT, "%s", (bind.index >= 7 && bind.index < 11 ? _type[4].c_str() : bind.type));
				}

				spectators++;
			}
		}
		else
		{
			if (prev_alpha > 0 && !feature::menu->_menu_opened)
				prev_alpha -= min(5, prev_alpha);
		}

		if (feature::menu->_menu_opened && !was_moved)
		{
			if (ctx.pressed_keys[1] && feature::menu->mouse_in_pos(Vector(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y, 0), Vector(ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x, ctx.m_settings.visuals_keybinds_pos.y + 15.f, 0)) || was_moved_hotkeys)
			{
				if (save_pos_hotkeys == false)
				{
					saved_x_hotkeys = feature::menu->_cursor_position.x - ctx.m_settings.visuals_keybinds_pos.x;
					saved_y_hotkeys = feature::menu->_cursor_position.y - ctx.m_settings.visuals_keybinds_pos.y;
					save_pos_hotkeys = true;
				}
				ctx.m_settings.visuals_keybinds_pos.x = feature::menu->_cursor_position.x;
				ctx.m_settings.visuals_keybinds_pos.y = feature::menu->_cursor_position.y;
				ctx.m_settings.visuals_keybinds_pos.x = ctx.m_settings.visuals_keybinds_pos.x - saved_x_hotkeys;
				ctx.m_settings.visuals_keybinds_pos.y = ctx.m_settings.visuals_keybinds_pos.y - saved_y_hotkeys;
			}
			else
				save_pos_hotkeys = was_moved_hotkeys = false;

			if (!was_moved_hotkeys)
				was_moved_hotkeys = ctx.pressed_keys[1] && feature::menu->mouse_in_pos(Vector(ctx.m_settings.visuals_keybinds_pos.x, ctx.m_settings.visuals_keybinds_pos.y, 0), Vector(ctx.m_settings.visuals_keybinds_pos.x + spectator_size.x, ctx.m_settings.visuals_keybinds_pos.y + 15.f, 0));
			else
				was_moved_hotkeys = ctx.pressed_keys[1];
		}
		else
			was_moved_hotkeys = false;
	}

	if (ctx.m_local() == nullptr || !csgo.m_engine()->IsInGame()) return;

	static auto linegoesthrusmoke = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
	static auto smokecout = *reinterpret_cast<DWORD*>(linegoesthrusmoke + 0x8);

	if (ctx.m_settings.visuals_no_smoke)
		*reinterpret_cast<int*>(smokecout) = 0;

	// SKEET COLORS ARE IN ABGR
	
	const int centerX = ctx.screen_size.x / 2, centerY = ctx.screen_size.y / 2;
	render_tracers();
	
	if (m_weapon() != nullptr && !ctx.m_local()->IsDead() && (DWORD)ctx.m_local()->get_animation_state() > 0x5000)
	{
		if (ctx.m_local()->m_bIsScoped() && ctx.m_settings.visuals_no_scope)
		{
			Drawing::DrawLine(0, centerY, centerX + 1, centerY, Color::Black());
			Drawing::DrawLine(centerX - 1, centerY, ctx.screen_size.x, centerY, Color::Black());

			Drawing::DrawLine(centerX, 0, centerX, centerY + 1, Color::Black());
			Drawing::DrawLine(centerX, centerY - 1, centerX, ctx.screen_size.y, Color::Black());

		}

		//skeleton(ctx.m_local(), Color::White(), ctx.zero_matrix);

		//static bool was_breaking = false;

		//auto is_breaking_lagcomp = (ctx.m_local()->m_vecOrigin() - feature::anti_aim->sent_data.m_vecOrigin).Length() >= 64;

		//for (auto i = 0; i < ctx.m_local()->get_animation_layers_count(); i++)
		//{
		////	//if (i != 6)
		////	//	continue;
		//	auto layer = &ctx.m_local()->get_animation_layer(i);
		//	Drawing::DrawString(F::ESPInfo, 10, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] weight: %.2f", i, layer->m_flWeight);
		//	Drawing::DrawString(F::ESPInfo, 120, 20 + 12 + i * 13, Color::White(150), FONT_LEFT, "[%d] weightdeltar: %.2f", i, layer->m_flWeightDeltaRate);
		//	Drawing::DrawString(F::ESPInfo, 260, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] playback: %.6f", i, layer->m_flPlaybackRate);
		//	Drawing::DrawString(F::ESPInfo, 380, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] cycle: %.6f", i, layer->m_flCycle);
		//	Drawing::DrawString(F::ESPInfo, 460, 20 + 12 + i * 13, Color::White(150), FONT_LEFT, "[%d] sequence: %d", i, layer->m_nSequence);
		//	//Drawing::DrawString(F::OldESP, 460, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] layer animtime: %.6f", i, layer->m_flLayerAnimtime);
		//}

		//auto layer = &ctx.local_layers[ANGLE_REAL][6];
		//Drawing::DrawString(F::ESPInfo, 10, 20 + 12 + 5 * 13, Color::White(200), FONT_LEFT, "[%d] weight: %.2f", 6, layer->m_flWeight);
		//Drawing::DrawString(F::ESPInfo, 120, 20 + 12 + 5 * 13, Color::White(200), FONT_LEFT, "[%d] weightdeltar: %.2f", 6, layer->m_flWeightDeltaRate);
		//Drawing::DrawString(F::ESPInfo, 260, 20 + 12 + 5 * 13, Color::White(200), FONT_LEFT, "[%d] playback: %.6f", 6, layer->m_flPlaybackRate);
		//Drawing::DrawString(F::ESPInfo, 380, 20 + 12 + 5 * 13, Color::White(200), FONT_LEFT, "[%d] cycle: %.6f", 6, layer->m_flCycle);
		//Drawing::DrawString(F::ESPInfo, 460, 20 + 12 + 5 * 13, Color::White(200), FONT_LEFT, "[%d] sequence: %d", 6, layer->m_nSequence);

		//Vector dummy = Vector::Zero; Vector dummy2 = Vector::Zero;

		auto color = ctx.autowall_crosshair == 0 ? Color::Red() : (ctx.autowall_crosshair == 2 ? Color::Green() : Color(0, 129, 255, 255));

		if (ctx.m_settings.visuals_autowall_crosshair) {
			//Drawing::DrawFilledCircle(ctx.screen_size.x / 2.f, ctx.screen_size.y / 2.f, 3.f, 10, Color(20, 20, 20, 220));
			//Drawing::DrawFilledCircle(ctx.screen_size.x / 2.f, ctx.screen_size.y / 2.f, 2.5f, 10, color);
			
			Drawing::DrawRect(ctx.screen_size.x / 2.f-1, ctx.screen_size.y / 2.f - 2, 2, 3, color);
			Drawing::DrawRect(ctx.screen_size.x / 2.f - 2, ctx.screen_size.y / 2.f-1, 3, 2, color);

			//Vector dummy;
			//Vector dummy2;
			//Vector dummy3;

			//Ray_t ray;
			//Vector forward;
			//Math::AngleVectors(Engine::Movement::Instance()->m_qRealAngles, &forward);
			//ray.Init(ctx.m_eye_position, ctx.m_eye_position + (forward * 8192.f));
			//CTraceFilter filter;
			//filter.pSkip = ctx.m_local();
			//trace_t tr;
			//csgo.m_engine_trace()->TraceRay(ray, MASK_SHOT, &filter, &tr);

			//if (!tr.startsolid)
			//{
			//	//tr.plane.normal.z = 0;
			//	//float proj = tr.plane.normal.Dot(forward);
			//	auto pos = tr.endpos + Vector(-2, -2,-2).Cross(tr.plane.normal);
			//	auto pos2 = tr.endpos + Vector(2, 2, 2).Cross(tr.plane.normal);

			//	if (Drawing::WorldToScreen(pos, dummy) && Drawing::WorldToScreen(pos2, dummy2) && Drawing::WorldToScreen(tr.endpos, dummy3))
			//	{
			//		auto dist = (dummy.x - dummy3.x) / 2.f;

			//		Drawing::DrawLine(dummy.x + dist, dummy2.y, dummy2.x, dummy2.y, Color::Red());
			//		Drawing::DrawLine(dummy2.x, dummy.y, dummy2.x, dummy2.y, Color::Blue());
			//		Drawing::DrawLine(dummy.x + dist, dummy.y, dummy2.x, dummy.y, Color::Green());
			//		Drawing::DrawLine(dummy.x + dist, dummy.y, dummy.x + dist, dummy2.y, Color::Purple());
			//	}
			//}
		}
		/*else if (ctx.m_settings.visuals_penetration_crosshair_type == 3)
		{

		}*/

		if (ctx.m_settings.misc_visuals_indicators_2[2])
		{
			//if (!is_breaking_lagcomp)
			//	is_breaking_lagcomp = !(ctx.m_local()->m_fFlags() & FL_ONGROUND) && was_breaking && ctx.m_local()->m_vecVelocity().Length() > 270.0 && ctx.m_local()->m_vecVelocity().Length() >= Engine::Prediction::Instance()->GetVelocity().Length();

			static c_indicator lc_indicator("LC", 1);

			if (ctx.m_local()->m_vecVelocity().Length2D() > 260) {
				lc_indicator.draw(1.f, (ctx.breaks_lc ? Color(123, 194, 21, csgo.m_client()->IsChatRaised() ? 5 : 250) : Color::Red(csgo.m_client()->IsChatRaised() ? 5 : 250)));
			}
			else
				indicators_count++;

			//static c_indicator fakelag_indicator("choke", 16.f);
			//fakelag_indicator.draw(csgo.m_client_state()->m_iChockedCommands, Color(123, 194, 21, 250));

			/*ctx.doubletap_charged*/

			//if (ctx.pressed_keys[(int)'N'])
			//	ctx.m_player_entities.clear();

			//if (ctx.last_usercmd != nullptr)
			/*static auto old_tickc = ctx.current_tickcount;
			auto serverTickcount = ctx.current_tickcount + int(ctx.latency[FLOW_OUTGOING] / csgo.m_globals()->interval_per_tick) + 2;
			Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(255, 255, 255, 210), FONT_LEFT, "[%d|%d|%d]", old_tickc, ctx.current_tickcount, serverTickcount);
			old_tickc = ctx.current_tickcount;*/

			//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(123, 194, 21, 250), FONT_LEFT, "%.2f", Engine::Prediction::Instance()->m_vecAfterPredVelocity.Length());
			//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(123, 194, 21, 250), FONT_LEFT, "%.2f", Engine::Prediction::Instance()->GetVelocity().Length());
		}

		//auto drawAngleLine = [&](const Vector& origin, const Vector& w2sOrigin, const float& angle, const char* text, Color clr) {
		//	Vector forward = QAngle(0.0f, angle, 0.0f).ToVectors();

		//	float AngleLinesLength = 30.0f;

		//	Vector w2sReal;
		//	if (Drawing::WorldToScreen(origin + forward * AngleLinesLength, w2sReal)) {
		//		Drawing::DrawLine(w2sOrigin.x, w2sOrigin.y, w2sReal.x, w2sReal.y, clr);
		//		//Drawing::DrawRect(w2sReal.x - 5.0f, w2sReal.y - 5.0f, 10.0f, 10.0f, Color(50, 50, 50, 150));
		//		Drawing::DrawString(F::ESPInfo, w2sReal.x, w2sReal.y, clr, FONT_CENTER, text);
		//	}
		//};
		//if (ctx.fakeducking)
		//if (m_weapon() != nullptr && m_weapon->can_shoot()
		//	Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, ctx.m_local()->m_flDuckSpeed() >= 4.0f ? Color(255, 255, 255, csgo.m_client()->IsChatRaised() ? 5 : 250) : Color::Red(csgo.m_client()->IsChatRaised() ? 5 : 250), FONT_LEFT, "DUCK");

		//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(255, 255, 255, csgo.m_client()->IsChatRaised() ? 5 : 250), FONT_LEFT, "%.5f | %.5f", Engine::Prediction::Instance()->m_flCalculatedInaccuracy , m_weapon()->m_flAccuracyPenalty());

		if (ctx.m_settings.misc_visuals_indicators_2[8] && ctx.has_exploit_toggled && (ctx.exploit_allowed || !ctx.fakeducking)) {

			static float draw_factor = 0.f;

			if (ctx.ticks_allowed > 2 && (csgo.m_client_state()->m_iChockedCommands < 2 || ctx.is_charging)) {
				draw_factor += min(1.f - draw_factor, csgo.m_globals()->frametime * 3.0f);

				if (draw_factor > 1)
					draw_factor = 1;
			}
			else
			{
				draw_factor -= min(draw_factor, csgo.m_globals()->frametime * 4.0f);

				if (draw_factor < 0)
					draw_factor = 0;
			}

			const auto text_size = Drawing::GetTextSize(F::LBY, ctx.main_exploit == 1 ? sxor("ONSHOT") : sxor("DT"));
			Drawing::DrawString(F::LBY,
				10,
				ctx.screen_size.y - 88 - 26 * indicators_count,
				Color(30, 30, 30, csgo.m_client()->IsChatRaised() ? 5 : 250),
				FONT_LEFT,
				ctx.main_exploit == 1 ? sxor("ONSHOT") : sxor("DT"));
			
			*reinterpret_cast<bool*>(uintptr_t(csgo.m_surface()) + 0x280) = true;
			int x, y, x1, y1;
			Drawing::GetDrawingArea(x, y, x1, y1);
			Drawing::LimitDrawingArea(0, ctx.screen_size.y - 88 - 26 * indicators_count + text_size.bottom * (1.f - draw_factor), text_size.right + 10, (int)text_size.bottom);
			Drawing::DrawString(F::LBY,
				10,
				ctx.screen_size.y - 88 - 26 * indicators_count++,
				ctx.ticks_allowed > 13 && !ctx.fakeducking && ctx.exploit_allowed ? Color(123, 194, 21, csgo.m_client()->IsChatRaised() ? 5 : 250) : Color::Red(csgo.m_client()->IsChatRaised() ? 5 : 250),
				FONT_LEFT,
				ctx.main_exploit == 1 ? sxor("ONSHOT") : sxor("DT"));
			Drawing::LimitDrawingArea(x, y, x1, y1);
			*reinterpret_cast<bool*>(uintptr_t(csgo.m_surface()) + 0x280) = false;

			/*if (csgo.m_client_state()->m_iChockedCommands > 1) {
				Drawing::DrawString(F::ESPInfo,
					45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7,
					Color::White(),
					FONT_LEFT,
					sxor("FL"));
			}*/

			if (ctx.main_exploit >= 2)
			{
				static float prev_alpha = 1.f;
				static float prev_alpha_first_bullet = 1.f;

				if (ctx.m_local() && !ctx.m_local()->IsDead() && m_weapon() && m_weapon()->can_shoot() && ctx.latest_weapon_data) {
					prev_alpha_first_bullet += min(1.f - prev_alpha_first_bullet, csgo.m_globals()->frametime * 2.0f);

					if (m_weapon()->can_exploit(ctx.latest_weapon_data->flCycleTime + 0.01f))
						prev_alpha_first_bullet = 1;

					if (prev_alpha_first_bullet > 1)
						prev_alpha_first_bullet = 1;
				}
				else
				{
					prev_alpha_first_bullet -= min(prev_alpha_first_bullet, csgo.m_globals()->frametime * 5.0f);

					if (prev_alpha_first_bullet < 0)
						prev_alpha_first_bullet = 0;
				}

				if (ctx.has_exploit_toggled && ctx.exploit_allowed && ctx.main_exploit >= 2) {
					if (ctx.ticks_allowed > 2 && prev_alpha_first_bullet >= 1) {
						prev_alpha += min(1.f - prev_alpha, csgo.m_globals()->frametime * 2.0f);

						if (prev_alpha > 1)
							prev_alpha = 1;
					}
					else
					{
						prev_alpha -= min(prev_alpha, csgo.m_globals()->frametime * 4.0f);

						if (prev_alpha < 0)
							prev_alpha = 0;
					}

					//Drawing::DrawLine(45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7 + 14, 52, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7 + 7, Color::White(prev_alpha * 255.f));
					//Drawing::DrawPixel(46, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7 + 13, Color::Black(max(1.5f * prev_alpha, 1) * 255.f));
					Drawing::DrawString(F::Icons, 45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 14, Color::White(prev_alpha * 255), FONT_LEFT, "u");
				}

				//Drawing::DrawLine(45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7 + 7, 52, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7, Color::White(prev_alpha_first_bullet * 255.f));
				//Drawing::DrawPixel(46, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7 + 6, Color::Black(max(1.5f * prev_alpha_first_bullet, 1) * 255.f));

				Drawing::DrawString(F::Icons, 45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 3, Color::White(prev_alpha_first_bullet * 255), FONT_LEFT, "u");

				//Drawing::DrawLine(45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 7, 50, -7, Color::White());
				//Drawing::DrawLine(45, ctx.screen_size.y - 88 - 26 * (indicators_count - 1) + 14, 50, -7, Color::White());

				//Drawing::DrawString(F::ESPInfo, 45, ctx.screen_size.y - 88 - 26 * (indicators_count-1) + 7, Color::White().alpha(alpha), FONT_LEFT, "/");
				//Drawing::DrawString(F::ESPInfo, 45, ctx.screen_size.y - 88 - 26 * (indicators_count-1) + 14, Color::White().alpha(alpha), FONT_LEFT, "/");
			}
		}
		//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(255, 255, 255, csgo.m_client()->IsChatRaised() ? 5 : 250), FONT_LEFT, "%.5f", Engine::Prediction::Instance()->m_flSpread);
		//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(255, 255, 255, csgo.m_client()->IsChatRaised() ? 5 : 250), FONT_LEFT, "%.5f", Engine::Prediction::Instance()->m_flInaccuracy);
		//if (m_weapon() != nullptr && m_weapon())
		//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++,  Color(255, 255, 255, csgo.m_client()->IsChatRaised() ? 5 : 250), FONT_LEFT, "%.5f", m_weapon()->m_fAccuracyPenalty());


		//Vector w2sOrigin;
		//if (Drawing::WorldToScreen(ctx.m_local()->m_vecOrigin(), w2sOrigin)) {
		//	//drawAngleLine(ctx.m_local()->m_vecOrigin(), w2sOrigin, cheat::game::last_cmd->viewangles.y, "viewangles", Color(0.937f * 255.f, 0.713f * 255.f, 0.094f * 255.f, 255.f));
		//	drawAngleLine(ctx.m_local()->m_vecOrigin(), w2sOrigin, ctx.m_local()->m_flLowerBodyYawTarget(), "lby", Color(0, 0, 255.f, 255.f));
		//	drawAngleLine(ctx.m_local()->m_vecOrigin(), w2sOrigin, ctx.m_local()->m_angEyeAngles().y, "networked angles", Color(0, 255.f, 0, 255.f));
		//	//drawAngleLine(ctx.m_local()->m_vecOrigin(), w2sOrigin, ctx.angles[ANGLE_FAKE], "fake abs", Color(255.f, 0, 0, 255.f));
		//	//drawAngleLine(ctx.m_local()->m_vecOrigin(), w2sOrigin, feature::anti_aim->visual_real_angle.y, "fake angle", Color(255.f, 0, 255.f, 255.f));
		//}

		//Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(123, 194, 21, 250), FONT_LEFT, "%.3f", ctx.m_local()->get_animation_layer(6).m_flWeight);

		//skeleton(ctx.m_local(), Color::White(), ctx.zero_matrix);

		if (ctx.m_settings.misc_visuals_indicators_2[1])
		{
			/*static auto lbysize = Drawing::GetTextSize(F::LBY, "FAKE");

			auto delta = feature::anti_aim->max_delta;

			Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators, Color(30, 30, 30, 250), FONT_LEFT, "FAKE");

			auto factor = Math::clamp((delta / 58.f), 0.f, 1.f);

			*(bool*)((DWORD)csgo.m_surface() + 0x280) = true;
			int x, y, x1, y1;
			Drawing::GetDrawingArea(x, y, x1, y1);
			Drawing::LimitDrawingArea(0, ctx.screen_size.y - 88 - 26 * indicators, int((lbysize.right + 15) * factor), (int)lbysize.bottom);

			const auto lby_broken = (factor > 0.5f);

			Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators++, lby_broken ? Color(123, 194, 21, 250) : Color::Red(250), FONT_LEFT, "FAKE");

			Drawing::LimitDrawingArea(x, y, x1, y1);
			*(bool*)((DWORD)csgo.m_surface() + 0x280) = false;*/

			/*static Color Colors[] = {
				{ 124, 195, 13 },
				{ 176, 205, 10 },
				{ 213, 201, 19 },
				{ 220, 169, 16 },
				{ 228, 126, 10 },
				{ 229, 104, 8 },
				{ 235, 63, 6 },
				{ 237, 27, 3 },
				{ 255, 0, 0 }
			};*/

			//const auto lol = fabs(Math::AngleDiff(ctx.angles[ANGLE_REAL], ctx.angles[ANGLE_FAKE])) - 15.f;
			//const auto max_lol = 58.f * (60.f / float(ctx.fside < 0 ? ctx.m_settings.anti_aim_fake_left_limit : ctx.m_settings.anti_aim_fake_right_limit));

			//auto factor = 8 - int(Math::clamp((lol / (max_lol - 15.f)), 0.f, 1.f) * 8);

			static c_indicator fake_indicator("FAKE", 1);
			fake_indicator.draw(1.f, g_ColorByInt(feature::anti_aim->max_delta, 60));

			/*int real = ctx.angles[ANGLE_REAL], fake = ctx.angles[ANGLE_FAKE];

			auto delta = abs(Math::normalize_angle(real % 360 - fake % 360)) / 2;

			auto frac = delta / 60;

			Drawing::DrawArc(x + 5, y, 6, 13, 0, 360, Color{ 10, 10, 10, 60 });
			Drawing::DrawArc(x + 5, y, 7, 11, 0, 360 * frac, color);*/
		}

		if (ctx.m_settings.misc_visuals_indicators_2[7])
		{
			static bool filled[3] = { false,false,false };

			auto left_pos = Vector2D(ctx.screen_size.x/2 - 45, ctx.screen_size.y / 2), right_pos = Vector2D(ctx.screen_size.x / 2 + 45, ctx.screen_size.y / 2), down_pos = Vector2D(ctx.screen_size.x / 2, ctx.screen_size.y / 2 + 45), siz = Vector2D(8, 8);

			static std::vector< Vertex_t > vertices_left =
			{
				Vertex_t{ Vector2D(left_pos.x - siz.x, left_pos.y + siz.y), Vector2D() },
				Vertex_t{ Vector2D(left_pos.x, left_pos.y - siz.y), Vector2D() },
				Vertex_t{ left_pos + siz, Vector2D() }
			};

			static std::vector< Vertex_t > vertices_right =
			{
				Vertex_t{ Vector2D(right_pos.x - siz.x, right_pos.y + siz.y), Vector2D() },
				Vertex_t{ Vector2D(right_pos.x, right_pos.y - siz.y), Vector2D() },
				Vertex_t{ right_pos + siz, Vector2D() }
			};

			static std::vector< Vertex_t > vertices_down =
			{
				Vertex_t{ Vector2D(down_pos.x - siz.x, down_pos.y + siz.y), Vector2D() },
				Vertex_t{ Vector2D(down_pos.x, down_pos.y - siz.y), Vector2D() },
				Vertex_t{ down_pos + siz, Vector2D() }
			};

			if (reset) {
				filled[0] = false;
				filled[1] = false;
				filled[2] = false;
			}

			if (!filled[0]) {
				vertices_left =
				{
					Vertex_t{ Vector2D(left_pos.x - siz.x, left_pos.y + siz.y), Vector2D() },
					Vertex_t{ Vector2D(left_pos.x, left_pos.y - siz.y), Vector2D() },
					Vertex_t{ left_pos + siz, Vector2D() }
				};

				for (unsigned int p = 0; p < vertices_left.size(); p++) {
					Drawing::rotate_point(vertices_left[p].m_Position, left_pos, false, 3.15);
				}
				filled[0] = true;
			}

			if (!filled[1]) {
				vertices_right =
				{
					Vertex_t{ Vector2D(right_pos.x - siz.x, right_pos.y + siz.y), Vector2D() },
					Vertex_t{ Vector2D(right_pos.x, right_pos.y - siz.y), Vector2D() },
					Vertex_t{ right_pos + siz, Vector2D() }
				};

				for (unsigned int p = 0; p < vertices_right.size(); p++) {
					Drawing::rotate_point(vertices_right[p].m_Position, right_pos, false, 0);
				}
				filled[1] = true;
			}

			if (!filled[2]) {
				vertices_down =
				{
					Vertex_t{ Vector2D(down_pos.x - siz.x, down_pos.y + siz.y), Vector2D() },
					Vertex_t{ Vector2D(down_pos.x, down_pos.y - siz.y), Vector2D() },
					Vertex_t{ down_pos + siz, Vector2D() }
				};

				for (unsigned int p = 0; p < vertices_down.size(); p++) {
					Drawing::rotate_point(vertices_down[p].m_Position, down_pos, false, 61.25);
				}
				filled[2] = true;
			}

			auto fc = (ctx.side == 0 ? ctx.m_settings.menu_color.alpha(160) : Color::Black(130));
			auto sc = (ctx.side == 1 ? ctx.m_settings.menu_color.alpha(160) : Color::Black(130));
			auto tc = (ctx.side == 2 ? ctx.m_settings.menu_color.alpha(160) : Color::Black(130));

			Drawing::TexturedPolygon(vertices_left.size(), vertices_left, fc); //Color(50, 122, 239, 200)
			Drawing::TexturedPolygon(vertices_right.size(), vertices_right, sc);
			Drawing::TexturedPolygon(vertices_down.size(), vertices_down, tc);

			if (ctx.m_settings.misc_visuals_indicators_2[9]) {
				auto angles_difference = Math::AngleDiff(ctx.angles[ANGLE_REAL], ctx.angles[ANGLE_FAKE]);

				if (abs(angles_difference) < 26.f) {
					//bottom line
					Drawing::DrawLine(vertices_down[0].m_Position.x, vertices_down[0].m_Position.y, vertices_down[1].m_Position.x, vertices_down[1].m_Position.y, ctx.m_settings.menu_color.alpha(220));
					//top line
					Drawing::DrawLine(vertices_down[2].m_Position.x, vertices_down[2].m_Position.y, vertices_down[1].m_Position.x, vertices_down[1].m_Position.y, ctx.m_settings.menu_color.alpha(220));
				}
				else if (angles_difference < 0) {
					//bottom line
					Drawing::DrawLine(vertices_left[0].m_Position.x, vertices_left[0].m_Position.y, vertices_left[1].m_Position.x, vertices_left[1].m_Position.y, ctx.m_settings.menu_color.alpha(220));
					//top line
					Drawing::DrawLine(vertices_left[2].m_Position.x, vertices_left[2].m_Position.y, vertices_left[1].m_Position.x, vertices_left[1].m_Position.y, ctx.m_settings.menu_color.alpha(220));
				}
				else if (angles_difference > 0) {
					//bottom line
					Drawing::DrawLine(vertices_right[0].m_Position.x, vertices_right[0].m_Position.y, vertices_right[1].m_Position.x, vertices_right[1].m_Position.y, ctx.m_settings.menu_color.alpha(220));
					//top line
					Drawing::DrawLine(vertices_right[2].m_Position.x, vertices_right[2].m_Position.y, vertices_right[1].m_Position.x, vertices_right[1].m_Position.y, ctx.m_settings.menu_color.alpha(220));
				}
			}
			//lowerdown topright
			//Drawing::Triangle(Vector(vertices_left[0].m_Position.x, vertices_left[2].m_Position.y,0), Vector(vertices_left[2].m_Position.x, vertices_left[2].m_Position.y, 0), Color::Red());
		}

		/*if (ctx.m_settings.misc_visuals_indicators_2[7] && ctx.m_settings.anti_aim_enabled && max(ctx.m_settings.anti_aim_fake_right_limit, ctx.m_settings.anti_aim_fake_left_limit) > 4) {
			static auto fsize = Drawing::GetTextSize(F::LBY, ("FAKE"));
			const auto x = 10 + (!ctx.m_settings.misc_visuals_indicators_2[1] ? 0 : fsize.right + 8);
			Drawing::DrawString(F::LBY, x, ctx.screen_size.y - 88 - 26 * (indicators_count - (ctx.m_settings.misc_visuals_indicators_2[1] ? 1 : 0)), Color(255, 255, 255, csgo.m_client()->IsChatRaised() ? 5 : 220), FONT_LEFT, ctx.fside > 0 ? "R" : "L");

			if (!ctx.m_settings.misc_visuals_indicators_2[1])
				indicators_count++;
		}*/

		//if (ctx.m_settings.misc_visuals_indicators_2[6] && ctx.active_keybinds[6].mode > 0/*ctx.get_key_press(ctx.m_settings.aimbot_min_damage_override)*/)
		//	Drawing::DrawString(F::LBY, 10, ctx.screen_size.y - 88 - 26 * indicators_count++, Color(123, 194, 21, csgo.m_client()->IsChatRaised() ? 5 : 250), FONT_LEFT, ("DMG"));

		//if (ctx.m_settings.misc_visuals_indicators_2[5]) {
		//	int pops = 1;

		//	if (ctx.m_settings.security_safety_mode == 0)
		//		Drawing::DrawString(F::ESPInfo, ctx.screen_size.x / 2, ctx.screen_size.y - 11 * pops++, Color(255, 255, 255, 250), FONT_CENTER, ("UNTRUSTED"));
		//	if (ctx.active_keybinds[5].mode > 0)
		//		Drawing::DrawString(F::ESPInfo, ctx.screen_size.x / 2, ctx.screen_size.y - 11 * pops++, Color(255, 255, 255, 250), FONT_CENTER, ("FORCE BODY-AIM"));
		//}

		static float alpha = 0;

		if (ctx.hurt_time > csgo.m_globals()->realtime)
			alpha = 255;
		else
			alpha = alpha - 255 / 0.3 * csgo.m_globals()->frametime;

		if (alpha > 0 && ctx.m_settings.misc_visuals_indicators_2[0]) {
			Drawing::DrawLine(ctx.screen_size.x / 2 - linesize, ctx.screen_size.y / 2 - linesize, ctx.screen_size.x / 2 - linedec, ctx.screen_size.y / 2 - linedec, Color(200, 200, 200, alpha));
			Drawing::DrawLine(ctx.screen_size.x / 2 - linesize, ctx.screen_size.y / 2 + linesize, ctx.screen_size.x / 2 - linedec, ctx.screen_size.y / 2 + linedec, Color(200, 200, 200, alpha));
			Drawing::DrawLine(ctx.screen_size.x / 2 + linesize, ctx.screen_size.y / 2 + linesize, ctx.screen_size.x / 2 + linedec, ctx.screen_size.y / 2 + linedec, Color(200, 200, 200, alpha));
			Drawing::DrawLine(ctx.screen_size.x / 2 + linesize, ctx.screen_size.y / 2 - linesize, ctx.screen_size.x / 2 + linedec, ctx.screen_size.y / 2 - linedec, Color(200, 200, 200, alpha));
		}
	}

	if (ctx.current_tickcount % 2 == 1)
	{
		/*static bool playing = false;
		static bool prev_value = false;

		ctx.active_keybinds[2] = 0;

		if (ctx.get_key_press(ctx.m_settings.music_key)) {
			playing = true;
			ctx.active_keybinds[2] = ctx.m_settings.music_key.mode + 1;
		}
		else
			playing = false;

		if (prev_value != playing && ctx.music_found.size() > 0 && strstr(ctx.music_found[0].c_str(), _m.operator std::string().c_str()) == nullptr)
		{
			if (playing)
				feature::music_player->play(std::string(sxor("C:\\2k17_v2\\sounds\\")) + ctx.music_found[ctx.m_settings.music_curtrack], 180.f);
			else
				feature::music_player->stop();

			prev_value = playing;
		}*/

		if (ctx.time_to_reset_sound > 0.f && ctx.time_to_reset_sound <= csgo.m_globals()->realtime) {

			feature::music_player->stop();
			ctx.time_to_reset_sound = 0.f;
		}
	}

	if (!ctx.m_settings.visuals_enabled)
		return;

	static auto max_bombtime = csgo.m_engine_cvars()->FindVar("mp_c4timer");
	static auto mp_teammates_are_enemies = csgo.m_engine_cvars()->FindVar("mp_teammates_are_enemies");
	player_info info;
	int x, y, w, h;

	auto radar_base = feature::find_hud_element<DWORD>(sxor("CCSGO_HudRadar"));
	auto hud_radar = (CCSGO_HudRadar*)(radar_base + 0x74);

	feature::sound_parser->draw_sounds();

	for (auto k = 0; k < csgo.m_entity_list()->GetHighestEntityIndex(); k++)
	{
		C_BasePlayer* entity = csgo.m_entity_list()->GetClientEntity(k);

		if (entity == nullptr ||
			!entity->GetClientClass() ||
			entity == ctx.m_local() ||
			((DWORD)entity->GetClientRenderable() < 0x1000))
			continue;

		if (entity->GetClientClass()->m_ClassID == class_ids::CCSPlayer && k < 64)
		{
			/*if (entity->IsDormant() && dormant_alpha[idx - 1] <= 0.f) {
				cheat::features::dormant.draw_sound(entity);
				continue;
			}*/

			if (!entity 
				/*entity->IsDormant() ||*/
				|| entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum() && ((int)mp_teammates_are_enemies < 1000 || !mp_teammates_are_enemies->GetBool())
				|| !csgo.m_engine()->GetPlayerInfo(entity->entindex(), &info)) continue;

			const auto idx = entity->entindex() - 1;

			const auto is_teammate = (entity->m_iTeamNum() == ctx.m_local()->m_iTeamNum());

			auto &radar_info = *reinterpret_cast<RadarPlayer_t*>(radar_base + (0x174 * (k + 1)) - 0x3C);

			auto was_spotted = (entity->m_bSpotted() || radar_info.spotted) && hud_radar && radar_base && !is_teammate;

			if (ctx.m_settings.misc_engine_radar && !entity->IsDormant() && entity->m_iHealth() > 0)
				entity->m_bSpotted() = true;

			c_player_records* log = &feature::lagcomp->records[idx];
			resolver_records* r_log = &feature::resolver->player_records[idx];

			if (entity->IsDormant() && was_spotted && TIME_TO_TICKS(csgo.m_globals()->curtime - entity->m_flSpawnTime()) > 5) {
				log->saved_hp = radar_info.health;
				//dormant_alpha[idx] = 150.f;
			}

			if (entity->IsDormant() && ctx.m_settings.esp_dormant && abs(log->last_sound - csgo.m_globals()->realtime) < 3.f && !log->render_origin.IsZero() && log->render_origin.IsValid()/*&& dormant_alpha[idx] > 0*/) {
				//dormant_alpha[idx] -= 2.5f;
				auto dd = 255 * (1.0f - min(1.0f, abs(log->last_sound - csgo.m_globals()->realtime) * 0.3f));

				Vector mins, maxs;
				Vector bottom, top;

				mins = csgo.m_movement()->GetPlayerMins(log->dormant_flags & FL_DUCKING);
				maxs = csgo.m_movement()->GetPlayerMaxs(log->dormant_flags & FL_DUCKING);

				// correct x and y coordinates.
				mins = { log->render_origin.x, log->render_origin.y, log->render_origin.z + mins.z };
				maxs = { log->render_origin.x, log->render_origin.y, log->render_origin.z + maxs.z + 8.f };

				if (!Drawing::WorldToScreen(mins, bottom) || !Drawing::WorldToScreen(maxs, top))
					continue;

				int h = bottom.y - top.y;
				int y = bottom.y - h;
				int w = h / 2.f;
				int x = bottom.x - (w / 2.f);

				int right = 0;
				int down = 0;

				static auto size_info = 9.f;

				auto box_color = ctx.flt2color(ctx.m_settings.box_enemy_color);
				auto skeletons_color = ctx.flt2color(ctx.m_settings.colors_skeletons_enemy);
				auto name_color = ctx.flt2color(ctx.m_settings.colors_esp_name);
				//auto weap_color = ctx.m_settings.colors_esp_weapon;

				int lol = 0;

				if (ctx.m_settings.esp_box) {
					Drawing::DrawOutlinedRect(x, y, w, h, box_color.alpha(min(box_color.a(), dd)));
					//Drawing::DrawOutlinedRect(x - 1, y - 1, w + 2, h + 2, Color(10, 10, 10, min(box_color.a(), dd * 0.5f)));
					//Drawing::DrawOutlinedRect(x + 1, y + 1, w - 2, h - 2, Color(10, 10, 10, min(box_color.a(), dd * 0.5f)));
				}

				if (ctx.m_settings.esp_name) {
					auto text_size = Drawing::GetTextSize(F::ESP, log->saved_info.name);
					Drawing::DrawString(F::ESP, x + w / 2 - text_size.right / 2, y - 14, name_color.alpha(min(name_color.a(), dd - 25.f)), FONT_LEFT, "%s", log->saved_info.name);
				}

				if (ctx.m_settings.esp_health) {
					int hp = log->saved_hp;

					if (hp > 100)
						hp = 100;

					int hp_percent = h - (int)((h * hp) / 100);

					int width = (w * (hp / 100.f));

					int red = 0x50;
					int green = 0xFF;
					int blue = 0x50;

					if (hp >= 25)
					{
						if (hp < 50)
						{
							red = 0xD7;
							green = 0xC8;
							blue = 0x50;
						}
					}
					else
					{
						red = 0xFF;
						green = 0x32;
						blue = 0x50;
					}

					char hps[10] = "";

					sprintf(hps, "%iHP", hp);

					auto text_size = Drawing::GetTextSize(F::ESPInfo, hps);

					Drawing::DrawRect(x - 5, y - 1, 4, h + 2, Color(80, 80, 80, dd * 0.49f));
					Drawing::DrawOutlinedRect(x - 5, y - 1, 4, h + 2, Color(10, 10, 10, (dd * 0.5f)));
					Drawing::DrawRect(x - 4, y + hp_percent, 2, h - hp_percent, Color(red, green, 0, dd));

					if (hp < 93)
						Drawing::DrawString(F::ESPInfo, x - text_size.right - 6, y - 1, Color(255, 255, 255, dd - 55.f), FONT_LEFT, hps);
				}

				if (ctx.m_settings.esp_flags[0] && entity->m_ArmorValue() > 1)
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, (entity->m_bHasHelmet() && entity->m_ArmorValue() > 0 ? _hk.operator std::string().c_str() : (entity->m_bHasHelmet() ? _h.operator std::string().c_str() : _k.operator std::string().c_str())));

				//feature::sound_parser->draw_sound(entity);
				continue;
			}

			if (entity->m_iHealth() <= 0)
				continue;

			log->saved_hp = entity->m_iHealth();
			log->render_origin = Math::interpolate(log->render_origin, entity->get_abs_origin(), Math::clamp((csgo.m_globals()->realtime - log->last_sound) * csgo.m_globals()->frametime, 0, 1));
			//const auto prev_simtime = entity->m_flSimulationTime();

			//if (!entity->IsDormant() && entity->m_vecOrigin() != entity->m_vecOldOrigin() && entity->m_flSimulationTime() == entity->m_flOldSimulationTime())
			//	entity->m_flSimulationTime() = TICKS_TO_TIME(*reinterpret_cast<int*>(uintptr_t(csgo.m_client_state()) + 0x170));

			dormant_alpha[idx] = 255 * (1.f - min(1.f, abs(entity->m_flSimulationTime() - TICKS_TO_TIME(csgo.m_globals()->tickcount)) * 0.3f));

			if (ctx.m_settings.esp_offscreen && !is_teammate)
				offscreen_esp(entity, max(1, dormant_alpha[idx]) / 255);

			auto bk = entity->m_vecOrigin();
			entity->m_vecOrigin() = log->render_origin;
			if (!get_espbox(entity, x, y, w, h)) { entity->m_vecOrigin() = bk; continue; }
			entity->m_vecOrigin() = bk;

			if (log)
				log->last_esp_box.Set(x, y, w, h);

			int right = 0;
			int down = 0;

			auto box_color = ctx.flt2color(ctx.m_settings.box_enemy_color);
			auto skeletons_color = ctx.flt2color(ctx.m_settings.colors_skeletons_enemy);
			auto name_color = ctx.flt2color(ctx.m_settings.colors_esp_name);
			auto ammo_color = ctx.flt2color(ctx.m_settings.colors_esp_ammo);

			const int lol = (is_teammate ? 1 : 0);

			if (ctx.m_settings.esp_box) {
				Drawing::DrawOutlinedRect(x, y, w, h, box_color.alpha(min(box_color.a(), dormant_alpha[idx])));
				Drawing::DrawOutlinedRect(x - 1, y - 1, w + 2, h + 2, Color(10, 10, 10, min(box_color.a(), dormant_alpha[idx] * 0.8)));
				Drawing::DrawOutlinedRect(x + 1, y + 1, w - 2, h - 2, Color(10, 10, 10, min(box_color.a(), dormant_alpha[idx] * 0.8)));
			}

			if (ctx.m_settings.esp_name)
				Drawing::DrawString(F::ESP, x + w / 2, y - 14, name_color.alpha(min(name_color.a(), dormant_alpha[idx] - 10)), FONT_CENTER, "%s", info.name);

			if (ctx.m_settings.esp_health) {
				int hp = log->saved_hp;

				if (hp > 100)
					hp = 100;

				const int hp_percent = h - (int)((h * hp) / 100);
				const int width = (w * float(hp / 100));

				/*const int red = 255 - (hp * 2.55);
				const int green = hp * 2.55;*/

				int red = 0x50;
				int green = 0xFF;
				int blue = 0x50;

				/*
				color_red = 0xA05032FF;
				color_yellow = 0xA050C8D7;
				color_green = 0xA050FF50;
				*/

				if (hp >= 25)
				{
					if (hp < 50)
					{
						red = 0xD7;
						green = 0xC8;
						blue = 0x50;
					}
				}
				else
				{
					red = 0xFF;
					green = 0x32;
					blue = 0x50;
				}
			
				Drawing::DrawRect(x - 5, y - 1, 4, h + 1, Color(0, 0, 0, min(0xA0, dormant_alpha[idx] * 0.49f)));
				//Drawing::DrawOutlinedRect(x - 5, y - 1, 4, h + 2, Color(10, 10, 10, (dormant_alpha[idx] * 0.5f)));
				Drawing::DrawRect(x - 4, y + hp_percent, 2, h - hp_percent, Color(red, green, blue, min(160, dormant_alpha[idx])));

				if (hp < 93)
					Drawing::DrawString(F::ESPInfo, x - 2, y + max(-1, min(h - 1, hp_percent - 1)), Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_CENTER, "%i", hp);
			}

			if (ctx.m_settings.esp_flags[0] && entity->m_ArmorValue() > 1)
				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, min(0xC8, dormant_alpha[idx]-10)), FONT_LEFT, (entity->m_bHasHelmet() && entity->m_ArmorValue() > 0 ? _hk.operator std::string().c_str() : (entity->m_bHasHelmet() ? _h.operator std::string().c_str() : _k.operator std::string().c_str())));

			//const auto r_log = feature::resolver->player_records[entity->entindex() - 1];
			//
			//if (!is_teammate && ctx.m_settings.esp_flags[lol][3] && r_log.did_anims_update)
			//	Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "R");

			/*if (cheat::Cvars.Visuals_enemies_flags.Has(6) && entity->m_flFlashTime() > 0.1f)
			{
				static auto flashsize = Drawing::GetTextSize(F::ESPInfo, "FLASHED");

				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right * size_info, Color(30, 30, 30, 250), FONT_LEFT, "FLASHED");

				auto factor = Math::clamp((entity->m_flFlashTime() / 255.f), 0.f, 1.f);

				*(bool*)((DWORD)csgo.m_surface() + 0x280) = true;
				int x1, y1, x2, y2;
				Drawing::GetDrawingArea(x1, y1, x2, y2);
				Drawing::LimitDrawingArea(x + w + 3, y + right * size_info, int(flashsize.right * factor), (int)flashsize.bottom);

				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 0, dormant_alpha[idx] - 55.f), FONT_LEFT, "FLASHED");

				Drawing::LimitDrawingArea(x1, y1, x2, y2);
				*(bool*)((DWORD)csgo.m_surface() + 0x280) = false;
			}*/

			/*if (auto a = entity->get_animation_state(); a != nullptr) {
				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "c%.2f", a->feet_cycle);
				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "r%.2f", a->feet_rate);

				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "sc%.2f", entity->get_animation_layer(6).m_flCycle);
				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "sr%.2f", entity->get_animation_layer(6).m_flWeight);
			}*/

			if (ctx.m_settings.esp_flags[1] && strlen(entity->m_szLastPlaceName()) > 1) {

				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, entity->IsDormant() ? Color(255, 170, 170, dormant_alpha[idx] - 55.f) : Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, entity->m_szLastPlaceName());

				const char* last_place = entity->m_szLastPlaceName();

				if (last_place && *last_place)
				{
					const wchar_t* u_last_place = csgo.m_localize()->find(last_place);

					if (u_last_place && *u_last_place)
						Drawing::DrawStringUnicode(F::ESPInfo, x + w + 3, y + right++ * size_info, entity->IsDormant() ? Color(255, 170, 170, min(0xC8, dormant_alpha[idx] - 10)) : Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, u_last_place);
				}
			}

			if (ctx.last_aim_index == entity->entindex())
				Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, "%i", ctx.last_aim_state);

			if (ctx.m_settings.esp_skeleton)
				skeleton(entity, skeletons_color.alpha(min(skeletons_color.a(), dormant_alpha[idx] - 10)), entity->m_CachedBoneData().Base());

			const auto &weapon = (C_WeaponCSBaseGun*)(csgo.m_entity_list()->GetClientEntityFromHandle(entity->m_hActiveWeapon()));
			//auto weapons = entity->m_hMyWeapons();

			if (weapon)
			{
				bool low_ammo = false;

				const auto& weap_info = weapon->GetCSWeaponData();

				if (!weap_info)
					continue;

				int ammo = weapon->m_iClip1();

				if (ammo > weap_info->max_clip)
					ammo = weap_info->max_clip;

				low_ammo = ammo < (weap_info->max_clip / 2);

				if (ctx.m_settings.esp_weapon_ammo) {
					//const int hp_percent = w - (int)((w * ammo) / 100);

					float width = w;//(w * (ammo / float(weap_info->max_clip)));

					const auto is_reloading = entity->get_sec_activity(entity->get_animation_layer(1).m_nSequence) == ACT_CSGO_RELOAD && entity->get_animation_layer(1).m_flWeight > 0;

					if (is_reloading)
						width *= entity->get_animation_layer(1).m_flCycle;
					else
						width = (w * (ammo / float(weap_info->max_clip)));

					char ammostr[10];

					if (!is_reloading)
						sprintf(ammostr, "%d", ammo);
					else
						sprintf(ammostr, sxor("R"));

					const auto text_size = Drawing::GetTextSize(F::ESPInfo, ammostr);

					Drawing::DrawRect(x, y + 1 + h, w + 1, 4, Color(0, 0, 0, min(0xA0, dormant_alpha[idx] * 0.49f)));
					//Drawing::DrawOutlinedRect(x - 1, y + 1 + h, w + 2, 4, Color(10, 10, 10, (dormant_alpha[idx] * 0.5)));
					Drawing::DrawRect(x + 1, y + 2 + h, max(0, (width - 1)), 2, ammo_color.alpha(min(ammo_color.a(), min(0xC8, dormant_alpha[idx] - 10))));

					if (ammo < (weap_info->max_clip / 2) && ammo > 0)
						Drawing::DrawString(F::ESPInfo, x + (is_reloading ? w / 2 : min(w - 2, width)), y + h - 1, Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_CENTER, ammostr);
				}

				if (ctx.m_settings.esp_weapon) {

					//char wpn_name[100] = "";

					//sprintf(wpn_name, "%s", weap_info->weapon_name + 7);

					//if (weapon->m_iItemDefinitionIndex() == 64)
					//	strcpy(wpn_name, "revolver");
					if (ctx.m_settings.esp_weapon_type == 0) {

						const auto& name = weapon_names[weapon->m_iItemDefinitionIndex()];

						if (name != nullptr && wcslen(name) < 1)
						{
							auto st = std::string(weap_info->weapon_name + 7);

							if (weapon->m_iItemDefinitionIndex() == 64)
								st = _r8;

							std::wstring wstr(st.begin(), st.end());

							weapon_names[weapon->m_iItemDefinitionIndex()] = wstr.c_str();
						}

						//if (weapon->m_nFallbackPaintKit() > 0)
						//	strcat(wpn_name, get_name_by_paint_kit(weapon->m_iItemDefinitionIndex(), weapon->m_nFallbackPaintKit()));

						//std::string name = wpn_name;
						//std::transform(name.begin(), name.end(), name.begin(), std::toupper);

						if (name != nullptr)
						{
							auto wpn_name_size = Drawing::GetTextSize(F::ESPInfo, name);
							Drawing::DrawStringUnicode(F::ESPInfo, x + w / 2 - wpn_name_size.right / 2, y + h + 5, low_ammo ? Color::Red().alpha(min(0xC8, dormant_alpha[idx] - 10)) : Color::White().alpha(min(0xC8, dormant_alpha[idx] - 10)), false, name);
						}

					}
					else
					{
						const auto name = weapon->get_icon();

						if (name.size() > 0)
							Drawing::DrawString(F::Icons, x + w / 2, y + h + 5, Color::White().alpha(min(0xC8, dormant_alpha[idx] - 10)), FONT_CENTER, name.c_str());
					}
				}

				if ((weapon->m_zoomLevel() > 0/* || entity->m_bIsScoped()*/) && ctx.m_settings.esp_flags[2] && weapon->IsSniper())
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(77, 137, 234, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, _z.operator std::string().c_str()); // ping color : 77, 137, 234

				//if (ctx.can_hit[idx])
				//	Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("HIT"));
				if (weapon->IsGrenade() && weapon->m_bPinPulled() && !is_teammate && ctx.m_settings.esp_flags[4])
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(230, 0, 0, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, _pin.operator std::string().c_str());

				if (csgo.m_player_resource())
				{
					if (ctx.m_settings.esp_flags[5] && csgo.m_player_resource()->get_c4_carrier() == entity->entindex())
						Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(230, 0, 0, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, _c4.operator std::string().c_str());
					if (ctx.m_settings.esp_flags[5] && csgo.m_player_resource()->get_hostage_carrier() == entity->entindex())
						Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(230, 0, 0, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, _vip.operator std::string().c_str());
				}
			}

			//for (auto i = 0; i < entity->get_animation_layers_count(); i++)
			//{
			//	auto layer = &feature::resolver->player_records[idx].server_anim_layers[i];
			//	Drawing::DrawString(F::ESPInfo, 10, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] weight: %.2f", i, layer->m_flWeight);
			//	Drawing::DrawString(F::ESPInfo, 120, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] weightdeltar: %.2f", i, layer->m_flWeightDeltaRate);
			//	Drawing::DrawString(F::ESPInfo, 260, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] playback: %.6f", i, layer->m_flPlaybackRate);
			//	Drawing::DrawString(F::ESPInfo, 380, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] cycle: %.6f", i, layer->m_flCycle);
			//	Drawing::DrawString(F::ESPInfo, 460, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] sequence: %d", i, layer->m_nSequence);
			//	//Drawing::DrawString(F::OldESP, 460, 20 + 12 + i * 13, Color::White(200), FONT_LEFT, "[%d] layer animtime: %.6f", i, layer->m_flLayerAnimtime);
			//}

			//if (!is_teammate) {

			//	for (auto j = 0; j < 19; j++)
			//	{
			//		bool retard;
			//		if (!entity->get_multipoints(j, ctx.points[idx][j], entity->m_CachedBoneData().Base(), retard))
			//			continue;

			//		for (auto i = 0; i < ctx.points[idx][j].size(); i++)
			//		{
			//			auto kek = ctx.points[idx][j][i];

			//			if (kek.IsZero()) continue;

			//			if (Vector screen = Vector::Zero; Drawing::WorldToScreen(kek, screen) && !screen.IsZero())
			//				Drawing::DrawString(F::ESPInfo, screen.x, screen.y, Color::White(200), FONT_LEFT, "%d", i);
			//		}
			//		//auto kek = cheat::features::aimbot.get_hitbox(entity, j);

			//		//if (Vector screen = Vector::Zero; Drawing::WorldToScreen(kek, screen) && !screen.IsZero())
			//		//	Drawing::DrawString(F::OldESP, screen.x, screen.y, Color::White(200), FONT_LEFT, "%d", j);
			//	}
			//}

			//if (!is_teammate) {

			//	for (auto j = 0; j < 19; j++)
			//	{

			//		if (ctx.multi_points[idx][j].empty())
			//			continue;

			//		for (auto i = 0; i < ctx.multi_points[idx][j].size(); i++)
			//		{
			//			auto kek = ctx.multi_points[idx][j][i];

			//			if (kek.point.IsZero()) continue;

			//			if (Vector screen = Vector::Zero; Drawing::WorldToScreen(kek.point, screen) && !screen.IsZero())
			//				Drawing::DrawString(F::ESPInfo, screen.x, screen.y, Color::White(200), FONT_LEFT, "%d", kek.damage);
			//		}
			//		//auto kek = cheat::features::aimbot.get_hitbox(entity, j);

			//		//if (Vector screen = Vector::Zero; Drawing::WorldToScreen(kek, screen) && !screen.IsZero())
			//		//	Drawing::DrawString(F::OldESP, screen.x, screen.y, Color::White(200), FONT_LEFT, "%d", j);
			//	}
			//}

			//if (entity->get_animation_state() && !is_teammate)
			//	Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "%.1f|%.1f", r_log.stop_to_full_run_frac, entity->get_animation_state()->stop_to_full_run_frac);

			//const auto at_target = Math::normalize_angle(Math::CalcAngle(ctx.m_local()->m_vecOrigin(), entity->m_vecOrigin()).y);
			//const auto diff = Math::AngleDiff(at_target, entity->m_angEyeAngles().y);

			/*if (fabs(diff) <= 60.f && fabs(diff) >= 52.f)
			{
				resolver_info->anims_resolving = (diff < 0.f ? 1 : 2);
				resolver_info->did_anims_update = true;
			}*/

			//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, "%.1f", diff);

			if (!is_teammate && ctx.m_settings.esp_flags[3] && log != nullptr && log->records_count > 0) {

				if (log->tick_records[log->records_count & 63].valid) {
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 0, 0, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, sxor("%d"), log->tick_records[log->records_count & 63].simulation_time_delay);

					if (log->tick_records[log->records_count & 63].shot_this_tick)
						Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, sxor("SHOT"));

					if (log->tick_records[log->records_count & 63].animations_updated)
						Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, sxor("R"));

					/*float delta1 = abs(log->tick_records[log->records_count & 63].anim_layers[6].m_flPlaybackRate - r_log->resolver_layers[0][6].m_flPlaybackRate);
					float delta2 = abs(log->tick_records[log->records_count & 63].anim_layers[6].m_flPlaybackRate - r_log->resolver_layers[1][6].m_flPlaybackRate);
					float delta3 = abs(log->tick_records[log->records_count & 63].anim_layers[6].m_flPlaybackRate - r_log->resolver_layers[2][6].m_flPlaybackRate);

					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%.0f"), delta1 * 100000.f);
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%.0f"), delta2 * 100000.f);
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%.0f"), delta3 * 100000.f);*/

					if (entity->m_flDuckSpeed() >= 4.f && entity->m_flDuckAmount() > 0.f && entity->m_flDuckAmount() < 1 && entity->m_fFlags() & FL_ONGROUND) 
					{
						++r_log->duck_ticks;

						if (r_log->duck_ticks > 30 || r_log->duck_ticks > 4 && (entity->OBBMaxs().DistanceSquared(csgo.m_movement()->GetPlayerMaxs(false)) < 5.f))
							Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, entity->m_flDuckAmount() < 0.4f ? Color(255, 0, 0, min(0xC8, dormant_alpha[idx] - 10)) : Color(255, 255, 255, min(0xC8, dormant_alpha[idx] - 10)), FONT_LEFT, sxor("FD"));
					}
					else
						r_log->duck_ticks = 0;

					//auto drawAngleLine = [&](const Vector& origin, const Vector& w2sOrigin, const float& angle, const char* text, Color clr) {
					//	Vector forward = QAngle(0.0f, angle, 0.0f).ToVectors();
					//
					//	float AngleLinesLength = 30.0f;
					//
					//	Vector w2sReal;
					//	if (Drawing::WorldToScreen(origin + forward * AngleLinesLength, w2sReal)) {
					//		Drawing::DrawLine(w2sOrigin.x, w2sOrigin.y, w2sReal.x, w2sReal.y, clr);
					//		//Drawing::DrawRect(w2sReal.x - 5.0f, w2sReal.y - 5.0f, 10.0f, 10.0f, Color(50, 50, 50, 150));
					//		Drawing::DrawString(F::ESPInfo, w2sReal.x, w2sReal.y, clr, FONT_CENTER, text);
					//	}
					//};
					//Vector w2sOrigin;
					//if (Drawing::WorldToScreen(log->tick_records[log->records_count & 63].origin, w2sOrigin)) {
					//	//const auto curr_direction = ToDegrees(atan2(log->tick_records[log->records_count & 63].velocity.y, log->tick_records[log->records_count & 63].velocity.x));
					//
					//	//drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, curr_direction, "DIR", Color::Red());
					//	drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, log->tick_records[log->records_count & 63].animstate.m_abs_yaw, "abs", Color::Red());
					//	drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, log->tick_records[log->records_count & 63].animstate.m_eye_yaw, "eye", Color::LightBlue());
					//	drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, log->tick_records[(log->records_count-1) & 63].animstate.m_abs_yaw, "last abs", Color::White());
					//	drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, log->tick_records[log->records_count & 63].lower_body_yaw, "lby", Color::Green());
					//	drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, log->tick_records[log->records_count & 63].right_side, "r eye", Color::Grey());
					//	drawAngleLine(log->tick_records[log->records_count & 63].origin, w2sOrigin, log->tick_records[log->records_count & 63].left_side, "l eye", Color::Purple());
					//}
		
					//if (log->records_count > 1)
					//	Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%d"), TIME_TO_TICKS(log->tick_records[(log->records_count - 1) & 63].anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flCycle - log->tick_records[(log->records_count - 2) & 63].anim_layers[ANIMATION_LAYER_ALIVELOOP].m_flCycle));


					/*for (auto i = 0; i < 19; i++)
					{
						std::vector<Vector> points;

						bool center;
						auto p = entity->get_multipoints(i, points, entity->m_CachedBoneData().Base(), center);

						if (!p)
							continue;

						for (auto point : points)
						{
							Vector d;

							if (!Drawing::WorldToScreen(point, d))
								continue;

							Drawing::DrawString(F::ESPInfo, d.x, d.y, Color(255, 255, 255, dormant_alpha[idx] - 10), FONT_CENTER, "+");
						}

					}*/
					//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%.1f"), log->tick_records[(log->records_count - 1) & 63].velocity.Length2D());
					//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%.1f"), log->tick_records[(log->records_count - 1) & 63].max_current_speed);
					//
					//if (ctx.m_settings.esp_skeleton[lol]) {
					//
					//	//if (r_log.resolving_method == 1)
					//skeleton(entity, Color::Red().alpha(dormant_alpha[idx] - 55.f), log->tick_records[log->records_count & 63].leftmatrixes);
					//	//else
					//skeleton(entity, Color::Blue().alpha(dormant_alpha[idx] - 55.f), log->tick_records[log->records_count & 63].rightmatrixes);
					//skeleton(entity, Color::White().alpha(dormant_alpha[idx] - 55.f), log->tick_records[(log->records_count - 1) & 63].matrixes);
					//}
				}

				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 10), FONT_LEFT, sxor("%d [%d]"), min(64, log.records_count), log.records_count & 63);

				//Vector maxs = entity->GetCollideable()->OBBMaxs();
				//Vector pos, pos3d;
				//Vector top, top3d, maxs2d;
				//pos3d = entity->get_abs_origin();

				//static float max_maxs = -FLT_MAX;

				//if (max_maxs < maxs.z)
				//	max_maxs = maxs.z;

				//top3d = pos3d + Vector(0, 0, max_maxs);
				//
				//if (Drawing::WorldToScreen(pos3d, pos) && Drawing::WorldToScreen(top3d, top))
				//{
				//	int iMiddle = (pos.y - top.y) + 4;
				//	int iWidth = iMiddle / 3.5;
				//	int iCornerWidth = iWidth / 4;

				//	const auto _x = top.x - iWidth;
				//	const auto _y = top.y;
				//	const auto _w = iWidth * 2;
				//	const auto _h = iMiddle;

				//	static auto str = std::string(sxor("FD"));

				//	if (max_maxs > -FLT_MAX)
				//	{
				//		//const auto duck_per_frame = (log.tick_records[0]->duck_amt - log.tick_records[1]->duck_amt) / csgo.m_globals()->frametime;

				//		const auto cur_diff = max_maxs * min(1.f, log.tick_records[0]->duck_amt/* + duck_per_frame*/);

				//		const auto text_size = Drawing::GetTextSize(F::ESPInfo, str.c_str());
				//		//Drawing::DrawString(F::ESPInfo, x + w / 2.f, _y - 1, Color(30, 30, 30, csgo.m_client()->IsChatRaised() ? 10 : 250), FONT_CENTER, str);
				//		auto draw_factor = Math::clamp(abs(max_maxs / cur_diff), 0.f, 1.f);
				//		*(bool*)(uintptr_t(csgo.m_surface()) + 0x280) = true;
				//		int _1x, _1y, _1x1, _1y1;
				//		Drawing::GetDrawingArea(_1x, _1y, _1x1, _1y1);
				//		Drawing::LimitDrawingArea(x/ 2, _y - h / 2, int(text_size.right * 2) + x/2, (int)text_size.bottom * draw_factor + h / 2);
				//		Drawing::DrawString(F::ESPInfo, x + w / 2.f, _y - 1, Color::White(dormant_alpha[idx] - 10), FONT_CENTER, str.c_str());
				//		Drawing::LimitDrawingArea(_1x, _1y, _1x1, _1y1);
				//		*(bool*)(uintptr_t(csgo.m_surface()) + 0x280) = false;
				//	}
				//auto desync_multiplier = fmaxf(0.0f, fminf(log.tick_records[0].animstate.feet_speed, 1.0f))
				//	* (log.tick_records[0].animstate.stop_to_full_run_frac * -0.30000001f + -0.19999999f)
				//	+ 1.0f;

				//if (log.tick_records[0].animstate.duck_amt > 0.0f)
				//	desync_multiplier = desync_multiplier + (0.5f - desync_multiplier) * (log.tick_records[0].animstate.duck_amt * fmaxf(0.0f, fminf(1.0f, log.tick_records[0].animstate.feet_shit)));

				//if ((entity->get_abs_origin() + entity->GetCollideable()->OBBMaxs()).z < entity->get_bone_pos(8).z)
				//	Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, "broken");
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.f"), log.tick_records[0].velocity.z);
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.f"), log.tick_records[0].desync_delta);
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.2f"), log.tick_records[0].pose_paramaters[6]);
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.f"), log.tick_records[0].animstate.last_origin_z);
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.f"), feature::anti_aim->get_max_desync_delta(entity));
				
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.1f"), log.tick_records[0].animstate.duck_amt);
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, sxor("%.1f"), entity->m_flDuckAmount());
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, entity->m_bDucked() ? sxor("+") : sxor("-"));
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, entity->m_bDucking() ? sxor("+") : sxor("-"));

				//if (log.best_record != nullptr)
				//	skeleton(entity, Color::White().alpha(dormant_alpha[idx] - 55.f), r_log.resolving_method == 2 ? log.best_record->leftmatrixes : log.best_record->rightmatrixes);

				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(170, 255, 170, dormant_alpha[idx] - 55.f), FONT_LEFT, "%1.f", abs(Math::AngleDiff(r_log.simulated_animstates[1].eye_yaw , r_log.simulated_animstates[1].abs_yaw)));

				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, "%2.f", abs(r_log.simulated_animstates[1].stop_to_full_run_frac - r_log.simulated_animstates[2].stop_to_full_run_frac));
			

				/*const auto diff = Math::AngleDiff(log.tick_records[0].original_abs_yaw, log.tick_records[0].eye_angles.y);

				if (fabs(diff) > (feature::resolver->get_delta(entity->get_animation_state()) * 0.8f))
					Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(200, 0, 0, dormant_alpha[idx] - 55.f), FONT_LEFT, "OP");*/
				//
				//Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, "%.1f", diff);
			}
		}
		else
		{
			draw_items(entity);
		}
	}

	VIRTUALIZER_MUTATE_ONLY_END;
}

void c_visuals::draw_items(C_BasePlayer* m_entity)
{
	int x, y, w, h;
	static ConVar* max_bombtime = csgo.m_engine_cvars()->FindVar("mp_c4timer");
	static c_indicator bomb_indicator("BOMB", max_bombtime->GetFloat());
	static c_indicator defuse_indicator("DEFUSE", 10.f);

	static auto scale_bomb_damage = [](float flDamage, int armor_value) -> float
	{
		const float flArmorRatio = 0.5f;
		const float flArmorBonus = 0.5f;
		if (armor_value > 0) {
			float flNew = flDamage * flArmorRatio;
			float flArmor = (flDamage - flNew) * flArmorBonus;

			if (flArmor > static_cast<float>(armor_value)) {
				flArmor = static_cast<float>(armor_value) * (1.f / flArmorBonus);
				flNew = flDamage - flArmor;
			}

			flDamage = flNew;
		}
		return flDamage;
	};

	if (m_entity == nullptr)
		return;

	const auto& client_class = m_entity->GetClientClass();

	if (client_class == nullptr)
		return;

	Vector screen;
	const Vector origin = m_entity->get_abs_origin();

	ctx.is_local_defusing = false;

	const auto class_id = client_class->m_ClassID;

	if (!Drawing::WorldToScreen(origin, screen) && class_id != class_ids::CPlantedC4)
		return;

	switch (class_id)
	{
	case class_ids::CPlantedC4: //BOMB
	{
		if (!ctx.m_settings.esp_world_bomb || !m_entity->m_bBombTicking())
			return;

		auto bomb_blow_timer = m_entity->get_bomb_blow_timer();
		const auto bomb_defuse_timer = m_entity->get_bomb_defuse_timer();

		const auto can_defuse = (bomb_defuse_timer <= bomb_blow_timer && ctx.m_local()->m_iTeamNum() == 3) || (5.0 > bomb_blow_timer && ctx.m_local()->m_iTeamNum() == 2) || (bomb_defuse_timer <= bomb_blow_timer && ctx.m_local()->m_iTeamNum() != 2 && ctx.m_local()->m_iTeamNum() != 3);

		const auto is_defusing = bomb_defuse_timer > 0.0 && m_entity->m_hBombDefuser();

		if (ctx.m_local()->m_iHealth() > 0)
		{
			const float damagePercentage = 1;

			float flDamage = 500; // 500 - default, if radius is not written on the map https://i.imgur.com/mUSaTHj.png
			auto flBombRadius = flDamage * 3.5f;
			auto flDistanceToLocalPlayer = ((m_entity->get_abs_origin() + m_entity->m_vecViewOffset()) - (ctx.m_local()->get_abs_origin() + ctx.m_local()->m_vecViewOffset())).Length();//c4bomb origin - localplayer origin
			float fSigma = flBombRadius / 3;
			float fGaussianFalloff = exp(-flDistanceToLocalPlayer * flDistanceToLocalPlayer / (2 * fSigma * fSigma));
			auto flAdjustedDamage = flDamage * fGaussianFalloff * damagePercentage;

			flDamage = scale_bomb_damage(flAdjustedDamage, ctx.m_local()->m_ArmorValue());

			float dmg = 100 - (float(ctx.m_local()->m_iHealth()) - flDamage);

			if (dmg > 100)
				dmg = 100;

			if (dmg < 0)
				dmg = 0;

			int Red = dmg * 2.55;
			int Green = 255 - dmg * 2.55;

			if (Red > 255)
				Red = 255;
			if (Green > 255)
				Green = 255;

			if (flDamage > 1) {
				if (flDamage >= ctx.m_local()->m_iHealth())
					Drawing::DrawString(F::LBY, ctx.screen_size.x - (is_defusing ? 30 : 10), 150, Color(Red, Green, 0, 250), FONT_RIGHT, sxor("FATAL"));
				else
					Drawing::DrawString(F::LBY, ctx.screen_size.x - (is_defusing ? 30 : 10), 150, Color(Red, Green, 0, 250), FONT_RIGHT, sxor("-%.1fHP"), flDamage);
			}
		}

		if (bomb_blow_timer < 0)
			bomb_blow_timer = 0;

		if (bomb_blow_timer >= 0)
		{
			const auto max_bombtimer = max_bombtime->GetFloat();

			auto btime = bomb_blow_timer * (100 / max_bombtimer);

			if (btime > 100)
				btime = 100;
			else if (btime < 0)
				btime = 0;

			//int blow_percent = ctx.screen_size.y - (int)((98 * btime) / 100);
			const int b_red = 255 - (btime * 2.55);
			const int b_green = btime * 2.55;

			/*Drawing::DrawRect(10, ind_h + 15 + 20 * indicators, 100, 15, { 0, 0, 0, 130 });
			Drawing::DrawString(F::ESPInfo, 60, ind_h + 15 + 20 * indicators, can_defuse ? Color::Green(215) : Color::Red(215), FONT_CENTER, "BOMB");

			Drawing::DrawRect(11,
				ind_h + 25 + 20 * indicators++,
				(98.f * float(bomb_blow_timer / max_bombtimer)),
				4,
				{ b_red,b_green ,0,130 });*/

			auto bcolor = Color{ b_red, b_green, 0, 250 };

			if (btime > 70)
				bcolor = Color(123, 194, 21, 250);

			if (bomb_blow_timer > 1.f && ctx.m_settings.misc_visuals_indicators_2[3]) {
				bomb_indicator.draw(bomb_blow_timer + 1.0, bcolor, max_bombtimer);

				//if (bomb_indicator.get_max_value() != max_bombtimer)
				bomb_indicator.get_max_value(max_bombtimer);
			}

			if (Vector dummy = Vector::Zero; Drawing::WorldToScreen(m_entity->m_vecOrigin(), dummy) && !dummy.IsZero())
				Drawing::DrawString(F::ESPInfo, dummy.x, dummy.y, can_defuse ? Color::Green(250) : Color::Red(250), FONT_CENTER, sxor("BOMB: %.1f"), bomb_blow_timer);

			if (is_defusing)
			{
				/*const*/ C_BasePlayer* defuser = m_entity->m_hBombDefuser();
				const auto max_defuse_timer = (defuser && !defuser->IsDead() && defuser->m_bHasDefuser()) ? 5 : 10;

				ctx.is_local_defusing = defuser == ctx.m_local();

				auto dtime = bomb_defuse_timer * (100 / max_defuse_timer);

				if (dtime > 100)
					dtime = 100;
				else if (dtime < 0)
					dtime = 0;

				const int defuse_percent = ctx.screen_size.y - (int)((ctx.screen_size.y * dtime) / 100);
				const int d_red = 255 - (dtime * 2.55);
				const int d_green = dtime * 2.55;

				auto dcolor = Color{ d_red, d_green, 0, 250 };

				if (can_defuse)
					dcolor = Color(123, 194, 21, 250);

				if (ctx.m_settings.misc_visuals_indicators_2[4])
					defuse_indicator.draw(max_defuse_timer - bomb_defuse_timer, can_defuse ? Color(123, 194, 21, 250) : Color::Red(250), max_defuse_timer);

				Drawing::DrawRect(ctx.screen_size.x - 15, ctx.screen_size.y - defuse_percent, 15, ctx.screen_size.y, dcolor.alpha(200));
				Drawing::DrawRect(ctx.screen_size.x - 15, 0, 15, ctx.screen_size.y, { 0, 0, 0, 90 });

				/*Drawing::DrawRect(10, ind_h + 15 + 20 * indicators, 100, 15, { 0, 0, 0, 130 });
				Drawing::DrawString(F::ESPInfo, 60, ind_h + 15 + 20 * indicators, can_defuse ? Color::Green(215) : Color::Red(215), FONT_CENTER, "DEFUSE");

				Drawing::DrawRect(11,
					ind_h + 25 + 20 * indicators++,
					(98.f * float(bomb_defuse_timer / max_defuse_timer)),
					4,
					{ b_red, b_green, 0,130 });*/
			}
		}
		break;
	}
	case class_ids::CInferno:
	{
		const auto owner = m_entity->m_hOwnerEntity();

		const auto& eowner = csgo.m_entity_list()->GetClientEntityFromHandle(owner);

		if (!ctx.m_settings.esp_world_nades || !get_espbox(m_entity, x, y, w, h) || !eowner || !ctx.m_local() || (eowner->m_iTeamNum() == ctx.m_local()->m_iTeamNum() && eowner != ctx.m_local()) && *(float*)(uintptr_t(m_entity) + 0x20) > 0) return;

		const double spawn_time = *(float*)(uintptr_t(m_entity) + 0x20);
		const double factor = ((spawn_time + 7.031) - csgo.m_globals()->curtime) / 7.031;

		if (factor <= 0)
			break;

		const int red = max(min(255 * factor, 255), 0);
		const int green = max(min(255 * (1.0 - factor), 255), 0);

		static auto text_size = Drawing::GetTextSize(F::ESPInfo, sxor("inferno"));
		//Drawing::DrawRect(x - 50, y, 100, 15, { 0, 0, 0, 205 });
		Drawing::DrawRect(x - 49, y + 10, 98.f, 4, { 80, 80, 80, 125 });
		Drawing::DrawRect(x - 49, y + 10, 98.f * factor, 4, { red, green, 0, 250 });
		Drawing::DrawString(F::ESPInfo, x, y, { 255, 255, 255, 250 }, FONT_CENTER, sxor("inferno"));

		Drawing::DrawString(F::ESPInfo, x - 49.0 + 98.0 * factor, y + 8.0, { 255, 255, 255, 250 }, FONT_CENTER, "%.0f", (spawn_time + 7.031) - csgo.m_globals()->curtime);

		//if (ctx.m_local() && !feature::grenades->last_smoke_endpos.IsZero() && !ctx.m_local()->IsDead() && m_weapon() && m_weapon()->m_iItemDefinitionIndex() == WEAPON_SMOKEGRENADE && (m_entity->m_vecOrigin() - feature::grenades->last_smoke_endpos).LengthSquared() < (150.f * 150.f)) {
		//	Drawing::DrawString(F::ESPInfo, x - 49.0 + 98.0 * factor, y + 8.0 * 2, Color(123, 194, 21, 250), FONT_CENTER, "smoke");
		//	feature::grenades->smoke_molly = true;
		//}

		break;
	}
	case class_ids::CSmokeGrenadeProjectile:
	{
		const auto owner = m_entity->m_hOwnerEntity();

		const auto& eowner = csgo.m_entity_list()->GetClientEntityFromHandle(owner);

		if (!ctx.m_settings.esp_world_nades || !get_espbox(m_entity, x, y, w, h) || !eowner || !ctx.m_local()) return;

		/*if (cheat::Cvars.Visuals_Box.GetValue()) {
			Drawing::DrawOutlinedRect(x, y, w, h, Color::Green().alpha(200));
			Drawing::DrawOutlinedRect(x - 1, y - 1, w + 2, h + 2, Color(10, 10, 10, 150));
			Drawing::DrawOutlinedRect(x + 1, y + 1, w - 2, h - 2, Color(10, 10, 10, 150));
		}*/

		const float spawn_time = TICKS_TO_TIME(m_entity->m_nSmokeEffectTickBegin());
		const double factor = ((spawn_time + 18.041) - csgo.m_globals()->curtime) / 18.041;

		if (factor <= 0)
			break;

		const int red = max(min(255 * factor, 255), 0);
		const int green = max(min(255 * (1.0 - factor), 255), 0);

		static auto text_size = Drawing::GetTextSize(F::ESPInfo, sxor("smoke"));
		//Drawing::DrawRect(x - 50, y, 100, 15, { 0, 0, 0, 205 });
		if (spawn_time > 0.f) {
			Drawing::DrawRect(x - 49, y + 10, 98, 4, { 80, 80, 80, 125 });
			Drawing::DrawRect(x - 49, y + 10, 98 * factor, 4, { red, green, 0, 245 });
			Drawing::DrawString(F::ESPInfo, x - 49 + 98 * factor, y + 8, { 255, 255, 255, 250 }, FONT_CENTER, "%.0f", (spawn_time + 18.04125) - csgo.m_globals()->curtime);
		}
		Drawing::DrawString(F::ESPInfo, x, y, { 255, 255, 255, 250 }, FONT_CENTER, sxor("smoke"));

		break;
	}
	case class_ids::CC4:
	{
		const auto owner = m_entity->m_hOwnerEntity();

		if (!ctx.m_settings.esp_world_bomb || !get_espbox(m_entity, x, y, w, h) || owner != -1) return;

		Drawing::DrawString(F::ESPInfo, x + w / 2, y + h / 2, Color(150, 200, 60, 0xb4), FONT_CENTER, sxor("bomb"));

		break;
	}
	default:
	{
		const auto owner = m_entity->m_hOwnerEntity();

		if (ctx.m_settings.esp_world_weapons)
		{
			if (owner == -1)
			{
				/*const char* name = "";

				if (strstr(client_class->m_pNetworkName, "CWeapon"))
				{
					auto weapon = (C_WeaponCSBaseGun*)entity;

					name = "unknown";

					if (weapon->m_iItemDefinitionIndex() != 64 && weapon->GetCSWeaponData())
						name = (weapon->GetCSWeaponData()->weapon_name + 7);
					else
						name = "R8 Revolver";
				}
				else if (client_class->m_ClassID == class_ids::CAK47)
					name = "AK-47";
				else if (client_class->m_ClassID == class_ids::CDEagle)
					name = "Deagle";
				else
					continue;*/

				auto* const weapon = reinterpret_cast<C_WeaponCSBaseGun*>(m_entity);

				if (!weapon || !weapon->is_weapon())
					return;

				const auto& name = weapon_names[weapon->m_iItemDefinitionIndex()];

				auto* const wdata = weapon->GetCSWeaponData();

				if (name != nullptr && wcslen(name) < 1 && wdata)
				{
					auto st = std::string(wdata->weapon_name + 7);

					if (weapon->m_iItemDefinitionIndex() == 64)
						st = sxor("revolver");

					std::wstring wstr(st.begin(), st.end());

					weapon_names[weapon->m_iItemDefinitionIndex()] = wstr.c_str();
				}

				auto dist = ctx.m_local()->get_abs_origin().DistanceSquared(m_entity->m_vecOrigin()) * 0.01905f;

				if (ctx.m_local()->IsDead())
					dist = csgo.m_input()->m_vecCameraOffset.DistanceSquared(m_entity->m_vecOrigin()) * 0.01905f;

				const auto cl_dist = Math::clamp(dist - 500.f, 0, 510);
				const auto alpha = min(0xb4, 255 - (cl_dist / 2));

				if (alpha <= 0 || name == nullptr || wcslen(name) <= 1 || !get_espbox(m_entity, x, y, w, h)) return;

				/*if (ctx.m_settings.esp_world_weapons[0]) {
					Drawing::DrawOutlinedRect(x, y, w, h, ctx.m_settings.world_esp_color.alpha(210));
					Drawing::DrawOutlinedRect(x - 1, y - 1, w + 2, h + 2, Color(10, 10, 10, 150));
					Drawing::DrawOutlinedRect(x + 1, y + 1, w - 2, h - 2, Color(10, 10, 10, 150));
				}*/

				const auto text_size = Drawing::GetTextSize(F::ESPInfo, name);

				if (ctx.m_settings.esp_world_weapons)
					Drawing::DrawStringUnicode(F::ESPInfo, x + w / 2 - text_size.right / 2, y + h / 2.0f, ctx.m_settings.world_esp_color.alpha(min(ctx.m_settings.world_esp_color.a(), alpha)), false, name);

				if (ctx.m_settings.esp_world_weapons && !weapon->IsGrenade() && weapon->GetCSWeaponData() != nullptr)
				{
					auto clip = weapon->m_iClip1();
					auto maxclip = weapon->GetCSWeaponData()->max_clip;
					clip = std::clamp(clip, 0, 1000);
					maxclip = std::clamp(maxclip, 1, 1000);

					const auto nx = x + w / 2 - text_size.right / 2;

					w = text_size.right;

					const auto width = Math::clamp(w * clip / maxclip, 0, w);

					/*Draw(x - 26, y + 13, 52, 4, Color(static_cast<int>(alpha * 0.3f), 0, 0, 0), Color(static_cast<int>(alpha * 0.8f), 0, 0, 0));
					render::get().filled_box(x - 25, y + 14, width, 2, Color(static_cast<int>(alpha), 206, 147, 216));*/

					Drawing::DrawRect(nx, (y + h / 2.f) + text_size.bottom, w, 3, Color(80, 80, 80, alpha * 0.49));
					Drawing::DrawOutlinedRect(nx - 1,  y + h / 2.f - 1 + text_size.bottom, w + 2, 4, Color(10, 10, 10, (alpha * 0.5)));
					Drawing::DrawRect(nx,  (y + h / 2.f)  + text_size.bottom, width, 2, ctx.m_settings.world_esp_color.alpha(min(ctx.m_settings.world_esp_color.a(), alpha)));

					//Drawing::DrawString(F::ESPInfo, x + max(min((w - text_size.right - 2), width), (1 + text_size.right)) - text_size.right, y + h - 1, Color(255, 255, 255, dormant_alpha[idx] - 55.f), FONT_LEFT, ammostr);
				}
			}
		}

		if (ctx.m_settings.esp_world_nades)
		{
			//auto name = hash_32_fnv1a(client_class->m_pNetworkName, 8);

			if (class_id == class_ids::CMolotovProjectile
				|| class_id == class_ids::CDecoyProjectile
				|| class_id == class_ids::CSmokeGrenadeProjectile
				|| class_id == class_ids::CSnowballProjectile
				|| class_id == class_ids::CBreachChargeProjectile
				|| class_id == class_ids::CBumpMineProjectile
				|| class_id == class_ids::CBaseCSGrenadeProjectile
				|| class_id == class_ids::CSensorGrenadeProjectile)
			{

				if (!get_espbox(m_entity, x, y, w, h)) return;

				// draw decoy.
				if (class_id == class_ids::CDecoyProjectile)
					Drawing::DrawString(F::ESPInfo, x + w / 2, y + h, Color(255, 255, 255, 0xb4), FONT_CENTER, sxor("decoy"));
				else if (class_id == class_ids::CBaseCSGrenadeProjectile) {
					auto model = m_entity->GetModel();

					if (model) {
						std::string name{ model->szName };

						if (name.find(sxor("flashbang")) != std::string::npos)
							Drawing::DrawString(F::ESPInfo, x + w / 2, y + h, Color::LightBlue(0xb4), FONT_CENTER, sxor("flash"));
						else if (name.find(sxor("fraggrenade")) != std::string::npos && m_entity->m_nExplodeEffectTickBegin() < 1)
							Drawing::DrawString(F::ESPInfo, x + w / 2, y + h, Color(255, /*10*/0, /*10*/0, 0xb4), FONT_CENTER, sxor("frag"));
					}
				}
				else if (class_id == class_ids::CMolotovProjectile)
					Drawing::DrawString(F::ESPInfo, x + w / 2, y + h, Color(255, 130, 0, 0xb4), FONT_CENTER, sxor("molotov"));
			}
			break;
		}
	}
	}
}