#include "keybind.h"
#include "..//..//..//menu.hpp"

keybind::keybind(std::string name, c_keybind* key, bool can_be_changed, bool* shown) {
	this->name = name;
	this->key = key;
	this->shown = shown;
	this->can_be_changed = can_be_changed;

	key_name = "";
}

void keybind::draw() {
	if (!*shown)
		return;

	// background
	Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + size.y - 19, pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline);
	Drawing::DrawRectA(pos.x + 21, pos.y + size.y - 18, pos.x + size.x, pos.y + size.y, menu_colors.control_background);

	if (opened_settings && can_be_changed)
	{
		// background
		Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + size.y - 19, pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline);
		Drawing::DrawRectA(pos.x + 21, pos.y + size.y - 18, pos.x + size.x, pos.y + size.y, menu_colors.control_background);

		// selected name
		Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 15, menu_colors.box_text_color, 0, elements.at(key->mode).c_str());

		// if theres a name, draw the name
		if (!name.empty())
			Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 2, menu_colors.text_color, 0, name.c_str());

		combo_size_factor = csgo.m_globals()->frametime * float(elements.size() * elements.size() + 70);

		// triangle (POG)
		Drawing::DrawOutlinedRectA(pos.x + size.x - 10, pos.y + size.y - 8, pos.x + size.x - 5, pos.y + size.y - 7, menu_colors.box_text_color);
		Drawing::DrawOutlinedRectA(pos.x + size.x - 9, pos.y + size.y - 9, pos.x + size.x - 6, pos.y + size.y - 8, menu_colors.box_text_color);
		Drawing::DrawOutlinedRectA(pos.x + size.x - 8, pos.y + size.y - 10, pos.x + size.x - 7, pos.y + size.y - 9, menu_colors.box_text_color);

		combobox_size += combo_size_factor;
		combobox_size = Math::clamp(combobox_size, 0.f, 3.f + 16.f * 4);

		// background
		Drawing::DrawRectA(pos.x + 21, pos.y + size.y + 3, pos.x + size.x, pos.y + size.y + int(combobox_size), menu_colors.control_background);

		// outline
		Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + size.y + 3, pos.x + size.x + 1, pos.y + size.y + combobox_size, menu_colors.control_outline);

		// text
		size_t n{ };
		for (; n < elements.size(); ++n) {
			const auto selected = n == key->mode;
			const auto seen = 16 * (n + 1) - 8 < combobox_size;

			if (seen)
				Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 11 + 16 * (n + 1),
					selected ? ctx.m_settings.menu_color : menu_colors.box_text_color, 0, elements.at(n).c_str());
		}
	}
	else {
		// selected name
		Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 15, pressed ? ctx.m_settings.menu_color : menu_colors.box_text_color, 0, pressed ? "press key" : key_name.c_str());

		// if theres a name, draw the name
		if (!name.empty())
			Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 2, menu_colors.text_color, 0, name.c_str());

		combobox_size = 0;
	}
}

int keybind::focused() {
	static const auto original_pos = pos;

	for (auto n = 0; n < elements.size(); ++n) {
		pos.y += 16;

		// catch the bag (index)
		if (g_menuinput.inbounds(Vector2D(pos.x + 21, pos.y + size.y - 16), pos + Vector2D(size.x, size.y + 3))) {
			pos = original_pos;
			return n;
		}
	}

	pos = original_pos;

	return -1;
}

void keybind::think() {
	if ( !*shown )
		return;

	// hovered
	hovered = g_menuinput.inbounds( pos + Vector2D( 21, size.y - 18 ), pos + size );

	static bool just_pressed; // bandaid fix monkas
	
	// pressed
	if ( hovered && !pressed && g_menuinput.is_key_pressed( VK_LBUTTON ) ) { pressed = true; just_pressed = true;  }
	if ( hovered && !pressed && g_menuinput.is_key_pressed(2) && can_be_changed) {
		//g_menu.focus(this); // focus this!
		//opened_settings = !opened_settings; 
		if (hovered && !opened_settings && size.y > 3) { // hovered and not opened
			g_menu.focus(this); // focus this!
			opened_settings = true;
		}
		else if (hovered && opened_settings) { // hovered and opened
			g_menu.focus(nullptr); // unfocus!
			opened_settings = false;
		}
	}

	if (ctx.init_finished)
		feature::menu->_keybind_toggled = false;

	if (opened_settings)
	{
		if (ctx.init_finished)
			feature::menu->_keybind_toggled = true;
		if (can_be_changed && g_menuinput.is_key_pressed(VK_LBUTTON))
		{
			const auto part = focused();
			if (part != -1) {
				key->mode = part;
				opened_settings = false;
				g_menu.focus(nullptr);
			}
			else {
				g_menu.focus(nullptr);
				opened_settings = false;
			}
		}
	}
	else if ( pressed ) {
		if (ctx.init_finished)
			feature::menu->_keybind_toggled = true;
		for ( auto i = 0; i < 256; i++ ) {
			if ( g_menuinput.is_key_pressed( i ) && !just_pressed ) {
				if ( i == VK_ESCAPE ) {
					key->key = -1;
					key_name = "";
					pressed = false;
					return;
				}

				if ( this->get_name( i ).length( ) > 0 ) {
					key->key = i;
					key_name = this->get_name( i );
					pressed = false;
					return;
				}
				
				return;
			}
		}

		just_pressed = false;
	}
}

void keybind::extra() {
	if ( *shown )
		size = { 174, static_cast<float>( !name.empty( ) ? 34 : 16 ) };
	else
		size = { 174, -8 };
}

void keybind::on_cfg_update()
{
	if (key->key != 0 && this->get_name(key->key).length() > 0)
		key_name = this->get_name(key->key);
	else
		key_name = "";

	key->mode = Math::clamp(key->mode, 0, 3);
	key->key = Math::clamp(key->key, 0, 256);
	key->time = 0;
	key->toggled = false;
}

void keybind::on_unload()
{
	//delete this;
}
