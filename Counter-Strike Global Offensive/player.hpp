#pragma once

#include "entity.hpp"

enum Indexes
{
	GET_ABS_ORIGIN = 10,
	GET_ABS_ANGLES = 11,
	IS_SELF_ANIMATING = 155,
	SET_MODEL = 9,
	UPDATE_CLIENTSIDE_ANIMATIONS = 223, //219
	GET_MAX_SPEED = 269, //219
	SET_LOCAL_VIEWANGLES = 372, //219
	PRE_THINK = 317,
	THINK = 138,
	IS_LOCAL_PLAYER = 157, //219
};

class CBoneAccessor
{
public:
	inline matrix3x4_t* get_bone_array_for_write(void) const
	{
		return m_pBones;
	}
	inline void set_bone_array_for_write(matrix3x4_t* bonearray)
	{
		m_pBones = bonearray;
	}
	alignas(16) matrix3x4_t* m_pBones;

	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};

class C_BasePlayer : public C_BaseCombatCharacter
{
public:
	unsigned int FindInDataMap(datamap_t* pMap, const char* name);
	int& m_iCrosshairID();
	typedescription_t* get_datamap_entry(datamap_t* pMap, const char* name);
	datamap_t* GetPredDescMap();
	QAngle& m_aimPunchAngle();
	QAngle m_aimPunchAngleScaled();
	QAngle& m_viewPunchAngle();
	Vector& m_vecViewOffset();
	Vector& m_vecVelocity();
	Vector& m_vecBaseVelocity();
	float& m_flFallVelocity();
	char& m_lifeState();
	int& m_nTickBase();
	int& m_nFinalPredictedTick();
	int& m_vphysicsCollisionState();
	int& m_iHealth();
	int& m_fFlags();

	int& m_iPlayerState();

	bool& m_bIsLocalPlayer();

	bool& m_bSpotted();

	float& m_flThirdpersonRecoil();

	bool& m_bIsScoped();

	bool& m_bIsRagdoll();

	bool IsDead();
	matrix3x4_t& GetCollisionBoundTrans();
	float& m_flLowerBodyYawTarget();
	CCSGOPlayerAnimState* get_animation_state();
	void SetCurrentCommand(CUserCmd* cmd );
	Vector& get_abs_origin();
	bool is_local_player();
	Vector& m_vecAbsOrigin();
	QAngle& get_abs_angles();
	float& get_player_max_speed();
	float& m_flSimulationTime();
	float& m_flAnimTime();
	void select_item(const char* name, int sub_type);
	void interpolate(float time);
	QAngle& get_render_angles();
	QAngle& get_local_angles();
	int& m_iObserverMode();
	int& m_nIsAutoMounting();
	Vector& GetAutoMoveOrigin();
	Vector& GetAutomoveTargetEnd();
	float& GetAutomoveTargetTime();
	float& GetAutomoveStartTime();
	void* animation_layers_ptr();
	C_AnimationLayer& animation_layer(int i);
	CUtlVector<C_AnimationLayer>& m_anim_overlay();
	C_AnimationLayer& get_animation_layer(int index);
	int get_animation_layers_count();
	Vector Weapon_ShootPosition();
	Vector EyePosition();
	Vector GetEyePosition();
	float& m_flCycle();
	int& m_iEFlags();
	Vector& m_vecAbsVelocity();
	float& m_flOldSimulationTime();
	float m_flSpawnTime();
	float& m_flNextAttack();
	float& m_flTimeOfLastInjury();
	float& m_flGroundAccelLinearFracLastTime();
	float& m_flDuckAmount();
	CUtlVector<matrix3x4_t>& m_CachedBoneData();
	int& m_bone_count();
	void force_bone_cache();
	bool& m_bGunGameImmunity();
	QAngle& m_aimPunchAngleVel();
	int& m_hGroundEntity();
	float& m_flDuckSpeed();
	Vector get_bone_pos(int iBone);
	void GetBonePosition(int iBone, Vector& origin, QAngle& angles);
	Vector get_bone_pos(int iBone, matrix3x4_t mx[]);
	void set_model_index(int index);
	int& m_iShotsFired();
	bool& m_bDucking();
	bool& m_bDucked();
	float& m_flLastDuckTime();
	int& LastBoneSetupFrame();
	void UpdateVisibilityAllEntities();
	void force_bone_rebuild();
	float& m_flVelocityModifier();
	HANDLE m_hViewModel();
	CBaseHandle& m_hNetworkMoveParent();
	CBaseHandle& m_pMoveParent();
	int& m_fEffects();
	const char* m_szLastPlaceName();
	int& m_iAccount();
	int* m_hMyWeapons();
	void pre_think();
	CBaseHandle* m_hObserverTarget();
	bool SetupBonesEx(int matrix = 0xFFF00);
	void update_animstate(CCSGOPlayerAnimState* state, QAngle angle);
	//float& first_bone_snapshot();
	//float& second_bone_snapshot();
	void set_abs_angles(QAngle origin);
	bool delay_unscope(float& fov);
	bool& m_bWaitForNoAttack();
	bool& m_bStrafing();
	float& m_flTimeLastTouchedGround();
	bool& client_side_animation();
	void pre_data_update(int updateType);
	void on_pre_data_change(int updateType);
	void on_data_changed(int updateType);
	void post_data_update(int updateType);
	void set_abs_origin(Vector origin);
	bool physics_run_think(int think_type = 0);
	void prediction_unk_func(int think_tick);
	void set_local_viewangles(QAngle angles);
	void think();
	int& m_nHitboxSet();
	std::array<float, 24> & m_flPoseParameter();
	int draw_model(int flags, uint8_t alpha);
	int& m_nSequence();
	float get_bomb_blow_timer();
	bool m_bBombTicking();
	bool m_bHasDefuser();
	int& m_nExplodeEffectTickBegin();
	void invalidate_anims(int m = 0x8);
	float& m_flHealthShotBoostExpirationTime();
	float& m_flMaxSpeed();
	bool& m_bIsDefusing();
	bool& m_bIsWalking();
	int& m_iMoveState();
	C_BasePlayer* m_hBombDefuser();
	CBoneAccessor& GetBoneAccessor();
	void set_collision_bounds(const Vector mins, const Vector maxs);
	int LookupSequence(const char* label);
	int LookupBone(const char* szName);
	float get_bomb_defuse_timer();
	float& m_flDefuseCountDown();
	int& m_nSkin();
	int& m_nBody();
	int& m_hOwnerEntity();
	int& m_hOwner();
	int& m_nSmokeEffectTickBegin();
	bool& m_bHasHelmet();
	bool& m_bHasHeavyArmor();
	bool& m_bResumeZoom();
	int get_sec_activity(int sequence);
	int& m_nModelIndex();
	//int SelectWeightedSequenceFromModifiers(int activity, CUtlSymbol* pActivityModifiers, int iModifierCount);
	int& TakeDamage();
	int& m_ArmorValue();
	float& m_flFlashMaxAlpha();
	float& m_flFlashDuration();
	float& m_flFlashTime();
	CStudioHdr* GetModelPtr();
	uint8_t* GetServerEdict();
	bool ComputeHitboxSurroundingBox(matrix3x4_t* mx, Vector* pVecWorldMins, Vector* pVecWorldMaxs);
	float GetSequenceCycleRate(CStudioHdr* pStudioHdr, int iSequence);
	float GetLayerSequenceCycleRate(C_AnimationLayer* layer, int iSequence);
	float GetSequenceMoveDist(CStudioHdr* pStudioHdr, int iSequence);
	void DrawServerHitboxes(float time = 0.f);
	bool IsBot();
	float& m_flLastBoneSetupTime();
	int& m_iMostRecentModelBoneCounter();
	CBoneAccessor& m_BoneAccessor();
	int& m_nComputedLODframe();
	bool* s_bEnableInvalidateBoneCache();
	void update_clientside_animations();
	int DetermineSimulationTicks(void);
	int AdjustPlayerTimeBase(int simulation_ticks);
	float& m_surfaceFriction();
	std::string m_szNickName();
	C_WeaponCSBaseGun* get_weapon();
	QAngle& m_angEyeAngles();
	Vector& m_vecOldOrigin();
	std::vector<Vector> build_capsules(Vector Min, Vector Max, float Radius, float scale, matrix3x4_t& Transform);
	float Studio_SetPoseParameter(const CStudioHdr* pStudioHdr, int iParameter, float flValue, float& ctlValue);
	void set_pose_param(int param, float value);
	bool should_fix_modify_eye_pos();
	bool get_multipoints(int ihitbox, std::vector<Vector>& points, matrix3x4_t mx[], bool &only_center, float force_pointscale = -1.f);
};

class C_BaseViewModel : public C_BasePlayer
{
public:
	CBaseHandle get_viewmodel_weapon();
	int m_nViewModelIndex();
	float& m_flCycle();
	float& m_flModelAnimTime();
	int& m_nAnimationParity();
	CBaseHandle& m_hNetworkMoveParent();
	int& m_nSequence();
	void send_matching_seq(int seq);
};

class C_CSPlayer : public C_BasePlayer
{
public:
};

FORCEINLINE C_CSPlayer* ToCSPlayer( C_BaseEntity* pEnt )
{
	if( !pEnt || !pEnt->IsPlayer() )
		return nullptr;

	return ( C_CSPlayer* )( pEnt );
}