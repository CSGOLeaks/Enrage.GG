#pragma once

#include "sdk.hpp"

enum class BreakableType
{
	func_breakable,
	func_breakable_surf,
	prop_physics_multiplayer,
	prop_dynamic,

	Child
};

struct Breakable
{
	BreakableType Type;
	std::string Model; // Maybe Hash?
	int Health;

	std::vector<Breakable*> Breakables;
};

class c_grenade_tracer {
private:
	enum GrenadeFlags : size_t {
		NONE = 0,
		DETONATE,
		BOUNCE,
	};

	struct bounce_t {
		bounce_t(Vector _point, Color _color)
		{
			point = _point;
			color = _color;
		}

		Vector point;
		Color  color;
	};

	using path_t    = std::vector< Vector >;
	using bounces_t = std::vector< bounce_t >;

private:
	int       m_id;
	int		  m_collision_group;
	path_t    m_path;
	bounces_t m_bounces;
	float     m_vel, m_power;
	Vector    m_start, m_velocity, m_move;

public:
	virtual void clear_broken();
	virtual void add_broken(const Breakable& Breakable, IClientUnknown* Entity);
	virtual void add_broken(IClientUnknown* Entity);
	virtual bool is_broken(IClientUnknown* Entity);
	virtual void   reset();
	virtual void   paint();
	virtual void think(CUserCmd* cmd);
	virtual void   simulate();
	virtual void   setup();
	virtual size_t advance(size_t tick);
	virtual bool   detonate(size_t tick, CGameTrace& trace);
	//void OnNewMap(const char* Map);
	virtual void   ResolveFlyCollisionBounce(CGameTrace& trace, size_t* i = nullptr, size_t* o = nullptr);
	virtual void   PhysicsPushEntity(Vector& start, const Vector& move, CGameTrace& trace, C_BasePlayer* ent, size_t* i = nullptr, size_t* o = nullptr);
	virtual void   TraceHull(const Vector& start, const Vector& end, CGameTrace& trace, C_BasePlayer* ent);
	virtual void   PhysicsAddGravityMove(Vector& move);
	virtual void   PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);

	Vector last_smoke_endpos;
	bool smoke_molly;

	void* m_collision_ent;
};

//extern Grenades g_grenades;