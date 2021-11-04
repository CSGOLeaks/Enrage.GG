#pragma once
#include "sdk.hpp"

class c_cmd
{
public:	
	c_cmd(const int& _command_number = 0,
		const QAngle &_original_angles = QAngle::Zero,
		const Vector& _move_data = Vector::Zero,
		const QAngle& _view_angles = QAngle::Zero,
		const float& _simulation_time = 0.f,
		const int& _flags = 0,
		const float& _duck_amount = 0.f,
		const float& _lby = 0.f,
		const int& _tickbase = 0,
		const Vector& _velocity = Vector::Zero)
	{
		 command_number = _command_number;
		 original_angles = _original_angles;
		 move_data = _move_data;
		 view_angles = _view_angles;
		 simulation_time = _simulation_time;
		 flags = _flags;
		 duck_amount = _duck_amount;
		 lby = _lby;
		 tickbase = _tickbase;
		 velocity = _velocity;
	}

	void store(const int& _command_number = 0,
		const QAngle& _original_angles = QAngle::Zero,
		const Vector& _move_data = Vector::Zero,
		const QAngle& _view_angles = QAngle::Zero,
		const float& _simulation_time = 0.f,
		const int& _flags = 0,
		const float& _duck_amount = 0.f,
		const float& _lby = 0.f,
		const int& _tickbase = 0,
		const Vector& _velocity = Vector::Zero)
	{
		command_number = _command_number;
		original_angles = _original_angles;
		move_data = _move_data;
		view_angles = _view_angles;
		simulation_time = _simulation_time;	
		flags = _flags;
		duck_amount = _duck_amount;
		lby = _lby;
		tickbase = _tickbase;
		velocity = _velocity;
	}

	int command_number = 0;
	QAngle original_angles = QAngle::Zero;
	Vector move_data = Vector::Zero;
	QAngle view_angles = QAngle::Zero;
	float simulation_time = 0.f;
	int flags = 0;
	float duck_amount = 0.f;
	float lby = 0.f;
	int tickbase = 0;
	Vector velocity = Vector::Zero;
};

class c_usercmd
{
public:
	//virtual CUserCmd* find_latest_non_sent_command();
	virtual  void run_fixes(CUserCmd* cmd);

	//std::deque<c_cmd> command_numbers;
	c_cmd cmd_info[150];
	//std::array<c_cmd, 64u> command_numbers;
	//int records_count;
};