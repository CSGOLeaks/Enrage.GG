#include "displacement.hpp"
#include "prop_manager.hpp"

namespace Engine::Displacement
{

	DECLSPEC_NOINLINE bool Create()
	{
		VIRTUALIZER_FISH_LITE_START;
		VIRTUALIZER_STR_ENCRYPT_START;

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		auto& pPropManager = PropManager::Instance();

		C_BaseEntity::m_MoveType = pPropManager->GetOffset(sxor("DT_BaseEntity"), sxor("m_nRenderMode")) + 1;
		C_BaseEntity::m_rgflCoordinateFrame = pPropManager->GetOffset(sxor("DT_BaseEntity"), sxor("m_CollisionGroup")) - 48;

		DT_BaseEntity::m_iTeamNum = pPropManager->GetOffset(sxor("DT_BaseEntity"), sxor("m_iTeamNum"));
		DT_BaseEntity::m_vecOrigin = pPropManager->GetOffset(sxor("DT_BaseEntity"), sxor("m_vecOrigin"));

		DT_BaseCombatCharacter::m_hActiveWeapon = pPropManager->GetOffset(sxor("DT_BaseCombatCharacter"), sxor("m_hActiveWeapon"));

		auto m_hConstraintEntity = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_hConstraintEntity"));

		C_BasePlayer::m_pCurrentCommand = int(m_hConstraintEntity - 0xC);//Memory::Scan( "client.dll"), sxor("89 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 85 FF 75" ) + 2 );

		DT_BasePlayer::m_aimPunchAngle = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_aimPunchAngle"));
		DT_BasePlayer::m_viewPunchAngle = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_viewPunchAngle"));
		DT_BasePlayer::m_vecViewOffset = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_vecViewOffset[0]"));
		DT_BasePlayer::m_vecVelocity = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_vecVelocity[0]"));
		DT_BasePlayer::m_vecBaseVelocity = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_vecBaseVelocity"));
		DT_BasePlayer::m_flFallVelocity = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_flFallVelocity"));
		DT_BasePlayer::m_lifeState = pPropManager->GetOffset(sxor("DT_CSPlayer"), sxor("m_lifeState"));
		DT_BasePlayer::m_nTickBase = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_nTickBase"));
		DT_BasePlayer::m_iHealth = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_iHealth"));
		DT_BasePlayer::m_fFlags = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_fFlags"));
		DT_BasePlayer::m_flSimulationTime = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_flSimulationTime"));

		DT_CSPlayer::m_angEyeAngles = pPropManager->GetOffset(sxor("DT_CSPlayer"), sxor("m_angEyeAngles[0]"));
		DT_CSPlayer::m_bIsScoped = pPropManager->GetOffset(sxor("DT_CSPlayer"), sxor("m_bIsScoped"));
		DT_CSPlayer::m_flDuckAmount = pPropManager->GetOffset(sxor("DT_BasePlayer"), sxor("m_flDuckAmount"));
		DT_CSPlayer::m_flPoseParameter = pPropManager->GetOffset(sxor("DT_CSPlayer"), sxor("m_flPoseParameter"));
		DT_CSPlayer::m_bHasHelmet = pPropManager->GetOffset(sxor("DT_CSPlayer"), sxor("m_bHasHelmet"));
		DT_CSPlayer::m_flLowerBodyYawTarget = pPropManager->GetOffset(sxor("DT_CSPlayer"), sxor("m_flLowerBodyYawTarget"));

		DT_BaseAnimating::m_nForceBone = pPropManager->GetOffset(sxor("DT_BaseAnimating"), sxor("m_nForceBone"));
		DT_BaseAnimating::m_bClientSideAnimation = pPropManager->GetOffset(sxor("DT_BaseAnimating"), sxor("m_bClientSideAnimation"));
		DT_BaseAnimating::m_nSequence = pPropManager->GetOffset(sxor("DT_BaseAnimating"), sxor("m_nSequence"));
		DT_BaseAnimating::m_nHitboxSet = pPropManager->GetOffset(sxor("DT_BaseAnimating"), sxor("m_nHitboxSet"));
		DT_BaseAnimating::m_flCycle = pPropManager->GetOffset(sxor("DT_BaseAnimating"), sxor("m_flCycle"));

		DT_BaseCombatWeapon::m_flNextPrimaryAttack = pPropManager->GetOffset(sxor("DT_BaseCombatWeapon"), sxor("m_flNextPrimaryAttack"));
		DT_BaseCombatWeapon::m_flNextSecondaryAttack = pPropManager->GetOffset(sxor("DT_BaseCombatWeapon"), sxor("m_flNextSecondaryAttack"));
		DT_BaseCombatWeapon::m_hOwner = pPropManager->GetOffset(sxor("DT_BaseCombatWeapon"), sxor("m_hOwner"));
		DT_BaseCombatWeapon::m_iClip1 = pPropManager->GetOffset(sxor("DT_BaseCombatWeapon"), sxor("m_iClip1"));
		DT_BaseCombatWeapon::m_iItemDefinitionIndex = pPropManager->GetOffset(sxor("DT_BaseCombatWeapon"), sxor("m_iItemDefinitionIndex"));

		DT_WeaponCSBase::m_flRecoilIndex = pPropManager->GetOffset(sxor("DT_WeaponCSBase"), sxor("m_flRecoilIndex"));

		auto m_nForceBone = pPropManager->GetOffset(sxor("DT_BaseAnimating"), sxor("m_nForceBone"));

		//Data::m_uMoveHelper = **(std::uintptr_t * *)(Memory::Scan(sxor("client.dll"), sxor("8B 0D ?? ?? ?? ?? 8B 45 ?? 51 8B D4 89 02 8B 01")) + 2);
		//Data::m_uInput = *(std::uintptr_t*)(Memory::Scan(sxor("client.dll"), sxor("B9 ?? ?? ?? ?? F3 0F 11 04 24 FF 50 10")) + 1);
		//Data::m_uPredictionRandomSeed = *(std::uintptr_t*)(Memory::Scan(sxor("client.dll"), sxor("8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04")) + 2);
		//Data::m_uPredictionPlayer = *(std::uintptr_t*)(Memory::Scan(sxor("client.dll"), sxor("89 ?? ?? ?? ?? ?? F3 0F 10 48 20")) + 2);

		const auto image_vstdlib = GetModuleHandleA(sxor("vstdlib.dll"));

		Function::m_uRandomSeed = (std::uintptr_t)(GetProcAddress(image_vstdlib, sxor("RandomSeed")));
		Function::m_uRandomFloat = (std::uintptr_t)(GetProcAddress(image_vstdlib, sxor("RandomFloat")));
		Function::m_uRandomInt = (std::uintptr_t)(GetProcAddress(image_vstdlib, sxor("RandomInt")));

		InitMiscSignatures();

		DT_BaseAnimating::m_BoneAccessor = m_nForceBone + 0x1C; // todo
		DT_BaseAnimating::m_iMostRecentModelBoneCounter = *(int*)(Signatures[INVALIDATEBONECACHE] + 0x1B);
		DT_BaseAnimating::m_flLastBoneSetupTime = *(int*)(Signatures[INVALIDATEBONECACHE] + 0x11);

		Data::m_uModelBoneCounter = *(std::uintptr_t*)(Signatures[INVALIDATEBONECACHE] + 0xA);

		Data::m_nLastCommandAck = *(int*)(Signatures[CL_PREDICT] + 0x20);
		Data::m_nDeltaTick = *(int*)(Signatures[CL_PREDICT] + 0x10);
		Data::m_nLastOutgoingCommand = *(int*)(Signatures[CL_PREDICT] + 0xA);
		Data::m_nChokedCommands = *(int*)(Signatures[CL_PREDICT] + 0x4);
		Data::m_bIsHLTV = *(int*)(Signatures[CL_MOVE] + 0x4);

#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER

		VIRTUALIZER_STR_ENCRYPT_END;
		VIRTUALIZER_FISH_LITE_END;
		return true;
	}

	DECLSPEC_NOINLINE void InitMiscSignatures()
	{
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
#endif // VIRTUALIZER
		VIRTUALIZER_FISH_LITE_START;
		VIRTUALIZER_STR_ENCRYPT_START;


		Signatures[TRACEFILTER_SIMPLE] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F0 83 EC 7C 56 52"));
		Signatures[INIT_KEY_VALUES] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 51 33 C0 C7 45"));
		Signatures[CL_PREDICT] = Memory::Scan(sxor("engine.dll"), sxor("75 30 8B 87 ?? ?? ?? ??"));
		Signatures[CL_MOVE] = Memory::Scan(sxor("engine.dll"), sxor("74 0F 80 BF ?? ?? ?? ?? ??"));
		Signatures[DX9DEVICE] = **(std::uintptr_t**)(Memory::Scan(sxor("shaderapidx9.dll"), sxor("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 1);
		Signatures[INVALIDATEBONECACHE] = Memory::Scan(sxor("client.dll"), sxor("80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81"));
		Signatures[MOVEHELPER] = **(std::uintptr_t**)(Memory::Scan(sxor("client.dll"), sxor("8B 0D ?? ?? ?? ?? 8B 45 ?? 51 8B D4 89 02 8B 01")) + 2);
		Signatures[IINPUT] = *(std::uintptr_t*)(Memory::Scan(sxor("client.dll"), sxor("B9 ?? ?? ?? ?? F3 0F 11 04 24 FF 50 10")) + 1);
		Signatures[PREDRANDOMSEED] = *(std::uintptr_t*)(Memory::Scan(sxor("client.dll"), sxor("8B 0D ?? ?? ?? ?? BA ?? ?? ?? ?? E8 ?? ?? ?? ?? 83 C4 04")) + 2);
		Signatures[PREDPLAYER] = *(std::uintptr_t*)(Memory::Scan(sxor("client.dll"), sxor("89 ?? ?? ?? ?? ?? F3 0F 10 48 20")) + 2);
		Signatures[BREAKABLE] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 51 56 8B F1 85 F6 74 68"));
		Signatures[SET_CLAN_TAG] = Memory::Scan(sxor("engine.dll"), sxor("53 56 57 8B DA 8B F9 FF 15"));
		Signatures[COMMANDS_LIMIT] = Memory::Scan(sxor("engine.dll"), sxor("55 8B EC A1 ? ? ? ? 81 EC ? ? ? ? B9 ? ? ? ? 53 8B 98"));
		Signatures[SETUPVEL_SPEED] = Memory::Scan(sxor("client.dll"), sxor("0F 2F 15 ? ? ? ? 0F 86 ? ? ? ? F3 0F 7E 4C 24"));
		Signatures[GAMERULES] = Memory::Scan(sxor("client.dll"), sxor("A1 ? ? ? ? 85 C0 0F 84 ? ? ? ? 80 B8 ? ? ? ? ? 74 7A"));
		Signatures[UPDATEHUDWEAPONS] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B FE 2C"));
		Signatures[CLEARDEATHNOTICES] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 EC 0C 53 56 8B 71 58"));
		Signatures[RESETANIMSTATE] = Memory::Scan(sxor("client.dll"), sxor("56 6A 01 68 ? ? ? ? 8B F1"));
		Signatures[OVERRIDEPOSTPROCESS] = Memory::Scan(sxor("client.dll"), sxor("80 3D ? ? ? ? ? 53 56 57 0F 85"));
		Signatures[VGUI_START_DRAW] = Memory::Scan(sxor("vguimatsurface.dll"), sxor("55 8B EC 83 E4 C0 83 EC 38"));
		Signatures[VGUI_END_DRAW] = Memory::Scan(sxor("vguimatsurface.dll"), sxor("8B 0D ? ? ? ? 56 C6 05"));
		Signatures[LIST_LEAVES] = Memory::Scan(sxor("client.dll"), sxor("56 52 FF 50 18"));
		Signatures[PTR_ANIMATION_LAYER] = Memory::Scan(sxor("client.dll"), sxor("8B 89 ?? ?? ?? ?? 8D 0C D1"));
		Signatures[GET_ANIMATION_LAYER] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 57 8B F9 8B 97 ? ? ? ? 85 D2"));
		Signatures[UPDATEVISIBILITY_ALLENTS] = Memory::Scan("client.dll", sxor("E8 ? ? ? ? 83 7D D8 00 7C 0F"));
		Signatures[UPDATEANIMSTATE] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24"));
		Signatures[SETABSANGLES] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8"));
		Signatures[DELAYUNSCOPE] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC A1 ? ? ? ? 57 8B F9 B9 ? ? ? ? FF 50 ? 85 C0 75 ? 32 C0"));
		Signatures[SETABSORIGIN] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		Signatures[INVALIDATEPHYSICS] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04"));
		Signatures[SETCOLLISIONBOUNDS] = Memory::Scan(sxor("client.dll"), sxor("53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 10 56 57 8B 7B"));
		Signatures[LOOKUPSEQUENCE] = Memory::follow_rel32((DWORD)Memory::Scan(sxor("client.dll"), sxor("E8 ? ? ? ? 5E 83 F8 FF ")), 1);
		Signatures[LOOKUPBONE] = Memory::follow_rel32((DWORD)Memory::Scan(sxor("client.dll"), sxor("E8 ? ? ? ? 89 44 24 5C")), 1);
		Signatures[GETSEQUENCEACTIVITY] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 53 8B 5D 08 56 8B F1 83"));
		Signatures[STUDIOHDR] = Memory::Scan(sxor("client.dll"), sxor("8B B7 ?? ?? ?? ?? 89 74 24 20"));
		Signatures[SERVER_GLOBALS] = Memory::Scan(sxor("server.dll"), sxor("8B 15 ? ? ? ? 33 C9 83 7A 18 01"));
		Signatures[GETSEQUENCEMOVEDIST] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 EC 0C 56 8B F1 57 8B FA 85 F6 75 14 68"));
		Signatures[DRAWSERVERHITBOXES] = Memory::Scan(sxor("server.dll"), sxor("55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE"));
		Signatures[STUDIOSETPOSEPARAM] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 08 F3 0F 11 54 24 ? 85 D2"));
		Signatures[SETPOSEPARAM] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 51 56 8B F1 0F 28 C2 57 F3 0F 11 45 FC"));
		Signatures[MD5_PSEUDORANDOM] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 70 6A 58"));
		Signatures[RET_MAINTAINSEQUENCETRANS] = Memory::Scan(sxor("client.dll"), sxor("84 C0 74 17 8B 87")); //C_BaseAnimating::MaintainSequenceTransitions
		Signatures[RET_SETUPBONES_PRED_TIME] = Memory::Scan(sxor("client.dll"), sxor("84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05"));
		Signatures[RET_CALCPLAYERVIEW_DRIFTPITCH] = Memory::Scan(sxor("client.dll"), sxor("84 C0 75 0B 8B 0D ? ? ? ? 8B 01 FF 50 4C"));
		Signatures[RET_CALCPLAYERVIEW] = Memory::Scan(sxor("client.dll"), sxor("84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06"));
		Signatures[GET_WEAPON_PREFIX] = Memory::Scan(sxor("client.dll"), sxor("53 56 57 8B F9 33 F6 8B 4F 60 8B 01"));
		Signatures[LOADFROMBUFFEREX] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 34 53 8B 5D 0C 89"));
		Signatures[PTR_PREDICTIONHTLV_MUSOR] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B 1D ? ? ? ? 56 57 85 DB 0F 84 ? ? ? ? 8B CB E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? A1 ? ? ? ? 8B 40 1C"));
		Signatures[ALLOW_EXTRAPOLATION] = Memory::Scan(sxor("client.dll"), sxor("A2 ? ? ? ? 8B 45 E8"));
		Signatures[PREDICTED_VIEWMODEL] = Memory::Scan(sxor("client.dll"), sxor("F3 0F 11 45 ? E8 ? ? ? ? 5E 5F"));
		Signatures[RET_CROSSHAIRCOLOR] = Memory::Scan(sxor("client.dll"), sxor("FF 50 3C 80 7D 2C"));
		Signatures[RET_CROSSHAIROUTLINECOLOR] = Memory::Scan(sxor("client.dll"), sxor("FF 50 3C F3 0F 10 4D ? 66 0F 6E C6"));
		Signatures[WRITEUSERCMD] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D"));
		Signatures[RET_SETUPVELOCITY] = Memory::Scan(sxor("client.dll"), sxor("8B CE E8 ? ? ? ? F3 0F 10 A6"));
		Signatures[RET_HLTV_ACCUMULATE_LAYERS] = Memory::Scan(sxor("client.dll"), sxor("84 C0 75 0D F6 87"));
		Signatures[RET_HLTV_SETUPVELOCITY] = Memory::Scan(sxor("client.dll"), sxor("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80"));
		Signatures[RET_ISLOADOUT_ALLOWED] = Memory::Scan(sxor("client.dll"), sxor("84 C0 75 04 B0 01 5F"));
		Signatures[CLIENT_STATE] = Memory::Scan(sxor("engine.dll"), sxor("A1 ? ? ? ? 8B 80 ? ? ? ? C3"));
		Signatures[BEAMS] = Memory::Scan(("client.dll"), sxor("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9"));
		Signatures[GLOW_OBJECTS] = Memory::Scan(("client.dll"), sxor("0F 11 05 ? ? ? ? 83 C8 01"));
		Signatures[PTR_STANDARDBLENDINGRULES] = (Memory::Scan(("client.dll"), ("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85 F6")));
		Signatures[PTR_SETUPVELOCITY] = (Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 30 56 57 8B 3D")));
		Signatures[PTR_SETUPMOVEMENT] = (Memory::Scan(sxor("client.dll"), sxor("55 8B EC 81 EC ? ? ? ? 53 56 57 8B 3D")));
		Signatures[PTR_SHOULDCOLLIDE] = (Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 B9 ? ? ? ? ? 75 0F")));
		Signatures[PTR_ACCUMULATELAYERS] = (Memory::Scan(("client.dll"), sxor("55 8B EC 57 8B F9 8B 0D ? ? ? ? 8B 01 8B")));
		Signatures[PTR_DOEXTRABONESPROCESSING] = (Memory::Scan(("client.dll"), sxor("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C")));
		Signatures[PTR_GETFOREIGNFALLBACKFONT] = (Memory::Scan(("vguimatsurface.dll"), sxor("80 3D ? ? ? ? ? 74 06 B8")));
		Signatures[PTR_UPDATECLIENTSIDEANIMS] = (Memory::Scan(("client.dll"), sxor("55 8B EC 51 56 8B F1 80 BE ? ? 00 00 00 74 36")));
		Signatures[PTR_SETUPBONES] = (Memory::Scan(("client.dll"), sxor("55 8B EC 83 E4 F0 B8 D8")));
		Signatures[PTR_CALCVIEWBOB] = (Memory::Scan(("client.dll"), sxor("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9")));
		Signatures[PTR_ADDVIEWMODELBOB] = (Memory::Scan(("client.dll"), sxor("55 8B EC A1 ? ? ? ? 56 8B F1 B9 ? ? ? ? FF 50 34 85 C0 74 35")));
		Signatures[PTR_SHOULDSKIPANIMFRAME] = (Memory::follow_rel32(Memory::Scan(("client.dll"), sxor("E8 ? ? ? ? 88 44 24 0B")), 1));
		Signatures[PTR_SHOULDHITENTITY] = (Memory::Scan(("client.dll"), sxor("55 8B EC 8B 55 0C 56 8B 75 08 57")));
		Signatures[PTR_MODIFYEYEPOS] = (Memory::Scan(("client.dll"), sxor("55 8B EC 83 E4 F8 83 EC 5C 53 8B D9 56 57 83")));
		Signatures[PTR_CL_MOVE] = (Memory::Scan(("engine.dll"), sxor("55 8B EC 81 EC 64 01 00 00 53 56 57 8B 3D")));
		Signatures[PTR_PROCESSINTERPLIST] = (Memory::Scan(("client.dll"), sxor("53 0F B7 1D ? ? ? ? 56")));
		Signatures[PTR_SETVIEWMODELOFFSETS] =  (Memory::Scan(("client.dll"), sxor("55 8B EC 8B 45 08 F3 0F 7E 45")));
		Signatures[PTR_GETCOLORMODULATION] = (Memory::Scan(("materialsystem.dll"), sxor("55 8B EC 83 EC ? 56 8B F1 8A 46")));
		Signatures[PTR_ISUSINGSTATICPROPDBGMODES] = (Memory::Scan(("engine.dll"), sxor("8B 0D ? ? ? ? 81 F9 ? ? ? ? 75 ? A1 ? ? ? ? 35 ? ? ? ? EB ? 8B 01 FF 50 ? 83 F8 ? 0F 85 ? ? ? ? 8B 0D")));
		Signatures[PTR_REPORTHIT] = (Memory::Scan(("client.dll"), sxor("55 8B EC 8B 55 08 83 EC 1C F6 42 1C 01")));
		Signatures[PTR_CALCVIEW] = (Memory::Scan(("client.dll"), sxor("55 8B EC 56 8B F1 57 8B 8E ? ? ? ? 83 F9 FF 74 3E 0F B7 C1 C1 E0 04 05 ? ? ? ? C1 E9 10 39 48 04 75 2B 8B 08 85 C9 74 25 8B 01")));
		Signatures[CLIENT_EFFECTS] = Memory::Scan(sxor("client.dll"), sxor("8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F ? 8B 3E"));
		Signatures[HOST_FRAMETICKS] = Memory::Scan(sxor("engine.dll"), sxor("03 05 ? ? ? ? 83 CF 10"));
		Signatures[FIND_HUD_ELEMENT_PTHIS] = Memory::Scan(sxor("client.dll"), sxor("B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08"));
		Signatures[FIND_HUD_ELEMENT] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));
		Signatures[LINEGOESTHRUSMOKE] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0"));
		Signatures[PHYSICS_RUN_THINK] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87"));
		Signatures[SETNEXTTHINK] = Memory::Scan(sxor("client.dll"), sxor("55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B"));

		VIRTUALIZER_STR_ENCRYPT_END;
		VIRTUALIZER_FISH_LITE_END;
#ifdef VIRTUALIZER
		VIRTUALIZER_FISH_LITE_END;
#endif // VIRTUALIZER
	}

	void Destroy()
	{

	}

	std::uintptr_t Signatures[SIGNATURES_MAX];

	namespace C_BaseEntity
	{
		int m_MoveType = 0;
		int m_rgflCoordinateFrame = 0;
	}
	namespace DT_BaseAnimating
	{
		int m_nForceBone = 0;
		int m_bClientSideAnimation = 0;
		int m_nSequence = 0;
		int m_flCycle = 0;
		int m_nHitboxSet = 0;
		DWORD32 m_BoneAccessor;
		DWORD32 m_iMostRecentModelBoneCounter;
		DWORD32 m_flLastBoneSetupTime;
	}
	namespace DT_BaseEntity
	{
		int m_iTeamNum = 0;
		int m_vecOrigin = 0;
	}
	namespace DT_BaseCombatCharacter
	{
		int m_hActiveWeapon = 0;
	}
	namespace C_BasePlayer
	{
		int m_pCurrentCommand = 0;
	}
	namespace DT_BasePlayer
	{
		int m_aimPunchAngle = 0;
		int m_viewPunchAngle = 0;
		int m_vecViewOffset = 0;
		int m_vecVelocity = 0;
		int m_vecBaseVelocity = 0;
		int m_flFallVelocity = 0;
		int m_lifeState = 0;
		int m_nTickBase = 0;
		int m_iHealth = 0;
		int m_fFlags = 0;
		int m_flSimulationTime = 0;
	}
	namespace DT_CSPlayer
	{
		int m_flLowerBodyYawTarget = 0;
		int m_angEyeAngles = 0;
		int m_bIsScoped = 0;
		int m_flDuckAmount = 0;
		int m_flPoseParameter = 0;
		int m_bHasHelmet = 0;
	}
	namespace DT_BaseCombatWeapon
	{
		int m_flNextPrimaryAttack = 0;
		int m_flNextSecondaryAttack = 0;
		int m_hOwner = 0;
		int m_iClip1 = 0;
		int m_iItemDefinitionIndex = 0;
	}
	namespace DT_WeaponCSBase
	{
		int m_flRecoilIndex = 0;
	}
	namespace Data
	{
		//std::uintptr_t m_uMoveHelper = 0u;
		//std::uintptr_t m_uInput = 0u;
		//std::uintptr_t m_uPredictionRandomSeed = 0u;
		//std::uintptr_t m_uPredictionPlayer = 0u;
		std::uintptr_t m_uModelBoneCounter = 0u;
		DWORD32 m_nDeltaTick = 0;
		DWORD32 m_nChokedCommands = 0;
		DWORD32 m_nLastOutgoingCommand = 0;
		DWORD32 m_nLastCommandAck = 0;
		DWORD32 m_bIsHLTV = 0;
	}
	namespace Function
	{
		std::uintptr_t m_uRandomSeed = 0u;
		std::uintptr_t m_uRandomFloat = 0u;
		std::uintptr_t m_uRandomInt = 0u;
	}

}
