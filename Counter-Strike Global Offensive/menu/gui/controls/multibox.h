#pragma once
#include "../../menu/menu_v2.h"

class multibox : public element {
public:
	multibox( std::string name, bool* enabled, std::vector< std::string > elements, bool* shown );

	int focused( );
	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	std::vector< std::string > elements;
	std::vector< std::string > elements_to_add;
	bool* enabled;
	float multibox_size = 0;
	float multi_size_factor = 0;
	bool opened = false;
	bool hovered = false;
	bool* shown;
};