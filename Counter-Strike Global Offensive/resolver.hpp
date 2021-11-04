#pragma once
#include "sdk.hpp"
#include <deque>
#include <array>
#include "source.hpp"

enum miss_resolver_type : int
{
	R_USUAL = 0,
	R_SHOT,
	R_60_DELTA,
	R_40_DELTA,
	R_MAX
};

class c_next_resolving_method
{
public:
	c_next_resolving_method(int _index, int _type)
	{
		index = _index;
		type = _type;
	}
	int index = 0;
	int type = 0;
};

class resolver_records
{
public:
	resolver_records() { reset(); };
	~resolver_records() { reset(); };

	void reset()
	{
		//if (!prev_server_anim_layers_saved)
		//	return;

		breaking_lc = false;
		has_been_resolved = false;
		//did_fill_velocity = false;
		//did_store_abs_yaw = 0;
		is_filled = false;
		brute_side = 0;
		did_anims_update = false;
		force_velocity = false;
		did_backtrack_onshot = false;
		missed_in_anim_resolver = false;
		new_velocity.clear();
		old_velocity.clear();
		last_shot_angle.clear();
		//last_simtime = 0.0f;
		original_abs_yaw = 0.0f;
		last_shot_time = 0.0f;
		old_anim_upd_time = 0.0f;
		negative_abs_yaw = 0.0f;
		//last_low_delta = 60.f;
		prev_feet_cycle = FLT_MAX;
		prev_feet_rate = FLT_MAX;
		//prev_resolving_method = resolving_method;
		prev_eflags = 0;
		freestanding_updates = 0;
		brute_banned_sides.clear();
		did_force_velocity = false;
		tick_stopped = -1;
		tick_jumped = -1;
		velocity_stopped.clear();
		//last_hurt_resolved = 0;
		resolver_type = 0;
		duck_ticks = 0;
		right_side = 0.0f;
		last_time_changed_direction = 0.0f;
		left_side = 0.0f;
		last_tick_damageable = 0;
		shots_missed = 0;
		anim_time = 0;
		missed_in_hitpos_resolver = false;
		desync_swap = false;
		//last_hitpos_side = 0;
	}

	/*struct freestanding_record
	{
		int right_damage = 0, left_damage = 0;

		void reset()
		{
			right_damage = 0;
			left_damage = 0;
		}
	};*/

	int missed_shots[R_MAX] = {};
	int missed_side1st[R_MAX] = {};
	int history_shot[R_MAX] = {};
	bool force_velocity		= false;
	bool did_force_velocity = false;
	//bool did_fill_velocity  = false;
	bool is_filled = false;
	bool breaking_lc = false;
	bool has_been_resolved = false;
	bool did_anims_update = false;
	int brute_side = 0;
	float anim_time = 0;
	int did_store_abs_yaw = 0;
	bool is_moving = false;
	bool is_standing = false;
	bool change = false;
	bool never_saw_movement = false;
	bool move_lby_fine = false;
	bool did_backtrack_onshot = false;
	bool last_shot_missed = false;
	int duck_ticks = 0;
	Vector new_velocity		= Vector::Zero;
	Vector old_velocity		= Vector::Zero;
	Vector last_valid_non_exp_origin = Vector::Zero;
	//QAngle last_non_shot_angle = QAngle::Zero;
	C_AnimationLayer resolver_layers[3][15];
	int tick_stopped = -1;
	int tick_jumped = -1;
	bool desync_swap = false;
	float desync_swap_angles = 0.f;
	Vector velocity_stopped = Vector::Zero;
	QAngle last_angle = QAngle::Zero;
	float use_fix_trigger = 0.f;
	float spawn_time = 0.0f;
	float last_time_hurt = 0.f;
	float last_simtime = 0.0f;
	float last_shot_time = 0.0f;
	float original_abs_yaw = 0.0f;
	float negative_abs_yaw = 0.0f;
	float old_anim_upd_time = 0.0f;
	float right_side = 0.0f;
	float left_side = 0.0f;
	float last_desync_delta = 0.0f;
	float prev_feet_cycle = FLT_MAX;
	float prev_feet_rate = FLT_MAX;
	float last_move_lby = FLT_MAX;
	float lby_update_timer = FLT_MAX;
	float old_lby = 0.f;
	float last_time_changed_direction = 0.f;
	int	last_tick_damageable = 0;
	int baim_tick;
	float move_lby_delta = 0.f;
	float last_time_shot = 0.f;
	float acceleration = 0.f;
	float last_low_delta = 60.f;
	float last_low_delta_diff = 0.f;

	float last_low_delta_time = 0.f;
	float stop_to_full_run_frac = 0.f;
	int prev_eflags = 0;
	//int resolving_method = 0;
	int last_resolving_method = 0;
	int last_resolved_side = 0;
	int last_hitpos_side = 0;
	float last_hitpos_side_correction = 0.f;
	bool last_hitbox_correction_was_onshot = false;
	int last_abs_yaw_side = 0;
	int anims_pre_resolving = -1;
	bool missed_in_anim_resolver = false;
	bool missed_in_hitpos_resolver = false;
	float freestanding_update_time = FLT_MAX;
	int freestanding_side = -1;
	int freestanding_updates = 0;
	int damage_ticks = 0;
	int shots_missed = 0;
	int last_hurt_resolved = -1;
	int resolver_type = 0;
	int resolver_index = 0;
	QAngle last_shot_angle;
	std::vector<int> brute_banned_sides;
	std::vector<c_next_resolving_method> next_resolving_index;

	int freestand_left_tick;
	int freestand_right_tick;

	//freestanding_record freestand_info = { 0,0 };
};

struct shot_t
{
	shot_t(const Vector& shotpos, const Vector& shotpoint, const int tick, const float realtime, const int hitgroup, const int damage, const int& enemy_index, const C_Tickrecord* record)
	{
		this->shotpos = shotpos;
		this->shotpoint = shotpoint;
		this->enemy_index = enemy_index;
		this->tick = tick;
		this->realtime = realtime;
		if (record)
			this->record = *record;
		else
			this->record.reset();

		this->hit = false;
		this->hurt = false;
		this->hitmarker_alpha = 0.0f;
		this->shotinfo.safe_point = false;
		this->shotinfo.hitgroup = hitgroup;
		this->shotinfo.damage = damage;

		//if (ctx.m_local() && m_weapon())
		this->shotinfo.spread = ctx.current_spread;
	}
	Vector shotpos;
	Vector shotpoint;
	Vector hitpos;
	bool hit;
	bool hurt;
	struct
	{
		int victim = -1;
		int damage = -1;
		int hitgroup = -1;
	}hitinfo;
	struct
	{
		bool safe_point = false;
		int hitgroup = -1;
		int damage = 0;
		float spread = 0.0f;
	}shotinfo;
	int tick;
	float realtime;
	float hitmarker_alpha;
	int enemy_index;
	C_Tickrecord record;
	C_Tickrecord backup;
};

class c_resolver
{
public:
	//void store_data(C_BasePlayer* m_player, C_Tickrecord* record, bool can_store);

	virtual bool resolve_using_animations(C_BasePlayer* m_player, C_Tickrecord* record, int& result);

	virtual bool add_shot(const Vector& shotpos, const Vector& shotpoint, C_Tickrecord* record, const int damage, const int hitgroup, const int& enemy_index);

	virtual void update_missed_shots(const ClientFrameStage_t& stage);

	virtual std::deque<shot_t>& get_shots();

	virtual void hurt_listener(IGameEvent* game_event);

	virtual shot_t* closest_shot(int tickcount);

	virtual bool is_record_equal(C_Tickrecord* a1, C_Tickrecord* a2);

	virtual C_Tickrecord* find_shot_record(C_BasePlayer* player, c_player_records* data);

	virtual C_Tickrecord* find_first_available(C_BasePlayer* player, c_player_records* data, bool oldest = false);

	virtual void record_shot(IGameEvent* game_event);

	virtual void listener(IGameEvent* game_event);

	virtual bool select_next_side(C_BasePlayer* m_player, C_Tickrecord* record);

	virtual void approve_shots(const ClientFrameStage_t& stage);

	virtual bool hurt_resolver(shot_t* record);

	virtual void collect_and_correct_info(shot_t* shot);

	virtual void reset()
	{
		shots.clear();
		current_shots.clear();
		unapproved_shots.clear();
	};

	//bool hurt_resolver(C_BasePlayer* m_player, _shotinfo* record);

	//void add_shot(const _shotinfo& data);
	//bool find_side(C_BasePlayer* m_player, int& side);

	resolver_records player_records[128];

	std::deque<shot_t> shots;
	std::deque<shot_t> current_shots;
	std::deque<shot_t> unapproved_shots;
};