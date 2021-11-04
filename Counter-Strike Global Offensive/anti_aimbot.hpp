#pragma once
#include "sdk.hpp"

struct last_sent_data
{
	void store(float _simtime,
		Vector _m_vecOrigin,
		Vector _m_vecVelocity,
		Vector _eye_position,
		QAngle _m_angEyeAngles,
		int _tickbase,
		int _command_number,
		int _m_fFlags)
	{
		 simtime = _simtime;
		 m_vecOrigin = _m_vecOrigin;
		 m_vecVelocity = _m_vecVelocity;
		 eye_position = _eye_position;
		 m_angEyeAngles = _m_angEyeAngles;
		 tickbase = _tickbase;
		 command_number = _command_number;
		 m_fFlags = _m_fFlags;
	}

	float simtime;
	Vector m_vecOrigin;
	Vector m_vecVelocity;
	Vector eye_position;
	QAngle m_angEyeAngles;
	int tickbase;
	int command_number;
	int m_fFlags;
};

class AdaptiveAngle {
public:
	float m_yaw;
	float m_dist;

public:
	// ctor.
	AdaptiveAngle(const float& yaw, const float& penalty = 0.f) {
		// set yaw.
		m_yaw = Math::normalize_angle(yaw);

		// init distance.
		m_dist = 0.f;

		// remove penalty.
		m_dist -= penalty;
	}
};

// best target.
struct AutoTarget_t { float fov; C_BasePlayer* player; };

class c_antiaimbot
{
public:
	virtual float get_max_desync_delta(C_BasePlayer* ent);
	virtual void get_targets();
	virtual void run_at_target(float& yaw);
	virtual void auto_direction();
	//bool run_freestand(float& yaw);
	/*virtual*/ void change_angles(CUserCmd* cmd, bool* send_packet);
	virtual bool peek_fake_lag(CUserCmd* cmd, bool* send_packet);
	virtual void fake_lag(CUserCmd* cmd, bool* send_packet);
	virtual void fake_lagv2(CUserCmd* cmd, bool* send_packet);
	virtual void work(CUserCmd* cmd, bool* send_packet);

	float enable_delay = 0.f;
	bool flip_side = false;
	int previous_side = 0;
	bool extend = false;
	QAngle last_real_angle = QAngle::Zero;
	float fake_yaw_diff_with_backwards = 0.f;
	float real_yaw_diff_with_backwards = 0.f;
	QAngle visual_real_angle = QAngle::Zero;

	float m_next_lby_update_time = 0.f, m_last_lby_update = 0.f, m_last_attempted_lby = 0.f;
	bool m_will_lby_update = false;

	float min_delta = 0.f, max_delta = 0.f,
		feet_speed_stand = 0.f, feet_speed_ducked = 0.f;

	float animation_speed = 0.f;
	Vector previous_velocity = Vector::Zero;
	Vector animation_velocity = Vector::Zero;
	float  m_auto_dist;
	float  m_auto;
	float  m_auto_time;
	float  m_auto_last;

	last_sent_data sent_data;
	float last_unchoke_time = 0.f;
	bool  unchocking = false;
	bool skip_fakelag_this_tick = false;
	int last_chocked_amount = 0;
	float lby_timer = 0.f;
	float lby_expected = 0.f;
	bool throw_nade = false;

	bool did_shot_in_chocked_cycle = false;
	std::vector<C_BasePlayer*> players;
	Vector origins[64];
};