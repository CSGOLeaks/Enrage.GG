#pragma once
#include "../../menu/menu_v2.h"

class window : public element {
public:
	window( Vector2D pos, Vector2D size );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
	
private:
	bool dragging = false;
};