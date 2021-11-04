#include "menu_v2.h"
#include "..//..//source.hpp"
#include <algorithm>
#include "../../menu.hpp"

mmenu g_menu;
settings g_settings;
menu_c menu_colors;

void mmenu::opening() {
	static float current_alpha = 0;
	
	const int prev_alpha = static_cast<int>(current_alpha);

	if ( feature::menu->_menu_opened )
		current_alpha = std::clamp<float>( current_alpha += csgo.m_globals()->frametime * 700.f, 0.0f, 255.0f);
	else
		current_alpha = std::clamp<float>( current_alpha -= csgo.m_globals()->frametime * 700.f, 0.0f, 255.0f );

	const int alpha = static_cast<int>( current_alpha );

	if (prev_alpha != alpha && alpha > 0)
	{
		// window
		menu_colors.window_outline_black = menu_colors.window_outline_black.alpha(alpha);
		menu_colors.window_outline_light_gray = menu_colors.window_outline_light_gray.alpha(alpha);
		menu_colors.window_outline_dark_gray = menu_colors.window_outline_dark_gray.alpha(alpha);
		menu_colors.window_background = menu_colors.window_background.alpha(alpha);

		// fields
		menu_colors.field_outer_outline = menu_colors.field_outer_outline.alpha(alpha);
		menu_colors.field_outline = menu_colors.field_outline.alpha(alpha);
		menu_colors.field_background = menu_colors.field_background.alpha(alpha);

		// text colors
		menu_colors.text_color = menu_colors.text_color.alpha(alpha);
		menu_colors.tab_unselected = menu_colors.tab_unselected.alpha(alpha);
		menu_colors.box_text_color = menu_colors.box_text_color.alpha(alpha);
		menu_colors.slider_addon_color = menu_colors.slider_addon_color.alpha(alpha);

		// controls
		menu_colors.control_outline = menu_colors.control_outline.alpha(alpha);
		menu_colors.control_background = menu_colors.control_background.alpha(alpha);

		// checkbox
		menu_colors.checkbox_gradient_top = menu_colors.checkbox_gradient_top.alpha(alpha);
		menu_colors.checkbox_gradient_bottom = menu_colors.checkbox_gradient_bottom.alpha(alpha);

		// slider
		menu_colors.slider_gradient_top = menu_colors.slider_gradient_top.alpha(alpha);
		menu_colors.slider_gradient_bottom = menu_colors.slider_gradient_bottom.alpha(alpha);

		// menu color
		ctx.m_settings.menu_color = ctx.m_settings.menu_color.alpha(alpha);

		// slider & checkbox gradient
		menu_colors.slider_checkbox_gradient = static_cast<int>(float(alpha) * 0.70588235294f); // AHAHAHA

		// color picker gradient
		menu_colors.color_picker_gradient = static_cast<int>(float(alpha) * 0.76470588235f); // AHAHAHA

		// just a extra alpha
		menu_colors.menu_alpha = alpha;
	}
	
	if ( alpha <= 0 )
		g_settings.menu_fullfade = true;
	else
		g_settings.menu_fullfade = false;
}

void mmenu::on_cfg_load()
{
	// extras
	for (auto i : children)
		i->on_cfg_update();
}

void mmenu::draw( )
{
	this->opening( );

	if ( g_settings.menu_fullfade )
		return;
	
	// think
	this->think( );

	// extras
	for ( auto i : children )
		i->extra( );

	// drawing
	for ( auto i : children )
		i->draw( );

	// if focused, draw last
	if ( focused )
		focused->draw( );

	// dont keep stuff focused forever
	if ( was_focused )
		was_focused = false;
}

void mmenu::think( )
{
	// code stolen from nasa headquarters ~ensidiya
	if ( !focused )
		for ( auto i : children )
			i->think( );
	else
		focused->think( );
}

void mmenu::on_unload()
{
	// extras
	for (auto i : children)
		i->on_unload();
}
