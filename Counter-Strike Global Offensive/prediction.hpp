#pragma once

#include "sdk.hpp"

struct PlayerData
{
	PlayerData() { 
		reset();
	};
	~PlayerData() {
		reset();
	};

	void reset()
	{
		m_aimPunchAngle.clear();
		m_aimPunchAngleVel.clear();
		m_viewPunchAngle.clear();

		m_vecViewOffset.clear();
		m_vecBaseVelocity.clear();
		m_vecVelocity.clear();
		m_vecOrigin.clear();

		m_flFallVelocity = 0.0f;
		m_flVelocityModifier = 0.0f;
		m_flDuckAmount = 0.0f;
		m_flDuckSpeed = 0.0f;
		m_fAccuracyPenalty = 0.0f;
		m_flThirdpersonRecoil = 0.0f;

		m_hGroundEntity = 0;
		m_nMoveType = 0;
		m_nFlags = 0;
		m_nTickBase = 0;
		m_flRecoilIndex = 0;
		tick_count = 0;
		command_number = INT_MAX;
		is_filled = false;
	}

	QAngle m_aimPunchAngle = {};
	QAngle m_aimPunchAngleVel = {};
	QAngle m_viewPunchAngle = {};

	Vector m_vecViewOffset = {};
	Vector m_vecBaseVelocity = {};
	Vector m_vecVelocity = {};
	Vector m_vecOrigin = {};

	float m_flFallVelocity = 0.0f;
	float m_flVelocityModifier = 0.0f;
	float m_flThirdpersonRecoil = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_flDuckSpeed = 0.0f;
	float m_fAccuracyPenalty = 0.0f;

	int m_hGroundEntity = 0;
	int m_nMoveType = 0;
	int m_nFlags = 0;
	int m_nTickBase = 0;
	int m_flRecoilIndex = 0;
	
	int tick_count = 0;
	int command_number = INT_MAX;

	bool is_filled = false;
};

struct c_tickbase_array
{
	int command_num = -1;
	int tickbase_original = 0;
	int tickbase_this_tick = 0;
	int extra_cmds = 0;
	int extra_shift = 0;
	bool m_set = false;
	bool increace = false;
	bool doubletap = false;
	bool charge = false;
};

namespace Engine
{

	struct RestoreData {
		void Reset() {
			m_aimPunchAngle.Set();
			m_aimPunchAngleVel.Set();
			m_viewPunchAngle.Set();
			m_local_view_angles.Set();

			m_vecViewOffset.Set();
			m_vecBaseVelocity.Set();
			m_vecVelocity.Set();
			m_vecLocalOrigin.Set();
			m_vecOrigin.Set();

			m_flFallVelocity = 0.0f;
			m_flVelocityModifier = 0.0f;
			m_flMaxspeed = 0.0f;
			m_flWaterJumpTime = 0.0f;
			m_flDuckAmount = 0.0f;
			m_flDuckSpeed = 0.0f;
			m_surfaceFriction = 0.0f;

			m_fAccuracyPenalty = 0.0f;
			m_flRecoilIndex = 0.f;

			m_hGroundEntity = 0;
			m_nMoveType = 0;
			m_nFlags = 0;
			m_nTickBase = 0;
			m_Activity = 0;
			m_flLastShotTime = 0;
			m_bResumeZoom = 0;
		}

		RestoreData& operator=(const RestoreData& other)
		{
			m_aimPunchAngle = other.m_aimPunchAngle;
			m_aimPunchAngleVel = other.m_aimPunchAngleVel;
			m_viewPunchAngle = other.m_viewPunchAngle;
			m_local_view_angles = other.m_local_view_angles;

			m_vecViewOffset = other.m_vecViewOffset;
			m_vecBaseVelocity = other.m_vecBaseVelocity;
			m_vecVelocity= other.m_vecVelocity;
			m_vecOrigin = other.m_vecOrigin;
			m_vecLocalOrigin = other.m_vecLocalOrigin;

			m_flFallVelocity = other.m_flFallVelocity;
			m_flVelocityModifier =other.m_flVelocityModifier;
			m_flDuckAmount = other.m_flDuckAmount;
			m_flDuckSpeed = other.m_flDuckSpeed;
			m_surfaceFriction = other.m_surfaceFriction;

			m_fAccuracyPenalty = other.m_fAccuracyPenalty;
			m_flRecoilIndex = other.m_flRecoilIndex;

			m_hGroundEntity = other.m_hGroundEntity;
			m_nMoveType = other.m_nMoveType;
			m_nFlags = other.m_nFlags;
			m_nTickBase = other.m_nTickBase;
			m_bResumeZoom = other.m_bResumeZoom;
			m_vecAbsVelocity = other.m_vecAbsVelocity;
			m_iMoveState = other.m_iMoveState;
			m_RefEHandle = other.m_RefEHandle;
			m_vecPreviouslyPredictedOrigin = other.m_vecPreviouslyPredictedOrigin;
			m_flTimeLastTouchedGround = other.m_flTimeLastTouchedGround;
			m_iEFlags = other.m_iEFlags;
			m_nNextThinkTick = other.m_nNextThinkTick;
			m_flMaxspeed = other.m_flMaxspeed;
			
			m_Activity = other.m_Activity;
			m_MoveCollide = other.m_MoveCollide;
			m_flLastShotTime = other.m_flLastShotTime;
			m_flWaterJumpTime = other.m_flWaterJumpTime;
			return *this;
		}

		void Setup(C_BasePlayer* player);

		void Apply(C_BasePlayer* player);

		QAngle m_aimPunchAngle = {};
		QAngle m_aimPunchAngleVel = {};
		QAngle m_viewPunchAngle = {};
		QAngle m_local_view_angles = {};

		Vector m_vecViewOffset = {};
		Vector m_vecBaseVelocity = {};
		Vector m_vecVelocity = {};
		Vector m_vecOrigin = {};
		Vector m_vecAbsVelocity = {};
		Vector m_vecPreviouslyPredictedOrigin = {};
		Vector m_vecNetworkOrigin = {};
		Vector m_vecLocalOrigin = {};

		float m_flFallVelocity = 0.0f;
		float m_flWaterJumpTime = 0.0f;
		float m_flVelocityModifier = 0.0f;
		float m_flDuckAmount = 0.0f;
		float m_flDuckSpeed = 0.0f;
		float m_surfaceFriction = 0.0f;

		float m_fAccuracyPenalty = 0.0f;
		float m_flRecoilIndex = 0;
		float m_Activity = 0.0f;
		float m_flLastShotTime = 0.0f;
		float m_flTimeLastTouchedGround = 0.0f;
		float m_flMaxspeed = 0.0f;

		int m_hGroundEntity = 0;
		int m_RefEHandle = 0;
		int m_nFlags = 0;
		int m_nTickBase = 0;
		int m_iEFlags = 0;
		int m_nNextThinkTick = 0;
		int m_iMoveState = 0;

		bool is_filled = false;
		bool m_bResumeZoom = false;
		unsigned char m_nMoveType = 0;
		unsigned char m_MoveCollide = 0;
	};

	struct latency_calc_t
	{
		int command_num = -1;
		int m_tick = 0;
	};


class Prediction : public Core::Singleton<Prediction>
{
public:
	void calculate_desync_delta();
	void SetupMovement(CUserCmd* cmd);
	void PrePrediction(CUserCmd* cmd);
	void Predict(CUserCmd* cmd);
	void End();

	virtual int GetFlags();

	virtual int GetMoveType();

	virtual Vector GetVelocity();

	virtual float GetDuckAmount();

	virtual float GetSpread();

	virtual float GetInaccuracy();

	virtual void FixNetvarCompression(int time);
	virtual void detect_prediction_error(PlayerData* m_data, int m_tick);


	//void OnRunCommand( C_BasePlayer* player );

	int last_time_filled = 0;
	float m_flCurrentTime = 0.0f;
	float m_flFrameTime = 0.0f;
	float sv_footsteps_backup = 0.0f;
	float sv_min_jump_landing_sound_backup = 0.0f;
	float pred_error_time = 0.0f;
	int m_MoveType = 0; 
	int m_fFlags = 0;
	bool m_bResumeZoom = 0;
	float m_flSpread = 0.0f;
	float m_flDuckAmount = 0.0f;
	float m_autostop_velocity_to_validate = 0.f;
	float m_flInaccuracy = 0.0f;
	float m_flCalculatedInaccuracy = 0.0f;
	Vector m_vecVelocity = Vector::Zero;
	Vector m_vecOrigin = Vector::Zero;
	CBaseHandle m_hWeapon = 0;

	int m_nViewModelIndex = 0;
	int m_nAnimationParity = 0;
	int m_nSequence = 0;
	float networkedCycle = 0.0f;
	float animationTime = 0.0f;
	int prev_buttons = 0;


	Vector m_vecPredVelocity = Vector::Zero;
	Vector m_vecPrePredVelocity = Vector::Zero;

	bool bFirstCommandPredicted;
	bool m_bInPrediction;
	int prev_cmd_command_num = 0;

	//int bRandomSeed;
	//uint32_t bUserCmd;
	//C_BasePlayer* bPredPlayer;
	c_tickbase_array m_tickbase_array[150] = {};
	latency_calc_t m_tick_history[150] = {};

	PlayerData m_Data[150] = { };
	//CMoveData newest_move_data = {};
	int m_nTickbase;
	//RestoreData restore_data;
	CMoveData move_data = {};
private:
	C_WeaponCSBaseGun* m_pWeapon = nullptr;
	ConVar* sv_footsteps = nullptr;
	ConVar* sv_min_jump_landing_sound = nullptr;
	//ConVar* cl_pred_optimize = nullptr;
	CMoveData something = {};
	//PlayerData m_InterpolatedData;
	//bool reset_move_data = false;
	//CMoveData move_data_clear = {};
};

}