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
#include "internet.hpp"
#include "grenades.hpp"
#include "weather_controller.hpp"
#include "sdk.hpp"
#include "linkedlist.hpp"
#include "aimbot.hpp"

//#include "threading/threading.h"
//#include "threading/shared_mutex.h"

//#include <thread>

#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MIN 4.0f
#define FIRSTPERSON_TO_THIRDPERSON_VERTICAL_TOLERANCE_MAX 10.0f
constexpr auto _box_pttrn = LIT(("\x85\xC0\x74\x2D\x83\x7D\x10\x00\x75\x1C"));

class VPlane
{
public:

	Vector		m_Normal;
	float		m_Dist;
};

typedef VPlane Frustum[6];

typedef void(__thiscall* StandardBlendingRules_t)(C_BasePlayer*, CStudioHdr*, Vector*, Quaternion*, float_t, int32_t);
typedef void(__thiscall* BuildTransformations_t)(C_BasePlayer*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, int, BYTE*);
typedef void(__thiscall* DataUpdate_universal_t)(IClientNetworkable*, int32_t);
typedef bool(__cdecl* ReportHit_t)(CCSUsrMsg_ReportHit*);
typedef int(__thiscall* SetViewmodelOffsets_t)(void*, int, float, float, float);
typedef bool(__stdcall* IsUsingStaticPropDebugModes_t)();
typedef bool(__thiscall* ShouldCollide_t)(C_BasePlayer*, int, int);
typedef float(__stdcall* AdjustFrameTime_t)(float);
typedef void(__thiscall* GetColorModulation_t)(void*, float*, float*, float*);
typedef void(__vectorcall* CL_Move_t)(float, bool);
typedef void(__stdcall* CL_ReadPackets_t)(bool);
typedef void(__stdcall* Host_RunFrame_Input_t)(float, bool);
typedef void(__stdcall* Host_RunFrame_Client_t)(bool);
typedef const char*(__thiscall* GetForeignFallbackFontName_t)(void*);
typedef const char*(__thiscall* GetForeignFallbackFontName_t)(void*);
typedef bool(__thiscall* ShouldSkipAnimationFrame_t)(C_BaseAnimating*);
typedef QAngle*(__thiscall* EyeAngles_t)(void*);
typedef void(__thiscall* PhysicsSimulate_t)(void*);
typedef void(__thiscall* CalcAbsoluteVelocity_t)(C_BasePlayer*);
typedef const Vector&(__thiscall* GetRenderOrigin_t)(C_BaseAnimating*);
typedef void(__thiscall* SetLocalOrigin_t)(void*, const Vector&);
typedef void(__thiscall* ModifyEyePosition_t)(CCSGOPlayerAnimState*, Vector&);
typedef void(__thiscall* DoExtraBonesProcessing_t)(uintptr_t*, CStudioHdr*, Vector*, Quaternion*, const matrix3x4_t&, uint8_t*, void*);
typedef void(__thiscall* UpdateClientSideAnimations_t)(C_BasePlayer*);
typedef void(__thiscall* SetupVelocity_t)(CCSGOPlayerAnimState*);
typedef void(__thiscall* SetUpMovement_t)(CCSGOPlayerAnimState*);
typedef void(__thiscall* RenderPopView_t)(void*, void*, Frustum);
typedef bool(__thiscall* ShouldFlipModel_t)(C_BaseViewModel*);
typedef bool(__thiscall* isHLTV_t)(IVEngineClient*);
typedef int(__stdcall* IsBoxVisible_t)(const Vector&, const Vector&);
typedef bool(__thiscall* SetupBones_t)(void*, matrix3x4_t*, int, int, float);
typedef void(__thiscall* CalcViewBob_t)(void*, Vector&);
//typedef void(__thiscall* PlayStepSound_t)(void*, Vector&, surface_data_t*, float, bool);
typedef void(__thiscall* AddViewModelBob_t)(C_PredictedViewModel*, void*, Vector&, QAngle&);
typedef void(__thiscall* FireEvents_t)(IVEngineClient*);
typedef INetChannelInfo*(__thiscall* GetNetChannelInfo_t)(IVEngineClient*);
typedef bool(__thiscall* OverrideConfig_t)(IMaterialSystem*, MaterialSystem_Config_t&, bool);
typedef float(__thiscall* GetAlphaModulation_t)(IMaterial*);
typedef bool(__thiscall* ShouldHitEntity_t)(void*, IHandleEntity*, int);
typedef void(__thiscall* ClipRayCollideable_t)(void*, const Ray_t&, uint32_t, ICollideable*, CGameTrace*);
typedef void(__thiscall* AttachmentHelper_t)(IClientRenderable*, void*);
typedef void(__thiscall* AnimEventHook_t)(void*, int);
typedef void(__thiscall* CheckForSequenceChange_t)(void*, void* , int , bool , bool );
typedef void(__thiscall* AdjustPlayerTimeBase_t)(void*, int);
typedef int(* ProcessInterpolatedList_t)(void);
using TraceRay_t = void(__thiscall*)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
using DrawSetColor_t = void(__thiscall*)(void*, int,int,int,int);
using CalcView_t = void(__thiscall*)(C_BasePlayer*, Vector&, QAngle&, float&, float&, float&);

/*
new drawmodel

Mark llama, [19.11.19 06:43]
( 53 8B 5D 08 56 57 8B F9 89 7C 24 18 ) - 12
new drawmodel
*/

DWORD OriginalSetupBones;
DWORD OriginalUpdateClientSideAnimations;
DWORD OriginalSetupVelocity;
DWORD OriginalSetUpMovement;
DWORD OriginalCalcAbsoluteVelocity;
DWORD OriginalStandardBlendingRules;
DWORD OriginalBuildTransformations;
DWORD OriginalShouldCollide;
DWORD OriginalDoExtraBonesProcessing;
DWORD OriginalGetForeignFallbackFontName;
DWORD OriginalShouldSkipAnimationFrame;
DWORD OriginalCL_ReadPackets;
DWORD OriginalModifyEyePosition;
DWORD OriginalPhysicsSimulate;
DWORD OriginalAdjustPlayerTimeBase;
DWORD OriginalCL_Move;
DWORD OriginalShouldHitEntity;
DWORD OriginalGetRenderOrigin;
DWORD OriginalSetViewmodelOffsets;
DWORD OriginalGetColorModulation;
DWORD OriginalIsUsingStaticPropDebugModes;
DWORD OriginalReportHit;
DWORD OriginalCalcView;
DWORD OriginalPreDataUpdate;
DWORD OriginalPostDataUpdate;
DWORD OriginalEyeAngles;
DWORD OriginalProcessInterpolatedList;
DWORD OriginalCalcViewBob;
DWORD OriginalAddViewModelBob;
DWORD OriginalSetLocalOrigin;
DWORD OriginalPlayStepSound;
DWORD OriginalAdjustFrameTime;
DWORD OriginalShouldFlipModel;
DWORD OriginalRenderPopView;
DWORD OriginalAnimEventHook;
DWORD OriginalCheckForSequenceChange;
DWORD OriginalHost_RunFrame_Input;
DWORD OriginalHost_RunFrame_Client;

GetAlphaModulation_t OriginalGetAlphaModulation;

RecvVarProxyFn m_flSimulationTime;
RecvVarProxyFn m_flLowerBodyYawTarget;
RecvVarProxyFn m_flAbsYaw;
RecvVarProxyFn m_flPlaybackRate;
RecvVarProxyFn m_flWeight;
RecvVarProxyFn m_flCycle;
RecvVarProxyFn m_Sequence;
RecvVarProxyFn m_nSequence;
RecvVarProxyFn m_flVelocityModifier;
RecvVarProxyFn m_vecForce;


ClientEffectCallback oImpact;

//ShouldHitEntity_t oShouldHitEntity = nullptr;

c_csgo	  csgo;
c_vmthooks	 vmt;
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
		vmt.m_engine->VCall<FireEvents_t>(59)(_this);
	}
	
	INetChannelInfo* __fastcall GetNetChannelInfo(IVEngineClient* _this, int edx)
	{
		//VIRTUALIZER_MUTATE_ONLY_START;
		static void* offs = nullptr;
		if (ctx.init_finished && !offs)
			offs = (void*)Memory::Scan(sxor("client.dll"), sxor("8B C8 85 C9 0F 84 ? ? ? ? 8B 01 51"));
		static auto ofc = vmt.m_engine->VCall<GetNetChannelInfo_t>(78);

		if (_ReturnAddress() == offs)
			return nullptr;
		//VIRTUALIZER_MUTATE_ONLY_END;

		return ofc(_this);
	}

	_declspec(noinline)void DoExtraBonesProcessing_Detour(uintptr_t* ecx, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context)
	{
		if (!ctx.is_updating_fake)
			return;
	}

	bool __cdecl ReportHit(CCSUsrMsg_ReportHit* info) {
		return ((ReportHit_t)OriginalReportHit)(info);
	}
	
	bool __fastcall ShouldSkipAnimationFrame(C_BaseAnimating* ecx, uint32_t*)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		auto player = reinterpret_cast<C_BasePlayer*>(uintptr_t(ecx) - 4);

		if (player && ctx.m_local() && (player == ctx.m_local() || player->m_iTeamNum() != ctx.m_local()->m_iTeamNum()))
			return false;

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER

		return reinterpret_cast<ShouldSkipAnimationFrame_t>(OriginalShouldSkipAnimationFrame)(ecx);
	}
	
	void __fastcall CheckForSequenceChange(void* ecx, int edx, void* hdr, int cur_sequence, bool force_new_sequence, bool interpolate)
	{
		reinterpret_cast<CheckForSequenceChange_t>(OriginalCheckForSequenceChange)(ecx, hdr, cur_sequence, force_new_sequence, false);
	}

	void __fastcall AnimEventHook(void* ecx, int edx, int data_type)
	{
		

		reinterpret_cast<AnimEventHook_t>(OriginalAnimEventHook)(ecx, data_type);
	}

	void __fastcall DoExtraBonesProcessing(uintptr_t* ecx, uint32_t, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& matrix, uint8_t* bone_computed, void* context)
	{
		DoExtraBonesProcessing_Detour(ecx, hdr, pos, q, matrix, bone_computed, context);
	}
	
	DECLSPEC_NOINLINE void modify_eye_position_server(CCSGOPlayerAnimState* ecx, Vector &pos)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		if (ecx && ecx->m_player != nullptr && (ecx->m_landing || ecx->m_anim_duck_amount != 0.0f || !ecx->m_player->m_hGroundEntity()))
		{
			int bone = ecx->m_player->LookupBone(sxor("head_0"));

			if (bone != -1)
			{
				Vector vecHeadPos;
				QAngle temp;
				ecx->m_player->GetBonePosition(bone, vecHeadPos, temp);

				auto flHeadHeight = vecHeadPos.z + 1.7f;
				if (pos.z > flHeadHeight)
				{
					auto tmp = 0.0f;
					tmp = (fabsf(pos.z - flHeadHeight) - 4.0f) * 0.16666667f;
					if (tmp >= 0.0f)
						tmp = fminf(tmp, 1.0f);
					pos.z = ((flHeadHeight - pos.z)
						* (((tmp * tmp) * 3.0f) - (((tmp * tmp) * 2.0f) * tmp)))
						+ pos.z;
				}
			}
		}
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void __fastcall CalcView(C_BasePlayer* m_player, uint32_t, Vector& m_eye_origin, QAngle& m_eye_angles, float& zNear, float& zFar, float& fov)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

		((CalcView_t)OriginalCalcView)(m_player, m_eye_origin, m_eye_angles, zNear, zFar, fov);

		if (ctx.m_local() != nullptr && m_player != nullptr) {
			//if (m_player == ctx.m_local())
			//{
			//	// Something is still overriding the local player's abs angles other than RunCommand, so just restore it here..
			//	//QAngle m_fixed_angles = { 0.0f, ctx.angles[ANGLE_REAL], 0.0f };
			//	//m_player->set_abs_angles(m_fixed_angles);
			//	//m_player->get_render_angles().y = m_fixed_angles.y;

			//	if (m_player->get_animation_state())
			//	{
			//		if (csgo.m_input()->is_in_tp() && *(bool*)(uintptr_t(m_player) + 0x2EA) && !m_player->IsDead()) //cl_camera_height_restriction_debug)
			//		{
			//			modify_eye_position_server(m_player->get_animation_state(), m_eye_origin);
			//		}
			//		else
			//		{
			//			if (m_player->m_iObserverMode() == 4)
			//			{
			//				if (ctx.m_local() && !ctx.m_local()->IsDead())
			//				{
			//					modify_eye_position_server(m_player->get_animation_state(), m_eye_origin);
			//				}
			//			}
			//		}
			//	}
			//}

			if (m_player == ctx.m_local() && ctx.fakeducking)
			{
				if (csgo.m_input()->m_fCameraInThirdPerson) // fix the up and down bob while fake ducking
					m_eye_origin.z = ctx.m_local()->get_abs_origin().z + 64.f;
			}

			m_player->delay_unscope(fov);

			if (m_player->m_nIsAutoMounting() > 0)
			{
				auto vectors = csgo.m_game_rules()->GetViewVectors();
				auto currentautomoveorigin = vectors->m_vDuckView + m_player->GetAutoMoveOrigin();
				auto automovetargetend = vectors->m_vDuckHullMax + m_player->GetAutomoveTargetEnd();

				float time = (csgo.m_globals()->curtime - (m_player->GetAutomoveTargetTime() - (m_player->GetAutomoveTargetTime() - m_player->GetAutomoveStartTime())))
					/ (m_player->GetAutomoveTargetTime() - m_player->GetAutomoveStartTime());
				time = Math::clamp(time, 0.0f, 1.0f);
				float fuck = powf(time, 0.6214906f);
				fuck = fminf(fuck + 0.5f, 1.0f);
				m_eye_origin = (automovetargetend - currentautomoveorigin) * fuck + currentautomoveorigin;
			}
		}

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}
	
	int ProcessInterpolatedList()
	{
		auto allow_extrapolation_addr = *(bool**)(Engine::Displacement::Signatures[c_signatures::ALLOW_EXTRAPOLATION] + 1);

		if (allow_extrapolation_addr)
			*allow_extrapolation_addr = false;

		return (ProcessInterpolatedList_t(OriginalProcessInterpolatedList))();
	}

	_declspec(noinline)const char* GetForeignFallbackFontName_Detour(uintptr_t* ecx)
	{
		if (strlen(Drawing::LastFontName) > 1)
			return Drawing::LastFontName;
		else
			return reinterpret_cast<GetForeignFallbackFontName_t>(OriginalGetForeignFallbackFontName)(ecx);
	}

	const char* __fastcall GetForeignFallbackFontName(uintptr_t* ecx, uint32_t)
	{
		return GetForeignFallbackFontName_Detour(ecx);
	}

	void __fastcall PhysicsSimulate(uintptr_t* ecx, uint32_t)
	{
		auto player = reinterpret_cast<C_BasePlayer*>(ecx);
		auto& m_nSimulationTick = *reinterpret_cast<int*>(uintptr_t(player) + 0x2AC);
		auto cctx = reinterpret_cast<C_CommandContext*>(uintptr_t(player) + 0x34FC);

		//C_CommandContext* ctx = player->GetCommandContext();
		//Assert(ctx);
		//Assert(ctx->needsprocessing);
		//if (!ctx->needsprocessing)
		//	return;	

		if (!player
			|| player->IsDead()
			|| csgo.m_globals()->tickcount == m_nSimulationTick
			|| player != ctx.m_local()
			|| !cctx->needsprocessing
			|| csgo.m_engine()->IsPlayingDemo()
			|| csgo.m_engine()->IsHLTV()
			|| player->m_fFlags() & 0x40)
		{
			reinterpret_cast<PhysicsSimulate_t>(OriginalPhysicsSimulate)(ecx);
			//ctx.did_set_shift = false;
			return;
		}

		player->m_vphysicsCollisionState() = 0;

		if ((ctx.hold_aim || !ctx.m_local()->IsDead()) && ctx.hold_angles == player->m_angEyeAngles())
			++ctx.hold_aim_ticks;

		auto pred = &Engine::Prediction::Instance();

		const bool bValid = (cctx->cmd.tick_count != 0x7FFFFFFF);

		if (!bValid)
		{
			//player->m_nTickBase()++;

			//auto shift_data = &pred->m_tickbase_array[cctx->cmd.command_number % 150];

			//if (shift_data->command_num == cctx->cmd.command_number && shift_data->charge)
			//	player->m_nTickBase() = shift_data->tickbase_original;

			auto* ndata = &pred->m_Data[cctx->cmd.command_number % 150];

			//if (!ndata->is_filled) {
			ndata->m_nTickBase = /*cctx->cmd.command_number < 0x7FFFFFFF ? (++player->m_nTickBase()) : */player->m_nTickBase();

			ndata->command_number = cctx->cmd.command_number;
			ndata->m_aimPunchAngle = player->m_aimPunchAngle();
			ndata->m_aimPunchAngleVel = player->m_aimPunchAngleVel();
			ndata->m_viewPunchAngle = player->m_viewPunchAngle();
			ndata->m_vecViewOffset = player->m_vecViewOffset();
			ndata->m_vecViewOffset.z = fminf(fmaxf(ndata->m_vecViewOffset.z, 46.0f), 64.0f);
			ndata->m_vecVelocity = player->m_vecVelocity();
			ndata->m_vecOrigin = player->m_vecOrigin();
			ndata->m_flFallVelocity = player->m_flFallVelocity();
			ndata->m_flDuckAmount = player->m_flDuckAmount();
			ndata->m_flVelocityModifier = player->m_flVelocityModifier();
			ndata->m_flThirdpersonRecoil = player->m_flThirdpersonRecoil();
			ndata->tick_count = cctx->cmd.tick_count;
			ndata->is_filled = true;

			m_nSimulationTick = csgo.m_globals()->tickcount;
			cctx->needsprocessing = false;

			//cctx->cmd.hasbeenpredicted = true;
			//}
			return;
		}
		else
		{
			const auto data = &pred->m_Data[(cctx->cmd.command_number - 1) % 150];

			if (data/* && data->m_nTickBase == player->m_nTickBase()*/ 
				&& data->command_number == (cctx->cmd.command_number - 1) 
				&& abs(data->command_number - cctx->cmd.command_number) <= 150)
			{
				pred->FixNetvarCompression(cctx->cmd.command_number - 1);
			}

			auto shift_data = &pred->m_tickbase_array[cctx->cmd.command_number % 150];

			const auto pre_change = player->m_nTickBase();

			if (shift_data->command_num == cctx->cmd.command_number && abs(shift_data->command_num - cctx->cmd.command_number) <= 150) {
				if (shift_data->tickbase_original > 0)
					player->m_nTickBase() = int(shift_data->tickbase_original);
				else if (shift_data->extra_shift != 0)
					player->m_nTickBase() += shift_data->extra_shift;

				shift_data->m_set = true;
				//_events.push_back({ "fixed SHIFT tb num: " + std::to_string(ctx.m_local()->m_nTickBase()) + " | cmd num: " + std::to_string(cctx->cmd.command_number) + " | old tb: " + std::to_string(pre_change) });
			}

			if (ctx.main_exploit == 1) {
				const auto pshift_data = &pred->m_tickbase_array[(cctx->cmd.command_number - 1) % 150];

				if (pshift_data->command_num == (cctx->cmd.command_number - 1) 
					&& abs(pshift_data->command_num - cctx->cmd.command_number) <= 150)
				{
					if (pshift_data->m_set && pshift_data->extra_shift != 0 && pshift_data->increace)
						player->m_nTickBase() += ~pshift_data->extra_shift;
				}
			}

			if (cctx->cmd.command_number == (csgo.m_client_state()->m_iCommandAck+1) 
				&& ctx.last_velocity_modifier >= 0.0f)
				player->m_flVelocityModifier() = ctx.last_velocity_modifier;

			((PhysicsSimulate_t)OriginalPhysicsSimulate)(ecx);

			//if (shift_data->command_num == cctx->cmd.command_number) {
			//	++player->m_nTickBase();
			//}

			auto* ndata = &pred->m_Data[cctx->cmd.command_number % 150];

			ndata->m_nTickBase = player->m_nTickBase();
			ndata->command_number = cctx->cmd.command_number;
			ndata->m_aimPunchAngle = player->m_aimPunchAngle();
			ndata->m_aimPunchAngleVel = player->m_aimPunchAngleVel();
			ndata->m_viewPunchAngle = player->m_viewPunchAngle();
			ndata->m_vecViewOffset = player->m_vecViewOffset();
			ndata->m_vecViewOffset.z = fminf(fmaxf(ndata->m_vecViewOffset.z, 46.0f), 64.0f);
			ndata->m_vecVelocity = player->m_vecVelocity();
			ndata->m_vecOrigin = player->m_vecOrigin();
			ndata->m_flFallVelocity = player->m_flFallVelocity();
			ndata->m_flThirdpersonRecoil = player->m_flThirdpersonRecoil();
			ndata->m_flDuckAmount = player->m_flDuckAmount();
			ndata->m_flVelocityModifier = player->m_flVelocityModifier();
			ndata->tick_count = csgo.m_globals()->tickcount;
			ndata->is_filled = true;

			if (!cctx->needsprocessing && ctx.last_frame_stage == 4)
				ctx.last_netvars_update_tick = cctx->cmd.command_number;

			//if (ctx.started_speedhack == csgo.m_globals()->tickcount)
			//{
			//	//ctx.m_local()->m_nTickBase() += ctx.exploit_tickbase_shift;
			//	ctx.m_local()->m_nTickBase() = tickbase
			//		+ ctx.m_local()->m_nTickBase()
			//		- (tickbase - 14);
			//}
		}
	}

	void __fastcall ClipRayCollideable(void* ecx, void* edx, const Ray_t& ray, uint32_t fMask, ICollideable* pCollide, CGameTrace* pTrace)
	{
		static auto ofc = vmt.m_engine_trace->VCall<ClipRayCollideable_t>(4);

		if (pCollide) {
			// extend the tracking
		const auto old_max = pCollide->OBBMaxs().z;

			//auto v9 = (C_BasePlayer*)(uintptr_t(pCollide) - 4);

		//	if (v9 && *(void**)v9 && !v9->IsDead() && *(int*)(uintptr_t(v9) + 0x64) < 64 && v9->get_weapon() && v9->get_weapon()->is_knife())
				pCollide->OBBMaxs().z += 5; // if the player is holding a knife and ducking in air we can still trace to this faggot and hit him

			ofc(ecx, ray, fMask, pCollide, pTrace);

			// restore Z
			pCollide->OBBMaxs().z = old_max;
		}
		else
			return ofc(ecx, ray, fMask, pCollide, pTrace);
	}

	void __fastcall TraceRay(void* thisptr, void*, const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)
	{
		static auto ofc = vmt.m_engine_trace->VCall<TraceRay_t>(5);

		//if (!ctx.in_hbp)
			return ofc(thisptr, ray, fMask, pTraceFilter, pTrace);
	}

	_declspec(noinline)void ModifyEyePosition_Detour(CCSGOPlayerAnimState* ecx, Vector& pos)
	{
		//auto islocal = ecx != nullptr && ecx->ent == ctx.m_local() && !ctx.m_local()->IsDead();

		ecx->m_smooth_height_valid = false;

		if (ctx.fix_modify_eye_pos && ecx != nullptr)
			((ModifyEyePosition_t)OriginalModifyEyePosition)(ecx, pos);
	}

	void __fastcall ModifyEyePosition(CCSGOPlayerAnimState* ecx, void* edx, Vector& pos)
	{
		ModifyEyePosition_Detour(ecx, pos);
	}
	
	_declspec(noinline)void CalcViewBob_Detour(C_BasePlayer* ecx, Vector& pos)
	{
		auto islocal = ecx != nullptr && ecx == ctx.m_local() && !ctx.m_local()->IsDead();

		if (!islocal)
			((CalcViewBob_t)OriginalCalcViewBob)(ecx, pos);
	}

	void __fastcall CalcViewBob(C_BasePlayer* ecx, void* edx, Vector& pos)
	{
		CalcViewBob_Detour(ecx, pos);
	}
	
	/*_declspec(noinline)void PlayStepSound_Detour(C_BasePlayer* ecx, Vector& vecOrigin, surface_data_t* psurface, float fvol, bool bForce)
	{
		if (ctx.process_movement_sound_fix)
			return;
		((PlayStepSound_t)OriginalPlayStepSound)(ecx, vecOrigin, psurface, fvol, bForce);
	}

	void __fastcall PlayStepSound(C_BasePlayer* ecx, void* edx, Vector& vecOrigin, surface_data_t* psurface, float fvol, bool bForce)
	{
		PlayStepSound_Detour(ecx, vecOrigin, psurface, fvol, bForce);
	}*/
	
	_declspec(noinline)void AddViewModelBob_Detour(C_PredictedViewModel* ecx, C_BasePlayer* owner, Vector& eyePosition, QAngle& eyeAngles)
	{
		const auto islocal = ecx != nullptr && owner == ctx.m_local() && !ctx.m_local()->IsDead();

		if (!islocal)
			reinterpret_cast<AddViewModelBob_t>(OriginalAddViewModelBob)(ecx, owner, eyePosition, eyeAngles);
	}

	void __fastcall AddViewModelBob(C_PredictedViewModel* ecx, void* edx, C_BasePlayer* owner, Vector& eyePosition, QAngle& eyeAngles)
	{
		AddViewModelBob_Detour(ecx, owner, eyePosition, eyeAngles);
	}

	_declspec(noinline)void crash_my_csgo(C_BasePlayer* ecx)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		C_BasePlayer* pEntity = ecx;
		auto v4 = pEntity && pEntity->entindex() < 64;
		if (pEntity)
		{
			if (pEntity == ctx.m_local())
			{
				if (!ctx.updating_anims)
				{
					C_AnimationLayer backuplayers[14];

					//static Vector old_origins[2] = { ctx.m_local()->get_abs_origin(), ctx.m_local()->get_abs_origin() };
					const auto old_angles = ctx.m_local()->get_abs_angles();

					memcpy(backuplayers, ctx.m_local()->animation_layers_ptr(), 0x38 * ctx.m_local()->get_animation_layers_count());

					ctx.allow_attachment_helper = true;
					const auto backup_poses = ctx.m_local()->m_flPoseParameter();
					ctx.m_local()->m_flPoseParameter() = ctx.poses[ANGLE_FAKE];
					ctx.m_local()->force_bone_rebuild();
					ctx.m_local()->set_abs_angles(QAngle(0, ctx.angles[ANGLE_FAKE], 0));
					memcpy(ctx.m_local()->animation_layers_ptr(), ctx.local_layers[ANGLE_FAKE], 0x38 * ctx.m_local()->get_animation_layers_count());

					ctx.m_local()->SetupBonesEx();
					memcpy(ctx.fake_matrix, ctx.m_local()->m_CachedBoneData().Base(), ctx.m_local()->m_bone_count() * sizeof(matrix3x4_t));
					ctx.m_local()->m_flPoseParameter() = backup_poses;
					feature::lagcomp->build_local_bones(ctx.m_local());
					memcpy(ctx.m_local()->animation_layers_ptr(), backuplayers, 0x38 * ctx.m_local()->get_animation_layers_count());
					ctx.allow_attachment_helper = false;
					ctx.m_local()->set_abs_angles(old_angles);
					return;
				}
				else
					return reinterpret_cast<UpdateClientSideAnimations_t>(OriginalUpdateClientSideAnimations)(pEntity);
			}
			else {
				if (ctx.updating_anims || pEntity->IsDormant() || !ctx.m_local() || ctx.m_local()->m_iTeamNum() == pEntity->m_iTeamNum() || pEntity->entindex() > 64)
					return reinterpret_cast<UpdateClientSideAnimations_t>(OriginalUpdateClientSideAnimations)(pEntity);

				/*if (!ctx.updating_anims && ctx.m_local()->m_iTeamNum() != pEntity->m_iTeamNum())
				{
					for (auto i = 0; i < pEntity->GetBoneCount(); i++) {
						Math::matrix_set_origin(pEntity->get_abs_origin(), pEntity->m_CachedBoneData().Base()[i]);
					}
				}*/
			}
		}
		else
			reinterpret_cast<UpdateClientSideAnimations_t>(OriginalUpdateClientSideAnimations)(pEntity);

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	bool copy_bone_cache(C_BasePlayer* pl, matrix3x4_t*a2, int a3)
	{
		if (!a2 || a3 == -1)
			return true;

		//int v4; // ecx

		if (pl->m_CachedBoneData().Count() < a3)
			return 0;

		//v4 = 128;

		//if (a3 <= 128)
		//	v4 = a3;

		//memcpy(a2, pl->m_CachedBoneData().Base(), 48 * v4);

		////*&*a2 = *&*pl->m_CachedBoneData().Base();
		////*(matrix3x4_t**)a2 = (matrix3x4_t*)&*pl->m_CachedBoneData().Base();
		//return 1;
		if (a2)
		{
			//auto v87 = pl->m_CachedBoneData().Count();

			//auto v4 = 128;

			//if (a3 <= 128)
			//	v4 = a3;

			//if (v4 >= v87)
			memcpy(a2, pl->m_CachedBoneData().Base(), 48 * pl->m_CachedBoneData().Count());
		}
		return 1;
	}

	bool __fastcall cl_clock_correction_get_bool(void* pConVar, void* ebx)
	{
		static auto CL_ReadPackets_ret = reinterpret_cast<void*>(Memory::Scan(sxor("engine.dll"), sxor("85 C0 0F 95 C0 84 C0 75 0C")));

		if (!csgo.m_engine()->IsInGame() || CL_ReadPackets_ret != _ReturnAddress())
			return vmt.m_cl_clock_correction->VCall<bool(__thiscall*)(void*)>(13)(pConVar);

		return false;
	}
	
	bool __fastcall cl_smooth_get_bool(void* pConVar, void* ebx)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		static auto GetPredictionErrorSmoothingVector_ret = reinterpret_cast<void*>(Memory::Scan(sxor("client.dll"), sxor("85 C0 0F 84 BF ? ? ? A1")));

		if (!csgo.m_engine()->IsInGame() || GetPredictionErrorSmoothingVector_ret != _ReturnAddress() || !(ctx.cheat_option_flags & hook_should_return_cl_smooth))
			return vmt.cl_smooth->VCall<bool(__thiscall*)(void*)>(13)(pConVar);

		ctx.cheat_option_flags &= ~hook_should_return_cl_smooth;

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
		return false;
	}

	void __fastcall DrawSetColor(void* _this, void* edx, int r, int g, int b, int a)
	{
		static auto ofc = vmt.m_surface->VCall<DrawSetColor_t>(15);

		ofc(_this, r, g, b, a);
	}

	void WriteUsercmd(void* buf, CUserCmd* incmd, CUserCmd* outcmd) {
		using WriteUsercmd_t = void(__fastcall*)(void*, CUserCmd*, CUserCmd*);
		static WriteUsercmd_t WriteUsercmdF = reinterpret_cast<WriteUsercmd_t>(Memory::Scan(
			sxor("client.dll"), sxor("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D")));

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
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		using Fn = bool(__thiscall*)(void*, int, void *, int, int, bool);
		static auto ofc = vmt.m_client->VCall<Fn>(24);

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
		bool is_instant = ctx.main_exploit == 2;

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

		auto* ucmd = csgo.m_input()->GetUserCmd(_from);
		if (!ucmd)
			return true;

		CUserCmd to_cmd{};
		CUserCmd from_cmd{};

		from_cmd = *ucmd;
		to_cmd = from_cmd;

		++to_cmd.command_number;
		to_cmd.tick_count += ctx.tickrate * 2;

		if (newcmds > choked_modifier)
			return true;

		for (int i = (choked_modifier - newcmds + 1); i > 0; --i)
		{
			WriteUsercmd( buf, &to_cmd, &from_cmd );

			from_cmd = to_cmd;
			++to_cmd.command_number;
			++to_cmd.tick_count;
		}

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER

		return true;
	}

	bool __fastcall SetupBones(void* ECX, void* EDX, matrix3x4_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		if (!ECX)
			return reinterpret_cast<SetupBones_t>(OriginalSetupBones)(0, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

		auto* v9 = reinterpret_cast<C_BasePlayer*>(uintptr_t(ECX) - 4);

		if (reinterpret_cast<int>(ECX) == 0x4 || v9 == nullptr || *reinterpret_cast<int*>(uintptr_t(v9) + 0x64) > 64 || v9->GetClientClass() != nullptr && v9->GetClientClass()->m_ClassID != class_ids::CCSPlayer)
			return reinterpret_cast<SetupBones_t>(OriginalSetupBones)(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

		const auto is_local = v9 == ctx.m_local() && v9 != nullptr;

		if (ctx.setup_bones || !ctx.m_local() || !is_local && v9->m_iTeamNum() == ctx.m_local()->m_iTeamNum() || v9->m_CachedBoneData().Count() == 0)
		{
			if (v9->GetModelPtr() && v9->GetModelPtr()->m_pStudioHdr) {
				for (auto i = 0; i < v9->GetModelPtr()->m_boneFlags.Count(); i++) {
					auto pBone = &v9->GetModelPtr()->m_boneFlags[i];

					if (!pBone)
						continue;

					*pBone &= ~5;
				}
			}

			for (int i = 0; i < v9->m_anim_overlay().Count(); ++i) {
				auto& elem = v9->m_anim_overlay().Element(i);

				if (v9 != elem.m_pOwner)
					elem.m_pOwner = (void*)v9;
			}

			auto prev_p = (/*!is_local ||*/ v9->get_animation_state() == nullptr) ? 0 : v9->get_animation_state()->m_weapon_last_bone_setup;

			if (prev_p != 0)
				v9->get_animation_state()->m_weapon_last_bone_setup = v9->get_animation_state()->m_weapon;

			const auto old_effects = v9->m_fEffects();
			v9->m_fEffects() |= 8;

			auto m_orig =  reinterpret_cast<SetupBones_t>(OriginalSetupBones)(ECX, pBoneToWorldOut, nMaxBones, boneMask, currentTime);

			v9->m_fEffects() = old_effects;

			if (prev_p != 0)
				v9->get_animation_state()->m_weapon_last_bone_setup = prev_p;

			return m_orig;
		}
		else {
			if (v9 != ctx.m_local())
			{
				const auto idx = *reinterpret_cast<int*>(uintptr_t(v9) + 0x64) - 1;

				if (idx < 64)
					return copy_bone_cache(v9, pBoneToWorldOut, nMaxBones);

				return v9->m_CachedBoneData().Count() > 0;
			}
			else
			{
				//auto idx = *(int*)(uintptr_t(v9) + 0x64) - 1;
				//const auto log = &feature::lagcomp->records[idx - 1];

				if (!pBoneToWorldOut || nMaxBones == -1)
					return true;

				if (pBoneToWorldOut)
				{
					memcpy(pBoneToWorldOut, v9->m_CachedBoneData().Base(), sizeof(matrix3x4_t) * v9->m_CachedBoneData().Count());
					return true;
				}

				return false;
			}
		}
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void __fastcall UpdateClientSideAnimation(C_BasePlayer* ecx, void* edx)
	{
		crash_my_csgo(ecx);
	}

	_declspec(noinline)bool __fastcall ShouldFlipModel(C_BaseViewModel* thisptr, void* edx)
	{
		auto m_original = ((ShouldFlipModel_t)OriginalShouldFlipModel)(thisptr);

		if (ctx.m_local() && thisptr && (CBaseHandle)ctx.m_local()->m_hViewModel() == thisptr->GetRefEHandle()
			&& ctx.latest_weapon_data && ctx.latest_weapon_data->pad_0090[0])
		{
			if (m_weapon() && m_weapon()->is_knife() && ctx.m_settings.misc_knife_hand_switch)
				return !m_original;
		}

		return m_original;
	}

	void __fastcall StandardBlendingRules(C_BasePlayer* ent, void* a2, CStudioHdr* hdr, Vector* pos, Quaternion* q, float curtime, int32_t bonemask)
	{
		if (ent != nullptr && ent && *reinterpret_cast<int*>(uintptr_t(ent) + 0x64) <= 64)
		{
			/*auto boneMask = bonemask;

			if (ctx.updating_resolver && ent != ctx.m_local())
				boneMask = 0x100;*/

			/*auto bone = ent->LookupBone(sxor("lean_root"));
			int b_flags = 0;

			if (bone != -1 && ent->GetModelPtr())
			{
				auto* pBone = &ent->GetModelPtr()->m_boneFlags[bone];
				if (pBone) {
					b_flags = *pBone;
					*pBone = 0;
				}
			}*/

			if (!(ent->m_fEffects() & 8))
				ent->m_fEffects() |= 8;

			reinterpret_cast<StandardBlendingRules_t>(OriginalStandardBlendingRules)(ent, hdr, pos, q, curtime, bonemask & ~0xC0000u);

			/*if (bone != -1 && ent->GetModelPtr())
			{
				auto* pBone = &ent->GetModelPtr()->m_boneFlags[bone];
				if (pBone)
					*pBone = b_flags;
			}*/

			if (ent->m_fEffects() & 8)
				ent->m_fEffects() &= ~8u;

			return;
		}

		reinterpret_cast<StandardBlendingRules_t>(OriginalStandardBlendingRules)(ent, hdr, pos, q, curtime, bonemask);
	}

	void __fastcall BuildTransformations(C_BasePlayer* ent, void* a2, CStudioHdr* hdr, Vector* pos, Quaternion* q, const matrix3x4_t& cameraTransform, int boneMask, BYTE* boneComputed)
	{
		if (ent != nullptr && ent && *reinterpret_cast<int*>(uintptr_t(ent) + 0x64) <= 64)
		{
			const auto flags = ent->m_fEffects();

			if (!(flags & 8))
				ent->m_fEffects() |= 8;
			//*reinterpret_cast<int*>(uintptr_t(ent) + 0x27D) = 0; //bye bye bone snapshots.

			reinterpret_cast<BuildTransformations_t>(OriginalBuildTransformations)(ent, hdr, pos, q, cameraTransform, boneMask, boneComputed);

			if (flags & 8)
				ent->m_fEffects() |= 8;

			return;
		}

		reinterpret_cast<BuildTransformations_t>(OriginalBuildTransformations)(ent, hdr, pos, q, cameraTransform, boneMask, boneComputed);
	}
	
	_declspec(noinline)int SetViewmodelOffsets_Detour(void* ent, int something, float x, float y, float z)
	{
		if (ctx.m_settings.visuals_viewmodel_control[0] != 0 || ctx.m_settings.visuals_viewmodel_control[1] != 0 || ctx.m_settings.visuals_viewmodel_control[2] != 0)
		{
			x = ctx.m_settings.visuals_viewmodel_control[0];
			y = ctx.m_settings.visuals_viewmodel_control[1];
			z = ctx.m_settings.visuals_viewmodel_control[2];
		}

		return reinterpret_cast<SetViewmodelOffsets_t>(OriginalSetViewmodelOffsets)(ent, something, x, y, z);
	}

	int __fastcall SetViewmodelOffsets(void* ecx, void* edx, int something, float x, float y, float z)
	{
		return SetViewmodelOffsets_Detour(ecx, something, x, y, z);
	}

	float __fastcall GetScreenAspectRatio(void* pEcx, void* pEdx, int32_t iWidth, int32_t iHeight)
	{
		using FnAR = float(__thiscall*)(void*, void*, int32_t, int32_t);
		auto original = vmt.m_engine->VCall<FnAR>(101)(pEcx, pEdx, iWidth, iHeight);

		if (ctx.m_settings.misc_aspect_ratio != 0)
			return float(ctx.m_settings.misc_aspect_ratio) / 80.f;
		else
			return ((float)ctx.screen_size.x / (float)ctx.screen_size.y);
	}

	void __fastcall GetColorModulation(IMaterial* material, void* edx, float* r, float* g, float* b)
	{
		(GetColorModulation_t(OriginalGetColorModulation))(material, r, g, b);

		if (ctx.m_settings.misc_visuals_world_modulation[1] && material != nullptr && !material->IsErrorMaterial() && !material->GetMaterialVarFlag(MATERIAL_VAR_UNUSED))
		{
			//const auto name = material->GetName();

			//// exclude stuff we dont want modulated
			//if (strstr(group, "Other") || strstr(name, "player") || strstr(name, "chams") ||
			//	strstr(name, "weapon") || strstr(name, "glow"))
			//	return;

			const auto group = hash_32_fnv1a_const(material->GetTextureGroupName());

			if (group != hash_32_fnv1a_const("World textures") && group != hash_32_fnv1a_const("StaticProp textures") && (group != hash_32_fnv1a_const("SkyBox textures")))
				return;

			const bool is_prop = (group == hash_32_fnv1a_const("StaticProp textures"));

			//const bool is_prop = strstr(group, "StaticProp");

			// PROP:  0.43f, 0.409f, 0.44f
			// WORLD: 0.13f, 0.109f, 0.14f

			const auto base_color = ctx.flt2color(ctx.m_settings.colors_world_color);



			*r *= (is_prop ? (max(1, base_color.r() * 1.f) / 255.f) : (max(1, base_color.r()) / 255.f));
			*g *= (is_prop ? (max(1, base_color.g() * 1.f) / 255.f) : (max(1, base_color.g()) / 255.f));
			*b *= (is_prop ? (max(1, base_color.b() * 1.f) / 255.f) : (max(1, base_color.b()) / 255.f));
		}
	}

	bool /*__stdcall*/ IsUsingStaticPropDebugModes()
	{
		const auto org = reinterpret_cast<IsUsingStaticPropDebugModes_t>(OriginalIsUsingStaticPropDebugModes)();//OriginalIsUsingStaticPropDebugModes();

		return (ctx.m_settings.misc_visuals_world_modulation[1] ? true : org);
	}

	bool __fastcall IsHLTV(IVEngineClient* _this, void* EDX)
	{
		static auto ofc = vmt.m_engine->VCall<isHLTV_t>(93);

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		static auto* const accumulate_layers_call = reinterpret_cast<void*>(Memory::Scan(sxor("client.dll"), sxor("84 C0 75 0D F6 87")));
		static auto* const setup_velocity = reinterpret_cast<void*>(Memory::Scan(sxor("client.dll"), sxor("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80")));

		if ((_ReturnAddress() == accumulate_layers_call
			|| _ReturnAddress() == setup_velocity) && ctx.m_local() && !ctx.m_local()->IsDead())
			return true;
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER

		return ofc(_this);
	}

	float old_remainder = 0;

	void __vectorcall CL_Move(float accumulated_extra_samples, bool bFinalTick)
	{
		ctx.is_in_teleport = false;
		ctx.skip_communication = true;

		if (ctx.m_local()) {
			ctx.ticks_allowed++;

			ctx.cmd_tickcount = csgo.m_globals()->tickcount;
			ctx.current_tickcount = csgo.m_globals()->tickcount;

			if (csgo.m_client_state() && csgo.m_client_state()->m_ptrNetChannel)
				ctx.out_sequence_nr = csgo.m_client_state()->m_ptrNetChannel->out_sequence_nr;
			else
				ctx.out_sequence_nr = 0;

			ctx.active_keybinds[1].mode = 0;
			ctx.active_keybinds[13].mode = 0;

			ctx.fakeducking_prev_state = ctx.fakeducking;
			ctx.fakeducking = ctx.get_key_press(ctx.m_settings.anti_aim_fakeduck_key) && ctx.m_local() != nullptr && ctx.m_local()->m_fFlags() & FL_ONGROUND && ctx.m_local()->m_flDuckSpeed() >= 4.0f;//&& int(1.0f / csgo.m_globals()->interval_per_tick) == 64;

			auto doubletap_toggled = ctx.get_key_press(ctx.m_settings.aimbot_doubletap_exploit_toggle);
			auto hideshots_toggled = ctx.get_key_press(ctx.m_settings.aimbot_hideshots_exploit_toggle);

			if (doubletap_toggled || hideshots_toggled) {
				ctx.exploit_allowed = true;
				ctx.has_exploit_toggled = true;

				if (ctx.main_exploit == 0)
					ctx.main_exploit = hideshots_toggled ? 1 : 2;
				else {
					if (!doubletap_toggled && ctx.main_exploit > 1 || ctx.prev_exploit_states[1] != hideshots_toggled && hideshots_toggled)
						ctx.main_exploit = 1;
					if (!hideshots_toggled && ctx.main_exploit < 2 || ctx.prev_exploit_states[0] != doubletap_toggled && doubletap_toggled)
						ctx.main_exploit = ctx.m_settings.aimbot_doubletap_method == 1 ? 2 : 3;
				}

				ctx.prev_exploit_states[0] = doubletap_toggled;
				ctx.prev_exploit_states[1] = hideshots_toggled;

				if (doubletap_toggled)
					ctx.active_keybinds[1].mode = ctx.m_settings.aimbot_doubletap_exploit_toggle.mode + 1;

				if (hideshots_toggled)
					ctx.active_keybinds[13].mode = ctx.m_settings.aimbot_hideshots_exploit_toggle.mode + 1;
			}
			else
			{
				ctx.ticks_allowed = 0;
				ctx.main_exploit = 0;
				ctx.exploit_allowed = false;
				ctx.has_exploit_toggled = false;
			}

			/*static bool lol = false;
			if (ctx.pressed_keys[VK_END] && !lol)
			{
				static auto AdjustPlayerTimeBase = (DWORD)(Memory::Scan(sxor("server.dll"), sxor("55 8B EC 51 53 56 8B 75 08 8B D9 85")));
				OriginalAdjustPlayerTimeBase = (DWORD)DetourFunction((byte*)AdjustPlayerTimeBase, (byte*)Hooked::AdjustPlayerTimeBase);
				lol = true;
			}*/
		}

		//readpacket fix
		//if (auto net = csgo.m_engine()->GetNetChannelInfo(); net == nullptr || !net->IsLoopback()) {
		//	(CL_ReadPackets_t(OriginalCL_ReadPackets))(bFinalTick);
		//
		//	rp_backup2.frametime = csgo.m_globals()->frametime;
		//	rp_backup2.curtime = csgo.m_globals()->curtime;
		//	rp_backup2.tickcount = csgo.m_globals()->tickcount;
		//	rp_backup2.cs_frametime = csgo.m_client_state()->m_flFrameTime;
		//
		//	/*rp_backup2.chocked_commands = csgo.m_client_state()->m_iChockedCommands;
		//	if (csgo.m_client_state()->m_ptrNetChannel) {
		//		rp_backup2.chocked_packets = csgo.m_client_state()->m_ptrNetChannel->choked_packets;
		//		rp_backup2.out_seq_nr = csgo.m_client_state()->m_ptrNetChannel->out_sequence_nr;
		//	}*/
		//
		//	csgo.m_globals()->frametime = rp_backup1.frametime;
		//	csgo.m_globals()->curtime = rp_backup1.curtime;
		//	csgo.m_globals()->tickcount = rp_backup1.tickcount;
		//	csgo.m_client_state()->m_flFrameTime = rp_backup1.cs_frametime;
		//	/*csgo.m_client_state()->m_iChockedCommands = rp_backup1.chocked_commands;
		//	if (csgo.m_client_state()->m_ptrNetChannel) {
		//	csgo.m_client_state()->m_ptrNetChannel->choked_packets = rp_backup1.chocked_packets;
		//	csgo.m_client_state()->m_ptrNetChannel->out_sequence_nr = rp_backup1.out_seq_nr;
		//	}*/
		//}

		if (ctx.has_exploit_toggled && !ctx.fakeducking)
		{
			auto can_charge = false;

			if (ctx.latest_weapon_data != nullptr)
			{
				auto charge = std::fmaxf(ctx.latest_weapon_data->flCycleTime, 0.5f);

				if (ctx.fakeducking_prev_state)
				{
					ctx.last_speedhack_time = csgo.m_globals()->realtime;
					can_charge = false;
				}

				if (std::fabsf(csgo.m_globals()->realtime - ctx.last_speedhack_time) >= charge)
					can_charge = true;
			}

			if (ctx.ticks_allowed < 15 && (csgo.m_globals()->realtime - ctx.last_speedhack_time) > 0.25f && !ctx.air_stuck && ctx.exploit_allowed && can_charge && m_weapon() != nullptr && !m_weapon()->IsGrenade()) { // createmove
				ctx.is_charging = true;
				ctx.charged_commands++;
				ctx.last_time_charged = csgo.m_globals()->realtime;
				csgo.m_globals()->interpolation_amount = 0.f;

				//auto v13 = csgo.m_client_state()->m_iLastOutgoingCommand + 1 + ctx.charged_commands;

				//auto shift_data = &Engine::Prediction::Instance()->m_tickbase_array[v13 % 150];

				//if (shift_data)
				//{
				//	shift_data->command_num = v13;
				//	shift_data->increace = false;
				//	shift_data->charge = true;
				//	shift_data->tickbase_original = ctx.charged_tickbase + ctx.charged_commands;
				//	//_events.push_back({"tb: " + std::to_string(nCorrectedTick)});
				//}
				//++ctx.m_local()->m_nTickBase();
				return;
			}
		}

		(CL_Move_t(OriginalCL_Move))(accumulated_extra_samples, bFinalTick);

		if (ctx.doubletap_now) {
			auto v13 = csgo.m_client_state()->m_iLastOutgoingCommand + 1;

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

			int nCorrectedTick = TIME_TO_TICKS(0.03f) - (13 + 1 + csgo.m_client_state()->m_iChockedCommands) + csgo.m_globals()->tickcount + ctx.last_cmd_delta + 1;

			auto shift_data = &Engine::Prediction::Instance()->m_tickbase_array[v13 % 150];

			if (shift_data)
			{
				shift_data->command_num = v13;
				shift_data->increace = true;
				shift_data->doubletap = true;
				shift_data->tickbase_original = nCorrectedTick;
				//_events.push_back({"tb: " + std::to_string(nCorrectedTick)});
			}

			ctx.m_local()->m_nTickBase() = nCorrectedTick;

			csgo.m_prediction()->m_nCommandsPredicted = 0;
			csgo.m_prediction()->m_previous_startframe = -1;
			csgo.m_prediction()->m_bPreviousAckHadErrors = 1;
			ctx.started_speedhack = ctx.current_tickcount;

			ctx.autopeek_back = true;
			ctx.allow_shooting = 0;

			auto i = 0;
			do
			{

				//ctx.speed_hacking = true;
				(CL_Move_t(OriginalCL_Move))(accumulated_extra_samples, bFinalTick);
				//ctx.speed_hacking = false;
				--ctx.ticks_allowed;

				if (i == 12 || csgo.m_client_state()->m_iChockedCommands >= 61) {
				//	ctx.force_aimbot = 10;
					ctx.speed_hacking = false;
				}

				++i;
			} while (i < 13);

			ctx.speed_hacking = false;

			//csgo.m_prediction()->Update(csgo.m_client_state()->m_iDeltaTick, true, csgo.m_client_state()->m_iLastCommandAck,
			//	csgo.m_client_state()->m_iLastOutgoingCommand + csgo.m_client_state()->m_iChockedCommands);

			ctx.doubletap_now = false;
		}

		if (!ctx.skip_communication && ctx.m_settings.aimbot_enabled)
		{
			if (!ctx.m_local()
				|| ctx.m_local()->IsDead()
				|| ctx.m_local()->m_fFlags() & 0x40
				|| csgo.m_game_rules() && csgo.m_game_rules()->IsFreezeTime())
				return;
			else if (csgo.m_client_state() != nullptr) {
				INetChannel* net_channel = csgo.m_client_state()->m_ptrNetChannel;
				if (net_channel != nullptr && !(csgo.m_client_state()->m_iChockedCommands % 4)) {
					const auto current_choke = net_channel->choked_packets;
					const auto out_sequence_nr = net_channel->out_sequence_nr;

					net_channel->choked_packets = 0;
					net_channel->out_sequence_nr = ctx.out_sequence_nr;

					net_channel->send_datagram(0);
					//ctx.did_communicate = true;

					net_channel->out_sequence_nr = out_sequence_nr;
					net_channel->choked_packets = current_choke;
				}
			}
		}
	}

	void m_flSimulationTimeHook(CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		//C_BasePlayer* m_player = (C_BasePlayer*)pStruct;

		if (m_flSimulationTime)
			m_flSimulationTime(pData, pStruct, pOut);
	}

	//void m_flPlaybackRate(CRecvProxyData* pData, void* pStruct, void* pOut) {
	//	// PlaybackRate

	//	auto pAnimOverlay = (C_AnimationLayer*)pStruct;
	//	if (pAnimOverlay) {
	//		auto player = (C_BasePlayer*)pAnimOverlay->m_pOwner;
	//		if (player && player == ctx.m_local())
	//		{
	//			if (pAnimOverlay->m_nOrder == 4)
	//				pAnimOverlay->m_flPlaybackRate = ctx.local_layers[ANGLE_POSDELTA][4].m_flPlaybackRate;
	//			else if(pAnimOverlay->m_nOrder == 5)
	//				pAnimOverlay->m_flPlaybackRate = ctx.local_layers[ANGLE_POSDELTA][5].m_flPlaybackRate;
	//		}
	//	}

	//	if (m_flSimulationTime)
	//		m_flSimulationTime(pData, pStruct, pOut);
	//}

	void m_flAbsYawHook(CRecvProxyData* pData, void* pStruct, void* pOut)
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		static auto m_hPlayer = Engine::PropManager::Instance()->GetOffset(sxor("DT_CSRagdoll"), sxor("m_hPlayer"));
		const auto player_handle = reinterpret_cast<CBaseHandle*>(reinterpret_cast<DWORD>(pStruct) + m_hPlayer);
		const auto abs_yaw = Math::normalize_angle(pData->m_Value.m_Float);

		if (*player_handle != 0xFFFFFFFF && *player_handle != -1 && ctx.m_local() != nullptr)
		{
			C_BasePlayer* hplayer = dynamic_cast<C_BasePlayer*>(csgo.m_entity_list()->GetClientEntityFromHandle(*player_handle));

			if (hplayer && hplayer->GetIClientEntity() != nullptr)
			{
				auto* player = (C_BasePlayer*)hplayer->GetIClientEntity()->GetBaseEntity();
				if (player != nullptr && player->entindex() > 0 && player->entindex() < 64 && player->entindex() != ctx.m_local()->entindex() && player->m_iTeamNum() != ctx.m_local()->m_iTeamNum())
				{
					player_info info;
					resolver_records* r_log = &feature::resolver->player_records[player->entindex() - 1];

					if (r_log != nullptr && abs(csgo.m_globals()->tickcount - r_log->did_store_abs_yaw) > 15 
						&& csgo.m_engine()->GetPlayerInfo(player->entindex(), &info) && !info.fakeplayer)
					{
						const auto delta = Math::angle_diff(abs_yaw, player->m_angEyeAngles().y);
						const auto r_method = delta > 0.f ? 2 : 1;

						if (abs(delta) > 0.f && abs(delta) < 65.f) {
							r_log->last_abs_yaw_side = r_method;

							//if (r_method == r_log->anims_resolving)
							//	prefer anims over other ones then? 

							if (abs(Math::AngleDiff(r_log->last_angle.y, player->m_angEyeAngles().y)) <= 120.f && (r_log->last_desync_delta - abs(delta)) > 5.f)
							{
								r_log->last_low_delta = abs(delta);
								r_log->last_low_delta_diff = r_log->last_desync_delta - abs(delta);

								if (r_log->last_low_delta < 41.f)
									r_log->last_low_delta_time = csgo.m_globals()->realtime;
							}
							else
								r_log->last_low_delta = 60.f;


							//_events.push_back(_event(std::string(sxor("Hit update arrived (") + std::string(info.name) + sxor(").")), std::string("server-side abs_yaw arrived [" + std::to_string(abs(delta)) + "]:[r" + std::to_string(r_method) + "]")));
						}

						r_log->did_store_abs_yaw = csgo.m_globals()->tickcount;
					}
				}
			}
		}

		m_flAbsYaw(pData, pStruct, pOut);

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	/*struct 
	{
		int last_tick_triggered = 0;
		int first_value = 0;
	} c_layers_handler;*/

	void m_flPlaybackHook(CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		m_flPlaybackRate(pData, pStruct, pOut);
		auto pAnimOverlay = (C_AnimationLayer*)pStruct;
		if (pAnimOverlay) {
			auto player = (C_BasePlayer*)pAnimOverlay->m_pOwner;
			if (!player || player != ctx.m_local() || (pAnimOverlay->m_nOrder != 4 && pAnimOverlay->m_nOrder != 5) || (csgo.m_globals()->realtime - ctx.last_time_layers_fixed) > 0.5f)
				return;

			pAnimOverlay->m_flPlaybackRate = ctx.local_layers[ANGLE_POSDELTA][pAnimOverlay->m_nOrder].m_flPlaybackRate;
		}
	}

	void m_SequenceHook(CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		m_Sequence(pData, pStruct, pOut);
		auto pAnimOverlay = (C_AnimationLayer*)pStruct;
		if (pAnimOverlay) {
			auto player = (C_BasePlayer*)pAnimOverlay->m_pOwner;
			if (!player || player != ctx.m_local() || (pAnimOverlay->m_nOrder != 4 && pAnimOverlay->m_nOrder != 5) || (csgo.m_globals()->realtime - ctx.last_time_layers_fixed) > 0.5f)
				return;

			pAnimOverlay->m_nSequence = ctx.local_layers[ANGLE_POSDELTA][pAnimOverlay->m_nOrder].m_nSequence;
		}
	}

	void m_flWeightHook(CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		m_flWeight(pData, pStruct, pOut);
		auto pAnimOverlay = (C_AnimationLayer*)pStruct;
		if (pAnimOverlay) {
			auto player = (C_BasePlayer*)pAnimOverlay->m_pOwner;
			if (!player || player != ctx.m_local() || (pAnimOverlay->m_nOrder != 4 && pAnimOverlay->m_nOrder != 5) || (csgo.m_globals()->realtime - ctx.last_time_layers_fixed) > 0.5f)
				return;

			pAnimOverlay->m_flWeight = ctx.local_layers[ANGLE_POSDELTA][pAnimOverlay->m_nOrder].m_flWeight;

		}
	}
		
	void m_flCycleHook(CRecvProxyData* pData, void* pStruct, void* pOut)
	{
		m_flCycle(pData, pStruct, pOut);
		auto pAnimOverlay = (C_AnimationLayer*)pStruct;
		if (pAnimOverlay) {
			auto player = (C_BasePlayer*)pAnimOverlay->m_pOwner;
			if (!player || player != ctx.m_local() || (pAnimOverlay->m_nOrder != 4 && pAnimOverlay->m_nOrder != 5) || (csgo.m_globals()->realtime - ctx.last_time_layers_fixed) > 0.5f)
				return;

			pAnimOverlay->m_flCycle = ctx.local_layers[ANGLE_POSDELTA][pAnimOverlay->m_nOrder].m_flCycle;
			
		}
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

					feature::anti_aim->lby_timer = m_player->m_flSimulationTime() + v15;
				}
			}
		}

		m_flLowerBodyYawTarget(pData, pStruct, pOut);
	}*/

	int	__stdcall IsBoxVisible(const Vector& mins, const Vector& maxs)
	{
		static auto ofc = vmt.m_engine->VCall<IsBoxVisible_t>(32);

		if (!memcmp(_ReturnAddress(), _box_pttrn.operator std::string().c_str(), 10))
			return 1;

		return ofc(mins, maxs);
	}

	bool __fastcall OverrideConfig(IMaterialSystem* ecx, void* edx, MaterialSystem_Config_t& config, bool bForceUpdate)
	{
		static auto ofc = vmt.m_material_system->VCall<OverrideConfig_t>(21);

		if (ctx.m_settings.misc_visuals_world_modulation[0])
			config.m_nFullbright = true;

		return ofc(ecx, config, bForceUpdate);
	}
	
	float __fastcall GetAlphaModulation(IMaterial* ecx, void* edx)
	{
		static auto ofc = (GetAlphaModulation_t)OriginalGetAlphaModulation;

		if (!ecx || ecx->IsErrorMaterial())
			return ofc(ecx);

		const auto get_group = hash_32_fnv1a_const(ecx->GetTextureGroupName());

		if (get_group != hash_32_fnv1a_const("StaticProp textures"))
			return ofc(ecx);

		return 0.67f; //your value
	}

	_declspec(noinline)bool ShouldHitEntity_Detour(void* pThis, IClientEntity* pHandleEntity, int contentsMask)
	{
		if ((DWORD)pHandleEntity == 0x7f7fffff || (DWORD)pHandleEntity < 0x20000)
			return false;

		if (ctx.force_low_quality_autowalling)
		{
			//if (auto Entity = pHandleEntity->GetBaseEntity())
			//{
			//	// Entities have a "Disable Bone Followers" thingy
			//	//if (entity_cast<CBoneFollower>(Entity))	// Really stupid fix that I don't know why it's needed... Here, inside ShouldHitEntity it's a CBoneFollower but the trace contains a prop_dynamic
			//	//{											// If I search for GetModelKeyValueText I find some bone following shit but that still doesn't explain the discrepancy between this function and the trace
			//	//	if (auto Owner = Entity->GetOwnerEntity())
			//	//		pHandleEntity = Owner;
			//	//}
			//	if (hash_32_fnv1a(Entity->GetClientClass()->m_pNetworkName + strlen(Entity->GetClientClass()->m_pNetworkName) - 10, 10) == hash_32_fnv1a_const("Projectile")) // IsProjectile function maybe
			//		return false;
			//}

			if (pHandleEntity == feature::grenades->m_collision_ent)//feature::grenades->is_broken((C_BasePlayer*)pHandleEntity))
				return false;
		}

		const auto result = reinterpret_cast<ShouldHitEntity_t>(OriginalShouldHitEntity)(pThis, pHandleEntity, contentsMask);

		if (result && csgo.m_static_prop()->IsStaticProp(pHandleEntity))
			return false;

		return result;
	}

	bool __fastcall ShouldHitEntity(void* pThis, void* edx, IClientEntity* pHandleEntity, int contentsMask)
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
		static auto ofc = vmt.m_engine->VCall<Fn>(27);

		//static void* is_loadout_allowed = (void*)(Memory::Scan(sxor("client.dll"), sxor("84 C0 75 04 B0 01 5F")));

		if (ctx.m_settings.misc_unlock_inventory && csgo.m_engine()->IsInGame() && _ReturnAddress() == (void*)(Engine::Displacement::Signatures[c_signatures::RET_ISLOADOUT_ALLOWED]))
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

		auto* const owner = csgo.m_entity_list()->GetClientEntityFromHandle((CBaseHandle)entity->m_hOwner());
		if (owner != ctx.m_local() || entity->get_viewmodel_weapon() == -1)
			return;

		auto* const view_model_weapon = (C_WeaponCSBaseGun*)csgo.m_entity_list()->GetClientEntityFromHandle(entity->get_viewmodel_weapon());

		if (!view_model_weapon || !view_model_weapon->is_knife())
			return;

		auto idx = view_model_weapon->m_iItemDefinitionIndex();

		const auto* const override_model = parser::knifes.list[ctx.m_settings.skinchanger_knife].model_player_path.c_str();

		auto& sequence = data->m_Value.m_Int;
		sequence = GetNewAnimation(hash_32_fnv1a_const(override_model), sequence, entity);
	}

	void m_nSequenceHook(CRecvProxyData* proxy_data_const, void* entity, void* output) {
		// Remove the constness from the proxy data allowing us to make changes.
		auto* const proxy_data = const_cast<CRecvProxyData*>(proxy_data_const);

		auto* const view_model = static_cast<C_BaseViewModel*>(entity);

		DoSequenceRemapping(proxy_data, view_model);

		// Call the original function with our edited data.
		m_nSequence(proxy_data_const, entity, output);
	}

	void m_flVelocityModifierHook(CRecvProxyData* data, void* entity, void* output) {
		m_flVelocityModifier(data, entity, output);

		if (entity)
		{
			auto ent = (C_BasePlayer*)((C_BasePlayer*)entity)->GetBaseEntity();

			if (ent->entindex() == csgo.m_engine()->GetLocalPlayer())
			{
				if (data->m_Value.m_Float != ctx.last_velocity_modifier)
				{
					if ((data->m_Value.m_Float - ctx.last_velocity_modifier) > 0.00625f) {
						if (ctx.m_settings.anti_aim_automatic_side[0])
							ctx.fside *= -1;
					}

					ctx.last_velocity_modifier = data->m_Value.m_Float;
					csgo.m_prediction()->m_bPreviousAckHadErrors = 1;
				}
			}
		}
	}

	void m_vecForceHook(CRecvProxyData* pData, void* pStruct, void* pOut) {
		// convert to ragdoll.
		static auto m_hPlayer = Engine::PropManager::Instance()->GetOffset(sxor("DT_CSRagdoll"), sxor("m_hPlayer"));
		const auto player_handle = reinterpret_cast<CBaseHandle*>(reinterpret_cast<DWORD>(pStruct) + m_hPlayer);
		player_info info;

		if (*player_handle != 0xFFFFFFFF && *player_handle != -1 && ctx.m_local() != nullptr && ctx.m_settings.misc_ragdoll_force)
		{
			//if (ctx.m_local()->m_iTeamNum() ) {
				// get m_vecForce.
				Vector vel = { pData->m_Value.m_Vector[0], pData->m_Value.m_Vector[1], pData->m_Value.m_Vector[2] };

				vel.x = 0;
				vel.y = 0;
				if (vel.z <= 1.f)
					vel.z = 2.f;

				// give some speed to all directions.
				vel.z *= 2000.f;

				// boost z up a bit.
				//if (vel.z <= 1.f)
				//	vel.z = 2.f;

				//vel.z *= 2.f;

				// don't want crazy values for this... probably unlikely though?
				//Math::clamp(vel.x, FLT_MIN, FLT_MAX);
				//Math::clamp(vel.y, FLT_MIN, FLT_MAX);
				Math::clamp(vel.z, FLT_MIN, FLT_MAX);

				// set new velocity.
				pData->m_Value.m_Vector[0] = vel.x;
				pData->m_Value.m_Vector[1] = vel.y;
				pData->m_Value.m_Vector[2] = vel.z;
			//}
		}

		if (m_vecForce)
			m_vecForce(pData, pStruct, pOut);
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
static ULONG64 VClient = 0xDEADBABE;
static ULONG64 VClientEntityList = 0xDEADBABE + 1;
static ULONG64 GameMovement = 0xDEADBABE + 2;
static ULONG64 VClientPrediction = 0xDEADBABE + 3;
static ULONG64 VEngineClient = 0xDEADBABE + 4;
static ULONG64 VGUI_Panel = 0xDEADBABE + 5;
static ULONG64 VGUI_Surface = 0xDEADBABE + 6;
static ULONG64 VEngineVGui001 = 0xDEADBABE + 7;
static ULONG64 VEngineCvar = 0xDEADBABE + 8;
static ULONG64 EngineTraceClient004 = 0xDEADBABE + 9;
static ULONG64 VModelInfoClient004 = 0xDEADBABE + 10;
static ULONG64 InputSystemVersion001 = 0xDEADBABE + 11;
static ULONG64 VEngineModel016 = 0xDEADBABE + 12;
static ULONG64 VEngineRenderView014 = 0xDEADBABE + 13;
static ULONG64 VMaterialSystem080 = 0xDEADBABE + 14;
static ULONG64 VPhysicsSurfaceProps001 = 0xDEADBABE + 15;
static ULONG64 VDebugOverlay004 = 0xDEADBABE + 16;
static ULONG64 GAMEEVENTSMANAGER002 = 0xDEADBABE + 17;
static ULONG64 StaticPropMgrServer002 = 0xDEADBABE + 18;
static ULONG64 Localize_001 = 0xDEADBABE + 19;
static ULONG64 MDLCache004 = 0xDEADBABE + 20;
static ULONG64 IEngineSoundClient = 0xDEADBABE + 21;

//#define SE_UNPROTECT_START __asm _emit 0xEB\
//	__asm _emit 0x09\
//	__asm _emit 0x53\
//	__asm _emit 0x45\
//	__asm _emit 0x55\
//	__asm _emit 0x50\
//	__asm _emit 0x42\
//	__asm _emit 0x45\
//	__asm _emit 0x47\
//	__asm _emit 0x4E\
//	__asm _emit 0x00;
//
//#define SE_UNPROTECT_END __asm _emit 0xEB\
//	__asm _emit 0x09\
//	__asm _emit 0x53\
//	__asm _emit 0x45\
//	__asm _emit 0x55\
//	__asm _emit 0x50\
//	__asm _emit 0x45\
//	__asm _emit 0x4E\
//	__asm _emit 0x44\
//	__asm _emit 0x50\
//	__asm _emit 0x00;

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

	/*struct patch
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
	};*/

	DECLSPEC_NOINLINE bool Create()
	{
		//SE_UNPROTECT_START
		//("EntryPoint");
		VIRTUALIZER_FISH_LITE_START;
		VIRTUALIZER_STR_ENCRYPT_START;

		auto& pPropManager = Engine::PropManager::Instance();

		//patch ppppp;

#ifndef AUTH
		csgo.m_client.set((IBaseClientDLL*)CreateInterface("client.dll", "VClient"));

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

		csgo.m_entity_list.set((IClientEntityList*)CreateInterface("client.dll", "VClientEntityList"));

		if (!csgo.m_entity_list())
		{
#ifdef DEBUG
			Win32::Error("IClientEntityList is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_movement.set((IGameMovement*)CreateInterface("client.dll", "GameMovement"));

		if (!csgo.m_movement())
		{
#ifdef DEBUG
			Win32::Error("IGameMovement is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_prediction.set((IPrediction*)CreateInterface("client.dll", "VClientPrediction"));

		if (!csgo.m_prediction())
		{
#ifdef DEBUG
			Win32::Error("IPrediction is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_move_helper.set((IMoveHelper*)(Engine::Displacement::Signatures[MOVEHELPER]));

		if (!csgo.m_move_helper())
		{
#ifdef DEBUG
			Win32::Error("IMoveHelper is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_input.set((IInput*)(Engine::Displacement::Signatures[IINPUT]));

		if (!csgo.m_input())
		{
#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		csgo.m_globals.set(**(CGlobalVarsBase ***)((*(DWORD**)csgo.m_client())[0] + 0x1F));

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

		csgo.m_client_state.set((CClientState*)(**(std::uintptr_t**)(Engine::Displacement::Signatures[CLIENT_STATE] + 1))); //ik we can grab it from vfunc but imma just took what i did b4

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

		csgo.m_beams.set(*(IViewRenderBeams**)(Engine::Displacement::Signatures[BEAMS] + 1));

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

		csgo.m_glow_object.set(*reinterpret_cast<CGlowObjectManager**>(Engine::Displacement::Signatures[GLOW_OBJECTS] + 3));

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
		csgo.m_client.set((IBaseClientDLL*)CreateInterface( VClient));

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

		csgo.m_entity_list.set((IClientEntityList*)CreateInterface( VClientEntityList));

		if (!csgo.m_entity_list())
		{
		#ifdef DEBUG
			Win32::Error("IClientEntityList is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 3"));
			return false;
		}

		csgo.m_movement.set((IGameMovement*)CreateInterface( GameMovement));

		if (!csgo.m_movement())
		{
		#ifdef DEBUG
			Win32::Error("IGameMovement is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 4"));
			return false;
		}

		csgo.m_prediction.set((IPrediction*)CreateInterface( VClientPrediction));

		if (!csgo.m_prediction())
		{
		#ifdef DEBUG
			Win32::Error("IPrediction is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			Beep(550, 200);
			create_log_file(sxor("ERROR 5"));
			return false;
		}

		csgo.m_move_helper.set((IMoveHelper*)(Engine::Displacement::Signatures[MOVEHELPER]));

		if (!csgo.m_move_helper())
		{
		#ifdef DEBUG
			Win32::Error("IMoveHelper is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 6"));
			return false;
		}

		csgo.m_input.set((IInput*)(Engine::Displacement::Signatures[IINPUT]));

		if (!csgo.m_input())
		{
		#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 7"));
			return false;
		}

		csgo.m_globals.set(**(CGlobalVarsBase***)((*(DWORD**)csgo.m_client())[0] + 0x1F));

		if (!csgo.m_globals())
		{
		#ifdef DEBUG
			Win32::Error("CGlobalVars is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 8"));
			return false;
		}

		csgo.m_engine.set((IVEngineClient*)CreateInterface(VEngineClient));

		if (!csgo.m_engine())
		{
		#ifdef DEBUG
			Win32::Error("IVEngineClient is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 9"));
			return false;
		}

		csgo.m_panel.set((IPanel*)CreateInterface( VGUI_Panel));

		if (!csgo.m_panel())
		{
		#ifdef DEBUG
			Win32::Error("IPanel is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 10"));
			return false;
		}

		csgo.m_surface.set((ISurface*)CreateInterface(VGUI_Surface));

		if (!csgo.m_surface())
		{
		#ifdef DEBUG
			Win32::Error("ISurface is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 11"));
			return false;
		}

		csgo.m_engine_vgui.set((IEngineVGui*)CreateInterface( VEngineVGui001));

		if (!csgo.m_engine_vgui())
		{
		#ifdef DEBUG
			Win32::Error("IEngineVGui is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 12"));
			return false;
		}

		csgo.m_client_state.set((CClientState*)(**(std::uintptr_t**)(Engine::Displacement::Signatures[CLIENT_STATE] + 1))); //ik we can grab it from vfunc but imma just took what i did b4

		if (!csgo.m_client_state())
		{
		#ifdef DEBUG
			Win32::Error("CClientState is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 13"));
			return false;
		}

		csgo.m_engine_cvars.set((ICvar*)CreateInterface( VEngineCvar));

		if (!csgo.m_engine_cvars())
		{
		#ifdef DEBUG
			Win32::Error("ICvar is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 14"));
			return false;
		}

		csgo.m_engine_trace.set((IEngineTrace*)CreateInterface( EngineTraceClient004));

		if (!csgo.m_engine_trace())
		{
		#ifdef DEBUG
			Win32::Error("IEngineTrace is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 15"));
			return false;
		}

		csgo.m_model_info.set((IVModelInfo*)CreateInterface( VModelInfoClient004));

		if (!csgo.m_model_info())
		{
		#ifdef DEBUG
			Win32::Error("IVModelInfo is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 16"));
			return false;
		}

		csgo.m_input_system.set((InputSystem*)CreateInterface(InputSystemVersion001));

		if (!csgo.m_input_system())
		{
		#ifdef DEBUG
			Win32::Error("IInput is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 17"));
			return false;
		}

		csgo.m_model_render.set((IVModelRender*)(CreateInterface(VEngineModel016)));

		if (!csgo.m_model_render())
		{
		#ifdef DEBUG
			Win32::Error("IVModelRender is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 18"));
			return false;
		}

		csgo.m_render_view.set((IVRenderView*)CreateInterface(VEngineRenderView014));

		if (!csgo.m_render_view())
		{
		#ifdef DEBUG
			Win32::Error("IVRenderView is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 19"));
			return false;
		}

		csgo.m_material_system.set((IMaterialSystem*)CreateInterface( VMaterialSystem080));

		if (!csgo.m_material_system())
		{
		#ifdef DEBUG
			Win32::Error("IMaterialSystem is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 20"));
			return false;
		}

		csgo.m_phys_props.set((IPhysicsSurfaceProps*)CreateInterface( VPhysicsSurfaceProps001));

		if (!csgo.m_phys_props())
		{
		#ifdef DEBUG
			Win32::Error("IPhysicsSurfaceProps is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 21"));
			return false;
		}

		csgo.m_debug_overlay.set((IVDebugOverlay*)CreateInterface( VDebugOverlay004));

		if (!csgo.m_debug_overlay())
		{
		#ifdef DEBUG
			Win32::Error("IVDebugOverlay is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 22"));
			return false;
		}

		csgo.m_event_manager.set((IGameEventManager*)(CreateInterface(GAMEEVENTSMANAGER002)));

		if (!csgo.m_event_manager())
		{
		#ifdef DEBUG
			Win32::Error("IGameEventManager is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 23"));
			return false;
		}

		csgo.m_static_prop.set((IStaticPropMgr*)(CreateInterface(StaticPropMgrServer002)));

		if (!csgo.m_static_prop())
		{
		#ifdef DEBUG
			Win32::Error("IStaticPropMgr is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 24"));
			return false;
		}

		csgo.m_beams.set(*(IViewRenderBeams**)(Engine::Displacement::Signatures[BEAMS] + 1));

		if (!csgo.m_beams())
		{
		#ifdef DEBUG
			Win32::Error("IViewRenderBeams is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 25"));
			return false;
		}

		csgo.m_localize.set((ILocalize*)(CreateInterface(Localize_001)));

		if (!csgo.m_localize())
		{
		#ifdef DEBUG
			Win32::Error("ILocalize is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 26"));
			return false;
		}

		csgo.m_glow_object.set(*reinterpret_cast<CGlowObjectManager**>(Engine::Displacement::Signatures[GLOW_OBJECTS] + 3));

		if (!csgo.m_glow_object())
		{
		#ifdef DEBUG
			Win32::Error("CGlowObjectManager is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 27"));
			return false;
		}

		csgo.m_mdl_cache.set((IMDLCache*)(CreateInterface(MDLCache004)));

		if (!csgo.m_mdl_cache())
		{
		#ifdef DEBUG
			Win32::Error("IMDLCache is nullptr (Source::%s)", __FUNCTION__);
		#endif // DEBUG
			create_log_file(sxor("ERROR 28"));
			return false;
		}

		csgo.m_engine_sound.set((IEngineSound*)(CreateInterface(IEngineSoundClient)));

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

		csgo.m_mem_alloc.set(*(IMemAlloc**)(GetProcAddress(GetModuleHandleA(("tier0.dll")), ("g_pMemAlloc"))));

		if (!csgo.m_mem_alloc())
		{
#ifdef DEBUG
			Win32::Error("IMemAlloc is nullptr (Source::%s)", __FUNCTION__);
#endif // DEBUG
			return false;
		}

		ctx.init_finished = false;

		for (CClientEffectRegistration* head = ctx.m_effects_head(); head; head = head->next)
		{
			if (strstr(head->effectName, ("Impact")) && strlen(head->effectName) <= 8) {
				oImpact = head->function;
				head->function = &Hooked::impact_callback;
				break;
			}
		}

#ifndef AUTH

		/*auto factory_client = GetModuleHandleA("client.dll");
		auto factory_engine = GetModuleHandleA("engine.dll");
		auto factory_vgui = GetModuleHandleA("vgui2.dll");
		auto factory_vguimatsurface = GetModuleHandleA("vguimatsurface.dll");
		auto factory_vstdlib = GetModuleHandleA("vstdlib.dll");
		auto factory_inputsystem = GetModuleHandleA("inputsystem.dll");
		auto factory_materialsystem = GetModuleHandleA("materialsystem.dll");
		auto factory_vphysics = GetModuleHandleA("vphysics.dll");
		auto factory_localize = GetModuleHandleA("localize.dll");
		auto factory_datacache = GetModuleHandleA("datacache.dll");
		ppppp.VClient = ((DWORD)csgo.m_client() - (DWORD)factory_client);
		ppppp.VClientEntityList = ((DWORD)csgo.m_entity_list() - (DWORD)factory_client);
		ppppp.GameMovement = ((DWORD)csgo.m_movement() - (DWORD)factory_client);
		ppppp.VClientPrediction = ((DWORD)csgo.m_prediction() - (DWORD)factory_client);
		ppppp.VEngineClient = ((DWORD)csgo.m_engine() - (DWORD)factory_engine);
		ppppp.VGUI_Panel = ((DWORD)csgo.m_panel() - (DWORD)factory_vgui);
		ppppp.VGUI_Surface = ((DWORD)csgo.m_surface() - (DWORD)factory_vguimatsurface);
		ppppp.VEngineVGui001 = ((DWORD)csgo.m_engine_vgui() - (DWORD)factory_engine);
		ppppp.VEngineCvar = ((DWORD)csgo.m_engine_cvars() - (DWORD)factory_vstdlib);
		ppppp.EngineTraceClient004 = ((DWORD)csgo.m_engine_trace() - (DWORD)factory_engine);
		ppppp.VModelInfoClient004 = ((DWORD)csgo.m_model_info() - (DWORD)factory_engine);
		ppppp.InputSystemVersion001 = ((DWORD)csgo.m_input_system() - (DWORD)factory_inputsystem);
		ppppp.VEngineModel016 = ((DWORD)csgo.m_model_render() - (DWORD)factory_engine);
		ppppp.VEngineRenderView014 = ((DWORD)csgo.m_render_view() - (DWORD)factory_engine);
		ppppp.VMaterialSystem080 = ((DWORD)csgo.m_material_system() - (DWORD)factory_materialsystem);
		ppppp.VPhysicsSurfaceProps001 = ((DWORD)csgo.m_phys_props() - (DWORD)factory_vphysics);
		ppppp.VDebugOverlay004 = ((DWORD)csgo.m_debug_overlay() - (DWORD)factory_engine);
		ppppp.GAMEEVENTSMANAGER002 = ((DWORD)csgo.m_event_manager() - (DWORD)factory_engine);
		ppppp.StaticPropMgrServer002 = ((DWORD)csgo.m_static_prop() - (DWORD)factory_engine);
		ppppp.Localize_001 = ((DWORD)csgo.m_localize() - (DWORD)factory_localize);
		ppppp.MDLCache004 = ((DWORD)csgo.m_mdl_cache() - (DWORD)factory_datacache);
		ppppp.IEngineSoundClient = ((DWORD)csgo.m_engine_sound() - (DWORD)factory_engine);



		std::ofstream test("data.bin", std::ios::binary);

		PBYTE bytes = (PBYTE)(&ppppp);

		for (int i = 0; i < 176; i++)  
		{ 
			test << bytes[i];
		}*/

		//test.close();

#endif // AUTH

		feature::menu = new c_menu;
		feature::misc = new c_misc;
		feature::anti_aim = new c_antiaimbot;
		feature::resolver = new c_resolver;
		feature::visuals = new c_visuals;
		feature::grenades = new c_grenade_tracer;
		feature::usercmd = new c_usercmd;
		feature::lagcomp = new c_lagcomp;
		feature::chams = new c_chams;
		feature::autowall = new c_autowall;
		feature::ragebot = new c_aimbot;
		feature::sound_parser = new c_dormant_esp;
		feature::music_player = new c_music_player;
		feature::weather = new c_weather_controller;
		feature::legitbot = new c_legitaimbot;

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

		//static const auto filter_simple = *reinterpret_cast<uint32_t*>(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F0 83 EC 7C 56 52")) + 0x3d);

		ctx.last_shot_info.point.clear();
		ctx.last_shot_info.hit = false;
		ctx.last_shot_info.find = false;
		ctx.last_shot_info.last_time_hit = 0;
		ctx.last_shot_info.resolver_index = 0;
		ctx.last_shot_info.entindex = -1;

		auto m_pClientMode = **(void***)((*(DWORD**)csgo.m_client())[10] + 5);
		//auto m_pClientMode = **reinterpret_cast<void***>((*reinterpret_cast<uintptr_t * *>(csgo.m_client()))[10] + 0x5);

		if (m_pClientMode)
		{
			vmt.m_clientmode = std::make_shared<Memory::VmtSwap>(m_pClientMode);
			vmt.m_clientmode->Hook(&Hooked::CreateMove, Index::IBaseClientDLL::CreateMove);
			vmt.m_clientmode->Hook(&Hooked::OverrideView, Index::IBaseClientDLL::OverrideView);
			vmt.m_clientmode->Hook(&Hooked::DoPostScreenEffects, 44);
			vmt.m_clientmode->Hook(&Hooked::GetViewModelFOV, 35);
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

		vmt.m_client = std::make_shared<Memory::VmtSwap>(csgo.m_client());
		vmt.m_prediction = std::make_shared<Memory::VmtSwap>(csgo.m_prediction());
		vmt.m_panel = std::make_shared<Memory::VmtSwap>(csgo.m_panel());
		vmt.m_surface = std::make_shared<Memory::VmtSwap>(csgo.m_surface());
		vmt.m_engine_vgui = std::make_shared<Memory::VmtSwap>(csgo.m_engine_vgui());
		vmt.m_render_view = std::make_shared<Memory::VmtSwap>(csgo.m_render_view());
		vmt.m_engine = std::make_shared<Memory::VmtSwap>(csgo.m_engine());
		vmt.m_model_render = std::make_shared<Memory::VmtSwap>(csgo.m_model_render());
		vmt.m_material_system = std::make_shared<Memory::VmtSwap>(csgo.m_material_system());
		//vmt.m_material = std::make_shared<Memory::VmtSwap>();
		vmt.m_engine_trace = std::make_shared<Memory::VmtSwap>(csgo.m_engine_trace());
		vmt.m_bsp_tree_query = std::make_shared<Memory::VmtSwap>(csgo.m_engine()->GetBSPTreeQuery());
		vmt.m_engine_sound = std::make_shared<Memory::VmtSwap>(csgo.m_engine_sound());
		vmt.m_movement = std::make_shared<Memory::VmtSwap>(csgo.m_movement());
		vmt.m_device = std::make_shared<Memory::VmtSwap>((void*)Engine::Displacement::Signatures[c_signatures::DX9DEVICE]);

		vmt.m_engine->Hook(&Hooked::IsHLTV, 93);
		vmt.m_engine->Hook(&Hooked::IsBoxVisible, 32);
		vmt.m_engine->Hook(&Hooked::IsConnected, 27);
		vmt.m_bsp_tree_query->Hook(&Hooked::ListLeavesInBox, 6);
		vmt.m_engine->Hook(&Hooked::FireEvents, 59);
		vmt.m_engine->Hook(&Hooked::GetNetChannelInfo, 78);
		vmt.m_engine->Hook(&Hooked::GetScreenAspectRatio, 101);

		vmt.m_movement->Hook(&Hooked::ProcessMovement, 1);

		vmt.m_device->Hook(&Hooked::EndScene, Index::IDirectX::EndScene);
		vmt.m_device->Hook(&Hooked::Reset, Index::IDirectX::Reset);

		vmt.m_engine_sound->Hook(&Hooked::EmitSound1, 5);
		//m_pTraceFilterSimple->Hook(&Hooked::ShouldHitEntity, 0);

		vmt.m_engine_trace->Hook(&Hooked::ClipRayCollideable, 4);
		/*m_pEngineTraceSwap->Hook(&Hooked::TraceRay, 5);*/

		vmt.m_client->Hook(&Hooked::FrameStageNotify, Index::IBaseClientDLL::FrameStageNotify);
		vmt.m_client->Hook(&Hooked::WriteUsercmdDeltaToBuffer, 24);

		vmt.m_client->Hook(&Hooked::LevelInitPostEntity, 6);
		vmt.m_client->Hook(&Hooked::LevelInitPreEntity, 5);
		vmt.m_client->Hook(&Hooked::LevelShutdown, 7);

		vmt.m_surface->Hook(&Hooked::LockCursor, Index::ISurface::LockCursor);
		vmt.m_surface->Hook(&Hooked::DrawSetColor, 15);	
		//vmt.m_prediction->Hook(&Hooked::SetupMove, Index::IPrediction::SetupMove);
		vmt.m_prediction->Hook(&Hooked::RunCommand, Index::IPrediction::RunCommand);
		vmt.m_prediction->Hook(&Hooked::InPrediction, Index::IPrediction::InPrediction);

		OriginalGetAlphaModulation = Memory::VFTableHook::HookManual((*(uintptr_t**)(Memory::Scan(sxor("materialsystem.dll"), sxor("C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? 89 4D E8")) + 0x2)), 44, (GetAlphaModulation_t)Hooked::GetAlphaModulation);

		vmt.m_panel->Hook(&Hooked::PaintTraverse, Index::IPanel::PaintTraverse);
		vmt.m_engine_vgui->Hook(&Hooked::EngineVGUI_Paint, 14); //index is not in enum idk why

		vmt.m_render_view->Hook(&Hooked::SceneEnd, 9);
		vmt.m_model_render->Hook(&Hooked::DrawModelExecute, 21);

		vmt.m_material_system->Hook(&Hooked::OverrideConfig, 21);
		//vmt.m_material->Hook(&Hooked::GetAlphaModulation, 44);

		//C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? 89 4D E8

		auto rate = csgo.m_engine_cvars()->FindVar(sxor("rate"));
		auto cl_cmdrate = csgo.m_engine_cvars()->FindVar(sxor("cl_cmdrate"));
		auto cl_updaterate = csgo.m_engine_cvars()->FindVar(sxor("cl_updaterate"));
		auto cl_interp = csgo.m_engine_cvars()->FindVar(sxor("cl_interp"));
		auto net_maxroutable = csgo.m_engine_cvars()->FindVar(sxor("net_maxroutable"));
		auto cl_interp_ratio = csgo.m_engine_cvars()->FindVar(sxor("cl_interp_ratio"));
		auto r_jiggle_bones = csgo.m_engine_cvars()->FindVar(sxor("r_jiggle_bones"));
		auto net_earliertempents = csgo.m_engine_cvars()->FindVar(sxor("net_earliertempents"));

		rate->SetValue(786432);
		cl_cmdrate->SetValue(128);
		cl_updaterate->SetValue(128);
		cl_interp->SetValue(0.03125f);
		net_maxroutable->SetValue(1200);
		cl_interp_ratio->SetValue(2);
		r_jiggle_bones->SetValue(0);
		net_earliertempents->RemoveFlags(FCVAR_CHEAT);
		net_earliertempents->SetValue(1);

		auto cl_smooth = csgo.m_engine_cvars()->FindVar(sxor("cl_smooth"));
		//auto cl_grenadepreview = csgo.m_engine_cvars()->FindVar(sxor("cl_grenadepreview"));
		ctx.cv_console_window_open = csgo.m_engine_cvars()->FindVar(sxor("console_window_open"));
		
		//vmt.m_cl_clock_correction = std::make_shared<Memory::VmtSwap>(cl_clock_correction);
		//vmt.m_cl_clock_correction->Hook(&Hooked::cl_clock_correction_get_bool, 13);	
		
		//vmt.cl_smooth = std::make_shared<Memory::VmtSwap>(cl_smooth);
		//vmt.cl_smooth->Hook(&Hooked::cl_smooth_get_bool, 13);
		
		//vmt.m_cl_grenadepreview = std::make_shared<Memory::VmtSwap>(cl_grenadepreview);
		//vmt.m_cl_grenadepreview->Hook(&Hooked::cl_grenadepreview_get_bool, 13);

		//auto* const lol = csgo.m_engine_cvars()->FindVar(("cl_foot_contact_shadows"));

		//if (lol != nullptr)
		//	lol->SetValue(0);

		game_events::init();

		feature::music_player->init();

		ctx.fix_modify_eye_pos = false;
		ctx.can_store_netvars = false;
		ctx.process_movement_sound_fix = false;
		ctx.breaks_lc = false;

		feature::ragebot->precomputed_seeds.fill(std::tuple<float,float,float,float,float>(0.f,0.f,0.f,0.f,0.f));
		feature::ragebot->precomputed_mini_seeds.fill(std::tuple<float, float, float, float, float>(0.f, 0.f, 0.f, 0.f, 0.f));
		feature::ragebot->build_mini_hc_table();
		feature::ragebot->build_seed_table();

		ctx.active_keybinds_visible = 0;
		ctx.main_exploit = 0;
		ctx.last_velocity_modifier = -1.f;
		ctx.cheat_option_flags = 0;
		ctx.prev_exploit_states[0] = false;
		ctx.prev_exploit_states[1] = false;

		/*static DWORD lol2 = (DWORD)(Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D")));*/

		//m_flSimulationTime = pPropManager->Hook(Hooked::m_flSimulationTimeHook, ("DT_BaseEntity"), ("m_flSimulationTime"));
		m_flAbsYaw = pPropManager->Hook(Hooked::m_flAbsYawHook, sxor("DT_CSRagdoll"), sxor("m_flAbsYaw"));
		m_nSequence = pPropManager->Hook(Hooked::m_nSequenceHook, sxor("DT_BaseViewModel"), sxor("m_nSequence"));
		m_vecForce = pPropManager->Hook(Hooked::m_vecForceHook, sxor("DT_CSRagdoll"), sxor("m_vecForce"));
		m_flVelocityModifier = pPropManager->Hook(Hooked::m_flVelocityModifierHook, sxor("DT_CSPlayer"), sxor("m_flVelocityModifier"));

		/*m_flPlaybackRate = pPropManager->Hook(Hooked::m_flPlaybackHook, ("DT_BaseAnimatingOverlay"), ("m_flPlaybackRate"));
		m_flCycle = pPropManager->Hook(Hooked::m_flCycleHook, ("DT_BaseAnimatingOverlay"), ("m_flCycle"));
		m_Sequence = pPropManager->Hook(Hooked::m_SequenceHook, ("DT_BaseAnimatingOverlay"), ("m_nSequence"));
		m_flWeight = pPropManager->Hook(Hooked::m_flWeightHook, ("DT_BaseAnimatingOverlay"), ("m_flWeight"));*/


		//oShouldHitEntity = Memory::VFTableHook::HookManual<ShouldHitEntity_t>((uintptr_t*)filter_simple, 0, (ShouldHitEntity_t)Hooked::ShouldHitEntity);
		//
		//m_flLowerBodyYawTarget = pPropManager->Hook(Hooked::m_flLowerBodyYawTargetHook, "DT_CSPlayer", "m_flLowerBodyYawTarget");

		/*static auto SetupVelocity = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D")));
		OriginalSetupVelocity = (DWORD)DetourFunction((byte*)SetupVelocity, (byte*)Hooked::SetupVelocity);*/
		//OriginalSetUpMovement = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_SETUPMOVEMENT], (byte*)Hooked::SetUpMovement);

		/*static auto CalcAbsoluteVelocity = (DWORD)(Memory::Scan("client.dll", "55 8B EC 83 E4 F8 83 EC 1C 53 56 57 8B F9 F7"));
		OriginalCalcAbsoluteVelocity = (DWORD)DetourFunction((byte*)CalcAbsoluteVelocity, (byte*)Hooked::CalcAbsoluteVelocity);*/

		//static auto StandardBlendingRules = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6")));
		OriginalStandardBlendingRules = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_STANDARDBLENDINGRULES], (byte*)Hooked::StandardBlendingRules);
		
		//static auto DoExtraBonesProcessing = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C")));
		OriginalDoExtraBonesProcessing = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_DOEXTRABONESPROCESSING], (byte*)Hooked::DoExtraBonesProcessing);

		//static auto GetForeignFallbackFontName = (DWORD)(Memory::Scan(("vguimatsurface.dll"), ("80 3D ? ? ? ? ? 74 06 B8")));
		OriginalGetForeignFallbackFontName = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_GETFOREIGNFALLBACKFONT], (byte*)Hooked::GetForeignFallbackFontName);

		//static auto m_uUpdateClientSideAnimation = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 51 56 8B F1 80 BE ? ? 00 00 00 74 36")));
		OriginalUpdateClientSideAnimations = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_UPDATECLIENTSIDEANIMS], (byte*)Hooked::UpdateClientSideAnimation);

		//static auto SetupBones = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F0 B8 D8")));
		OriginalSetupBones = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_SETUPBONES], (byte*)Hooked::SetupBones);
		
		//static auto CalcViewBob = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9")));
		OriginalCalcViewBob = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_CALCVIEWBOB], (byte*)Hooked::CalcViewBob);
		
		//static auto AddViewModelBob = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 34 85 C0 74 35")));
		OriginalAddViewModelBob = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_ADDVIEWMODELBOB], (byte*)Hooked::AddViewModelBob);
		
		//static auto ShouldSkipAnimationFrame = (DWORD)(Memory::follow_rel32(Memory::Scan(("client.dll"), ("E8 ? ? ? ? 88 44 24 0B")), 1));
		OriginalShouldSkipAnimationFrame = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_SHOULDSKIPANIMFRAME], (byte*)Hooked::ShouldSkipAnimationFrame);
		
		//static auto CAM_Think = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F8 81 EC ? ? ? ? 56 8B F1 8B 0D ? ? ? ? 57 85 C9")));
		
		//static auto PlayStepSound = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 8B 45 18 81 EC")));
		//OriginalPlayStepSound = (DWORD)DetourFunction((byte*)PlayStepSound, (byte*)Hooked::PlayStepSound);

		//static auto ShouldHitEntity = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 8B 55 0C 56 8B 75 08 57")));
		//static auto trace_filter_simple_vtable = *(uintptr_t*)(Memory::Scan("client.dll", "C7 44 24 ? ? ? ? ? F3 0F 5C D9") + 4);
		//auto ShouldHitEntity = (*(uintptr_t * *)trace_filter_simple_vtable)[0];
		//OriginalShouldHitEntity = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_SHOULDHITENTITY], (byte*)Hooked::ShouldHitEntity);
		
		//static auto ApproachAngle = (DWORD)(Memory::Scan("client.dll", "F3 0F 59 05 A0 CB C3 10 0F"));
		//OriginalApproachAngle = (DWORD)DetourFunction((byte*)ApproachAngle, (byte*)Hooked::ApproachAngle);

		//static auto ModifyEyePosition = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83")));
		OriginalModifyEyePosition = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_MODIFYEYEPOS], (byte*)Hooked::ModifyEyePosition);

		static auto PhysicsSimulate = (DWORD)(Memory::Scan(sxor("client.dll"), sxor("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 21")));
		OriginalPhysicsSimulate = (DWORD)DetourFunction((byte*)PhysicsSimulate, (byte*)Hooked::PhysicsSimulate);
		
		static auto BuildTransformations = (DWORD)(Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F0 81 EC ? ? ? ? 56 57 8B F9 8B 0D ? ? ? ? 89 7C 24 1C")));
		OriginalBuildTransformations = (DWORD)DetourFunction((byte*)BuildTransformations, (byte*)Hooked::BuildTransformations);
		
		static auto ShouldFlipModel = (DWORD)(Memory::Scan(sxor("client.dll"), sxor("8B 89 ? ? ? ? 56 57 83 F9 FF")));
		OriginalShouldFlipModel = (DWORD)DetourFunction((byte*)ShouldFlipModel, (byte*)Hooked::ShouldFlipModel);
		
		//static auto CL_ReadPackets = (DWORD)(Memory::Scan(sxor("engine.dll"), sxor("53 8A D9 8B 0D ? ? ? ? 56 57 8B B9")));
		//OriginalCL_ReadPackets = (DWORD)DetourFunction((byte*)CL_ReadPackets, (byte*)Hooked::CL_ReadPackets);

		//static auto _Host_RunFrame_Client = (DWORD)(Memory::Scan(sxor("engine.dll"), sxor("55 8B EC 83 EC 08 53 56 8A")));
		//OriginalHost_RunFrame_Client = (DWORD)DetourFunction((byte*)_Host_RunFrame_Client, (byte*)Hooked::_Host_RunFrame_Client);

		//static auto _Host_RunFrame_Input = (DWORD)(Memory::Scan(sxor("engine.dll"), sxor("55 8B EC 83 EC 10 53 8A D9 F3 0F 11 45")));
		//OriginalHost_RunFrame_Input = (DWORD)DetourFunction((byte*)_Host_RunFrame_Input, (byte*)Hooked::_Host_RunFrame_Input);
		
		static auto CheckForSequenceChange = (DWORD)(Memory::Scan(sxor("client.dll"), sxor("55 8B EC 51 53 8B 5D 08 56 8B F1 57 85")));
		OriginalCheckForSequenceChange = (DWORD)DetourFunction((byte*)CheckForSequenceChange, (byte*)Hooked::CheckForSequenceChange);
		
		//static auto C_TEPlayerAnimEvent_PostDataUpdate = (DWORD)(Memory::Scan(sxor("client.dll"), sxor("57 8B F9 8B 47 0C 83 F8 FF")));
		//OriginalAnimEventHook = (DWORD)DetourFunction((byte*)C_TEPlayerAnimEvent_PostDataUpdate, (byte*)Hooked::AnimEventHook);
		
		//static auto AdjustFrameTime = (DWORD)(Memory::Scan(sxor("engine.dll"), sxor("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 1C 83")));
		//OriginalAdjustFrameTime = (DWORD)DetourFunction((byte*)AdjustFrameTime, (byte*)Hooked::AdjustFrameTime);

		//static auto CL_Move = (DWORD)(Memory::Scan(("engine.dll"), ("55 8B EC 81 EC 64 01 00 00 53 56 57 8B 3D")));
		OriginalCL_Move = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_CL_MOVE], (byte*)Hooked::CL_Move);

		/*static auto GetRenderOrigin = (DWORD)(Memory::Scan(("client.dll"), ("8B D1 83 BA ? ? ? ? ? 8D 4A FC")));
		OriginalGetRenderOrigin = (DWORD)DetourFunction((byte*)GetRenderOrigin, (byte*)Hooked::GetRenderOrigin);*/
		
		//static auto EyeAngles = (DWORD)(Memory::Scan(("client.dll"), ("56 8B F1 85 F6 74 32")));
		//OriginalEyeAngles = (DWORD)DetourFunction((byte*)EyeAngles, (byte*)Hooked::EyeAngles);
		
		//static auto ProcessInterpolatedList = (DWORD)(Memory::Scan(("client.dll"), ("53 0F B7 1D ? ? ? ? 56")));
		OriginalProcessInterpolatedList = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_PROCESSINTERPLIST], (byte*)Hooked::ProcessInterpolatedList);

		//static auto SetViewmodelOffsets = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 8B 45 08 F3 0F 7E 45")));
		OriginalSetViewmodelOffsets = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_SETVIEWMODELOFFSETS], (byte*)Hooked::SetViewmodelOffsets);
		
		//static auto GetColorModulation = (DWORD)(Memory::Scan(("materialsystem.dll"), ("55 8B EC 83 EC ? 56 8B F1 8A 46")));
		OriginalGetColorModulation = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_GETCOLORMODULATION], (byte*)Hooked::GetColorModulation);
		
		//static auto IsUsingStaticPropDebugModes = (DWORD)(Memory::Scan(("engine.dll"), ("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D")));
		OriginalIsUsingStaticPropDebugModes = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_ISUSINGSTATICPROPDBGMODES], (byte*)Hooked::IsUsingStaticPropDebugModes);
		
		//static auto ReportHit = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 8B 55 08 83 EC 1C F6 42 1C 01")));
		OriginalReportHit = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_REPORTHIT], (byte*)Hooked::ReportHit);
		
		//static auto CalcView = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 56 8B F1 57 8B 8E ? ? ? ? 83 F9 FF 74 3E 0F B7 C1 C1 E0 04 05 ? ? ? ? C1 E9 10 39 48 04 75 2B 8B 08 85 C9 74 25 8B 01")));
		//OriginalCalcView = (DWORD)DetourFunction((byte*)Engine::Displacement::Signatures[c_signatures::PTR_CALCVIEW], (byte*)Hooked::CalcView);

		//static auto PreDataUpdate = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 56 8B 75 08 8B D6 57 8B F9 8D 4F F8 F7 D9")));
		//OriginalPreDataUpdate = (DWORD)DetourFunction((byte*)PreDataUpdate, (byte*)Hooked::PreDataUpdate);

		//static auto PostDataUpdate = (DWORD)(Memory::Scan(("client.dll"), ("55 8B EC 53 56 8B F1 57 80 BE ? ? ? ? ? 74 0A")));
		//OriginalPostDataUpdate = (DWORD)DetourFunction((byte*)PostDataUpdate, (byte*)Hooked::PostDataUpdate);

		Window = FindWindowA(("Valve001"), NULL);
		Hooked::oldWindowProc = (WNDPROC)SetWindowLongPtr(Window, GWL_WNDPROC, (LONG_PTR)Hooked::WndProc);
		//static auto vtable = *(void**)(Memory::Scan("client.dll", "C7 05 F0 FB 19 15 F8 5A C5 10") + 2);
		feature::menu->_menu_opened = true;

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

		_events.emplace_back(sxor("Initialized and ready."));

		ctx.init_finished = true;

		//_events.push_back(_event(get_user_data()));
//#ifdef AUTH
//		auto data = get_user_data();
//
//		auto &net = Internet::Instance();
//
//		if (net.Connect(std::string(sxor("http://2k17.club/")).c_str()))
//		{
//			static std::string response = sxor("");
//			PostRequest request{ std::string(sxor("cheat_gate.php")).c_str() };
//			static const char* junk3 = Internet::Instance().random_string(RandomInt(0, 99999)).c_str();
//			request.Set(std::string(sxor("s")).c_str(), net.base64_encode(net.encryptDecrypt(data.c_str())));
//			static const char* junk5 = Internet::Instance().random_string(RandomInt(0, 99999)).c_str();
//			net.SendRequest(request, response);
//		}
//#endif

		VIRTUALIZER_STR_ENCRYPT_END;
		VIRTUALIZER_FISH_LITE_END;

		return true;
		/*("EntryPoint");
		SE_UNPROTECT_END*/
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
#ifndef AUTH
		//static const auto filter_simple = *reinterpret_cast<uint32_t*>(Memory::Scan("client.dll", "55 8B EC 83 E4 F0 83 EC 7C 56 52") + 0x3d);

		if ((DWORD)ctx.clantag_cvar > 0x2000) {
			csgo.m_engine_cvars()->UnregisterConCommand(ctx.clantag_cvar);
			csgo.m_mem_alloc()->Free((void*)ctx.clantag_cvar);
			ctx.clantag_cvar = nullptr;
		}

		DetourRemove((byte*)OriginalUpdateClientSideAnimations, (byte*)Hooked::UpdateClientSideAnimation);
		//DetourRemove((byte*)OriginalCalcAbsoluteVelocity, (byte*)Hooked::CalcAbsoluteVelocity);
		//DetourRemove((byte*)OriginalSetupVelocity, (byte*)Hooked::SetupVelocity);
		DetourRemove((byte*)OriginalStandardBlendingRules, (byte*)Hooked::StandardBlendingRules);
		//DetourRemove((byte*)OriginalSetUpMovement, (byte*)Hooked::SetUpMovement);
		DetourRemove((byte*)OriginalDoExtraBonesProcessing, (byte*)Hooked::DoExtraBonesProcessing);
		DetourRemove((byte*)OriginalGetForeignFallbackFontName, (byte*)Hooked::GetForeignFallbackFontName);
		DetourRemove((byte*)OriginalPhysicsSimulate, (byte*)Hooked::PhysicsSimulate);
		DetourRemove((byte*)OriginalSetupBones, (byte*)Hooked::SetupBones);
		DetourRemove((byte*)OriginalBuildTransformations, (byte*)Hooked::BuildTransformations);
		//DetourRemove((byte*)OriginalShouldHitEntity, (byte*)Hooked::ShouldHitEntity);
		//DetourRemove((byte*)OriginalApproachAngle, (byte*)Hooked::ApproachAngle);
		//DetourRemove((byte*)OriginalAnimEventHook, (byte*)Hooked::AnimEventHook);
		DetourRemove((byte*)OriginalModifyEyePosition, (byte*)Hooked::ModifyEyePosition);
		DetourRemove((byte*)OriginalShouldFlipModel, (byte*)Hooked::ShouldFlipModel);
		//DetourRemove((byte*)OriginalCL_ReadPackets, (byte*)Hooked::CL_ReadPackets);
		//DetourRemove((byte*)OriginalHost_RunFrame_Client, (byte*)Hooked::_Host_RunFrame_Client);
		//DetourRemove((byte*)OriginalHost_RunFrame_Input, (byte*)Hooked::_Host_RunFrame_Input);
		DetourRemove((byte*)OriginalCL_Move, (byte*)Hooked::CL_Move);
		//DetourRemove((byte*)OriginalGetRenderOrigin, (byte*)Hooked::GetRenderOrigin);
		DetourRemove((byte*)OriginalSetViewmodelOffsets, (byte*)Hooked::SetViewmodelOffsets);
		DetourRemove((byte*)OriginalCalcViewBob, (byte*)Hooked::CalcViewBob);
		DetourRemove((byte*)OriginalReportHit, (byte*)Hooked::ReportHit);
		DetourRemove((byte*)OriginalAddViewModelBob, (byte*)Hooked::AddViewModelBob);
		DetourRemove((byte*)OriginalCheckForSequenceChange, (byte*)Hooked::CheckForSequenceChange);
		//DetourRemove((byte*)OriginalPlayStepSound, (byte*)Hooked::PlayStepSound);
		//DetourRemove((byte*)OriginalEyeAngles, (byte*)Hooked::EyeAngles);
		DetourRemove((byte*)OriginalShouldSkipAnimationFrame, (byte*)Hooked::ShouldSkipAnimationFrame);
		//DetourRemove((byte*)OriginalAdjustFrameTime, (byte*)Hooked::AdjustFrameTime);
		DetourRemove((byte*)OriginalGetColorModulation, (byte*)Hooked::GetColorModulation);
		DetourRemove((byte*)OriginalIsUsingStaticPropDebugModes, (byte*)Hooked::IsUsingStaticPropDebugModes);
		DetourRemove((byte*)OriginalProcessInterpolatedList, (byte*)Hooked::ProcessInterpolatedList);
		//DetourRemove((byte*)OriginalCalcView, (byte*)Hooked::CalcView);
		for (CClientEffectRegistration* head = ctx.m_effects_head(); head; head = head->next)
		{
			if (strstr(head->effectName, sxor("Impact")) && strlen(head->effectName) <= 8) {
				head->function = oImpact;
				break;
			}
		}

		//if (feature::weather->m_precipitation != nullptr)
		//	feature::weather->m_precipitation->GetClientNetworkable()->Release();

		//Memory::VFTableHook::HookManual<ShouldHitEntity_t>((uintptr_t*)filter_simple, 0, oShouldHitEntity);
		Memory::VFTableHook::HookManual<GetAlphaModulation_t>((*(uintptr_t**)(Memory::Scan(sxor("materialsystem.dll"), sxor("C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? 89 4D E8")) + 0x2)), 44, OriginalGetAlphaModulation);


		//Engine::PropManager::Instance()->Hook(m_flSimulationTime, sxor("DT_BaseEntity"), sxor("m_flSimulationTime")); //unhook
		Engine::PropManager::Instance()->Hook(m_flAbsYaw, sxor("DT_CSRagdoll"), sxor("m_flAbsYaw"));
		Engine::PropManager::Instance()->Hook(m_nSequence, sxor("DT_BaseViewModel"), sxor("m_nSequence"));
		Engine::PropManager::Instance()->Hook(m_vecForce, sxor("DT_CSRagdoll"), sxor("m_vecForce"));
		Engine::PropManager::Instance()->Hook(m_flVelocityModifier, sxor("DT_CSPlayer"), sxor("m_flVelocityModifier"));
		/*Engine::PropManager::Instance()->Hook(m_flPlaybackRate, ("DT_BaseAnimatingOverlay"), ("m_flPlaybackRate"));
		Engine::PropManager::Instance()->Hook(m_flCycle, ("DT_BaseAnimatingOverlay"), ("m_flCycle"));
		Engine::PropManager::Instance()->Hook(m_Sequence, ("DT_BaseAnimatingOverlay"), ("m_nSequence"));
		Engine::PropManager::Instance()->Hook(m_flWeight, ("DT_BaseAnimatingOverlay"), ("m_flWeight"));*/

		//Engine::PropManager::Instance()->Hook(m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget"); //unhook

		vmt.m_bsp_tree_query.reset();
		vmt.m_movement.reset();
		vmt.m_material_system.reset();
		vmt.m_engine_sound.reset();
		vmt.m_client.reset();
		vmt.m_prediction.reset();
		vmt.m_panel.reset();
		vmt.m_clientmode.reset();
		vmt.m_surface.reset();
		vmt.m_render_view.reset();
		vmt.m_engine.reset();
		vmt.m_clientstate.reset();
		vmt.m_engine_vgui.reset();
		vmt.m_model_render.reset();
		vmt.m_net_channel.reset();
		vmt.m_device.reset();
		vmt.m_cl_clock_correction.reset();
		vmt.m_cl_grenadepreview.reset();
		vmt.cl_smooth.reset();
		vmt.m_engine_trace.reset();
		g_menu.on_unload();

		SetWindowLongPtr(Window, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(Hooked::oldWindowProc));

		/*delete feature::menu.Xor();
		delete feature::misc.Xor();
		delete feature::anti_aim.Xor();
		delete feature::resolver.Xor();
		delete feature::visuals.Xor();
		delete feature::grenades.Xor();
		delete feature::usercmd.Xor();
		delete feature::lagcomp.Xor();
		delete feature::chams.Xor();
		delete feature::autowall.Xor();
		delete feature::ragebot.Xor();
		delete feature::sound_parser.Xor();
		delete feature::music_player.Xor();*/

#endif 
	}

	void* CreateInterface(const std::string& image_name, const std::string& name, bool force /*= false */)
	{

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER

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

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER

		return nullptr;
	}

	__declspec(noinline) void* CreateInterface(ULONG64 offset)
	{
		VIRTUALIZER_FISH_LITE_START;
		int cpu_info[4] = { 0 };
		__cpuid(cpu_info, 1);
		auto ptr = (void*)((DWORD)offset ^ cpu_info[0]);

		VIRTUALIZER_FISH_LITE_END;
		return ptr;
		/*auto image = GetModuleHandleA(image_name.c_str());

		if (!image)
			return nullptr;

		auto _interface = (DWORD)image + offset;

		std::cout << std::hex << _interface << std::endl;

		return (void*)_interface;*/
	}
}

namespace feature
{
	Encrypted_t <c_menu > menu = nullptr;
	Encrypted_t < c_misc > misc = nullptr;
	Encrypted_t < c_antiaimbot > anti_aim = nullptr;
	Encrypted_t < c_resolver > resolver = nullptr;
	Encrypted_t < c_visuals > visuals = nullptr;
	Encrypted_t < c_grenade_tracer > grenades = nullptr;
	Encrypted_t < c_usercmd > usercmd = nullptr;
	Encrypted_t < c_lagcomp > lagcomp = nullptr;
	Encrypted_t < c_chams > chams = nullptr;
	Encrypted_t < c_autowall > autowall = nullptr;
	Encrypted_t < c_aimbot > ragebot = nullptr;
	Encrypted_t < c_dormant_esp > sound_parser = nullptr;
	Encrypted_t < c_music_player > music_player = nullptr;
	Encrypted_t < c_weather_controller > weather = nullptr;
	Encrypted_t < c_legitaimbot > legitbot = nullptr;
}

C_WeaponCSBaseGun* m_weapon()
{
	auto client = ctx.m_local();

	if (!client || client->IsDead() || client->m_hActiveWeapon() <= 0) {
		ctx.latest_weapon_data = nullptr;
		return nullptr;
	}

	return (C_WeaponCSBaseGun*)(csgo.m_entity_list()->GetClientEntityFromHandle(client->m_hActiveWeapon()));
}