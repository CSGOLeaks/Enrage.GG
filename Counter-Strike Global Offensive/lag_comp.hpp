#pragma once
#include "sdk.hpp"
#include <deque>
#include <array>
#include <unordered_map>
#include "CUtlVector.hpp"

class C_Tickrecord
{
public:
	//// default ctor.
	//C_Tickrecord() 
	//{
	//	reset();
	//}

	C_Tickrecord() : data_filled(false), dormant(true), first_after_dormancy(true), valid(false)
	{
	}

	// ctor.
	C_Tickrecord(C_BasePlayer* player, bool backup = false)
	{
		store(player, backup);
	}

	/*~C_Tickrecord()
	{

	}*/

	void store(C_BasePlayer* player, bool backup = false);

	void apply(C_BasePlayer* player, bool backup = false, bool dont_force_matrix = false);

	/*
	* reset
	* Resets the tick record
	*/
	void reset()
	{
		/*if (!data_filled)
			return;*/

		origin = Vector::Zero;
		abs_origin = Vector::Zero;
		abs_velocity = Vector::Zero;
		velocity = Vector::Zero;
		object_mins = Vector::Zero;
		object_maxs = Vector::Zero;

		eye_angles = QAngle::Zero;

		abs_angles = 0.f;
		original_abs_yaw = 0.f;
		entity_flags = 0;
		ientity_flags = 0;
		m_tick = 0;
		bones_count = 0;
		lag = 0;

		latency = 0.f;
		desync_delta = 0.f;
		duck_amt = 0.f;
		simulation_time = 0.f;
		animation_time = 0.f;
		simulation_time_old = 0.f;
		shot_time = 0.f;
		lower_body_yaw = 0.f;
		thirdperson_recoil = 0.f;
		ground_accel_last_time = 0.f;
		max_current_speed = 0.f;
		m_primary_cycle = 0.f;
		m_move_weight = 0.f;
		time_of_last_injury = 0.f;
		time_delta = 0.f;
		velocity_modifier = 0.f;
		 m_strafe_change_weight = 0.f;
		 m_strafe_change_cycle = 0.f;
		 m_acceleration_weight = 0.f;

		left_side = 0.f, right_side = 0.f;
		stop_to_full_run_frac = FLT_MAX;
		head_pos = Vector::Zero;

		fill(begin(pose_paramaters), end(pose_paramaters), 0.f);
		fill(begin(animstate_left_params), end(animstate_left_params), 0.f);
		fill(begin(animstate_right_params), end(animstate_right_params), 0.f);
		fill(begin(left_poses), end(left_poses), 0.f);
		fill(begin(right_poses), end(right_poses), 0.f);

		resolver_index = -1;
		resolver_type = -1;
		freestanding_index = -1;
		animations_index = -1;

		m_strafe_sequence = 0;
		tickcount = 0;
		land_type = 0;
		entity_anim_flags = 0;
		entity_index = 0;
		simulation_time_delay = 0;
		resolver_delta_multiplier = 0.f;

		did_interpolate = false;
		accurate_anims = false;
		data_filled = false;
		shot_this_tick = false;
		animations_updated = false;
		had_use_key = false;
		valid = false;
		exploit = false;
		lc_exploit = false;
		animated = false;
		dormant = true;
		//corrected_velocity = false;
		breaking_lc = false;
		land_time = 0.0f;
		lby_flicked_time = 0.0f;
		is_landed = false;
		land_in_cycle = false;
		first_after_dormancy = true;
		can_rotate = false;
		fake_walking = false;
		resolved = false;
		not_desyncing = false;
		in_jump = false;
	}

	matrix3x4_t matrixes[128] = {};
	matrix3x4_t leftmatrixes[128] = {};
	matrix3x4_t rightmatrixes[128] = {};
	C_AnimationLayer anim_layers[13] = {};
	CCSGOPlayerAnimState animstate;
	std::array<float, 24> pose_paramaters = {};
	std::array<float, 8> animstate_left_params = {};
	std::array<float, 8> animstate_right_params = {};
	std::array<float, 24> left_poses = {};
	std::array<float, 24> right_poses = {};

	Vector head_pos = Vector::Zero;
	Vector origin = Vector::Zero;
	Vector abs_origin = Vector::Zero;
	Vector abs_velocity = Vector::Zero;
	Vector velocity = Vector::Zero;
	Vector object_mins = Vector::Zero;
	Vector object_maxs = Vector::Zero;
	QAngle eye_angles = QAngle::Zero;

	float abs_angles = 0.f;
	float time_of_last_injury = 0.f;
	float original_abs_yaw = 0.f;
	float time_delta = 0.f;

	float latency = 0.f;
	float desync_delta = 0.f;
	float resolver_delta_multiplier = 0.f;
	float thirdperson_recoil = 0.f;
	float duck_amt = 0.f;
	float simulation_time = 0.f;
	float animation_time = 0.f;
	float simulation_time_old = 0.f;
	float shot_time = 0.f;
	float lower_body_yaw = 0.f;
	float ground_accel_last_time = 0.f;
	float lby_flicked_time;
	float max_current_speed = 0.f;
	float animation_speed = 0.f;
	float velocity_modifier = 0.f;
	float animation_update_start_time = 0.f;
	float duck_amount_per_tick = 0.f;

	int m_strafe_sequence = 0;
	float m_primary_cycle = 0.f;
	float m_move_weight = 0.f;
	float m_strafe_change_weight = 0.f;
	float m_strafe_change_cycle = 0.f;
	float m_acceleration_weight = 0.f;

	float left_side = 0.f, right_side = 0.f;
	//float desync_multiplier = 0.f;
	float stop_to_full_run_frac = FLT_MAX;

	//matrix3x4_t interpolated[128];

	//matrix3x4_t aleftmatrixes[128];
	//matrix3x4_t arightmatrixes[128];
	//matrix3x4_t zeromatrixes[128];
	//matrix3x4_t leftlmatrixes[128];
	//matrix3x4_t rightlmatrixes[128];

	//int type = RECORD_NORMAL;
	//int writable_bones = 0;

	int simulation_time_delay = 0;
	int resolver_index = -1;
	int entity_index = 0;
	int record_index = 0;
	int freestanding_index = -1;
	int animations_index = -1;
	int resolver_type = -1;
	int entity_flags = 0;
	int entity_anim_flags = 0;
	int	ientity_flags = 0;
	int m_tick = 0;
	int bones_count = 0;
	int lag = 0;
	int tickcount = 0;
	int land_type = 0;
	float land_time = 0.0f;

	bool is_landed = false;
	bool land_in_cycle = false;
	bool not_desyncing = false;
	bool in_jump = false;
	bool had_use_key = false;


	bool did_interpolate = false;
	bool accurate_anims = false;
	bool data_filled = false;
	bool shot_this_tick = false;
	bool animations_updated = false;
	bool valid = false;
	bool exploit = false;
	bool lc_exploit = false;
	bool animated = false;
	bool dormant = true;
	/*bool can_aim_at_foot = false;
	bool corrected_velocity = false;*/
	bool breaking_lc = false;
	bool can_rotate = false;
	bool fake_walking = false;
	bool resolved = false;
	//bool extrapolated = false;
	//bool breaking_duck = false;
	bool first_after_dormancy = false;

	//// lagfix stuff.
	//bool   m_broke_lc;
	//Vector m_pred_origin;
	//Vector m_pred_velocity;
	//float  m_pred_time;
	//int    m_pred_flags;
};

class C_Simulationdata
{
public:
	C_Simulationdata() : entity(nullptr), flags(0), simtime(0.f)
	{
	}

	/*~C_Simulationdata()
	{
	}*/

	C_BasePlayer* entity;

	Vector origin;
	Vector velocity;
	float simtime;

	int flags;
	bool jumped = false;
	bool extrapolation = false;

	bool data_filled = false;
};

class c_player_records
{
public:
	/*c_player_records()
	{
		reset();
	}*/
	//~c_player_records()
	//{
	//	reset();
	//}

	//using records_t = std::deque< C_Tickrecord >;
	/*std::array <*/C_Tickrecord/*, 64u> */tick_records[64] = {};
	C_Tickrecord restore_record;
	//std::array<int, 64> resolver_indexes = {};
	player_info saved_info;
	std::array<Vector, HITBOX_MAX> hitboxes_positions = {};
	std::array<float, HITBOX_MAX> hitboxes_damage = {};
	std::array<bool, HITBOX_MAX> hitboxes_viable = {};
	Vector4D last_esp_box = Vector4D(0, 0, 0, 0);
	Vector render_origin = Vector::Zero;
	Vector prev_render_origin = Vector::Zero;
	float previous_dormant = 0.f;
	int dormant_flags = 0;
	float last_sound = 0.f;
	float prev_last_sound = 0.f;

	int saved_hp = 0;
	int records_count = 0;
	//records_t tick_records = {};
	float spawntime = 0;
	float m_old_sim = 0.0f;
	float m_cur_sim = 0.0f;	
	float m_sim_cycle = 0.0f;
	float m_sim_rate = 0.0f;

	C_Tickrecord* best_record;
	C_Tickrecord* oldest_valid_record;
	C_Tickrecord* pre_old_valid_record;
	int interpolated_count = 0;
	int tick_count = 0;
	int backtrack_ticks = 0;
	float last_low_delta_time = 0.f;
	float last_low_shot_time = 0.f;
	//float last_dormancy_time = 0.f;
	C_BasePlayer* player = nullptr;
	int userid = 0;
	int last_scan_time = 0;
	int resolver_index;
	bool skip_next_tick = false;
	bool is_restored = false;
	bool checked_freestand = false;

	//int missed_at_onshot = 0;

	void reset()
	{
		if (player == nullptr && records_count <= 0)
			return;

		//if (tick_records.size() > 0)

		is_restored = true;
		best_record = nullptr;
		oldest_valid_record = nullptr;
		pre_old_valid_record = nullptr;

		tick_count = -1;
		backtrack_ticks = 0;
		 interpolated_count = 0;
		 m_old_sim = 0.0f;
		 m_cur_sim = 0.0f;
		//missed_at_onshot = 0;
		render_origin = Vector::Zero;
		//previous_dormant = 0.f;
		last_esp_box = Vector4D(0, 0, 0, 0);
		//last_dormancy_time = 0.f;
		checked_freestand = false;
		resolver_index = 0;
		//memset(&hitboxes_positions[0], 0, sizeof(Vector) * HITBOX_MAX);
		//memset(&hitboxes_viable[0], 0, sizeof(bool) * HITBOX_MAX);
		//memset(&hitboxes_damage[0], 0, sizeof(float) * HITBOX_MAX);
		hitboxes_viable.fill(0);
		hitboxes_positions.fill(Vector(0,0,0));
		hitboxes_damage.fill(0.f);
		restore_record.reset();

		//tick_records.fill(C_Tickrecord());
		records_count = 0;

		userid = 0;
		player = nullptr;
	}
};

class local_data
{
public:
	QAngle angles;
	int flags;
	float duck_amt;
	float simtime;
	float lby;
};

//class virtual_lagcomp
//{
//
//};

class c_lagcomp
{
public:
	virtual void backup_players(bool restore);
	virtual void update_lerp();
	//bool StartPrediction(C_BasePlayer* player, c_player_records* data);
	//void PlayerMove(C_Tickrecord* record);
	//void AirAccelerate(C_BasePlayer* m_player, C_Tickrecord* record, QAngle angle, float fmove, float smove);
	//void PredictAnimations(C_BasePlayer* m_player, CCSGOPlayerAnimState* state, C_Tickrecord* record);
	//virtual void sync_animations();
	//void sync_animations(C_BasePlayer * entity);
	virtual void update_network_info();
	virtual void simulate_movement(C_Simulationdata& data);
	virtual bool is_time_delta_too_large(C_Tickrecord* wish_record, bool ignore_deadtime = false);
	//bool is_time_delta_too_large(const float& simulation_time);
	virtual void reset(CCSGOPlayerAnimState* state);
	virtual void fix_anim_layers(CUserCmd* cmd, CCSGOPlayerAnimState* state);
	//void update_player_record_data(C_BasePlayer* entity);
	//void update_local_animations_data(CUserCmd* cmd, bool* send_packet);
	virtual void update_local_animations(CUserCmd* cmd, bool* send_packet);
	virtual void build_local_bones(C_BasePlayer* local);
	virtual bool can_resolve_by_anims(C_Tickrecord *record, C_Tickrecord* prev_record);
	virtual bool has_firing_animation(C_BasePlayer* m_player, C_Tickrecord* record);
	virtual void prepare_player_anim_update(C_BasePlayer* m_player, C_Tickrecord* record, C_Tickrecord* previous, int resolver_side);
	virtual void update_animation_system(C_BasePlayer* m_player, C_Tickrecord* record, C_Tickrecord* previous, int resolver_side);
	virtual void recalculate_velocity(C_Tickrecord* record, C_BasePlayer* m_player, C_Tickrecord* previous);
	virtual void parse_player_data(C_Tickrecord* record, C_BasePlayer* m_player);
	virtual void run_animation_data_resolver(C_Tickrecord* record, C_BasePlayer* m_player, C_Tickrecord* previous);
	virtual void update_animations_data(C_Tickrecord* record, C_BasePlayer* m_player);
	virtual void interpolate(ClientFrameStage_t stage);
	virtual void store_records(ClientFrameStage_t stage);
	virtual void net_update(C_BasePlayer* m_player);
	//void store_records();
	virtual void reset();

	virtual float get_interpolated_time();
	 
	/*std::array<*/c_player_records/*,64u>*/ records[64];
	std::deque<local_data> m_local_chocked_angles;
};

//struct _shotinfo
//{
//	_shotinfo(C_BasePlayer* target, matrix3x4_t* mx, Vector& eyepos, Vector& hitbox, c_player_records* record/*, C_Tickrecord trecord*/, float sprd, float velmod, int hitboxid, int predicted_damage, int hitgroup, int walls_penetrated, float hitchance, float time)
//	{
//		memcpy(_matrix, mx, sizeof(matrix3x4_t) * 128);
//		_eyepos = eyepos;
//		_target = target;
//		_hitbox = hitbox;
//		_hitboxid = hitboxid;
//		_predicted_damage = predicted_damage;
//		_hitgroup = hitgroup;
//		_velmod = velmod;
//
//		_record = record;
//		_tickrecord = record->best_record;
//		_time = time;
//
//		_spread = sprd;
//		_walls_penetrated = walls_penetrated;
//		_hurt_called = _impact_called = _printed = false;
//		_impact_pos = { 0,0,0 };
//		_hitchance = hitchance;
//		_impacts.clear();
//	}
//
//	_shotinfo()
//	{
//		_eyepos = { 0,0,0 };
//		_hitbox = { 0,0,0 };
//		_target = nullptr;
//		_record = nullptr;
//		_tickrecord = nullptr;
//		_avg_hurt_tick = 0;
//		_hurt_called = false;
//		_headshot = false;
//		_impact_called = false;
//		_spread = 0.f;
//		_velmod = 0.f;
//		_time = 0.f;
//		_hitgroup = 0;
//		_predicted_damage = 0;
//		_impact_pos = { 0,0,0 };
//		_printed = false;
//		_hitboxid = -1;
//		_final_damage = 0;
//		_walls_penetrated = 0;
//		_impacts_count = 0;
//		_final_hitgroup = 0;
//		_hitchance = 0;
//		_impacts.clear();
//	}
//
//	int _avg_hurt_tick = 0;
//	bool _hurt_called = false;
//	int _hitgroup = 0;
//	bool _headshot = false;
//	bool _impact_called = false;
//	float _spread = 0.f;
//	float _time = 0.f;
//	float _velmod = 0.f;
//	Vector _impact_pos = { 0,0,0 };
//	std::deque <Vector>_impacts = {};
//	bool _printed = false;
//	int _hitboxid = -1;
//	int _predicted_damage = 0;
//	int _final_damage = 0;
//	int _final_hitgroup = 0;
//	int _walls_penetrated = 0;
//	int _impacts_count = 0;
//	int _hitchance = 0;
//	matrix3x4_t _matrix[128];
//	Vector _eyepos = { 0,0,0 };
//	Vector _hitbox = { 0,0,0 };
//	C_BasePlayer* _target = nullptr;
//	c_player_records *_record;
//	C_Tickrecord *_tickrecord;
//	C_Tickrecord _restore;
//};