#include "player.hpp"
#include "sound_parser.hpp"
#include "Visuals.hpp"
#include "source.hpp"
#include <algorithm>
#include "menu.hpp"
#include "hooked.hpp"

void c_dormant_esp::start()
{
	if (!csgo.m_engine()->IsInGame() || !ctx.m_local())
		return;

	// No active sounds.
	if (!m_utlCurSoundList.Count())
		return;

	c_player_records * mlag;

	// Accumulate sounds for esp correction
	for (int iter = 0; iter < m_utlCurSoundList.Count(); iter++)
	{
		SndInfo_t& sound = m_utlCurSoundList[iter];

		if (sound.m_nSoundSource == 0 || // World
			sound.m_nSoundSource > 64)   // Most likely invalid
			continue;

		C_BasePlayer* player = csgo.m_entity_list()->GetClientEntity(sound.m_nSoundSource);

		if (sound.m_nSoundSource > 0 && sound.m_nSoundSource < 64 && !player)
		{
			auto lag = &feature::lagcomp->records[sound.m_nSoundSource - 1];

			if (!lag || lag->userid < 1 || !lag->player || lag->saved_info.userid < 0)
				continue;

			lag->last_sound = csgo.m_globals()->realtime;

			goto pass;
		}

		if (!player || player == ctx.m_local() || sound.m_pOrigin->IsZero())
			continue;

		mlag = &feature::lagcomp->records[player->entindex() - 1];

		if (!mlag)
			continue;

		mlag->prev_last_sound = mlag->last_sound;
		mlag->last_sound = csgo.m_globals()->realtime;

		pass:

		if (!valid_sound(sound))
			continue;

		m_cSoundPlayers.push_back({ sound });

		setup_adjust(player, sound);

		/*if (player && player->m_iTeamNum() != ctx.m_local()->m_iTeamNum())
		{
			BeamInfo_t beimInfo;
			beimInfo.m_vecCenter = *sound.m_pOrigin;
			beimInfo.m_flStartRadius = 2.f;
			beimInfo.m_flEndRadius = 90.f;

			beimInfo.m_pszModelName = "sprites/laser.vmt";
			beimInfo.m_nModelIndex = csgo.m_model_info()->GetModelIndex("sprites/laser.vmt");

			beimInfo.m_nStartFrame = 0;
			beimInfo.m_flFrameRate = 1;

			beimInfo.m_flLife = 1.f;
			beimInfo.m_flWidth = 5;
			beimInfo.m_flAmplitude = 0;

			beimInfo.m_flSpeed = 5;
			beimInfo.m_nFlags = 0;
			beimInfo.m_flFadeLength = 0;

			beimInfo.m_vecCenter.z += beimInfo.m_flWidth * 0.5f;

			beimInfo.m_flRed = 255;
			beimInfo.m_flGreen = 223;
			beimInfo.m_flBlue = 0;

			beimInfo.m_flBrightness = 355.f;

			beimInfo.m_bRenderable = true;

			Beam_t* beam = csgo.m_beams()->CreateBeamRingPoint(beimInfo);

			if (beam)
				csgo.m_beams()->DrawBeam(beam);
		}*/
	}

	//SoundPlayer* prev_sound = nullptr;

	for (auto sound = m_cSoundPlayers.begin(); sound != m_cSoundPlayers.end();)
	{
		auto& m_sound = *sound;

		/*if (prev_sound && m_sound.m_iIndex == prev_sound->m_iIndex)
		{
			if (prev_sound->m_iReceiveTime > m_sound.m_iReceiveTime) {
				m_sound.m_iReceiveTime = -1;
			}
			else
				prev_sound->m_iReceiveTime = -1;
		}*/

		if (m_sound.m_iReceiveTime < 1 || (csgo.m_globals()->realtime - m_sound.m_iReceiveTime) >= 3.f ) {
			sound = m_cSoundPlayers.erase(sound);
			continue;
		}

		//prev_sound = &m_sound;

		++sound;
	}

	m_utlvecSoundBuffer = m_utlCurSoundList;
}

void c_dormant_esp::setup_adjust(C_BasePlayer* player, SndInfo_t& sound)
{
	if (m_cSoundPlayers.size() < 1)
		return;//how?

	Vector src3D, dst3D;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	if (player) {
		filter.pSkip = player;
	}
	src3D = (*sound.m_pOrigin) + Vector(0, 0, 1); // So they dont dig into ground incase shit happens /shrug
	dst3D = src3D - Vector(0, 0, 500);
	ray.Init(src3D, dst3D);

	csgo.m_engine_trace()->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	// step = (tr.fraction < 0.20)
	// shot = (tr.fraction > 0.20)
	// stand = (tr.fraction > 0.50)
	// crouch = (tr.fraction < 0.50)

	/* Corrects origin and important flags. */

	//csgo.m_debug_overlay()->AddLineOverlay(src3D, tr.endpos, 255, 0, 0, true, 2.f);
	//csgo.m_debug_overlay()->AddBoxOverlay(*sound.m_pOrigin, Vector(-2,-2,-2), Vector(2,2,2), Vector(0,0,0), 255, 0, 0, true, 2.f);

	// Player stuck, idk how this happened
	if (tr.allsolid)
	{
		m_cSoundPlayers.back().m_iReceiveTime = -1;
	}

	*sound.m_pOrigin = ((tr.fraction < 0.97f) ? tr.endpos : *sound.m_pOrigin);
	m_cSoundPlayers.back().m_nFlags = player ? player->m_fFlags() : 0;
	m_cSoundPlayers.back().m_nFlags |= (tr.fraction < 0.50f ? FL_DUCKING : 0) | (tr.fraction != 1.f ? FL_ONGROUND : 0);   // Turn flags on
	m_cSoundPlayers.back().m_nFlags &= (tr.fraction > 0.50f ? ~FL_DUCKING : 0) | (tr.fraction == 1.f ? ~FL_ONGROUND : 0); // Turn flags off
}

void c_dormant_esp::get_active_sounds()
{
	//nothing
	m_utlCurSoundList.RemoveAll();
	csgo.m_engine_sound()->GetActiveSounds(m_utlCurSoundList);
}

void c_dormant_esp::draw_sounds()
{
	// Adjusts player's origin and other vars so we can show full-ish esp.

	//if ((entity->entindex() - 1) < 0 || (entity->entindex() - 1) > 64)
	//	return;

	//std::vector<int> drawn{};

	for (auto sound = m_cSoundPlayers.begin(); sound != m_cSoundPlayers.end();)
	{
		auto& sound_player = *sound;

		if (sound_player.m_iIndex > 64 || sound_player.m_iIndex <= 0 || sound_player.m_vecOrigin.IsZero()) {
			++sound;
			continue;
		}

		bool sound_expired = fabs(csgo.m_globals()->realtime - sound_player.m_iReceiveTime) > 3.f;

		float meme = min(0xC8, ((3.f - (csgo.m_globals()->realtime - sound_player.m_iReceiveTime)) / 3.f) * 255.f);

		if (sound_expired) {
			++sound;
			sound_player.m_iReceiveTime = -1;
			continue;
		}

		auto ent = csgo.m_entity_list()->GetClientEntity(sound_player.m_iIndex);

		if (ent && !ent->IsDormant()) {
			sound_player.m_iReceiveTime = 0.f;
			++sound;
			continue;
		}

		auto lag = &feature::lagcomp->records[sound_player.m_iIndex - 1];

		//const auto origin = entity->get_abs_origin();
		//const auto flags = entity->m_fFlags();
		//const auto vecorigin = entity->m_vecOrigin();

		//entity->m_fFlags() = sound_player.m_nFlags;
		//entity->m_vecOrigin() = sound_player.m_vecOrigin;
		//entity->set_abs_origin(sound_player.m_vecOrigin);

		if (lag->userid <= 0 || lag->saved_info.userid < 0 || lag->previous_dormant >= sound_player.m_anim_time) {
			++sound;
			continue;
		}

		//if (std::find(drawn.begin(), drawn.end(), sound_player.m_iIndex) != drawn.end()) {
		//	sound_player.m_iReceiveTime = 0.f;
		//	continue;
		//}

		//if (!lag->render_origin.IsZero() && abs(sound_player.m_iReceiveTime - lag->previous_dormant) < 1.f)
		//{
		//	sound_player.m_vecOrigin = Math::interpolate(lag->render_origin, sound_player.m_vecOrigin, csgo.m_globals()->frametime * Math::clamp((csgo.m_globals()->realtime - sound_player.m_iReceiveTime), csgo.m_globals()->interval_per_tick, 1.f));
		//}
		//const auto o_origin = sound_player.m_vecOrigin;
		auto time_delta = (feature::lagcomp->get_interpolated_time() - sound_player.m_anim_time) / (sound_player.m_anim_time - lag->previous_dormant);

		lag->render_origin = (sound_player.m_vecOrigin - lag->render_origin).LengthSquared() < 1.f || (sound_player.m_vecOrigin - lag->render_origin).LengthSquared() > 4096.f ? sound_player.m_vecOrigin : Math::interpolate(lag->render_origin, sound_player.m_vecOrigin, Math::clamp(time_delta, 0, 1));//Math::interpolate(lag->prev_render_origin, sound_player.m_vecOrigin, Math::clamp((lag->last_sound - lag->prev_last_sound) * (1.f - min(1.f,csgo.m_globals()->realtime - lag->last_sound)), csgo.m_globals()->interval_per_tick, 1.f));
		lag->previous_dormant = sound_player.m_anim_time;
		lag->dormant_flags = sound_player.m_nFlags;

		lag->prev_render_origin = sound_player.m_vecOrigin;
		//drawn.push_back(sound_player.m_iIndex);

		//sound_player.m_vecOrigin = o_origin;

		//if (ctx.m_settings.esp_flags[lol][0] && entity->m_ArmorValue() > 0.f)
		//	Drawing::DrawString(F::ESPInfo, x + w + 3, y + right++ * size_info, Color(255, 255, 255, meme - 55.f), FONT_LEFT, (entity->m_bHasHelmet() ? "HK" : "K"));



		//entity->set_abs_origin(origin);
		//entity->m_vecOrigin() = vecorigin;
		//entity->m_fFlags() = flags;

		sound = m_cSoundPlayers.erase(sound);
	}
}

bool c_dormant_esp::valid_sound(SndInfo_t& sound)
{
	// Use only server dispatched sounds.
	if (!sound.m_bFromServer)
		return false;

	// We don't want the sound to keep following client's predicted origin.
	for (int iter = 0; iter < m_utlvecSoundBuffer.Count(); iter++)
	{
		SndInfo_t& cached_sound = m_utlvecSoundBuffer[iter];
		if (cached_sound.m_nGuid == sound.m_nGuid)
		{
			return false;
		}
	}

	return true;
}