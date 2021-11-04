#include "hooked.hpp"
#include "displacement.hpp"
#include "player.hpp"
#include "weapon.hpp"
//#include "prediction.hpp"
//#include "movement.hpp"
//#include "anti_aimbot.hpp"
//#include <intrin.h>
//#include "menu.hpp"
//#include "player.hpp"
#include "prop_manager.hpp"
#include "autowall.hpp"

namespace Hooked
{
	void __fastcall OverrideView(void* ecx, void* edx, CViewSetup* vsView)
	{
		using Fn = void(__thiscall*)(void*, CViewSetup*);

		if (!csgo.m_engine()->IsInGame() || ctx.m_local() == nullptr || !ctx.m_local())
			return vmt.m_clientmode->VCall<Fn>(Index::IBaseClientDLL::OverrideView)(ecx, vsView);

		if (ctx.m_settings.misc_override_fov && (ctx.m_settings.visuals_no_first_scope || !ctx.m_local()->m_bIsScoped()))
			vsView->fov = ctx.m_settings.misc_override_fov_val + 90;

		if (ctx.m_settings.visuals_no_first_scope) {
			if (m_weapon() && m_weapon()->IsSniper() && ctx.m_settings.misc_override_fov/*ctx.m_local()->m_bIsScoped()*/)
			{
				vsView->fov = ctx.m_settings.misc_override_fov_val + 90;
				const auto zl = m_weapon()->m_zoomLevel();

				if (ctx.m_local()->m_bIsScoped()) {
					/*if (zl == 1)
						vsView->fov = 90.0f + (ctx.m_settings.visuals_no_first_scope ? ctx.m_settings.misc_override_fov_val : 0.f);
					else if (zl == 2)
						vsView->fov = 45.0f;*/
					vsView->fov = vsView->fov / zl;
				}

				/*if (zl <= 0 || !ctx.m_local()->m_bIsScoped()) {
					if (zoom_sensitivity_ratio_mouse->GetFloat() != defaultSens)
						zoom_sensitivity_ratio_mouse->SetValue(defaultSens);

					ctx.fov = vsView->fov;
				}
				else
				{
					if (ctx.m_settings.visuals_no_first_scope && zl != 2) {
						vsView->fov = ctx.fov;
						if (zoom_sensitivity_ratio_mouse->GetFloat() != 2.0f)
							zoom_sensitivity_ratio_mouse->SetValue(2.0f);
					}
				}

				if (local_weapon->m_zoomLevel() <= 0 || !ctx.m_local()->m_bIsScoped() || !local_weapon->IsSniper())
					vsView->fov += ctx.m_settings.misc_override_fov_val;*/
			}
		}

		if (ctx.fakeducking && !ctx.m_local()->IsDead())
			vsView->origin = ctx.m_local()->m_vecAbsOrigin() + Vector(0, 0, csgo.m_movement()->GetPlayerViewOffset(false).z);

		static int tp_anim = 0;
		ctx.in_tp = ctx.get_key_press(ctx.m_settings.visuals_tp_key);

		const auto is_in_tp = ctx.in_tp || tp_anim > 0;

		if (is_in_tp && ctx.m_settings.visuals_tp_force && !ctx.m_local()->IsDead())
		{
			if (!csgo.m_input()->is_in_tp())
				csgo.m_input()->m_fCameraInThirdPerson = true;

			if (csgo.m_input()->is_in_tp())
			{
				trace_t trace;
				auto angles = QAngle(0, 0, 0);

				csgo.m_engine()->GetViewAngles(angles);

				Vector camForward;
				Vector camAngles;

				/*static bool prev_value = ctx.in_tp;
				if (ctx.in_tp != prev_value)
				{
					tp_anim
				}*/

				// = Math::clamp(tp_anim, 0, 1);

				camAngles[0] = angles.x;
				camAngles[1] = angles.y;
				camAngles[2] = 0;

				Math::AngleVectors(camAngles, &camForward, 0, 0);

				camAngles[2] = max(30, min(500, ctx.m_settings.visuals_tp_dist));

				const auto eyeorigin = ctx.m_local()->get_abs_origin() + (ctx.fakeducking ? Vector(0, 0, csgo.m_movement()->GetPlayerViewOffset(false).z) : ctx.m_local()->m_vecViewOffset());

				const Vector vecCamOffset(eyeorigin - (camForward * camAngles[2])/* + (camRight * 1.f) + (camUp * 1.f)*/);

				Ray_t ray;
				ray.Init(eyeorigin, vecCamOffset, Vector(-16, -16, -16), Vector(16, 16, 16));

				uint32_t filter[4] = { feature::autowall->get_filter_simple_vtable(), uint32_t(ctx.m_local()), 0, 0 };
				csgo.m_engine_trace()->TraceRay(ray, MASK_NPCWORLDSTATIC, reinterpret_cast<CTraceFilter*>(filter), &trace);

				static float old_frac = 0;

				if (tp_anim == 1 && !ctx.in_tp) {
					csgo.m_input()->m_fCameraInThirdPerson = false;
					old_frac = 0;
				}
				//else if (tp_anim = 0 && ctx.in_tp)
				//	old_frac = 0;

				old_frac = Math::interpolate(old_frac, trace.fraction, (trace.fraction < old_frac && old_frac - trace.fraction > 0.1f) ? 0.125f : (csgo.m_globals()->frametime * 3.4f));

				camAngles[2] *= old_frac;

				if (ctx.in_tp)
					tp_anim = 1;
				else
					tp_anim = 0;

				csgo.m_input()->m_vecCameraOffset = camAngles;
			}
		}
		else {
			csgo.m_input()->m_fCameraInThirdPerson = false;
			csgo.m_input()->m_vecCameraOffset.z = 0;
		}

		//if (vmt.m_clientmode)
 		vmt.m_clientmode->VCall<Fn>(Index::IBaseClientDLL::OverrideView)(ecx, vsView);

		// remove scope edge blur.
		if (ctx.m_settings.visuals_no_scope) {
			if (ctx.m_local() && ctx.m_local()->m_bIsScoped())
				vsView->m_EdgeBlur = 0;
		}
	}

	bool __fastcall DoPostScreenEffects(void* clientmode, void*, int a1)
	{
		using Fn = bool(__thiscall*)(void*, int);
		static auto ofc =  vmt.m_clientmode->VCall<Fn>(44);

		//ctx.glow_rendering = true;

		//if (csgo.m_engine()->IsInGame() && ctx.m_local())
		//{
		//	ctx.m_settings.esp_glow[3]
		//}

		auto penis = ofc(clientmode, a1);
		//ctx.glow_rendering = false;

		return penis;
	}

	float __fastcall GetViewModelFOV(void* a1, int ecx)
	{
		using Fn = float(__thiscall*)(void*);
		static auto ofc = vmt.m_clientmode->VCall<Fn>(35);

		auto fov = ofc(a1);

		if (ctx.m_settings.misc_override_viewmodel && ctx.m_local() != nullptr)
		{
			auto weapon = m_weapon();
			if (weapon && (weapon->m_iItemDefinitionIndex() != 8 && weapon->m_iItemDefinitionIndex() != 39 || !ctx.m_local()->m_bIsScoped()))
				fov = ctx.m_settings.misc_override_viewmodel_val + fov;
		}

		return fov;
	}
}