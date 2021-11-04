#pragma once
#include "sdk.hpp"
#include <deque>
#include "source.hpp" 

class c_misc
{
public:
	virtual void unlock_cvars();
	virtual void unlock_cl_cvars();
	//void begin(CUserCmd* cmd);
	virtual void pre_prediction(CUserCmd* cmd);
	virtual int hitbox_to_hitgroup(int Hitbox);
	virtual int hitgroup_to_hitbox(int hitgroup);
	virtual void end(CUserCmd* cmd);
	virtual bool save_cfg();
	virtual bool load_cfg();
	virtual int get_client_num();
	virtual std::string get_steam_name();
	virtual __int64 get_friend_id(const char* pszAuthID);
	virtual std::string get_steam_id();
	virtual std::string get_user_data();
};