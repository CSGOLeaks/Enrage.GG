#pragma once
#include "../../menu/menu_v2.h"

class combobox : public element {
public:
	combobox( std::string name, int* value, std::vector< std::string > elements, bool* shown );
	combobox(std::string name, int* value, std::vector< std::string >* elements, bool* shown);

	int focused( );
	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	std::vector< std::string > elements;
	std::vector< std::string > *t_elements;
	int* value;
	float combobox_size = 0;
	float combo_size_factor = 0;
	bool opened = false;
	bool hovered = false;
	bool* shown;
};