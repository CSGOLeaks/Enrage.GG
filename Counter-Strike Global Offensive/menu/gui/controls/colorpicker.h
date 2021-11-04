#pragma once
#include "../../menu/menu_v2.h"

class colorpicker : public element {
public:
	colorpicker( std::string name, Color* colour, bool show_alpha, bool* shown );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	Vector2D picker_start_pos;
	Vector2D last_color_pos = Vector2D::Zero;
	Color* colour;
	float col_hsl[3] = {0.f,0.f,0.f};
	bool* shown;
	bool hovered = false;
	bool hovered_alpha = false;
	bool dragging_alpha = false;
	bool allow_alpha = false;
	bool opened = false;
};