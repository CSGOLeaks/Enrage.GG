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

#include <thread>
#include <cctype>

void c_visuals::draw_beam(Vector Start, Vector End, Color color, float Width)
{
	BeamInfo_t beamInfo;
	beamInfo.m_nType = TE_BEAMPOINTS;
	beamInfo.m_pszModelName = "sprites/purplelaser1.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = 4.f;

	beamInfo.m_flWidth = Width;
	beamInfo.m_flEndWidth = Width;

	beamInfo.m_flFadeLength = 0.0f;
	beamInfo.m_flAmplitude = 2.0f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.2f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

	beamInfo.m_flRed = color.r();
	beamInfo.m_flGreen = color.g();
	beamInfo.m_flBlue = color.b();

	beamInfo.m_vecStart = Start;
	beamInfo.m_vecEnd = End;

	auto myBeam = csgo.m_beams()->CreateBeamPoints(beamInfo);

	if (myBeam)
		csgo.m_beams()->DrawBeam(myBeam);
}

void c_visuals::render_tracers()
{
	if (!ctx.m_local() || ctx.m_local()->IsDead()) return;

	if (bullet_tracers.empty() || !ctx.m_settings.visuals_draw_local_beams) return;

	/*for (size_t i = 0; i < bullet_tracers.size(); i++)
	{*/
	/*for (auto it = bullet_tracers.begin(); it != bullet_tracers.end();)
	{
		const auto& current = *it;

		if (it == bullet_tracers.end())
			break;*/
	auto it = bullet_tracers.begin();
	while (it != bullet_tracers.end())
	{
		const auto& current = *it;
		auto next = (it + 1);

		if (next != bullet_tracers.end() && current.time == (*next).time/* || current.time == csgo.m_globals()->curtime*/) {
			it = bullet_tracers.erase(it);
			continue;
		}

		draw_beam(current.src - (current.is_local ? Vector(0.f,0.f,2.f) : Vector(0.f,0.f,0.f)), current.dst, current.color1, 2.0f);

		it = bullet_tracers.erase(it);
	}

	bullet_tracers.clear();
}