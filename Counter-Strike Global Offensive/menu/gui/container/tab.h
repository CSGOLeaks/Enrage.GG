#pragma once
#include "../../menu/menu_v2.h"

class tab : public element {
public:
	tab( std::string name );
	
	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
	void setup_positions( );
private:
	int tabs;
	bool hovered = false;
};