#pragma once
#include "../../menu/menu_v2.h"

class button : public element {
public:
	button( std::string name, std::function< void() >, bool* shown, bool ask_for_confirmation = false );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	bool* shown;
	bool should_confirm = false;
	bool hovered = false;
	float last_time_pressed = 0.f;
	bool show_confirm_msg = false;

	std::function< void() > callback;
};