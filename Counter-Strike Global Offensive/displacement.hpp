#pragma once

#include "sdk.hpp"

enum c_signatures : int
{
	TRACEFILTER_SIMPLE,
	INIT_KEY_VALUES,
	CL_PREDICT,
	CL_MOVE,
	INVALIDATEBONECACHE,
	MOVEHELPER,
	IINPUT,
	PREDRANDOMSEED,
	PREDPLAYER,
	BREAKABLE,
	SET_CLAN_TAG,
	COMMANDS_LIMIT,
	SETUPVEL_SPEED,
	GAMERULES,
	UPDATEHUDWEAPONS,
	CLEARDEATHNOTICES,
	RESETANIMSTATE,
	OVERRIDEPOSTPROCESS,
	VGUI_START_DRAW,
	VGUI_END_DRAW,
	LIST_LEAVES,
	PTR_ANIMATION_LAYER,
	GET_ANIMATION_LAYER,
	UPDATEVISIBILITY_ALLENTS,
	UPDATEANIMSTATE,
	SETABSANGLES,
	SETABSORIGIN,
	DELAYUNSCOPE,
	INVALIDATEPHYSICS,
	SETCOLLISIONBOUNDS,
	LOOKUPSEQUENCE,
	LOOKUPBONE,
	GETSEQUENCEACTIVITY,
	STUDIOHDR,
	SERVER_GLOBALS,
	GETSEQUENCEMOVEDIST,
	DRAWSERVERHITBOXES,
	STUDIOSETPOSEPARAM,
	SETPOSEPARAM,
	MD5_PSEUDORANDOM,
	RET_MAINTAINSEQUENCETRANS,
	RET_SETUPBONES_PRED_TIME,
	RET_CALCPLAYERVIEW_DRIFTPITCH,
	RET_CALCPLAYERVIEW,
	GET_WEAPON_PREFIX,
	LOADFROMBUFFEREX,
	ALLOW_EXTRAPOLATION,
	PREDICTED_VIEWMODEL,
	RET_CROSSHAIRCOLOR,
	RET_CROSSHAIROUTLINECOLOR,
	WRITEUSERCMD,
	RET_SETUPVELOCITY,
	PTR_SETUPVELOCITY,
	PTR_SETUPMOVEMENT,
	PTR_ACCUMULATELAYERS,
	RET_HLTV_ACCUMULATE_LAYERS,
	RET_HLTV_SETUPVELOCITY,
	RET_ISLOADOUT_ALLOWED,
	CLIENT_STATE,
	BEAMS,
	GLOW_OBJECTS,
	DX9DEVICE,
	PTR_STANDARDBLENDINGRULES,
	PTR_PREDICTIONHTLV_MUSOR,
	PTR_SHOULDCOLLIDE,
	PTR_DOEXTRABONESPROCESSING,
	PTR_GETFOREIGNFALLBACKFONT,
	PTR_UPDATECLIENTSIDEANIMS,
	PTR_SETUPBONES,
	PTR_CALCVIEWBOB,
	PTR_ADDVIEWMODELBOB,
	PTR_SHOULDSKIPANIMFRAME,
	PTR_SHOULDHITENTITY,
	PTR_MODIFYEYEPOS,
	PTR_PROCESSINTERPLIST,
	PTR_SETVIEWMODELOFFSETS,
	PTR_GETCOLORMODULATION,
	PTR_ISUSINGSTATICPROPDBGMODES,
	PTR_REPORTHIT,
	PTR_CALCVIEW,
	PTR_CL_MOVE,
	CLIENT_EFFECTS,
	HOST_FRAMETICKS,
	FIND_HUD_ELEMENT_PTHIS,
	FIND_HUD_ELEMENT,
	LINEGOESTHRUSMOKE,
	PHYSICS_RUN_THINK,
	SETNEXTTHINK,
	SIGNATURES_MAX
};

namespace Engine::Displacement
{

bool Create();
void InitMiscSignatures();
void Destroy();

extern std::uintptr_t Signatures[SIGNATURES_MAX];

namespace C_BaseEntity
{
	extern int m_MoveType;
	extern int m_rgflCoordinateFrame;
}
namespace DT_BaseAnimating
{
	extern int m_nForceBone;
	extern int m_bClientSideAnimation;
	extern int m_nSequence;
	extern int m_flCycle;
	extern int m_nHitboxSet;
	extern DWORD32 m_BoneAccessor;
	extern DWORD32 m_iMostRecentModelBoneCounter;
	extern DWORD32 m_flLastBoneSetupTime;
}
namespace DT_BaseEntity
{
	extern int m_iTeamNum;
	extern int m_vecOrigin;
}
namespace DT_BaseCombatCharacter
{
	extern int m_hActiveWeapon;
}
namespace C_BasePlayer
{
	extern int m_pCurrentCommand;
}
namespace DT_BasePlayer
{
	extern int m_aimPunchAngle;
	extern int m_viewPunchAngle;
	extern int m_vecViewOffset;
	extern int m_vecVelocity;
	extern int m_vecBaseVelocity;
	extern int m_flFallVelocity;
	extern int m_lifeState;
	extern int m_nTickBase;
	extern int m_iHealth;
	extern int m_fFlags;
	extern int m_flSimulationTime;
}
namespace DT_CSPlayer
{
	extern int m_flLowerBodyYawTarget;
	extern int m_angEyeAngles;
	extern int m_bIsScoped;
	extern int m_flDuckAmount;
	extern int m_flPoseParameter;
	extern int m_bHasHelmet;
}
namespace DT_BaseCombatWeapon
{
	extern int m_flNextPrimaryAttack;
	extern int m_flNextSecondaryAttack;
	extern int m_hOwner;
	extern int m_iClip1;
	extern int m_iItemDefinitionIndex;
}
namespace DT_WeaponCSBase
{
	extern int m_flRecoilIndex;
}
namespace Data
{
//extern std::uintptr_t m_uMoveHelper;
//extern std::uintptr_t m_uInput;
//extern std::uintptr_t m_uPredictionRandomSeed;
//extern std::uintptr_t m_uPredictionPlayer;
extern std::uintptr_t m_uModelBoneCounter;
extern DWORD32 m_nDeltaTick;
extern DWORD32 m_nChokedCommands;
extern DWORD32 m_nLastOutgoingCommand;
extern DWORD32 m_nLastCommandAck;
extern DWORD32 m_bIsHLTV;
}
namespace Function
{
extern std::uintptr_t m_uRandomSeed;
extern std::uintptr_t m_uRandomFloat;
extern std::uintptr_t m_uRandomInt;
}

}
