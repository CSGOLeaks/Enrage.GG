#pragma once

#include "sdk.hpp"

namespace Engine
{
	class Movement : public Core::Singleton<Movement>
	{
	public:
		void Quick_stop(CUserCmd* cmd, Vector add = Vector(0,0,0));
		void limit_speed(CUserCmd* cmd, float max_speed);
		//void LegMovement(CUserCmd* a1);
		void Begin(CUserCmd* cmd, bool& send_packet);
		void PreMovement(CUserCmd* cmd);
		void FixMove(CUserCmd* cmd, const QAngle& wish_angles);
		//void PostMovement(CUserCmd* cmd);
		void Fix_Movement(CUserCmd* cmd, QAngle original_angles);
		void End(CUserCmd* cmd);

		QAngle m_qRealAngles = {};
		Vector forcemovement = {};
		bool did_force = false;
		QAngle m_qAngles = {};
		QAngle m_qAnglesView = {};
		Vector old_movement = {};
		QAngle m_oldangle = {};
		float m_oldforward = 0.f;
		float m_oldsidemove = 0.f;
	};
}