#pragma once

class c_legitaimbot
{
public:
	virtual void run(CUserCmd*);
	virtual void kill_delay(IGameEvent* event);
};