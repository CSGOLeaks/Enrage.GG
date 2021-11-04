#pragma once
#include "sdk.hpp"
#include <deque>
#include <array>
#include <unordered_map>
#include "lag_comp.hpp"
#include "source.hpp"

struct c_chams_settings
{
	Color color;
	Color hidden_color;
	Color white_color = Color::White();
	Color reflectivity_clr;
	float reflectivity;
	float shine;
	float pearlescent;
	float rim;
	int	  type;
	bool  wireframe;
	int   setting_type;
	IMaterial* setuped_mat;
	bool  redraw;
	bool discoball;
	float scope_blend;
};

class c_chams
{
public:
	virtual void set_ignorez(const bool enabled, IMaterial* mat);
	virtual void set_wireframe(const bool enabled, IMaterial* mat);
	virtual IMaterial* get_material(int material);
	virtual void modifications(IMaterial* mat, c_chams_settings settings);
	virtual void set_modulation(c_chams_settings settings, IMaterial* material);
	virtual void modulate_exp(IMaterial* material, const float alpha = 1., const float width = 6.f);
	virtual void player_chams(const std::function<void()> original, c_chams_settings settings, bool scope_blend, bool* vis_type = nullptr);
	//virtual bool get_backtrack_matrix(C_BasePlayer* player, matrix3x4_t* out);
	virtual int work(void* context, void* state, ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld);
	virtual void night_mode();
};

class c_hit_chams
{
public:
	c_hit_chams(C_Tickrecord src, int entindex)
	{
		this->_src = src;
		this->_entindex = entindex;
		this->_time = csgo.m_globals()->realtime + 4.f;
	}

	C_Tickrecord _src;
	int _entindex;
	float _time;
};

extern std::vector<c_hit_chams> hit_chams;