#pragma once
class item_t;

#include <vector>
#include <functional>
#include "../gui/element.h"
#include "../setup/settings.h"
#include "../input/input.h"
#include "../../sdk.hpp"
#include "../../hooked.hpp"

class mmenu {
	std::vector< element* > children{ };
	bool was_focused = false;
public:
	void setup( );

	void draw( );
	void think( );
	void on_unload();

	void opening( );

	void on_cfg_load();

	element* focused{ };

	void focus( element* focus ) {
		focused = focus;

		if ( focused == nullptr ) {
			was_focused = true;
		}
	}

	bool do_focus( element* focus ) {
		if ( !focused )
			return false;

		return ( focused != focus || was_focused );
	}
private:
	bool initialized = false;
};

// yes there are repeat colors, but its just so u can change the colors later on
class menu_c {
public:
	// window
	Color window_outline_black		= { 5, 5, 5 };
	Color window_outline_light_gray = { 60, 60, 60 };
	Color window_outline_dark_gray	= { 40, 40, 40 };
	Color window_background			= { 12, 12, 12 };

	// fields
	Color field_outer_outline		= { 0, 0, 0 };
	Color field_outline				= { 48, 48, 48 };
	Color field_background			= { 17, 17, 17 };

	// text colors
	Color text_color				= { 205, 205, 205 };
	Color tab_unselected			= { 152, 152, 152 };
	Color box_text_color			= { 152, 152, 152 };
	Color slider_addon_color		= { 255, 255, 255 };

	// controls
	Color control_outline			= { 0, 0, 0 };
	Color control_background		= { 41, 41 , 41 };

	// checkbox
	Color checkbox_gradient_top		= { 75, 75, 75 };
	Color checkbox_gradient_bottom	= { 48, 48, 48 };

	// slider
	Color slider_gradient_top		= { 75, 75, 75 };
	Color slider_gradient_bottom	= { 50, 50, 50 };

	// extras
	int slider_checkbox_gradient = 180;
	int color_picker_gradient = 180;
	int menu_alpha;
};

extern menu_c menu_colors;

extern mmenu g_menu;