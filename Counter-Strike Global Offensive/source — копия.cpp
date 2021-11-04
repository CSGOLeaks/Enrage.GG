#include "source.hpp"
#include "hooked.hpp"
#include "detours.h" 
#include "prop_manager.hpp"
#include "displacement.hpp"

#include "player.hpp"

#include "menu.hpp"
#include "anti_aimbot.hpp"
#include "misc.hpp"
#include "resolver.hpp"
#include "visuals.hpp"
#include "usercmd.hpp"
#include "lag_comp.hpp"
#include "chams.hpp"
#include "autowall.hpp"
#include "rage_aimbot.hpp"
#include "prediction.hpp"
#include "weapon.hpp"
#include "menu/menu/menu_v2.h"
#include "sound_parser.hpp"
#include "music_player.hpp"
//#include "threading/threading.h"
//#include "threading/shared_mutex.h"

//#include <thread>

constexpr auto _box_pttrn = LIT(("\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C"));

typedef void(__thiscall* StandardBlendingRules_t)(C_BasePlayer*, CStudioHdr*, Vector*, Quaternion*, float_t, int32_t);
typedef int(__thiscall* SetViewmodelOffsets_t)(void*, int, float, float, float);
typedef void(__vectorcall* CL_Move_t)(float, bool);
typedef const char*(__thiscall* GetForeignFallbackFontName_t)(void*);
typedef void(__thiscall* PhysicsSimulate_t)(void*);
typedef void(__thiscall* CalcAbsoluteVelocity_t)(C_BasePlayer*);
typedef const Vector&(__thiscall* GetRenderOrigin_t)(C_BaseAnimating*);
typedef void(__thiscall* ModifyEyePosition_t)(CCSGOPlayerAnimState*, Vector*);
typedef void(__thiscall* DoExtraBonesProcessing_t)(uintptr_t*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
typedef void(__thiscall* UpdateClientSideAnimations_t)(C_BasePlayer*);
typedef void(__thiscall* SetupVelocity_t)(CCSGOPlayerAnimState*);
typedef bool(__thiscall* isHLTV_t)(IVEngineClient*);
typedef int(__stdcall* IsBoxVisible_t)(const Vector&, const Vector&);
typedef bool(__thiscall* SetupBones_t)(void*, matrix3x4_t*, int, int, float);
typedef void(__thiscall* FireEvents_t)(IVEngineClient*);
typedef bool(__thiscall* OverrideConfig_t)(IMaterialSystem*, MaterialSystem_Config_t&, bool);
typedef bool(__thiscall* ShouldHitEntity_t)(void*, IHandleEntity*, int);
typedef void(__thiscall* ClipRayCollideable_t)(void*, const Ray_t&, uint32_t, ICollideable*, CGameTrace*);
using TraceRay_t = void(__thiscall*)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
using DrawSetColor_t = void(__thiscall*)(void*, int,int,int,int);

/*
new drawmodel

Mark llama, [19.11.19 06:43]
( 53 8B 5D 08 56 57 8B F9 89 7C 24 18 ) - 12
new drawmodel
*/

DWORD OriginalSetupBones;
DWORD OriginalUpdateClientSideAnimations;
DWORD OriginalSetupVelocity;
DWORD OriginalCalcAbsoluteVelocity;
DWORD OriginalStandardBlendingRules;
DWORD OriginalDoExtraBonesProcessing;
DWORD OriginalGetForeignFallbackFontName;
DWORD OriginalModifyEyePosition;
DWORD OriginalPhysicsSimulate;
DWORD OriginalCL_Move;
DWORD OriginalShouldHitEntity;
DWORD OriginalGetRenderOrigin;
DWORD OriginalSetViewmodelOffsets;

RecvVarProxyFn m_flSimulationTime;
RecvVarProxyFn m_flLowerBodyYawTarget;
RecvVarProxyFn m_flAbsYaw;
RecvVarProxyFn m_nSequence;


ClientEffectCallback oImpact;

//ShouldHitEntity_t oShouldHitEntity = nullptr;

c_csgo	  csgo;
c_context ctx;

struct lagcomp_mt
{
	lagcomp_mt() {  };
	bool job_done = false;
};

namespace Hooked
{
	void __fastcall FireEvents(IVEngineClient* _this, void* EDX)
	{
		if (csgo.m_engine()->IsInGame() && ctx.m_local() && !ctx.m_local()->IsDead()) {
			//static auto offs = Memory::Scan(sxor("engine.dll"), "8B BB ? ? ? ? 85 FF 0F 84");
			for (auto i = *(uintptr_t*)(uintptr_t(csgo.m_client_state()) + 0x4E6C); i; i = *(uintptr_t*)(i + 56))
				*(uintptr_t*)(i + 4) = 0;
		}

		Source::m_pEngineSwap->VCall<FireEvents_t>(59)(_this);
	}

	_declspec(noinline)void DoExtraBonesProcessing_Detour(uintptr_t* ecx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context)
	{
		auto v7 = (C_BasePlayer*)ecx;
		static auto old_tickcount = 0;

		if (ecx != (void*)4 && v7)
		{
			
			auto v8 = v7->get_animation_state();
			if (v8)
			{
				old_tickcount = *(int*)(uintptr_t(v8) + 8);
				*(int*)(uintptr_t(v8) + 8) = 0;
			}
		}
		((DoExtraBonesProcessing_t)OriginalDoExtraBonesProcessing)(ecx, hdr, pos, q, matrix, bone_computed, context);
		if (v7)
		{
			auto v8 = v7->get_animation_state();
			if (v8)
				*(int*)(uintptr_t(v8) + 8) = old_tickcount;
		}
	}

	void __fastcall DoExtraBonesProcessing(uintptr_t* ecx, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context)
	{
		DoExtraBonesProcessing_Detour(ecx, hdr, pos, q, matrix, bone_computed, context);
	}

	_declspec(noinline)const char* GetForeignFallbackFontName_Detour(uintptr_t* ecx)
	{
		if (strlen(Drawing::LastFontName) > 1)
			return Drawing::LastFontName;
		else
			return ((GetForeignFallbackFontName_t)OriginalGetForeignFallbackFontName)(ecx);
	}

	const char* __fastcall GetForeignFallbackFontName(uintptr_t* ecx, uint32_t)
	{
		return GetForeignFallbackFontName_Detour(ecx);
	}

	_declspec(noinline)void PhysicsSimulate_Detour(uintptr_t* ecx)
	{
		ctx.can_store_netvars = false;

		if (ecx && ctx.m_local() && !ctx.m_local()->IsDead() && (uintptr_t)ctx.m_local() == (uintptr_t)ecx) {
			const auto m_nSimulationTick = *(int*)(uintptr_t(ecx) + 0x2AC);
			//const auto needsprocessing = *(bool*)(uintptr_t(ecx) + 0x34F8);
			ctx.can_store_netvars = csgo.m_globals()->tickcount != m_nSimulationTick /*&& needsprocessing*/; //m_nSimulationTick && needsprocessing
		}
		else
			ctx.can_store_netvars = false;

		((PhysicsSimulate_t)OriginalPhysicsSimulate)(ecx);

		ctx.can_store_netvars = ctx.run_cmd_got_called = false;
	}

	void __fastcall PhysicsSimulate(uintptr_t* ecx, uint32_t)
	{
		PhysicsSimulate_Detour(ecx);
	}

	void __fastcall ClipRayCollideable(void* ecx, void* edx, const Ray_t& ray, uint32_t fMask, ICollideable* pCollide, CGameTrace* pTrace)
	{
		static auto ofc = Source::m_pEngineTraceSwap->VCall<ClipRayCollideable_t>(4);

		// extend the tracking
		auto old_max = pCollide->OBBMaxs().z;
		pCollide->OBBMaxs().z += 5; // if the player is holding a knife and ducking in air we can still trace to this faggot and hit him

		ofc(ecx, ray, fMask, pCollide, pTrace);

		// restore Z
		pCollide->OBBMaxs().z = old_max;
	}

	void __fastcall TraceRay(void* thisptr, void*, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)
	{
		static auto ofc = Source::m_pEngineTraceSwap->VCall<TraceRay_t>(5);

		//if (!ctx.in_hbp)
			return ofc(thisptr, ray, fMask, pTraceFilter, pTrace);

		ofc(thisptr, ray, fMask, pTraceFilter, pTrace);

		pTrace->surface.flags |= SURF_SKY;
	}

	_declspec(noinline)void ModifyEyePosition_Detour(CCSGOPlayerAnimState* ecx, Vector* pos)
	{
		//auto islocal = ecx != nullptr && ecx->ent == ctx.m_local() && !ctx.m_local()->IsDead();

		if (ctx.fix_modify_eye_pos && ecx != nullptr && !ctx.m_local()->IsDead())
			((ModifyEyePosition_t)OriginalModifyEyePosition)(ecx, pos);
	}

	void __fastcall ModifyEyePosition(CCSGOPlayerAnimState* ecx, void* edx, Vector* pos)
	{
		ModifyEyePosition_Detour(ecx, pos);
	}

	C_AnimationLayer previous[14];
	bool stored = false;

	_declspec(noinline)void crash_my_csgo(C_BasePlayer* ecx)
	{
		auto is_player = (csgo.m_engine() != nullptr && csgo.m_engine()->IsInGame() && (DWORD)ecx > 0x00001000 && ecx != nullptr && (*(int*)((DWORD)ecx + 0x64) - 1) <= 63) && !ecx->IsDead();
		auto is_local = is_player && ctx.m_local() == ecx;

		//C_AnimationLayer backup_layers[15];

		if (is_player)
		{
			if (ctx.updating_anims == true || ctx.m_local() == nullptr || ecx->m_iTeamNum() == ctx.m_local()->m_iTeamNum() && !is_local || ctx.m_local()->IsDead()) {
				((UpdateClientSideAnimations_t)OriginalUpdateClientSideAnimations)(ecx);

				return;
			}

			if (is_local)
			{
				C_AnimationLayer backuplayers[14];
				memcpy(backuplayers, ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());

				if (feature::usercmd.command_numbers.size() > 0)
				{
					const auto dword_3CF22C70 = csgo.m_globals()->realtime;
					const auto dword_3CF22C74 = csgo.m_globals()->curtime;
					const auto dword_3CF22C78 = csgo.m_globals()->frametime;
					const auto dword_3CF22C7C = csgo.m_globals()->absoluteframetime;
					const auto dword_3CF22C80 = csgo.m_globals()->interpolation_amount;
					const auto dword_3CF22C84 = csgo.m_globals()->framecount;
					const auto dword_3CF22C88 = csgo.m_globals()->tickcount;

					csgo.m_globals()->realtime = TICKS_TO_TIME(feature::anti_aim.sent_data.tickbase);
					csgo.m_globals()->curtime = TICKS_TO_TIME(feature::anti_aim.sent_data.tickbase);
					csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;
					csgo.m_globals()->absoluteframetime = csgo.m_globals()->interval_per_tick;
					csgo.m_globals()->framecount = feature::anti_aim.sent_data.tickbase;
					csgo.m_globals()->tickcount = feature::anti_aim.sent_data.tickbase;
					csgo.m_globals()->interpolation_amount = 0.0f;

					const auto backup_poses = ctx.m_local()->m_flPoseParameter();
					const auto backup_flags = ctx.m_local()->m_fFlags();
					const auto backup_duckamt = ctx.m_local()->m_flDuckAmount();
					const auto backup_lby = ctx.m_local()->m_flLowerBodyYawTarget();
					const auto backup_renderang = ctx.m_local()->get_render_angles();
					const auto backup_absvel = ctx.m_local()->m_vecAbsVelocity();

					//if (stored) {
						//ctx.m_local()->get_animation_state()->feet_cycle = previous[6].m_flCycle;

						//if (ctx.m_local()->m_fFlags() & FL_ONGROUND)
							ctx.m_local()->get_animation_state()->feet_rate = 0.f;
					//}

					//ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_REAL];

					//for (auto i = int(feature::usercmd.command_numbers.size() - 1); i >= 0; i--)
						//for (auto i = 0; i < int(feature::usercmd.command_numbers.size()); i++)
					//{
					const auto& c_cmd = feature::usercmd.command_numbers.back();
					const auto& ucmd = csgo.m_input()->GetUserCmd(c_cmd.command_number);

					const auto curtime = csgo.m_globals()->curtime;
					const auto framecount = csgo.m_globals()->framecount;
					const auto frametime = csgo.m_globals()->frametime;

					const auto viewangles = (ucmd == nullptr ? c_cmd.view_angles : ucmd->viewangles);

					//csgo.m_globals()->curtime = TICKS_TO_TIME(c_cmd.tickbase);
					csgo.m_globals()->frametime = csgo.m_globals()->interval_per_tick;

					ctx.m_local()->m_fFlags() = c_cmd.flags;
					ctx.m_local()->m_flDuckAmount() = c_cmd.duck_amount;
					ctx.m_local()->m_flLowerBodyYawTarget() = c_cmd.lby;
					//ctx.m_local()->m_vecAbsVelocity() = c_cmd.velocity;
					ctx.m_local()->m_vecAbsVelocity() = (c_cmd.velocity.Length2D() > 6.f ? c_cmd.velocity : Vector::Zero);

					if (ctx.m_local()->get_animation_state()->last_anim_upd_tick >= csgo.m_globals()->framecount)
						ctx.m_local()->get_animation_state()->last_anim_upd_tick = csgo.m_globals()->framecount - 1;

					//ctx.m_local()->get_animation_state()->feet_rate = 0;
					ctx.m_local()->get_animation_state()->ent = (void*)ctx.m_local();
					//ctx.m_local()->get_animation_state()->stop_to_full_run_frac = feature::anti_aim.stop_to_full_running_fraction;
					//ctx.m_local()->get_animation_state()->landing_duck = 0.f;

					//if (c_cmd.flags & FL_ONGROUND)
					//	ctx.m_local()->get_animation_state()->time_since_inair = 0.f;

					ctx.m_local()->get_animation_state()->abs_yaw = (ctx.m_ragebot_shot_nr > c_cmd.command_number && feature::usercmd.command_numbers.front().command_number <= ctx.m_ragebot_shot_nr ? feature::usercmd.command_numbers.front().view_angles.y : viewangles.y);
					ctx.m_local()->get_render_angles() = QAngle(ctx.fake_state.eye_pitch, ctx.fake_state.eye_yaw, viewangles.z);
					//if (i <= 1)
					//	ctx.m_local()->invalidate_anims();
					for (int i = 0; i < ctx.m_local()->m_anim_overlay().Count(); ++i) {
						ctx.m_local()->m_anim_overlay().Element(i).m_pOwner = (void*)ctx.m_local();
						//m_anim_overlay().Element(i).m_pStudioHdr = this->GetModelPtr();
					}

					// first bone snapshot fix
					//*(int*)((uintptr_t)this + 0x3AC0 + 0x4) = 1;
					// second bone snapshot fix
					//*(int*)((uintptr_t)this + 0x6F10 + 0x4) = 1;

					ctx.updating_anims = true;
					ctx.m_local()->client_side_animation() = true;

					// force animlayers to be correct
					/*for (int i = 0; i < get_animation_layers_count(); ++i) {
						animation_layer(i).m_pOwner = this;
						animation_layer(i).m_pStudioHdr = GetModelPtr();
					}*/

					((UpdateClientSideAnimations_t)OriginalUpdateClientSideAnimations)(ecx);

					/*if (this != ctx.m_local()) {
						this->client_side_animation() = false;
					}*/
					ctx.updating_anims = false;

					//ctx.m_local()->update_animstate(ctx.m_local()->get_animation_state(), QAngle(feature::anti_aim.visual_real_angle.x, ctx.m_local()->m_angEyeAngles().y, ctx.m_local()->m_angEyeAngles().z));

					csgo.m_globals()->curtime = curtime;
					csgo.m_globals()->frametime = frametime;
					ctx.m_local()->m_vecAbsVelocity() = backup_absvel;
					//}

					ctx.m_local()->m_fFlags() = backup_flags;
					ctx.m_local()->m_flDuckAmount() = backup_duckamt;
					ctx.m_local()->m_flLowerBodyYawTarget() = backup_lby;

					memcpy(ctx.local_layers[ANGLE_REAL], backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());
					ctx.angles[ANGLE_REAL] = ctx.m_local()->get_animation_state()->abs_yaw;
					ctx.poses[ANGLE_REAL] = ctx.m_local()->m_flPoseParameter();
					feature::usercmd.command_numbers.pop_back();

					ctx.m_local()->m_fFlags() = backup_flags;
					ctx.m_local()->m_flDuckAmount() = backup_duckamt;
					ctx.m_local()->m_flLowerBodyYawTarget() = backup_lby;
					ctx.m_local()->get_render_angles() = backup_renderang;
					ctx.m_local()->m_flPoseParameter() = backup_poses;
					ctx.m_local()->m_vecAbsVelocity() = backup_absvel;

					csgo.m_globals()->realtime = dword_3CF22C70;
					csgo.m_globals()->curtime = dword_3CF22C74;
					csgo.m_globals()->frametime = dword_3CF22C78;
					csgo.m_globals()->absoluteframetime = dword_3CF22C7C;
					csgo.m_globals()->interpolation_amount = dword_3CF22C80;
					csgo.m_globals()->framecount = dword_3CF22C84;
					csgo.m_globals()->tickcount = dword_3CF22C88;
					stored = true;
				}

				//memcpy(ctx.m_local()->animation_layers_ptr(), backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());


				static Vector old_origins[2] = {ctx.m_local()->get_abs_origin(), ctx.m_local()->get_abs_origin() };

				memcpy(backuplayers, ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());

				const auto backup_poses = ctx.m_local()->m_flPoseParameter();
				ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
				ctx.m_local()->force_bone_rebuild();
				ctx.m_local()->set_abs_angles(QAngle(0, ctx.angles[ANGLE_FAKE], 0));
				memcpy(ctx.m_local()->animation_layers_ptr(), ctx.local_layers[ANGLE_FAKE], 0x38 * ctx.m_local()->get_animation_layers_count());
				
				ctx.m_local()->SetupBonesEx();
				memcpy(ctx.fake_matrix, ctx.m_local()->m_CachedBoneData().Base(), ctx.m_local()->GetBoneCount() * sizeof(matrix3x4_t));
				ctx.m_local()->m_flPoseParameter() = backup_poses;
				feature::lagcomp.build_local_bones(ctx.m_local());
				memcpy(ctx.m_local()->animation_layers_ptr(), backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());
			//	//C_AnimationLayer backup_layers[15];
			//
			//	const auto backup_poses = ecx->m_flPoseParameter();
			//	//memcpy(backup_layers, ecx->animation_layers_ptr(), 0x38 * ecx->get_animation_layers_count());
			//
			//	ecx->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
			//	ecx->force_bone_rebuild();
			//	ecx->set_abs_angles(QAngle(0, ctx.angles[ANGLE_FAKE], 0));
			//	ecx->SetupBonesEx();
			//
			//	memcpy(ctx.fake_matrix, ecx->m_CachedBoneData().Base(), ecx->GetBoneCount() * sizeof(matrix3x4_t));
			//	//memcpy(ecx->animation_layers_ptr(), backup_layers, 0x38 * ecx->get_animation_layers_count());
			//
			//	ecx->m_flPoseParameter() = ctx.poses[ANGLE_REAL];
			//	feature::lagcomp.build_local_bones(ecx);
			//
			//	//memcpy(ecx->animation_layers_ptr(), backup_layers, 0x38 * ecx->get_animation_layers_count());
			//	ecx->m_flPoseParameter() = backup_poses;
			}
		}
		else
			((UpdateClientSideAnimations_t)OriginalUpdateClientSideAnimations)(ecx);
	}

	bool copy_bone_cache(C_BasePlayer* pl, int a1, matrix3x4_t*a2, int a3)
	{
		int v4; // ecx

		const auto log = feature::lagcomp.records[a1];

		if (log.tick_records.empty())
			return 0;

		v4 = 128;

		if (a3 <= 128)
			v4 = a3;

		memcpy(a2, pl->m_CachedBoneData().Base(), 48 * v4);

		//*&*a2 = *&*pl->m_CachedBoneData().Base();
		//*(matrix3x4_t**)a2 = (matrix3x4_t*)&*pl->m_CachedBoneData().Base();
		return 1;
	}

	_declspec(noinline)bool SetupBones_Detour(void* ECX, matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		if (!ECX)
			return ((SetupBones_t)OriginalSetupBones)(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

		auto v9 = (C_BasePlayer*)(uintptr_t(ECX) - 4);

		if ((DWORD)ECX == 0x4 || (*(int*)(uintptr_t(v9) + 0x64)) > 64 || v9->GetClientClass() != nullptr && v9->GetClientClass()->m_ClassID != class_ids::CCSPlayer)
			return ((SetupBones_t)OriginalSetupBones)(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

		const auto is_local = v9 == ctx.m_local() && v9 != nullptr;

		if (ctx.setup_bones || !ctx.m_local() || !is_local && v9->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || ctx.m_local()->IsDead())
		{
			/*for (auto i = 0; i < v9->GetModelPtr()->m_pStudioHdr->numbones; i++) {
				auto pBone = v9->GetModelPtr()->m_pStudioHdr->pBone(i);

				if (!pBone)
					continue;

				pBone->proctype &= ~5u;
			}*/

			return ((SetupBones_t)OriginalSetupBones)(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		}
		else 
		{
			if (v9 != ctx.m_local())
			{
				auto idx = *(int*)(uintptr_t(v9) + 0x64) - 1;

				auto v14 = 128;

				if (nMaxBones <= 128)
					v14 = nMaxBones;

				if (idx < 64 && pBoneToWorldOut && v14 > 0)
					return copy_bone_cache(v9, idx, pBoneToWorldOut, v14);
				
				return v9->GetBoneCount() > 0;
			}
			else
			{
				//auto idx = *(int*)(uintptr_t(v9) + 0x64) - 1;
				//const auto log = feature::lagcomp.records[idx - 1];

				if (pBoneToWorldOut)
				{
					auto v14 = 128;

					if (nMaxBones <= 128)
						v14 = nMaxBones;

					//if (v9->GetBoneCount() > v14)
					memcpy(pBoneToWorldOut, v9->m_CachedBoneData().Base(), sizeof(matrix3x4_t) * v14);
					//else
					//	return false;

					return true;
				}

				return true;
			}
		}
	}

	/*bool __fastcall net_showfragments_get_bool(void* pConVar, void* ebx)
	{
		static auto read_sub_channel_data_ret = reinterpret_cast<uintptr_t*>(Memory::Scan(sxor("engine.dll"), sxor("85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C")));
		static auto check_receiving_list_ret = reinterpret_cast<uintptr_t*>(Memory::Scan(sxor("engine.dll"), sxor("8B 1D ? ? ? ? 85 C0 74 16 FF B6")));

		if (!csgo.m_engine()->IsInGame())
			return Source::net_showfragmentsSwap->VCall<bool(__thiscall*)(void*)>(13)(pConVar);

		if (csgo.m_client_state() != nullptr) {
			static uint32_t last_fragment = 0;

			auto NetChannel = *reinterpret_cast<INetChannel**>(reinterpret_cast<uintptr_t>(csgo.m_client_state()) + 0x9C);

			if (_ReturnAddress() == reinterpret_cast<void*>(read_sub_channel_data_ret) && last_fragment > 0)
			{
				const auto data = &reinterpret_cast<uint32_t*>(NetChannel)[0x56];
				const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

				if (bytes_fragments == last_fragment)
				{
					auto& buffer = reinterpret_cast<uint32_t*>(data)[0x42];
					buffer = 0;
				}
			}

			if (_ReturnAddress() == check_receiving_list_ret)
			{
				const auto data = &reinterpret_cast<uint32_t*>(NetChannel)[0x56];
				const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

				last_fragment = bytes_fragments;
			}
		}

		return Source::net_showfragmentsSwap->VCall<bool(__thiscall*)(void*)>(13)(pConVar);
	}*/

	void __fastcall DrawSetColor(void* _this, void* edx, int r, int g, int b, int a)
	{
		static auto ofc = Source::m_pSurfaceSwap->VCall<DrawSetColor_t>(15);

		static const auto crosshair_color_return = Memory::Scan("client_panorama.dll", "FF 50 3C 80 7D 28") + 3;
		static const auto crosshair_outline_color_return = Memory::Scan("client_panorama.dll", "FF 50 3C F3 0F 10 4D ? 66 0F 6E C6") + 3;

		if (_ReturnAddress() == (void*)crosshair_color_return && ctx.autowall_crosshair >= 0)
		{
			auto color = ctx.autowall_crosshair == 0 ? Color::Red() : (ctx.autowall_crosshair == 2 ? Color::Green() : Color(0, 129, 255, 255));

			//if (ctx.autowall_crosshair)
			if (ctx.m_settings.visuals_penetration_crosshair_type == 1)
				return ofc(_this, color.r(), color.g(), color.b(), a);
		}

		if (_ReturnAddress() == (void*)crosshair_outline_color_return) {
			return ofc(_this, r, g, b, a);
		}

		ofc(_this, r, g, b, a);
	}

	void WriteUsercmd(void* buf, CUserCmd* incmd, CUserCmd* outcmd) {
		using WriteUsercmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);
		static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)Memory::Scan(sxor("client_panorama.dll"), sxor("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));

		__asm
		{
			mov     ecx, buf
			mov     edx, incmd
			push    outcmd
			call    WriteUsercmdF
			add     esp, 4
		}
	}

	bool __fastcall WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, void* buf, int from, int to, bool new_cmd)
	{
		using Fn = bool(__thiscall*)(void*, int, void *, int, int, bool);
		static auto ofc = Source::m_pClientSwap->VCall<Fn>(24);

		if (ctx.shift_amount <= 0)
			return ofc(ecx, slot, buf, from, to, new_cmd);

		if (from != -1)
			return true;

		int _from = -1;

		uintptr_t frame_ptr{};
		__asm {
			mov frame_ptr, ebp;
		}

		int* backup_commands = reinterpret_cast<int*>(frame_ptr + 0xFD8);
		int* new_commands = reinterpret_cast<int*>(frame_ptr + 0xFDC);
		int32_t newcmds = *new_commands;

		const auto shift_amt = ctx.shift_amount;

		ctx.shift_amount = 0;
		*backup_commands = 0;

		int choked_modifier = newcmds + shift_amt;

		if (choked_modifier > 62)
			choked_modifier = 62;

		*new_commands = choked_modifier;

		const int next_cmdnr = csgo.m_client_state()->m_iChockedCommands + csgo.m_client_state()->m_iLastOutgoingCommand + 1;
		int _to = next_cmdnr - newcmds + 1;
		if (_to <= next_cmdnr)
		{
			while (ofc(ecx, slot, buf, _from, _to, true))
			{
				_from = _to++;
				if (_to > next_cmdnr)
				{
					goto LABEL_11; // jump out of scope.
				}
			}
			return false;
		}
	LABEL_11:

		auto ucmd = csgo.m_input()->GetUserCmd(_from);
		if (!ucmd)
			return true;

		CUserCmd to_cmd{};
		CUserCmd from_cmd{};

		from_cmd = *ucmd;
		to_cmd = from_cmd;

		++to_cmd.command_number;
		to_cmd.tick_count += csgo.m_globals()->tickcount * 3;

		if (newcmds > choked_modifier)
			return true;

		for (int i = (choked_modifier - newcmds + 1); i > 0; --i)
		{
			WriteUsercmd( buf, &to_cmd, &from_cmd );

			from_cmd = to_cmd;
			++to_cmd.command_number;
			++to_cmd.tick_count;
		}

		return true;
	}

	bool __fastcall SetupBones(void* ECX, void* EDX, matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		return SetupBones_Detour(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	void __fastcall UpdateClientSideAnimation(C_BasePlayer* ecx, void* edx)
	{
		crash_my_csgo(ecx);
	}

	_declspec(noinline)void CalcAbsoluteVelocity_Detour(C_BasePlayer* ent, void* retaddr)
	{
		static const auto setup_velocity = (void*)(Memory::Scan(sxor("client_panorama.dll"), sxor("8B CE E8 ? ? ? ? F3 0F 10 A6")) + 7);
	
		if (retaddr == setup_velocity)
		{
			if (ent != nullptr && ctx.m_local() != nullptr)
			{
				auto v7 = *(int*)(uintptr_t(ent) + 0x64);
				if (v7 <= 64)
				{
					//ent->get_animation_state()->anim_update_delta = csgo.m_globals()->interval_per_tick;
	
					if (ent != ctx.m_local())
					{
						auto lag_data = &feature::resolver.player_records[v7 - 1];
						if (lag_data)
						{
							if (lag_data->force_velocity)
							{
								lag_data->did_force_velocity = true;
								//lag_data->old_velocity = ent->m_vecAbsVelocity();
								ent->m_vecAbsVelocity() = lag_data->new_velocity;
								return;
							}
	
							((CalcAbsoluteVelocity_t)OriginalCalcAbsoluteVelocity)(ent);
						}
					}
				}
			}
		}
	}

	void __fastcall CalcAbsoluteVelocity(C_BasePlayer* ecx, void* edx)
	{
		CalcAbsoluteVelocity_Detour(ecx, _ReturnAddress());
	}

	_declspec(noinline)const Vector& GetRenderOrigin_Detour(C_BaseAnimating* ent, void* retaddr)
	{
		auto v2 = *((DWORD*)ent + 0x27C) == 0;
		auto v3 = (char*)ent - 4;

		if (!v2 && *((DWORD*)v3 + 0x279) && (void*)((DWORD)ent + 0x9F0) == nullptr) {
			//std::cout << "GetRenderOrigin crash bypassed." << std::endl;
			return Vector(0, 0, 0);
		}

		return ((GetRenderOrigin_t)OriginalGetRenderOrigin)(ent);
	}

	const Vector& __fastcall GetRenderOrigin(C_BaseAnimating* ecx, void* edx)
	{
		return GetRenderOrigin_Detour(ecx, _ReturnAddress());
	}

	//float last_angle = FLT_MAX;

	_declspec(noinline)void SetupVelocity_Detour(CCSGOPlayerAnimState* state)
	{
		if (state != nullptr) 
		{
			auto ent = (C_BasePlayer*)state->ent;

			if (ent == nullptr || *(void**)ent == nullptr || ent == ctx.m_local())
			{
				((SetupVelocity_t)OriginalSetupVelocity)(state);
			}
			else {
				auto v7 = *(int*)(uintptr_t(ent) + 0x64);

				//if (v7 <= 64)
				//	last_angle = ent->m_flLowerBodyYawTarget();

				if (ent != ctx.m_local())
				{
					//if (v7 <= 64)
					//	ent->get_animation_state()->anim_update_delta = csgo.m_globals()->interval_per_tick;

					((SetupVelocity_t)OriginalSetupVelocity)(state);

					auto v7 = *(int*)(uintptr_t(ent) + 0x64);

					if (v7 <= 64)
					{
						//auto lag_data = &feature::resolver.player_records[v7 - 1];
						//if (lag_data)
						//{
							//ent->get_animation_state()->anim_update_delta = csgo.m_globals()->interval_per_tick;
							//ent->m_vecAbsVelocity() = ent->m_vecVelocity();
							state->speed_up = ent->m_vecAbsVelocity().z;

							//if (lag_data->force_velocity/* && !lag_data->old_velocity.IsZero()*/)
							//{
							//	lag_data->did_force_velocity = false;
							//	ent->m_vecAbsVelocity() = lag_data->old_velocity;
							//	ent->m_iEFlags() = lag_data->prev_eflags;
							//	lag_data->did_fill_velocity = false;
							//}
						//}
					}
				}
				else
				{
					const float lol = ent->get_animation_state()->anim_update_delta;
					const auto eflags = ent->m_iEFlags();
					//const auto absvelocity = ent->m_vecAbsVelocity();
					ent->m_iEFlags() &= ~EFL_DIRTY_ABSVELOCITY;

					//ent->m_vecAbsVelocity() = ent->m_vecVelocity();

					//if (ctx.is_updating_fake)
						ent->get_animation_state()->anim_update_delta = csgo.m_globals()->curtime - csgo.m_globals()->interval_per_tick;

					((SetupVelocity_t)OriginalSetupVelocity)(state);

					//state->speed_up = ent->m_vecAbsVelocity().z;

					//ent->m_vecAbsVelocity() = absvelocity;
					ent->m_iEFlags() = eflags;

					//if (ctx.is_updating_fake)
						ent->get_animation_state()->anim_update_delta = lol;
				}

				//last_angle = FLT_MAX;
			}
		}
	}

	void __fastcall SetupVelocity(CCSGOPlayerAnimState* a1, void* edx)
	{
		SetupVelocity_Detour(a1);
	}

	_declspec(noinline)void StandardBlendingRules_Detour(C_BasePlayer* ent, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int32_t bonemask)
	{
		if (ent != nullptr && *(int*)(uintptr_t(ent) + 0x64) <= 64)
		{
			auto boneMask = bonemask;

			if (ctx.m_settings.aimbot_enabled && ctx.m_settings.aimbot_position_adjustment && ent != ctx.m_local() && ctx.m_local() != nullptr && ent->m_iTeamNum() != ctx.m_local()->m_iTeamNum() && !ctx.m_local()->IsDead())
				boneMask = 0x100;

			((StandardBlendingRules_t)OriginalStandardBlendingRules)(ent, hdr, pos, q, curtime, boneMask);

			if (*(int*)(uintptr_t(ent) + 0xF0) & 8)
				*(int*)(uintptr_t(ent) + 0xF0) &= ~8;
		}
		else
		{
			((StandardBlendingRules_t)OriginalStandardBlendingRules)(ent, hdr, pos, q, curtime, bonemask);
		}
	}

	void __fastcall StandardBlendingRules(C_BasePlayer* a1, void* a2, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int32_t boneMask)
	{
		StandardBlendingRules_Detour(a1, hdr, pos, q, curtime, boneMask);
	}
	
	_declspec(noinline)int SetViewmodelOffsets_Detour(void* ent, int something, float x, float y, float z)
	{
		if (ctx.m_settings.visuals_viewmodel_control[0] != 0 || ctx.m_settings.visuals_viewmodel_control[1] != 0 || ctx.m_settings.visuals_viewmodel_control[2] != 0)
		{
			x = ctx.m_settings.visuals_viewmodel_control[0];
			y = ctx.m_settings.visuals_viewmodel_control[1];
			z = ctx.m_settings.visuals_viewmodel_control[2];
		}

		return ((SetViewmodelOffsets_t)OriginalSetViewmodelOffsets)(ent, something, x, y, z);
	}

	int __fastcall SetViewmodelOffsets(void* ecx, void* edx, int something, float x, float y, float z)
	{
		return SetViewmodelOffsets_Detour(ecx, something, x, y, z);
	}

	bool __fastcall IsHLTV(IVEngineClient* _this, void* EDX)
	{
		/*
		C_CSPlayer::AccumulateLayers
		sub_1037A9B0+6    8B 0D B4 13 15 15                 mov     ecx, dword_151513B4
		sub_1037A9B0+C    8B 01                             mov     eax, [ecx]
		sub_1037A9B0+E    8B 80 74 01 00 00                 mov     eax, [eax+174h]
		sub_1037A9B0+14   FF D0                             call    eax
		sub_1037A9B0+16   84 C0                             test    al, al
		sub_1037A9B0+18   75 0D                             jnz     short loc_1037A9D7
		sub_1037A9B0+1A   F6 87 28 0A 00 00+                test    byte ptr [edi+0A28h], 0Ah ; ent check here, whatever
		sub_1037A9B0+21   0F 85 F1 00 00 00                 jnz     loc_1037AAC8
		*/

		if (!Source::m_pEngineSwap || (DWORD)_this < 0x20000)
			return false;

		auto ofc = Source::m_pEngineSwap->VCall<isHLTV_t>(93);

		static const auto accumulate_layers_call = (void*)Memory::Scan(sxor("client_panorama.dll"), sxor("84 C0 75 0D F6 87"));
		static const auto setup_velocity = (void*)Memory::Scan(sxor("client_panorama.dll"), sxor("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80"));

		if (!ctx.m_settings.aimbot_position_adjustment)
			return ofc(_this);

		if (!ofc(_this) && (_ReturnAddress() == accumulate_layers_call))// || ctx.updating_anims && _ReturnAddress() == setup_velocity))
			return true;

		return ofc(_this);
	}

	//F3 0F 59 05 A0 CB C3 10 0F 28 E2

	/*bool can_shift_tickbase()
	{
		ctx.ticks_allowed++;

		return (ctx.m_settings.aimbot_tickbase_exploit == 0 || ctx.ticks_allowed >= 13);
	}*/

	_declspec(noinline)void CL_Move_Detour(float accumulated_extra_samples, bool bFinalTick)
	{
		//static bool speedhacking = false;

		//if (speedhacking)
		//	return ((CL_Move_t)OriginalCL_Move)(accumulated_extra_samples, bFinalTick);

		if (csgo.m_engine()->IsInGame() && csgo.m_game_rules() && ctx.m_local() && !ctx.m_local()->IsDead()/* && ctx.m_settings.aimbot_tickbase_exploit != 0*/)
			ctx.ticks_allowed++;
		else
			ctx.ticks_allowed = 0;

		if (ctx.speed_hack > 0)
		{
			ctx.last_speedhack_time = csgo.m_globals()->realtime;

			for (auto i = 0; i <= ctx.speed_hack; i++) {

				ctx.allow_shooting = i + csgo.m_client_state()->m_iLastOutgoingCommand + 2;

				ctx.speed_hacking = true;
				((CL_Move_t)OriginalCL_Move)(accumulated_extra_samples, bFinalTick);
				ctx.speed_hacking = false;
			}

			csgo.m_prediction()->m_nCommandsPredicted = 0;
			*(int*)((DWORD)csgo.m_prediction() + 0xC) = -1;

			ctx.speed_hack = 0;
			return;
		}
		else
			ctx.allow_shooting = 0;

		((CL_Move_t)OriginalCL_Move)(accumulated_extra_samples, bFinalTick);
	}

	void __vectorcall CL_Move(float accumulated_extra_samples, bool bFinalTick)
	{
		return CL_Move_Detour(accumulated_extra_samples, bFinalTick);
	}

	void m_flSimulationTimeHook(const CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		//C_BasePlayer *pEntity = (C_BasePlayer *)pStruct;

		if (pData->m_Value.m_Int)
			m_flSimulationTime(pData, pStruct, pOut);
	}

	void m_flAbsYawHook(const CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		static auto m_hPlayer = Engine::PropManager::Instance()->GetOffset(sxor("DT_CSRagdoll"), sxor("m_hPlayer"));
		auto player_handle = (CBaseHandle*)((DWORD)pStruct + m_hPlayer);
		player_info info;
		const auto abs_yaw = Math::normalize_angle(pData->m_Value.m_Float);

		if (*player_handle != 0xFFFFFFFF && *player_handle != -1 && ctx.m_local() != nullptr)
		{
			auto hplayer = (C_BasePlayer*)csgo.m_entity_list()->GetClientEntityFromHandle(*player_handle);

			if (hplayer && hplayer->GetIClientEntity() != nullptr)
			{
				auto player = (C_BasePlayer*)hplayer->GetIClientEntity()->GetBaseEntity();
				if (player != nullptr && player->entindex() < 64 && player != ctx.m_local() && !ctx.m_local()->IsDead() && player->m_iTeamNum() != ctx.m_local()->m_iTeamNum())
				{
					auto r_log = &feature::resolver.player_records[player->entindex() - 1];

					if (!r_log->did_store_abs_yaw && player->IsDead()/*&& r_log->resolving_method > 0*/ && csgo.m_engine()->GetPlayerInfo(player->entindex(), &info))
					{
						const auto delta = Math::AngleDiff(abs_yaw, player->m_angEyeAngles().y);
						const auto r_method = delta > 0.f ? 1 : 2;

						if (abs(delta) > 20.f && abs(delta) < 65.f) {
							r_log->last_abs_yaw_side = r_method;
							//if (r_method == r_log->anims_resolving)
							//	prefer anims over other ones then? 


							//_events.push_back(_event(std::string(sxor("server update arrived (") + std::string(info.name) + sxor(").")), std::string("server-side abs_yaw arrived [" + std::to_string(abs(delta)) + "]:[" + std::to_string(abs(r_log->server_anim_layers[6].m_flWeight)) + "]:[r" + std::to_string(r_method) + "]:[" + std::to_string(r_log->resolving_method) + "]")));
						}
						else if (abs(delta) <= 20.f && r_log->last_simtime > 0.f)
							r_log->last_abs_yaw_side = 0;

						r_log->did_store_abs_yaw = true;
					}
				}
			}
		}

		m_flAbsYaw(pData, pStruct, pOut);
	}

	/*void m_flLowerBodyYawTargetHook(const CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		C_BasePlayer* m_player = (C_BasePlayer*)pStruct;

		if (m_player == ctx.m_local())
		{
			auto new_lby = pData->m_Value.m_Float;
			if (ctx.m_local()->m_flLowerBodyYawTarget() != new_lby)
			{
				auto animstate = ctx.m_local()->get_animation_state();
				if (animstate)
				{
					auto v15 = 0.f;

					if (animstate->speed_2d <= 0.1f)
						v15 = 1.1f;
					else
						v15 = 0.22f;

					feature::anti_aim.lby_timer = m_player->m_flSimulationTime() + v15;
				}
			}
		}

		m_flLowerBodyYawTarget(pData, pStruct, pOut);
	}*/

	int	__stdcall IsBoxVisible(const Vector& mins, const Vector& maxs)
	{
		static auto ofc = Source::m_pEngineSwap->VCall<IsBoxVisible_t>(32);

		if (!memcmp(_ReturnAddress(), _box_pttrn.operator std::string().c_str(), 10))
			return 1;

		return ofc(mins, maxs);
	}

	bool __fastcall OverrideConfig(IMaterialSystem* ecx, void* edx, MaterialSystem_Config_t& config, bool bForceUpdate)
	{
		static auto ofc = Source::m_pMaterialSystemSwap->VCall<OverrideConfig_t>(21);

		if (ctx.m_settings.misc_visuals_world_modulation[0])
			config.m_nFullbright = true;

		return ofc(ecx, config, bForceUpdate);
	}

	_declspec(noinline)bool ShouldHitEntity_Detour(void* pThis, IHandleEntity* pHandleEntity, int contentsMask)
	{
		if ((DWORD)pHandleEntity == 0x7f7fffff || (DWORD)pHandleEntity < 0x20000)
			return false;

		auto result = ((ShouldHitEntity_t)OriginalShouldHitEntity)(pThis, pHandleEntity, contentsMask);

		if (result && csgo.m_static_prop()->IsStaticProp(pHandleEntity))
			return false;

		return result;
	}

	bool __fastcall ShouldHitEntity(void* pThis, void* edx, IHandleEntity* pHandleEntity, int contentsMask)
	{
		return ShouldHitEntity_Detour(pThis, pHandleEntity, contentsMask);
	}

	bool __fastcall IsConnected(void* ecx, void* edx)
	{
		/*string: "IsLoadoutAllowed"
		- follow up v8::FunctionTemplate::New function
		- inside it go to second function that is being called after "if" statement.
		- after that u need to open first function that is inside it. [before (*(int (**)(void))(*(_DWORD *)dword_152350E4 + 516))();]
		*/
		/*
		.text:103A2110 57                          push    edi
		.text:103A2111 8B F9                       mov     edi, ecx
		.text:103A2113 8B 0D AC E5+                mov     ecx, dword_14F8E5AC
		.text:103A2119 8B 01                       mov     eax, [ecx]
		.text:103A211B 8B 40 6C                    mov     eax, [eax+6Ch]
		.text:103A211E FF D0                       call    eax             ; Indirect Call Near Procedure
		.text:103A2120 84 C0                       test    al, al          ; Logical Compare <-
		.text:103A2122 75 04                       jnz     short loc_103A2128 ; Jump if Not Zero (ZF=0)
		.text:103A2124 B0 01                       mov     al, 1
		.text:103A2126 5F                          pop     edi
		*/

		using Fn = bool(__thiscall*)(void* ecx);
		static auto ofc = Source::m_pEngineSwap->VCall<Fn>(27);

		static void* is_loadout_allowed = (void*)(Memory::Scan(sxor("client_panorama.dll"), sxor("84 C0 75 04 B0 01 5F")));

		if (ctx.m_settings.misc_unlock_inventory && csgo.m_engine()->IsInGame() && _ReturnAddress() == is_loadout_allowed)
			return false;

		return ofc(ecx);
	}

	void impact_callback(const CEffectData& data)
	{
		auto org = data.origin;

		if (ctx.m_settings.visuals_draw_local_impacts && !org.IsZero() && ctx.m_local() && fabs(ctx.last_shot_time_clientside - csgo.m_globals()->realtime) <= csgo.m_globals()->interval_per_tick)
			csgo.m_debug_overlay()->AddBoxOverlay(data.origin, Vector(-2.f, -2.f, -2.f), Vector(2.f, 2.f, 2.f), Vector(0.f, 0.f, 0.f), 255, 0, 0, 127, 4.f);

		oImpact(data);
	}

	void fill_players_list(void* _data) {
		lagcomp_mt* data = (lagcomp_mt*)_data;

		data->job_done = false;

		if (!csgo.m_engine() || !csgo.m_engine()->IsInGame()) {
			ctx.m_player_entities.clear();
			return;
		}

		if (csgo.m_client_state()->m_iDeltaTick < 0)
			ctx.m_player_entities.clear();

		if (ctx.m_player_entities.empty())
		{
			for (auto idx = 1; idx < 64; idx++)
			{
				auto entity = (C_BasePlayer*)csgo.m_entity_list()->GetClientEntity(idx);

				if (!entity ||
					!entity->IsPlayer() ||
					entity->IsDormant() ||
					!entity->GetClientClass() ||
					entity->m_iHealth() <= 0)
				{
					ctx.m_player_entities.push_back(nullptr);
					continue;
				}

				ctx.m_player_entities.push_back(entity);
			}
		}
		else
		{
			for (auto idx = 0; idx < 64; idx++)
			{
				auto entity = (C_BasePlayer*)csgo.m_entity_list()->GetClientEntity(idx);

				if (!entity ||
					!entity->GetClientClass() ||
					!entity->IsPlayer() ||
					entity->IsDormant() ||
					entity->m_iHealth() <= 0)
				{
					if (ctx.m_player_entities.size() >= (idx-1) && ctx.m_player_entities[idx-1] == entity)
						ctx.m_player_entities[idx-1] = nullptr;
					
					continue;
				}

				if ((ctx.m_player_entities.size()-1) <= idx) {
					ctx.m_player_entities.push_back(entity);
					continue;
				}

				ctx.m_player_entities[idx-1] = entity;
			}
		}

		data->job_done = true;
	};

	void find_player_who_can_autowall(void* _data) {
		lagcomp_mt* data = (lagcomp_mt*)_data;

		data->job_done = false;

		if (!csgo.m_engine() || !csgo.m_engine()->IsInGame()) {
			ctx.m_player_entities.clear();
			return;
		}

		if (ctx.m_player_entities.empty() || ctx.m_local() == nullptr || ctx.m_local()->IsDead() || ctx.m_local()->IsDormant())
			return;

		std::vector<Vector> points = {};

		for (auto& player : ctx.m_player_entities)
		{
			if (!player ||
				player == ctx.m_local() || 
				!player->IsPlayer() ||
				player->IsDormant() ||
				!player->GetClientClass() ||
				ctx.m_local()->m_CachedBoneData().Base() == nullptr ||
				player->get_weapon() == nullptr ||
				player->m_iTeamNum() == ctx.m_local()->m_iTeamNum() ||
				player->m_iHealth() <= 0)
			{
				player = nullptr;
				continue;
			}
			else
				ctx.can_hit[player->entindex() - 1] = false;

			const model_t* model = ctx.m_local()->GetModel();

			if (!model)
				return;

			studiohdr_t* pStudioHdr = csgo.m_model_info()->GetStudioModel(model);

			if (!pStudioHdr)
				return;

			matrix3x4_t mx[128];
			memcpy(mx, ctx.m_local()->m_CachedBoneData().Base(), ctx.m_local()->GetBoneCount() * sizeof(matrix3x4_t));
			memcpy(ctx.m_local()->m_CachedBoneData().Base(), ctx.matrix, ctx.m_local()->GetBoneCount() * sizeof(matrix3x4_t));

			for (auto i = 0; i < HITBOX_MAX; i++)
			{
				points.clear();

				mstudiobbox_t* hitbox = pStudioHdr->pHitbox(i, ctx.m_local()->m_nHitboxSet());

				if (!hitbox)
					continue;

				auto rs = hitbox->radius;
				Vector min, max;
				Math::VectorTransform(hitbox->bbmin, ctx.matrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, ctx.matrix[hitbox->bone], max);

				auto center = (min + max) * 0.5f;

				const auto cur_angles = Math::CalcAngle(center, player->GetEyePosition());

				Vector forward;
				Math::AngleVectors(cur_angles, &forward);

				const auto right = forward.Cross(Vector(0, 0, 1)) * rs;
				const auto left = Vector(-right.x, -right.y, right.z);
				const auto top = Vector(0, 0, 1) * rs;
				points.emplace_back(center + right);
				points.emplace_back(center + left);

				auto srs = hitbox->radius * 0.5f;

				const auto sright = forward.Cross(Vector(0, 0, 1)) * srs;
				const auto sleft = Vector(-sright.x, -sright.y, sright.z);

				points.emplace_back(center + sright);
				points.emplace_back(center + sleft);

				ctx.can_hit[player->entindex() - 1] = false;

				for (auto point : points) {

					float dmg = 0.f;

					if (player->get_weapon() && player->get_weapon()->GetClientClass()) {
						const auto p = ctx.last_penetrated_count;
						dmg = feature::autowall.CanHit(player->GetEyePosition(), point, player, ctx.m_local(), i);
						ctx.last_penetrated_count = p;
					}
					else
					{
						ctx.can_hit[player->entindex() - 1] = false;
						continue;
					}

					if (dmg > 0.f) {
						csgo.m_debug_overlay()->AddBoxOverlay(point, Vector(-1, -1, -1), Vector(1, 1, 1), Vector(0, 0, 0), 255, 0, 0, 255, csgo.m_globals()->interval_per_tick * 2.f);

						ctx.can_hit[player->entindex() - 1] = true;
					}
					/*dmg += feature::autowall.CanHit(player->GetEyePosition(), point, player, ctx.m_local(), i);*/
				}
			}

			memcpy(ctx.m_local()->m_CachedBoneData().Base(), mx, ctx.m_local()->GetBoneCount() * sizeof(matrix3x4_t));
		}

		data->job_done = true;
	};

	int GetNewAnimation(const uint32_t model, const int sequence, C_BaseViewModel* viewModel) {

		// This only fixes if the original knife was a default knife.
		// The best would be having a function that converts original knife's sequence
		// into some generic enum, then another function that generates a sequence
		// from the sequences of the new knife. I won't write that.
		enum ESequence {
			SEQUENCE_DEFAULT_DRAW = 0,
			SEQUENCE_DEFAULT_IDLE1 = 1,
			SEQUENCE_DEFAULT_IDLE2 = 2,
			SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
			SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
			SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
			SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
			SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
			SEQUENCE_DEFAULT_LOOKAT01 = 12,

			SEQUENCE_BUTTERFLY_DRAW = 0,
			SEQUENCE_BUTTERFLY_DRAW2 = 1,
			SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
			SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

			SEQUENCE_FALCHION_IDLE1 = 1,
			SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
			SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
			SEQUENCE_FALCHION_LOOKAT01 = 12,
			SEQUENCE_FALCHION_LOOKAT02 = 13,

			SEQUENCE_DAGGERS_IDLE1 = 1,
			SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
			SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
			SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
			SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

			SEQUENCE_BOWIE_IDLE1 = 1,
		};

		auto random_sequence = [](const int low, const int high) -> int {
			return rand() % (high - low + 1) + low;
		};

		// Hashes for best performance.
		switch (model) {
		case hash_32_fnv1a_const(("models/weapons/v_knife_butterfly.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
			default:
				return sequence + 1;
			}
		}
		case hash_32_fnv1a_const(("models/weapons/v_knife_falchion_advanced.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_FALCHION_IDLE1;
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence - 1;
			}
		}
		case hash_32_fnv1a_const(("models/weapons/v_knife_push.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_DAGGERS_IDLE1;
			case SEQUENCE_DEFAULT_LIGHT_MISS1:
			case SEQUENCE_DEFAULT_LIGHT_MISS2:
				return random_sequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
			case SEQUENCE_DEFAULT_HEAVY_MISS1:
				return random_sequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
			case SEQUENCE_DEFAULT_HEAVY_HIT1:
			case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
			case SEQUENCE_DEFAULT_LOOKAT01:
				return sequence + 3;
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			default:
				return sequence + 2;
			}
		}
		case hash_32_fnv1a_const(("models/weapons/v_knife_survival_bowie.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_DRAW:
			case SEQUENCE_DEFAULT_IDLE1:
				return sequence;
			case SEQUENCE_DEFAULT_IDLE2:
				return SEQUENCE_BOWIE_IDLE1;
			default:
				return sequence - 1;
			}
		}
		case hash_32_fnv1a_const(("models/weapons/v_knife_ursus.mdl")):
		case hash_32_fnv1a_const(("models/weapons/v_knife_skeleton.mdl")):
		case hash_32_fnv1a_const(("models/weapons/v_knife_outdoor.mdl")):
		case hash_32_fnv1a_const(("models/weapons/v_knife_canis.mdl")):
		case hash_32_fnv1a_const(("models/weapons/v_knife_cord.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_DRAW:
				return random_sequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
			default:
				return sequence + 1;
			}
		}
		case hash_32_fnv1a_const(("models/weapons/v_knife_stiletto.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(12, 13);
			}
		}
		case hash_32_fnv1a_const(("models/weapons/v_knife_widowmaker.mdl")):
		{
			switch (sequence) {
			case SEQUENCE_DEFAULT_LOOKAT01:
				return random_sequence(14, 15);
			}
		}

		default:
			return sequence;
		}
	}

	void DoSequenceRemapping(CRecvProxyData* data, C_BaseViewModel* entity) {
		
		if (!csgo.m_engine()->IsInGame() || !ctx.m_local() || ctx.m_local()->IsDead() || parser::knifes.list.empty())
			return;

		const auto owner = csgo.m_entity_list()->GetClientEntityFromHandle((CBaseHandle)entity->m_hOwner());
		if (owner != ctx.m_local() || entity->get_viewmodel_weapon() == -1)
			return;

		const auto view_model_weapon = (C_WeaponCSBaseGun*)csgo.m_entity_list()->GetClientEntityFromHandle(entity->get_viewmodel_weapon());

		if (!view_model_weapon || !view_model_weapon->is_knife())
			return;

		auto idx = view_model_weapon->m_iItemDefinitionIndex();

		const auto override_model = parser::knifes.list[ctx.m_settings.skinchanger_knife].model_player_path.c_str();

		auto& sequence = data->m_Value.m_Int;
		sequence = GetNewAnimation(hash_32_fnv1a_const(override_model), sequence, entity);
	}

	void m_nSequenceHook(const CRecvProxyData* proxy_data_const, void* entity, void* output) {
			// Remove the constness from the proxy data allowing us to make changes.
			const auto proxy_data = const_cast<CRecvProxyData*>(proxy_data_const);

			const auto view_model = static_cast<C_BaseViewModel*>(entity);

			DoSequenceRemapping(proxy_data, view_model);

			// Call the original function with our edited data.
			m_nSequence(proxy_data_const, entity, output);
	}
}

//static DWORD a1 = 0xDEADBABE;
//static DWORD a2 = 0xDEADBABE;
//static DWORD a3 = 0xDEADBABE;
//static DWORD a4 = 0xDEADBABE;
//static DWORD a5 = 0xDEADBABE;
//static DWORD a6 = 0xDEADBABE;
//static DWORD a7 = 0xDEADBABE;
//static DWORD a8 = 0xDEADBABE;
//static DWORD a9 = 0xDEADBABE;
//static DWORD a10 = 0xDEADBABE;
//static DWORD a11 = 0xDEADBABE;
//static DWORD a12 = 0xDEADBABE;
//static DWORD a13 = 0xDEADBABE;
//static DWORD a14 = 0xDEADBABE;
//static DWORD a15 = 0xDEADBABE;
//static DWORD a16 = 0xDEADBABE;
//static DWORD a17 = 0xDEADBABE;
//static DWORD a18 = 0xDEADBABE;
//static DWORD a19 = 0xDEADBABE;
//static DWORD a20 = 0xDEADBABE;
//static DWORD a21 = 0xDEADBABE;
static ULONG64 VClient = 0xFADEBEEF;
static ULONG64 VClientEntityList = 0xFADEBEEF;
static ULONG64 GameMovement = 0xFADEBEEF;
static ULONG64 VClientPrediction = 0xFADEBEEF;
static ULONG64 VEngineClient = 0xFADEBEEF;
static ULONG64 VGUI_Panel = 0xFADEBEEF;
static ULONG64 VGUI_Surface = 0xFADEBEEF;
static ULONG64 VEngineVGui001 = 0xFADEBEEF;
static ULONG64 VEngineCvar = 0xFADEBEEF;
static ULONG64 EngineTraceClient004 = 0xFADEBEEF;
static ULONG64 VModelInfoClient004 = 0xFADEBEEF;
static ULONG64 InputSystemVersion001 = 0xFADEBEEF;
static ULONG64 VEngineModel016 = 0xFADEBEEF;
static ULONG64 VEngineRenderView014 = 0xFADEBEEF;
static ULONG64 VMaterialSystem080 = 0xFADEBEEF;
static ULONG64 VPhysicsSurfaceProps001 = 0xFADEBEEF;
static ULONG64 VDebugOverlay004 = 0xFADEBEEF;
static ULONG64 GAMEEVENTSMANAGER002 = 0xFADEBEEF;
static ULONG64 StaticPropMgrServer002 = 0xFADEBEEF;
static ULONG64 Localize_001 = 0xFADEBEEF;
static ULONG64 MDLCache004 = 0xFADEBEEF;
static ULONG64 IEngineSoundClient = 0xFADEBEEF;

#define SE_UNPROTECT_START __asm _emit 0xEB\
	__asm _emit 0x09\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x55\
	__asm _emit 0x50\
	__asm _emit 0x42\
	__asm _emit 0x45\
	__asm _emit 0x47\
	__asm _emit 0x4E\
	__asm _emit 0x00;

#define SE_UNPROTECT_END __asm _emit 0xEB\
	__asm _emit 0x09\
	__asm _emit 0x53\
	__asm _emit 0x45\
	__asm _emit 0x55\
	__asm _emit 0x50\
	__asm _emit 0x45\
	__asm _emit 0x4E\
	__asm _emit 0x44\
	__asm _emit 0x50\
	__asm _emit 0x00;

 bool create_log_file(std::string error_message)
{
	std::fstream file(sxor("C:\\2k17_v2\\error.log"), std::ios::out | std::ios::in | std::ios::trunc);
	file.close();

	file.open(sxor("C:\\2k17_v2\\error.log"), std::ios::out | std::ios::in);
	if (!file.is_open())
	{
		file.close();
		return false;
	}

	file.clear();

	file << error_message;

	file.close();

	return true;
}

namespace Source
{
	Memory::VmtSwap::Shared m_pClientSwap = nullptr;
	Memory::VmtSwap::Shared m_pClientStateSwap = nullptr;
	Memory::VmtSwap::Shared m_pClientModeSwap = nullptr;
	Memory::VmtSwap::Shared m_pSurfaceSwap = nullptr;
	Memory::VmtSwap::Shared m_pPredictionSwap = nullptr;
	Memory::VmtSwap::Shared m_pPanelSwap = nullptr;
	Memory::VmtSwap::Shared m_pRenderViewSwap = nullptr;
	Memory::VmtSwap::Shared m_pEngineSwap = nullptr;

	Memory::VmtSwap::Shared m_pFireBulletsSwap = nullptr;
	Memory::VmtSwap::Shared m_pEngineVGUISwap = nullptr;
	Memory::VmtSwap::Shared m_pModelRenderSwap = nullptr;
	Memory::VmtSwap::Shared m_pNetChannelSwap = nullptr;
	Memory::VmtSwap::Shared m_pShowImpactsSwap = nullptr;
	Memory::VmtSwap::Shared m_pMaterialSystemSwap = nullptr;
	Memory::VmtSwap::Shared m_pEngineTraceSwap = nullptr;
	Memory::VmtSwap::Shared m_pBSPTreeQuerySwap = nullptr;

	Memory::VmtSwap::Shared m_pDeviceSwap = nullptr;

	Memory::VmtSwap::Shared net_showfragmentsSwap = nullptr;

	HWND Window = nullptr;

	int translate_id(int wpn)
	{
		for (auto i = 0; i < parser::weapons.list.size(); i++) {
			auto& skin = parser::weapons.list[i];

			if (skin.id == wpn) {
				return i;
			}
		}

		return 0;
	};

	struct patch
	{
		ULONG64 VClient;
		ULONG64 VClientEntityList;
		ULONG64 GameMovement;
		ULONG64 VClientPrediction;
		ULONG64 VEngineClient;
		ULONG64 VGUI_Panel;
		ULONG64 VGUI_Surface;
		ULONG64 VEngineVGui001;
		ULONG64 VEngineCvar;
		ULONG64 EngineTraceClient004;
		ULONG64 VModelInfoClient004;
		ULONG64 InputSystemVersion001;
		ULONG64 VEngineModel016;
		ULONG64 VEngineRenderView014;
		ULONG64 VMaterialSystem080;
		ULONG64 VPhysicsSurfaceProps001;
		ULONG64 VDebugOverlay004;
		ULONG64 GAMEEVENTSMANAGER002;
		ULONG64 StaticPropMgrServer002;
		ULONG64 Localize_001;
		ULONG64 MDLCache004;
		ULONG64 IEngineSoundClient;
	};

	_declspec(noinline) bool Create()
	{
		SE_UNPROTECT_START
		VMProtectBeginUltra("EntryPoint");

		auto& pPropManager = Engine::PropManager::Instance();

		patch ppppp;

#ifndef AUTH
		csgo.m_client.set((IBaseClientDLL*)CreateInterface("client_panorama.dll", "VClient"));

		if (!csgo.m_client())
		{
#ifdef DEBUG
			Win32::Error("IBaseClientDLL is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		if (!pPropManager->Create(csgo.m_client()))
		{
#ifdef DEBUG
			Win32::Error("Engine::PropManager::Create failed (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		if (!Engine::Displacement::Create())
		{
#ifdef DEBUG
			Win32::Error("Engine::Displacement::Create failed (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_entity_list.set((IClientEntityList*)CreateInterface("client_panorama.dll", "VClientEntityList"));

		if (!csgo.m_entity_list())
		{
#ifdef DEBUG
			Win32::Error("IClientEntityList is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_movement.set((IGameMovement*)CreateInterface("client_panorama.dll", "GameMovement"));

		if (!csgo.m_movement())
		{
#ifdef DEBUG
			Win32::Error("IGameMovement is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_prediction.set((IPrediction*)CreateInterface("client_panorama.dll", "VClientPrediction"));

		if (!csgo.m_prediction())
		{
#ifdef DEBUG
			Win32::Error("IPrediction is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_move_helper.set((IMoveHelper*)(Engine::Displacement::Data::m_uMoveHelper));

		if (!csgo.m_move_helper())
		{
#ifdef DEBUG
			Win32::Error("IMoveHelper is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_input.set((IInput*)(Engine::Displacement::Data::m_uInput));

		if (!csgo.m_input())
		{
#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_globals.set(**(CGlobalVarsBase ***)((*(DWORD**)csgo.m_client())[0] + 0x1B));

		if (!csgo.m_globals())
		{
#ifdef DEBUG
			Win32::Error("CGlobalVars is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_engine.set((IVEngineClient*)CreateInterface("engine.dll", "VEngineClient"));

		if (!csgo.m_engine())
		{
#ifdef DEBUG
			Win32::Error("IVEngineClient is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_panel.set((IPanel*)CreateInterface("vgui2.dll", "VGUI_Panel"));

		if (!csgo.m_panel())
		{
#ifdef DEBUG
			Win32::Error("IPanel is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_surface.set((ISurface*)CreateInterface("vguimatsurface.dll", "VGUI_Surface"));

		if (!csgo.m_surface())
		{
#ifdef DEBUG
			Win32::Error("ISurface is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_engine_vgui.set((IEngineVGui*)CreateInterface("engine.dll", "VEngineVGui001", true));

		if (!csgo.m_engine_vgui())
		{
#ifdef DEBUG
			Win32::Error("IEngineVGui is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_client_state.set((CClientState*)(**(std::uintptr_t**)(Memory::Scan("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1))); //ik we can grab it from vfunc but imma just took what i did b4

		if (!csgo.m_client_state())
		{
#ifdef DEBUG
			Win32::Error("CClientState is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_engine_cvars.set((ICvar*)CreateInterface("vstdlib.dll", "VEngineCvar"));

		if (!csgo.m_engine_cvars())
		{
#ifdef DEBUG
			Win32::Error("ICvar is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_engine_trace.set((IEngineTrace*)CreateInterface("engine.dll", "EngineTraceClient004", true));

		if (!csgo.m_engine_trace())
		{
#ifdef DEBUG
			Win32::Error("IEngineTrace is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_model_info.set((IVModelInfo*)CreateInterface("engine.dll", "VModelInfoClient004", true));

		if (!csgo.m_model_info())
		{
#ifdef DEBUG
			Win32::Error("IVModelInfo is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_input_system.set((InputSystem*)CreateInterface("inputsystem.dll", "InputSystemVersion001", true));

		if (!csgo.m_input_system())
		{
#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_model_render.set((IVModelRender*)(CreateInterface("engine.dll", "VEngineModel016", true)));

		if (!csgo.m_model_render())
		{
#ifdef DEBUG
			Win32::Error("IVModelRender is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_render_view.set((IVRenderView*)CreateInterface("engine.dll", "VEngineRenderView014", true));

		if (!csgo.m_render_view())
		{
#ifdef DEBUG
			Win32::Error("IVRenderView is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_material_system.set((IMaterialSystem*)CreateInterface("materialsystem.dll", "VMaterialSystem080", true));

		if (!csgo.m_material_system())
		{
#ifdef DEBUG
			Win32::Error("IMaterialSystem is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_phys_props.set((IPhysicsSurfaceProps*)CreateInterface("vphysics.dll", "VPhysicsSurfaceProps001", true));

		if (!csgo.m_phys_props())
		{
#ifdef DEBUG
			Win32::Error("IPhysicsSurfaceProps is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_debug_overlay.set((IVDebugOverlay*)CreateInterface("engine.dll", "VDebugOverlay004", true));

		if (!csgo.m_debug_overlay())
		{
#ifdef DEBUG
			Win32::Error("IVDebugOverlay is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_event_manager.set((IGameEventManager*)(CreateInterface("engine.dll", "GAMEEVENTSMANAGER002", true)));

		if (!csgo.m_event_manager())
		{
#ifdef DEBUG
			Win32::Error("IGameEventManager is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}
		
		csgo.m_static_prop.set((IStaticPropMgr*)(CreateInterface("engine.dll", "StaticPropMgrServer002", true)));

		if (!csgo.m_static_prop())
		{
#ifdef DEBUG
			Win32::Error("IStaticPropMgr is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_beams.set(*(IViewRenderBeams**)(Memory::Scan("client_panorama.dll", "B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9") + 1));

		if (!csgo.m_beams())
		{
#ifdef DEBUG
			Win32::Error("IViewRenderBeams is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_localize.set((ILocalize*)(CreateInterface("localize.dll", "Localize_001", true)));

		if (!csgo.m_localize())
		{
#ifdef DEBUG
			Win32::Error("ILocalize is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_glow_object.set(*reinterpret_cast<CGlowObjectManager**>(Memory::Scan("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3));

		if (!csgo.m_glow_object())
		{
#ifdef DEBUG
			Win32::Error("CGlowObjectManager is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_mdl_cache.set((IMDLCache*)(CreateInterface("datacache.dll", "MDLCache004", true)));

		if (!csgo.m_mdl_cache())
		{
#ifdef DEBUG
			Win32::Error("IMDLCache is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_engine_sound.set((IEngineSound*)(CreateInterface("engine.dll", "IEngineSoundClient")));

		if (!csgo.m_engine_sound())
		{
#ifdef DEBUG
			Win32::Error("IEngineSound is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

#else

		csgo.m_client.set((IBaseClientDLL*)CreateInterface(VMProtectDecryptStringA("client_panorama.dll"), VClient));

		if (!csgo.m_client())
		{
		#ifdef DEBUG
			Win32::Error("IBaseClientDLL is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			Beep(550, 200);
			return false;
		}

		if (!pPropManager->Create(csgo.m_client()))
		{
		#ifdef DEBUG
			Win32::Error("Engine::PropManager::Create failed (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 1"));
			return false;
		}

		if (!Engine::Displacement::Create())
		{
		#ifdef DEBUG
			Win32::Error("Engine::Displacement::Create failed (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 2"));
			return false;
		}

		csgo.m_entity_list.set((IClientEntityList*)CreateInterface(VMProtectDecryptStringA("client_panorama.dll"), VClientEntityList));

		if (!csgo.m_entity_list())
		{
		#ifdef DEBUG
			Win32::Error("IClientEntityList is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 3"));
			return false;
		}

		csgo.m_movement.set((IGameMovement*)CreateInterface(VMProtectDecryptStringA("client_panorama.dll"), GameMovement));

		if (!csgo.m_movement())
		{
		#ifdef DEBUG
			Win32::Error("IGameMovement is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 4"));
			return false;
		}

		csgo.m_prediction.set((IPrediction*)CreateInterface(VMProtectDecryptStringA("client_panorama.dll"), VClientPrediction));

		if (!csgo.m_prediction())
		{
		#ifdef DEBUG
			Win32::Error("IPrediction is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			Beep(550, 200);
			create_log_file(sxor("ERROR 5"));
			return false;
		}

		csgo.m_move_helper.set((IMoveHelper*)(Engine::Displacement::Data::m_uMoveHelper));

		if (!csgo.m_move_helper())
		{
		#ifdef DEBUG
			Win32::Error("IMoveHelper is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 6"));
			return false;
		}

		csgo.m_input.set((IInput*)(Engine::Displacement::Data::m_uInput));

		if (!csgo.m_input())
		{
		#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 7"));
			return false;
		}

		csgo.m_globals.set(**(CGlobalVarsBase***)((*(DWORD**)csgo.m_client())[0] + 0x1B));

		if (!csgo.m_globals())
		{
		#ifdef DEBUG
			Win32::Error("CGlobalVars is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 8"));
			return false;
		}

		csgo.m_engine.set((IVEngineClient*)CreateInterface(VMProtectDecryptStringA("engine.dll"), VEngineClient));

		if (!csgo.m_engine())
		{
		#ifdef DEBUG
			Win32::Error("IVEngineClient is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 9"));
			return false;
		}

		csgo.m_panel.set((IPanel*)CreateInterface(VMProtectDecryptStringA("vgui2.dll"), VGUI_Panel));

		if (!csgo.m_panel())
		{
		#ifdef DEBUG
			Win32::Error("IPanel is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 10"));
			return false;
		}

		csgo.m_surface.set((ISurface*)CreateInterface(VMProtectDecryptStringA("vguimatsurface.dll"), VGUI_Surface));

		if (!csgo.m_surface())
		{
		#ifdef DEBUG
			Win32::Error("ISurface is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 11"));
			return false;
		}

		csgo.m_engine_vgui.set((IEngineVGui*)CreateInterface(VMProtectDecryptStringA("engine.dll"), VEngineVGui001));

		if (!csgo.m_engine_vgui())
		{
		#ifdef DEBUG
			Win32::Error("IEngineVGui is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 12"));
			return false;
		}

		csgo.m_client_state.set((CClientState*)(**(std::uintptr_t**)(Memory::Scan(VMProtectDecryptStringA("engine.dll"), VMProtectDecryptStringA("A1 ? ? ? ? 8B 80 ? ? ? ? C3")) + 1))); //ik we can grab it from vfunc but imma just took what i did b4

		if (!csgo.m_client_state())
		{
		#ifdef DEBUG
			Win32::Error("CClientState is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 13"));
			return false;
		}

		csgo.m_engine_cvars.set((ICvar*)CreateInterface(VMProtectDecryptStringA("vstdlib.dll"), VEngineCvar));

		if (!csgo.m_engine_cvars())
		{
		#ifdef DEBUG
			Win32::Error("ICvar is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 14"));
			return false;
		}

		csgo.m_engine_trace.set((IEngineTrace*)CreateInterface(VMProtectDecryptStringA("engine.dll"), EngineTraceClient004));

		if (!csgo.m_engine_trace())
		{
		#ifdef DEBUG
			Win32::Error("IEngineTrace is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 15"));
			return false;
		}

		csgo.m_model_info.set((IVModelInfo*)CreateInterface(VMProtectDecryptStringA("engine.dll"), VModelInfoClient004));

		if (!csgo.m_model_info())
		{
		#ifdef DEBUG
			Win32::Error("IVModelInfo is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 16"));
			return false;
		}

		csgo.m_input_system.set((InputSystem*)CreateInterface(VMProtectDecryptStringA("inputsystem.dll"), InputSystemVersion001));

		if (!csgo.m_input_system())
		{
		#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 17"));
			return false;
		}

		csgo.m_model_render.set((IVModelRender*)(CreateInterface(VMProtectDecryptStringA("engine.dll"), VEngineModel016)));

		if (!csgo.m_model_render())
		{
		#ifdef DEBUG
			Win32::Error("IVModelRender is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 18"));
			return false;
		}

		csgo.m_render_view.set((IVRenderView*)CreateInterface(VMProtectDecryptStringA("engine.dll"), VEngineRenderView014));

		if (!csgo.m_render_view())
		{
		#ifdef DEBUG
			Win32::Error("IVRenderView is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 19"));
			return false;
		}

		csgo.m_material_system.set((IMaterialSystem*)CreateInterface(VMProtectDecryptStringA("materialsystem.dll"), VMaterialSystem080));

		if (!csgo.m_material_system())
		{
		#ifdef DEBUG
			Win32::Error("IMaterialSystem is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 20"));
			return false;
		}

		csgo.m_phys_props.set((IPhysicsSurfaceProps*)CreateInterface(VMProtectDecryptStringA("vphysics.dll"), VPhysicsSurfaceProps001));

		if (!csgo.m_phys_props())
		{
		#ifdef DEBUG
			Win32::Error("IPhysicsSurfaceProps is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 21"));
			return false;
		}

		csgo.m_debug_overlay.set((IVDebugOverlay*)CreateInterface(VMProtectDecryptStringA("engine.dll"), VDebugOverlay004));

		if (!csgo.m_debug_overlay())
		{
		#ifdef DEBUG
			Win32::Error("IVDebugOverlay is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 22"));
			return false;
		}

		csgo.m_event_manager.set((IGameEventManager*)(CreateInterface(VMProtectDecryptStringA("engine.dll"), GAMEEVENTSMANAGER002)));

		if (!csgo.m_event_manager())
		{
		#ifdef DEBUG
			Win32::Error("IGameEventManager is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 23"));
			return false;
		}

		csgo.m_static_prop.set((IStaticPropMgr*)(CreateInterface(VMProtectDecryptStringA("engine.dll"), StaticPropMgrServer002)));

		if (!csgo.m_static_prop())
		{
		#ifdef DEBUG
			Win32::Error("IStaticPropMgr is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 24"));
			return false;
		}

		csgo.m_beams.set(*(IViewRenderBeams**)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 1));

		if (!csgo.m_beams())
		{
		#ifdef DEBUG
			Win32::Error("IViewRenderBeams is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 25"));
			return false;
		}

		csgo.m_localize.set((ILocalize*)(CreateInterface(VMProtectDecryptStringA("localize.dll"), Localize_001)));

		if (!csgo.m_localize())
		{
		#ifdef DEBUG
			Win32::Error("ILocalize is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 26"));
			return false;
		}

		csgo.m_glow_object.set(*reinterpret_cast<CGlowObjectManager**>(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("0F 11 05 ? ? ? ? 83 C8 01")) + 3));

		if (!csgo.m_glow_object())
		{
		#ifdef DEBUG
			Win32::Error("CGlowObjectManager is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 27"));
			return false;
		}

		csgo.m_mdl_cache.set((IMDLCache*)(CreateInterface(VMProtectDecryptStringA("datacache.dll"), MDLCache004)));

		if (!csgo.m_mdl_cache())
		{
		#ifdef DEBUG
			Win32::Error("IMDLCache is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 28"));
			return false;
		}

		csgo.m_engine_sound.set((IEngineSound*)(CreateInterface(VMProtectDecryptStringA("engine.dll"), IEngineSoundClient)));

		if (!csgo.m_engine_sound())
		{
		#ifdef DEBUG
			Win32::Error("IEngineSound is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 29"));
			return false;
		}

		VClient = 0;
		VClientEntityList = 0;
		GameMovement = 0;
		VClientPrediction = 0;
		VEngineClient = 0;
		VGUI_Panel = 0;
		VGUI_Surface = 0;
		VEngineVGui001 = 0;
		VEngineCvar = 0;
		EngineTraceClient004 = 0;
		VModelInfoClient004 = 0;
		InputSystemVersion001 = 0;
		VEngineModel016 = 0;
		VEngineRenderView014 = 0;
		VMaterialSystem080 = 0;
		VPhysicsSurfaceProps001 = 0;
		VDebugOverlay004 = 0;
		GAMEEVENTSMANAGER002 = 0;
		StaticPropMgrServer002 = 0;
		Localize_001 = 0;
		MDLCache004 = 0;
		IEngineSoundClient = 0;

#endif // DEBUG

		ctx.init_finished = false;

		for (auto head = ctx.m_effects_head(); head; head = head->next)
		{
			if (strstr(head->effectName, VMProtectDecryptStringA("Impact")) && strlen(head->effectName) <= 8) {
				oImpact = head->function;
				head->function = &Hooked::impact_callback;
				break;
			}
		}

#ifndef AUTH
		ppppp.VClient = 0x4d3f730;
		ppppp.VClientEntityList = 0x4d64a04;
		ppppp.GameMovement = 0x51950b0;
		ppppp.VClientPrediction = 0x51963e8;
		ppppp.VEngineClient = 0x588140;
		ppppp.VGUI_Panel = 0x608c4;
			ppppp.VGUI_Surface = 0xf47c0;
			ppppp.VEngineVGui001 = 0x38b10e0;
			ppppp.VEngineCvar = 0x3c270;
			ppppp.EngineTraceClient004 = 0x59453c;
			ppppp.VModelInfoClient004 = 0x59aae8;
			ppppp.InputSystemVersion001 = 0x31188;
			ppppp.VEngineModel016 = 0x76c650;
			ppppp.VEngineRenderView014 = 0x58f140;
			ppppp.VMaterialSystem080 = 0xe0420;
			ppppp.VPhysicsSurfaceProps001 = 0x116018;
			ppppp.VDebugOverlay004 = 0x58ab10;
			ppppp.GAMEEVENTSMANAGER002 = 0x7e1cd0;
			ppppp.StaticPropMgrServer002 = 0x38aeb50;
			ppppp.Localize_001 = 0x38a70;
			ppppp.MDLCache004 = 0x63da8;
			ppppp.IEngineSoundClient = 0x516494;



		std::ofstream test("data.bin", std::ios::binary);

		PBYTE bytes = (PBYTE)(&ppppp);

		for (int i = 0; i < 176; i++)  
		{ 
			test << bytes[i];
		}

		//test.close();

#endif // AUTH

		g_menu.setup();

		/*std::ofstream output;
		output.open("C:\\2k17_lol\\CSGOclassids.txt", std::ofstream::out | std::ofstream::app);
		output << "enum class_ids" << std::endl << "{" << std::endl;

		static auto entry = csgo.m_client()->GetAllClasses();

		while (entry)
		{
			output << "\t" << entry->m_pNetworkName << " = " << entry->m_ClassID << "," << std::endl;
			entry = entry->m_pNext;
		}

		output << "};";
		output.close();*/

		static const auto filter_simple = *reinterpret_cast<uint32_t*>(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3d);

		m_pClientSwap = std::make_shared<Memory::VmtSwap>(csgo.m_client());
		m_pPredictionSwap = std::make_shared<Memory::VmtSwap>(csgo.m_prediction());
		m_pPanelSwap = std::make_shared<Memory::VmtSwap>(csgo.m_panel());
		m_pSurfaceSwap = std::make_shared<Memory::VmtSwap>(csgo.m_surface());
		m_pEngineVGUISwap = std::make_shared<Memory::VmtSwap>(csgo.m_engine_vgui());
		m_pRenderViewSwap = std::make_shared<Memory::VmtSwap>(csgo.m_render_view());
		m_pEngineSwap = std::make_shared<Memory::VmtSwap>(csgo.m_engine());
		m_pModelRenderSwap = std::make_shared<Memory::VmtSwap>(csgo.m_model_render());
		m_pMaterialSystemSwap = std::make_shared<Memory::VmtSwap>(csgo.m_material_system());
		m_pEngineTraceSwap = std::make_shared<Memory::VmtSwap>(csgo.m_engine_trace());
		m_pBSPTreeQuerySwap = std::make_shared<Memory::VmtSwap>(csgo.m_engine()->GetBSPTreeQuery());
		
		auto m_pClientMode = **(void***)((*(DWORD**)csgo.m_client())[10] + 5);

		if (m_pClientMode)
		{
			m_pClientModeSwap = std::make_shared<Memory::VmtSwap>(m_pClientMode);
			m_pClientModeSwap->Hook(&Hooked::CreateMove, Index::IBaseClientDLL::CreateMove);
			m_pClientModeSwap->Hook(&Hooked::OverrideView, Index::IBaseClientDLL::OverrideView);
			m_pClientModeSwap->Hook(&Hooked::DoPostScreenEffects, 44);
		}
		else
		{
#ifdef DEBUG
			Win32::Error("IClientMode is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			Destroy();
			create_log_file(sxor("ERROR 30"));
			return false;
		}

		m_pEngineSwap->Hook(&Hooked::IsHLTV, 93);
		m_pEngineSwap->Hook(&Hooked::IsBoxVisible, 32);
		m_pEngineSwap->Hook(&Hooked::IsConnected, 27);
		m_pBSPTreeQuerySwap->Hook(&Hooked::ListLeavesInBox, 6);
		m_pEngineSwap->Hook(&Hooked::FireEvents, 59);

		//m_pTraceFilterSimple->Hook(&Hooked::ShouldHitEntity, 0);

		m_pEngineTraceSwap->Hook(&Hooked::ClipRayCollideable, 4);
		m_pEngineTraceSwap->Hook(&Hooked::TraceRay, 5);

		m_pClientSwap->Hook(&Hooked::FrameStageNotify, Index::IBaseClientDLL::FrameStageNotify);
		m_pClientSwap->Hook(&Hooked::WriteUsercmdDeltaToBuffer, 24);

		m_pSurfaceSwap->Hook(&Hooked::LockCursor, Index::ISurface::LockCursor);
		m_pSurfaceSwap->Hook(&Hooked::DrawSetColor, 15);
		m_pPredictionSwap->Hook(&Hooked::RunCommand, Index::IPrediction::RunCommand);
		m_pPredictionSwap->Hook(&Hooked::InPrediction, Index::IPrediction::InPrediction);
		//m_pPredictionSwap->Hook(&Hooked::SetupMove, Index::IPrediction::SetupMove);

		m_pPanelSwap->Hook(&Hooked::PaintTraverse, Index::IPanel::PaintTraverse);
		m_pEngineVGUISwap->Hook(&Hooked::EngineVGUI_Paint, 14); //index is not in enum idk why

		m_pRenderViewSwap->Hook(&Hooked::SceneEnd, 9);
		m_pModelRenderSwap->Hook(&Hooked::DrawModelExecute, 21);

		m_pMaterialSystemSwap->Hook(&Hooked::OverrideConfig, 21);

		//const auto net_showfragments = csgo.m_engine_cvars()->FindVar("net_showfragments");

		//net_showfragmentsSwap = std::make_shared<Memory::VmtSwap>(net_showfragments);
		//net_showfragmentsSwap->Hook(&Hooked::net_showfragments_get_bool, 13);

		const auto lol = csgo.m_engine_cvars()->FindVar(VMProtectDecryptStringA("cl_foot_contact_shadows"));

		if (lol != nullptr)
			lol->SetValue(0);

		game_events::init();

		feature::music_player.init();

		ctx.fix_modify_eye_pos = false;
		ctx.can_store_netvars = false;

		m_flSimulationTime = pPropManager->Hook(Hooked::m_flSimulationTimeHook, VMProtectDecryptStringA("DT_BaseEntity"), VMProtectDecryptStringA("m_flSimulationTime"));
		m_flAbsYaw = pPropManager->Hook(Hooked::m_flAbsYawHook, VMProtectDecryptStringA("DT_CSRagdoll"), VMProtectDecryptStringA("m_flAbsYaw"));
		m_nSequence = pPropManager->Hook(Hooked::m_nSequenceHook, VMProtectDecryptStringA("DT_BaseViewModel"), VMProtectDecryptStringA("m_nSequence"));

		//oShouldHitEntity = Memory::VFTableHook::HookManual<ShouldHitEntity_t>((uintptr_t*)filter_simple, 0, (ShouldHitEntity_t)Hooked::ShouldHitEntity);

		//m_flLowerBodyYawTarget = pPropManager->Hook(Hooked::m_flLowerBodyYawTargetHook, "DT_CSPlayer", "m_flLowerBodyYawTarget");

		static auto SetupVelocity = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D")));
		OriginalSetupVelocity = (DWORD)DetourFunction((byte*)SetupVelocity, (byte*)Hooked::SetupVelocity);

		//static auto CalcAbsoluteVelocity = (DWORD)(Memory::Scan("client_panorama.dll", "55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8B F9 F7"));
		//OriginalCalcAbsoluteVelocity = (DWORD)DetourFunction((byte*)CalcAbsoluteVelocity, (byte*)Hooked::CalcAbsoluteVelocity);

		static auto StandardBlendingRules = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6")));
		OriginalStandardBlendingRules = (DWORD)DetourFunction((byte*)StandardBlendingRules, (byte*)Hooked::StandardBlendingRules);

		static auto DoExtraBonesProcessing = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C")));
		OriginalDoExtraBonesProcessing = (DWORD)DetourFunction((byte*)DoExtraBonesProcessing, (byte*)Hooked::DoExtraBonesProcessing);

		static auto GetForeignFallbackFontName = (DWORD)(Memory::Scan(VMProtectDecryptStringA("vguimatsurface.dll"), VMProtectDecryptStringA("80 3D ? ? ? ? ? 74 06 B8")));
		OriginalGetForeignFallbackFontName = (DWORD)DetourFunction((byte*)GetForeignFallbackFontName, (byte*)Hooked::GetForeignFallbackFontName);

		static auto m_uUpdateClientSideAnimation = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 51 56 8B F1 80 BE ? ? 00 00 00 74 36")));
		OriginalUpdateClientSideAnimations = (DWORD)DetourFunction((byte*)m_uUpdateClientSideAnimation, (byte*)Hooked::UpdateClientSideAnimation);

		static auto SetupBones = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 83 E4 F0 B8 D8")));
		OriginalSetupBones = (DWORD)DetourFunction((byte*)SetupBones, (byte*)Hooked::SetupBones);	

		//static auto ShouldHitEntity = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 8B 55 0C 56 8B 75 08 57")));
		//static auto trace_filter_simple_vtable = *(uintptr_t*)(Memory::Scan("client_panorama.dll", "C7 44 24 ? ? ? ? ? F3 0F 5C D9") + 4);
		//auto ShouldHitEntity = (*(uintptr_t * *)trace_filter_simple_vtable)[0];
		//OriginalShouldHitEntity = (DWORD)DetourFunction((byte*)ShouldHitEntity, (byte*)Hooked::ShouldHitEntity);
		
		//static auto ApproachAngle = (DWORD)(Memory::Scan("client_panorama.dll", "F3 0F 59 05 A0 CB C3 10 0F"));
		//OriginalApproachAngle = (DWORD)DetourFunction((byte*)ApproachAngle, (byte*)Hooked::ApproachAngle);

		static auto ModifyEyePosition = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 83 E4 F8 83 EC 60 56 57 8B F9 89")));
		OriginalModifyEyePosition = (DWORD)DetourFunction((byte*)ModifyEyePosition, (byte*)Hooked::ModifyEyePosition);

		//static auto PhysicsSimulate = (DWORD)(Memory::Scan(sxor("client_panorama.dll"), sxor("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21")));
		//OriginalPhysicsSimulate = (DWORD)DetourFunction((byte*)PhysicsSimulate, (byte*)Hooked::PhysicsSimulate);

		static auto CL_Move = (DWORD)(Memory::Scan(VMProtectDecryptStringA("engine.dll"), VMProtectDecryptStringA("55 8B EC 81 EC 64 01 00 00 53 56 57 8B 3D")));
		OriginalCL_Move = (DWORD)DetourFunction((byte*)CL_Move, (byte*)Hooked::CL_Move);

		static auto GetRenderOrigin = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("8B D1 83 BA ? ? ? ? ? 8D 4A FC")));
		OriginalGetRenderOrigin = (DWORD)DetourFunction((byte*)GetRenderOrigin, (byte*)Hooked::GetRenderOrigin);
		
		static auto SetViewmodelOffsets = (DWORD)(Memory::Scan(VMProtectDecryptStringA("client_panorama.dll"), VMProtectDecryptStringA("55 8B EC 8B 45 08 F3 0F 7E 45")));
		OriginalSetViewmodelOffsets = (DWORD)DetourFunction((byte*)SetViewmodelOffsets, (byte*)Hooked::SetViewmodelOffsets);

		Window = FindWindowA(VMProtectDecryptStringA("Valve001"), NULL);
		Hooked::oldWindowProc = (WNDPROC)SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)Hooked::WndProc);

		feature::menu._menu_opened = true;

		ctx.ticks_allowed = 0;

		if (!parser::parse())
		{
#ifdef DEBUG
			Win32::Error("parser failed (Source::%s)", __FUNCTION__);
#endif
			//Destroy();
			create_log_file(sxor("ERROR 31"));
			Beep(400, 200);
			//return false;
		}
		ctx.knifes.clear();

		_events.push_back(_event(sxor("\t  ________  __    _____________     ")));
		_events.push_back(_event(sxor("\t  \\_____  \\|  | _/_   |\\____   \\ ")));
		_events.push_back(_event(sxor("\t   /  ____/|  |/ /|   |   /    /  ")));
		_events.push_back(_event(sxor("\t  /       \\|    < |   |  /    /   ")));
		_events.push_back(_event(sxor("\t  \\_______ \\__|_ \\|___| /____/  ")));
		_events.push_back(_event(sxor("\t          \\/    \\/            .club\n")));

		ctx.init_finished = true;


		//_events.push_back(_event(feature::misc.get_user_data()));
#ifdef AUTH
		auto data = feature::misc.get_user_data();


#endif

		return true;
		VMProtectBeginUltra("EntryPoint");
		SE_UNPROTECT_END
		/*erase_end;*/
	}

	void QueueJobs()
	{
		return;

		//lagcomp_mt adata;
		//Threading::QueueJobRef(Hooked::fill_players_list, &adata);

		//Threading::QueueJobRef(Hooked::find_player_who_can_autowall, &adata);
	}

	void Destroy()
	{
		//static const auto filter_simple = *reinterpret_cast<uint32_t*>(Memory::Scan("client_panorama.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52") + 0x3d);

		g_menu.on_unload();
		DetourRemove((byte*)OriginalUpdateClientSideAnimations, (byte*)Hooked::UpdateClientSideAnimation);
		//DetourRemove((byte*)OriginalCalcAbsoluteVelocity, (byte*)Hooked::CalcAbsoluteVelocity);
		DetourRemove((byte*)OriginalSetupVelocity, (byte*)Hooked::SetupVelocity);
		DetourRemove((byte*)OriginalStandardBlendingRules, (byte*)Hooked::StandardBlendingRules);
		DetourRemove((byte*)OriginalDoExtraBonesProcessing, (byte*)Hooked::DoExtraBonesProcessing);
		DetourRemove((byte*)OriginalGetForeignFallbackFontName, (byte*)Hooked::GetForeignFallbackFontName);
		//DetourRemove((byte*)OriginalPhysicsSimulate, (byte*)Hooked::PhysicsSimulate);
		DetourRemove((byte*)OriginalSetupBones, (byte*)Hooked::SetupBones);
		//DetourRemove((byte*)OriginalShouldHitEntity, (byte*)Hooked::ShouldHitEntity);
		//DetourRemove((byte*)OriginalApproachAngle, (byte*)Hooked::ApproachAngle);
		DetourRemove((byte*)OriginalModifyEyePosition, (byte*)Hooked::ModifyEyePosition);
		DetourRemove((byte*)OriginalCL_Move, (byte*)Hooked::CL_Move);
		DetourRemove((byte*)OriginalGetRenderOrigin, (byte*)Hooked::GetRenderOrigin);
		DetourRemove((byte*)OriginalSetViewmodelOffsets, (byte*)Hooked::SetViewmodelOffsets);

		for (auto head = ctx.m_effects_head(); head; head = head->next)
		{
			if (strstr(head->effectName, sxor("Impact")) && strlen(head->effectName) <= 8) {
				head->function = oImpact;
				break;
			}
		}

		//Memory::VFTableHook::HookManual<ShouldHitEntity_t>((uintptr_t*)filter_simple, 0, oShouldHitEntity);

		Engine::PropManager::Instance()->Hook(m_flSimulationTime, sxor("DT_BaseEntity"), sxor("m_flSimulationTime")); //unhook
		Engine::PropManager::Instance()->Hook(m_flAbsYaw, sxor("DT_CSRagdoll"), sxor("m_flAbsYaw"));
		Engine::PropManager::Instance()->Hook(m_nSequence, sxor("DT_BaseViewModel"), sxor("m_nSequence"));
		//Engine::PropManager::Instance()->Hook(m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget"); //unhook

		m_pBSPTreeQuerySwap.reset();
		m_pMaterialSystemSwap.reset();
		m_pClientSwap.reset();
		m_pPredictionSwap.reset();
		m_pPanelSwap.reset();
		m_pClientModeSwap.reset();
		m_pSurfaceSwap.reset();
		m_pRenderViewSwap.reset();
		m_pEngineSwap.reset();
		m_pClientStateSwap.reset();
		m_pEngineVGUISwap.reset();
		m_pModelRenderSwap.reset();
		m_pNetChannelSwap.reset();
		m_pDeviceSwap.reset();
		net_showfragmentsSwap.reset();
		m_pNetChannelSwap.reset();
		m_pEngineTraceSwap.reset();
		SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)Hooked::oldWindowProc);
	}

	void* CreateInterface(const std::string& image_name, const std::string& name, bool force /*= false */)
	{
#ifndef AUTH
		static std::ofstream offsets(sxor("test.txt"), std::ofstream::out);
#endif // 

		/*patch ppppp;
		ppppp.VClient = 0x4d383b0;
		ppppp.VClientEntityList = 0x4d5d6ac;
		ppppp.GameMovement = 0x5187968;
		ppppp.VClientPrediction = 0x5188ca0;
		ppppp.VEngineClient = 0x587140;
		ppppp.VGUI_Panel = 0x608c4;
		ppppp.VGUI_Surface = 0xf47c0;
		ppppp.VEngineVGui001 = 0x860860;
		ppppp.VEngineCvar = 0x3c270;
		ppppp.EngineTraceClient004 = 0x59353c;
		ppppp.VModelInfoClient004 = 0x599ae8;
		ppppp.InputSystemVersion001 = 0x31188;
		ppppp.VEngineModel016 = 0x76b5d0;
		ppppp.VEngineRenderView014 = 0x58e140;
		ppppp.VMaterialSystem080 = 0xdf420;
		ppppp.VPhysicsSurfaceProps001 = 0x116018;
		ppppp.VDebugOverlay004 = 0x589b10;
		ppppp.GAMEEVENTSMANAGER002 = 0x7e0c50;
		ppppp.StaticPropMgrServer002 = 0x85e2d0;
		ppppp.Localize_001 = 0x38a70;
		ppppp.MDLCache004 = 0x63da8;
		ppppp.IEngineSoundClient = 0x515494;*/

		//offsets << "patch ppppp;" << std::endl;


		auto image = GetModuleHandleA(image_name.c_str());

		if (!image)
			return nullptr;

		auto fn = (CreateInterfaceFn)(GetProcAddress(image, sxor("CreateInterface")));

		if (!fn)
			return nullptr;

		if (force) {
			auto factory = fn(name.c_str(), nullptr);
			//offsets << "0x" << std::hex << ((DWORD)factory - (DWORD)image) << std::endl;
			//offsets << "ppppp." << name.c_str() << " = 0x" << std::hex << ((DWORD)factory - (DWORD)image) << std::endl;
			return factory;
		}

		char format[1024] = { };

		for (auto i = 0u; i < 1000u; i++)
		{
			sprintf_s(format, sxor("%s%03u"), name.c_str(), i);

			auto factory = fn(format, nullptr);

			if (factory) {
				//offsets << "ppppp." << name.c_str() << " = 0x" << std::hex << ((DWORD)factory - (DWORD)image) << std::endl;
				return factory;
			}
		}

		return nullptr;
	}

	void* CreateInterface(const std::string& image_name, ULONG64 offset)
	{
		auto image = GetModuleHandleA(image_name.c_str());

		if (!image)
			return nullptr;

		auto _interface = (DWORD)image + offset;

		std::cout << std::hex << _interface << std::endl;

		return (void*)_interface;
	}
}

namespace feature
{
	c_menu menu;
	c_misc misc;
	c_antiaimbot anti_aim;
	c_resolver resolver;
	c_visuals visuals;
	c_usercmd usercmd;
	c_lagcomp lagcomp;
	c_chams chams;
	c_autowall autowall;
	c_aimbot ragebot;
	c_dormant_esp sound_parser;
	c_music_player music_player;
}

FORCEINLINE C_WeaponCSBaseGun* m_weapon()
{
	auto client = ctx.m_local();

	if (!client || client->IsDead() || client->m_hActiveWeapon() <= 0)
		return nullptr;

	return (C_WeaponCSBaseGun*)(csgo.m_entity_list()->GetClientEntityFromHandle(client->m_hActiveWeapon()));
}