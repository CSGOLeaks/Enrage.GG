#pragma once
#include "../../menu/menu_v2.h"

class slider : public element {
public:
	slider( std::string name, int min, int max, int* value, std::wstring addon, bool* shown, int increase_by );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	int* value;
	int min;
	int max;
	int increase_by;
	std::wstring addon;
	int slider_pos;

	bool dragging = false;
	bool hovered = false;
	bool* shown;
};