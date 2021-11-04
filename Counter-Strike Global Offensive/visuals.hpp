#pragma once
#include "sdk.hpp"
#include <deque>
#include "source.hpp"

struct _event
{
	_event(const std::string& msg, const std::string& secretmsg = "")
	{
		_time = csgo.m_globals()->realtime + 4.f;
		_displayticks = 4.f;
		_msg = msg;
		_displayed = false;
		_secretmsg = secretmsg;
	}

	float _time = 0;
	float _displayticks = 0;
	bool _displayed = false;
	std::string _msg = "";
	std::string _secretmsg = "";
};

extern std::vector<_event> _events;

class c_bullet_tracer
{
public:
	c_bullet_tracer(const Vector& _src, const Vector& _dst, const float& _time, const Color& _colorLine, const bool& _islocal)
	{
		src = _src;
		dst = _dst;
		time = _time;
		color1 = _colorLine;
		is_local = _islocal;
	}

	Vector src = Vector::Zero, dst = Vector::Zero;
	float time = 0.f;
	Color color1 = Color::White();
	bool is_local = false;
};

extern std::vector<c_bullet_tracer> bullet_tracers;

class c_damage_indicator
{
public:
	c_damage_indicator(const Vector spot, const int tick, const int damage, const bool is_headshot)
	{
		_tick = tick;
		_spot = spot;
		_damage = damage;
		_headshot = is_headshot;
		_time = csgo.m_globals()->realtime + 4.f;
		w2s = Vector::Zero;
	}

	Vector _spot;
	Vector w2s;
	int _damage;
	int _tick;
	float _time;
	bool _headshot;
};

extern std::vector<c_damage_indicator> damage_indicators;

class c_visuals
{
public:
	virtual bool get_espbox(C_BasePlayer* entity, int& x, int& y, int& w, int& h);
	virtual void damage_esp();
	virtual void draw_beam(Vector Start, Vector End, Color color, float Width);
	virtual void render_tracers();
	virtual void logs();
	virtual void skeleton(C_BasePlayer* Entity, Color color, matrix3x4_t* pBoneToWorldOut);
	virtual void offscreen_esp(C_BasePlayer* player, float alpha);
	//void draw_pov_arrows(C_BasePlayer* entity, float alpha);
	virtual void render(bool reset);

	virtual void draw_items(C_BasePlayer* m_entity);

	//float dormant_alpha[129];
	std::array<float, 128u> dormant_alpha = {};
	bool save_pos = false;
	int saved_x = 0;
	int saved_y = 0;
	bool was_moved = false;

	bool save_pos_hotkeys = false;
	int saved_x_hotkeys = 0;
	int saved_y_hotkeys = 0;
	bool was_moved_hotkeys = false;
	//const wchar_t* weapon_names[900] = {};
	std::array<const wchar_t*, 900u> weapon_names = {};
};