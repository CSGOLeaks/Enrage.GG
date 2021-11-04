#include "hooked.hpp"
#include "menu.hpp"
#include <algorithm>
#include <unordered_map>
#include <sstream>
#include "misc.hpp"

#define width_menu 600
#define height_menu 416

// optimization.
struct subtab_icon_saved
{
	float alpha;
	std::string icon;
	std::string name;
};

// optimization.
struct keybinds_saved
{
	float alpha = 0.f;
	float start_listen_time = 0.f;
	bool prev_state = false;
	bool toggled = false;
	bool toggled_settings = false;
	Vector2D lock_settings_pos = {};
};

std::unordered_map<int, float> hover_alpha = {};
std::unordered_map<int, float> toggle_alpha = {};
std::unordered_map<int, float> tab_hover_shit_alpha = {};
std::unordered_map<int, keybinds_saved> was_pressed_keybind = {};
std::unordered_map<int, subtab_icon_saved> subtab_icons_alpha = {};
std::unordered_map<int, bool> was_holding = {};

auto main_color = Color(57, 121, 217);//Color(175, 255, 0);
auto main_color_fade = Color(57, 121, 217);//Color(120, 175, 0);

//std::unordered_map <int, Vector2D> combo_mouse_pos = {};
std::unordered_map <int, float> combo_buttons = {};
std::unordered_map <int, Vector2D> hovering = {};
std::unordered_map <int, bool> opened = {};

bool was_moved;

bool c_menu::mouse_in_pos(Vector start, Vector end)
{
	return ((_cursor_position.x >= start.x) && (_cursor_position.y >= start.y) && (_cursor_position.x <= end.x) && (_cursor_position.y <= end.y));
}

bool c_menu::mouse_in_pos(Vector2D start, Vector2D end)
{
	return ((_cursor_position.x >= start.x) && (_cursor_position.y >= start.y) && (_cursor_position.x <= end.x) && (_cursor_position.y <= end.y));
}

int ignore_items = -1;

bool c_menu::menuitem(int& n, std::string text, bool& var, bool parent)
{
	bool value_changed = false;
	Vector menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * n), 0);
	if (line != 0) menu_pos.x += 200 * line;
	Vector max_menu_pos = Vector(menu_pos.x + 160, menu_pos.y + 14, 0);

	//if (ignore_items < n * (line + 1) {

	const int combobox_idx = int(int(n + int((parent ? 19 : 0) + (17 * n) + line)));

	auto& t_alpha = toggle_alpha[combobox_idx];

	auto hovered = mouse_in_pos(menu_pos, max_menu_pos);

	Drawing::DrawString(F::MenuV2, menu_pos.x + 12, menu_pos.y + 1, Color::White(alpha * 0.9f), FONT_LEFT, text.c_str());

	Drawing::DrawRect(menu_pos.x - 1, menu_pos.y + 4, 8, 7, Color(37, 37, 37, alpha));

	if (var)
	{
		if (t_alpha < 1.f)
			t_alpha += min(1.f - t_alpha, csgo.m_globals()->frametime * 2.4f);
	}
	else
	{
		if (t_alpha > 0.f)
			t_alpha -= min(t_alpha, csgo.m_globals()->frametime * 3.4f);
	}

	t_alpha = Math::clamp(t_alpha, 0.f, 1.f);

	if (t_alpha < 1.f)
		Drawing::DrawRectGradientVertical(menu_pos.x - 1, menu_pos.y + 4, 8, 7, Color(37, 37, 37, alpha), Color(36, 36, 36, alpha));

	//if (!hovered)
	if (t_alpha > 0.f || var) {
		Drawing::DrawRectGradientVertical(menu_pos.x, menu_pos.y + 4, 7, 7, Color(ctx.m_settings.menu_color.r(), ctx.m_settings.menu_color.g(), ctx.m_settings.menu_color.b(), alpha * t_alpha), Color(ctx.m_settings.menu_color.r(), ctx.m_settings.menu_color.g(), ctx.m_settings.menu_color.b(), alpha * t_alpha));
	}

	Drawing::DrawOutlinedRect(menu_pos.x - 1, menu_pos.y + 3, 9, 9, /*hovered ? Color(120, 120, 120, alpha) : */Color(22, 22, 22, alpha));
	Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y + 4, 7, 7, /*hovered ? Color(120, 120, 120, alpha) : */Color(55, 55, 55, alpha));

	//if (var)
	//	Drawing::DrawString(F::ESPMisc, menu_pos.x, menu_pos.y + 1, Color::White(alpha), FONT_LEFT, "g");

	if (hovered && _mouse_pressed && ignore_items < n * max(line, 1) && !combo_was_opened) {
		var = !var;
		value_changed = true;
	}

	ignore_items = -1;
	//}

	n += 1;

	return value_changed;
}

std::unordered_map<int, bool> combo_opened = {};

bool c_menu::keybind(int& n, std::string text, c_keybind* var, bool parent, bool same_line)
{
	bool value_changed = false;
	Vector menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * max(0, (same_line ? n-1 : n))), 0);
	if (line != 0) menu_pos.x += 200 * line;
	Vector max_menu_pos = Vector(menu_pos.x + 200, menu_pos.y + 14, 0);

	//if (ignore_items < n * (line + 1) {

	const int combobox_idx = int(int(n + int((parent ? 19 : 0) + (16 * n) + line)));

	auto& keybind_prev_state = was_pressed_keybind[combobox_idx];

	auto hovered = mouse_in_pos(menu_pos - Vector(20,0,0), max_menu_pos);

	if (!same_line)
		Drawing::DrawString(F::MenuV2, menu_pos.x + 1, menu_pos.y + 1, Color::White(var->key > 0 ? (alpha * 0.8f) : (alpha * 0.4f)), FONT_LEFT, text.c_str());
	
	std::string draw = var->key > 0 ? get_key_name(var->key) : "	";

	if (keybind_prev_state.toggled)
		draw = std::string(sxor("?"));

	if (draw.size() > 4)
		draw.resize(4);

	auto combo_idx = (line != 0 ? ((n + text.c_str()[0] + 1) * max(line, 1)) : n);

	auto t_s = Drawing::GetTextSize(F::ESPInfo, draw.c_str());
	Drawing::DrawString(F::ESPInfo, menu_pos.x + 180, menu_pos.y + 2, Color::White(var->key > 0 ? (alpha * 0.65f) : (alpha * 0.4f)), FONT_RIGHT, "[%s]", draw.c_str());
	//}

	//Drawing::DrawRectGradientHorizontal(menu_pos.x + 180 + t_s.right + (t_s.right / 4), menu_pos.y + 2, t_s.right + (t_s.right / 4), 2, Color(30, 30, 30, alpha - 20), ctx.m_settings.menu_color.alpha(alpha));
	//Drawing::DrawRectGradientHorizontal(menu_pos.x + 180 + t_s.right - (t_s.right / 4), menu_pos.y - 1, t_s.right + (t_s.right / 4) + 1, 2, ctx.m_settings.menu_color.alpha(alpha), Color(30, 30, 30, alpha - 20));

	//Drawing::DrawRectGradientVertical(menu_pos.x + 180 + t_s.right - (t_s.right / 4) - 1, menu_pos.y, /*menu_pos.x*/2, t_s.bottom + 1, ctx.m_settings.menu_color.alpha(alpha), Color(30, 30, 30, alpha - 20)); // left
	//Drawing::DrawRectGradientVertical(+180 + t_s.right + (t_s.right / 4) - 1, menu_pos.y + t_s.right + (t_s.right / 4), 2, t_s.bottom, Color(30, 30, 30, alpha - 20), ctx.m_settings.menu_color.alpha(alpha)); // right


	//Drawing::DrawRect(menu_pos.x - 1, menu_pos.y + 4, 8, 7, Color(60, 60, 60, 0.7 * alpha));

	//Drawing::DrawOutlinedRect(menu_pos.x - 1, menu_pos.y + 3, 9, 9, /*hovered ? Color(120, 120, 120, alpha) : */Color(33, 33, 33, alpha));

	//if (var)
	//	Drawing::DrawString(F::ESPMisc, menu_pos.x, menu_pos.y + 1, Color::White(alpha), FONT_LEFT, "g");

	const auto prev_was_toggled = keybind_prev_state.toggled;
	const auto prev_was_toggled_settings = keybind_prev_state.toggled_settings;

	/*if (combobox_items.empty() && hovered && _mouse_pressed && ignore_items < n * max(line, 1) && !combobox_had_items)
		combo_opened[combo_idx] = true;

	bool did_set = false;

	if (combo_opened[combo_idx]) {
		ignore_items = (n + keybind_states.size()) * max(line, 1);
		did_set = true;
	}*/

	if (hovered && ignore_items < n * max(line, 1)) {

		if (_mouse_pressed && !feature::menu->_keybind_toggled) {
			//_mouse_pressed = false;
			feature::menu->_keybind_toggled = true;
			feature::menu->_keybind_toggled_this_tick = true;
			value_changed = true;
			keybind_prev_state.toggled = true;
			keybind_prev_state.start_listen_time = csgo.m_globals()->realtime;
		}
		else if (ctx.get_key_press(2) && !feature::menu->_keybind_toggled)
		{
			keybind_prev_state.toggled_settings = true;
			feature::menu->_keybind_toggled = true;
			feature::menu->_keybind_toggled_this_tick = true;
			keybind_prev_state.start_listen_time = csgo.m_globals()->realtime;
		}
	}

		if (prev_was_toggled && keybind_prev_state.toggled && !keybind_prev_state.toggled_settings && abs(keybind_prev_state.start_listen_time - csgo.m_globals()->realtime) > 0.1f)
		{
			for (auto i = 2; i < 256; i++) {

				if (prev_was_toggled && ctx.pressed_keys[i]) {
					if (i == VK_ESCAPE) {
						var->key = -1;
						keybind_prev_state.toggled = false;
						feature::menu->_keybind_toggled = false;
						ctx.pressed_keys[27] = false;
						break;
					}

					if (get_key_name(i).length() > 0) {
						var->key = i;
						ctx.pressed_keys[i] = false;
						keybind_prev_state.toggled = false;
						feature::menu->_keybind_toggled = false;
						feature::menu->_keybind_toggled_this_tick = true;
						break;
					}
				}
			}
		}

		if (keybind_prev_state.toggled_settings)
		{
			ignore_items = (n + keybind_states.size()) * max(line, 1);

			if (!prev_was_toggled_settings)
				keybind_prev_state.lock_settings_pos = _cursor_position;

			bool did_not_hover_anything = true;

			for (auto i = 0; i < int(keybind_states.size()); i++)
			{
				auto hovered_item = mouse_in_pos(Vector(menu_pos.x, menu_pos.y + 12 + i * 14, 0), Vector(menu_pos.x + 170, menu_pos.y + 14 + i * 14 + 14, 0));

				auto string_unformatted = keybind_states[i];
				combobox_items.emplace_back(string_unformatted, hovered_item || i == var->mode, Vector(menu_pos.x, menu_pos.y + 14, 0));

				if (hovered_item && _mouse_pressed) {
					did_not_hover_anything = false;
					var->mode = i;
					value_changed = true;
					keybind_prev_state.toggled_settings = false;
					feature::menu->_keybind_toggled_this_tick = true;
					_keybind_toggled = false;
					break;
				}
			}

			if (did_not_hover_anything && _mouse_pressed)
			{
				did_not_hover_anything = false;
				keybind_prev_state.toggled_settings = false;
				feature::menu->_keybind_toggled_this_tick = true;
				_keybind_toggled = false;
			}
		}


	if (ctx.pressed_keys[27] && keybind_prev_state.toggled_settings) {
		keybind_prev_state.toggled_settings = false;
		feature::menu->_keybind_toggled = false;
		ctx.pressed_keys[27] = false;
	}

	//if (!did_set)
	ignore_items = -1;

	if (value_changed)
		_mouse_pressed = ctx.pressed_keys[1] = false;
	//}

	if (!same_line)
		n += 1;

	return value_changed;
}

void c_menu::scrollbar(Vector pos, float sizeY, int& n)
{
	int col = 80;
	/*Drawing::DrawRect(pos.x, pos.y + 3, 5, 1, Color(col, col, col, 255));
	Drawing::DrawRect(pos.x, pos.y + 1 + 3, 5, 1, Color(col - 2, col - 2, col - 2, 255));
	Drawing::DrawRect(pos.x, pos.y + 2 + 3, 5, 1, Color(col - 4, col - 4, col - 4, 255));
	Drawing::DrawRect(pos.x, pos.y + 3 + 3, 5, 1, Color(col - 8, col - 8, col - 8, 255));
	Drawing::DrawRect(pos.x, pos.y + 4 + 3, 5, 1, Color(col - 12, col - 12, col - 12, 255));
	Drawing::DrawRect(pos.x, pos.y + 5 + 3, 5, 1, Color(col - 14, col - 14, col - 14, 255));
	Drawing::DrawRect(pos.x, pos.y + 6 + 3, 5, sizeY, Color(col - 16, col - 16, col - 16, 255));*/

	static bool startedscroll = false;

	if (mouse_in_pos(Vector(pos.x, pos.y + 3, 0), Vector(pos.x, pos.y + 3, 0) + Vector(5, sizeY, 0))) {
		Drawing::DrawRectGradientHorizontal(pos.x, pos.y + 3, 5, sizeY, main_color.alpha(alpha), main_color_fade.alpha(alpha));

		if (ctx.pressed_keys[1])
			startedscroll = true;
	}

	if (!ctx.pressed_keys[1])
		startedscroll = false;

	static int yX = 0;

	startedscroll ? Drawing::DrawRectGradientHorizontal(pos.x, pos.y + 3, 5, sizeY, main_color.alpha(alpha), main_color_fade.alpha(alpha)) : Drawing::DrawRectGradientHorizontal(pos.x, pos.y + 3, 5, sizeY, Color(col, col, col, alpha), Color(col - 16, col - 16, col - 16, alpha));

	if (startedscroll) {
		int speed = _cursor_position.x - yX;

		if (n == 0 && ((speed) <= 0 || (speed - 1) < 1))
			n = 0;
		else
			n += _cursor_position.y - yX;

		yX = _cursor_position.y;
	}
	else yX = _cursor_position.y;

}

bool c_menu::combobox(int& n, std::string name, std::vector<std::string> items, int& value, bool parent)//c_drawhack::combobox(std::string name, std::vector<std::string> items, int & value, bool )
{
	if (name.size() > 1)
		n++;
	auto value_changed = false;
	//auto pressed_and_had_to_close = false;
	auto menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * n), 0);
	if (line != 0) menu_pos.x += 200 * line;
	auto max_menu_pos = Vector(menu_pos.x + 180, menu_pos.y + 14, 0);

	static auto start_idx = 0;
	auto nSize = items.size();
	auto bHover = mouse_in_pos(menu_pos, max_menu_pos);

	//Draw->DrawOutlinedRect(MX, MY, MWidth, MHeight, Color(30, 30, 30, 200)); // Tabs main //OUTLINE
	bool no_name = name.size() <= 1;


	RECT textsizeñombo = Drawing::GetTextSize(F::MenuV2, items[value].c_str());

	if (nSize > 8)
		nSize = 8;

	auto combo_idx = (line != 0 ? ((n + (no_name ? 300 : name.c_str()[0])) * max(line, 1)) : n);

	const bool was_closed = !combo_opened[combo_idx];

	if (combobox_items.empty() && bHover && _mouse_pressed && ignore_items < n * max(line, 1) && !combobox_had_items)
		combo_opened[combo_idx] = true;

	bool did_set = false;

	if (combo_opened[combo_idx]) {
		ignore_items = (n + nSize) * max(line, 1);
		did_set = true;
	}

	//if (ignore_items < n * line || did_set) {
	if (name.size() > 1)
		Drawing::DrawString(F::MenuV2, menu_pos.x, menu_pos.y - 15, Color(255, 255, 255, (alpha * 0.9f)), false, name.c_str());

		Drawing::DrawRect(menu_pos.x, menu_pos.y, 180, 16, Color(37, 37, 37, alpha));
		Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 16, Color(22, 22, 22, alpha));
		Drawing::DrawOutlinedRect(menu_pos.x + 1, menu_pos.y + 1, 178, 14, Color(55, 55, 55, alpha));

		//if (!combo_opened[combo_idx]) {
			if (value >= items.size())
				value = items.size() - 1;

			auto main_string_unformatted = items[value]; if (auto pos = main_string_unformatted.find(".wav"); pos != std::string::npos) main_string_unformatted.erase(pos); if (main_string_unformatted.size() > 31) main_string_unformatted.resize(31);
			Drawing::DrawString(F::MenuV2, menu_pos.x + 3/* + (180 / 2) - (textsizeñombo.right / 2)*/, menu_pos.y + 1, Color(255, 255, 255, (alpha * 0.9f)), FONT_LEFT, main_string_unformatted.c_str());
		//}

		if (!did_set)
			ignore_items = -1;
	//}


	if (combo_opened[combo_idx]) {
		auto scroll_pos = Vector(max_menu_pos.x - 7, menu_pos.y + 13 + start_idx, 0); auto in_scroll_pos = mouse_in_pos(scroll_pos, scroll_pos + Vector(5, ((items.size() * 40) / max(items.size() - min(8, nSize), 1))/*14*/, 0));

		if (bHover)
		{
			if (_mouse_pressed && !in_scroll_pos && !was_closed)
				combo_opened[combo_idx] = !combo_opened[combo_idx];
		}
		else
		{
			if (_mouse_pressed && !was_closed) {

				if (combo_opened[combo_idx]) {
					for (int i = 0; i < nSize; i++)
					{
						if (mouse_in_pos(Vector(menu_pos.x, menu_pos.y + 14 + i * 14, 0), Vector(menu_pos.x + 180, menu_pos.y + 14 + i * 14 + 13, 0)) && !in_scroll_pos) {
							value = start_idx + i;
							value_changed = true;
						}
					}
				}

				if ((value_changed || !mouse_in_pos(menu_pos + Vector(0, nSize, 0), max_menu_pos + Vector(0, nSize * 14, 0))) && !in_scroll_pos)
					combo_opened[combo_idx] = false;
			}
		}

		//if (bHover)	// hover on top item that in our bar
		//	Drawing::DrawRect(menu_pos.x, menu_pos.y, 180, 12, Color(192, 192, 192, 0.15 * alpha));

		//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 14, bHover ? main_color.alpha(0.78431372549 * alpha) : Color(30, 30, 30, 0.78431372549 * alpha));

		if (nSize > 0)// checking our items
		{
			if (combo_opened[combo_idx])
			{
				if (_scroll_value != 0 && items.size() > nSize) {
					start_idx += _scroll_value * -1;

					_scroll_value = 0;
				}

				if (start_idx < 0)
					start_idx = 0;
				else if (start_idx >= (items.size() - nSize))
					start_idx = items.size() - nSize;

				//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y + 14, 180, nSize * 14, Color(30, 30, 30, 0.78431372549 * alpha));
				//Drawing::DrawRect(menu_pos.x, menu_pos.y + 14, 179, nSize * 14, Color(45, 45, 45, alpha));
				//Drawing::DrawRect(menu_pos.x, menu_pos.y + 13, 180, nSize * 13, Color(161, 161, 161, 255));

				for (int i = 0; i < nSize; i++)// drawing all items
				{
					auto hovered_item = mouse_in_pos(Vector(menu_pos.x, menu_pos.y + 14 + i * 14, 0), Vector(menu_pos.x + 180, menu_pos.y + 14 + i * 14 + 13, 0));

					auto string_unformatted = items[(i + start_idx)]; if (auto pos = string_unformatted.find(".wav"); pos != std::string::npos) string_unformatted.erase(pos); if (string_unformatted.size() > 31) string_unformatted.resize(31);

					combobox_items.emplace_back(string_unformatted, hovered_item || ((i + start_idx) == value), Vector(menu_pos.x + 2, menu_pos.y + 14, 0));
					//Drawing::DrawString(F::MenuV2, menu_pos.x + 2, menu_pos.y + 14 + i * 14, (hovered_item || ((i + start_idx) == value)) ? Color(255, 255, 255, alpha * 0.7) : Color(255, 255, 255, 0.3 * alpha), FONT_LEFT, string_unformatted.c_str());
				}

				if (items.size() > nSize)
					scrollbar(Vector(max_menu_pos.x - 7, menu_pos.y + 13 + start_idx, 0), ((items.size() * 40) / max(items.size() - min(8, nSize), 1)), start_idx);
			}
		}

		if (value >= items.size())
			value = items.size() - 1;

		//auto main_string_unformatted = items[value]; if (auto pos = main_string_unformatted.find(".wav"); pos != std::string::npos) main_string_unformatted.erase(pos); if (main_string_unformatted.size() > 31) main_string_unformatted.resize(31);

		//Drawing::DrawString(F::MenuV2, menu_pos.x + 3/* + (180 / 2) - (textsizeñombo.right / 2)*/, menu_pos.y, Color(255, 255, 255, alpha * 0.7), FONT_LEFT, main_string_unformatted.c_str());
	}

	n++;

	if (value_changed)
		_mouse_pressed = ctx.pressed_keys[1] = false;

	return value_changed;
}

bool c_menu::multicombo(int& n, std::string name, std::vector<MultiSelectable> items, bool parent)//c_drawhack::combobox(std::string name, std::vector<std::string> items, int & value, bool )
{
	n++;
	auto value_changed = false;
	auto menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * n), 0);
	if (line != 0) menu_pos.x += 200 * line;
	auto max_menu_pos = Vector(menu_pos.x + 180, menu_pos.y + 14, 0);

	auto nSize = items.size();
	auto bHover = mouse_in_pos(menu_pos, max_menu_pos);
	//auto bkeypress = (name.find("hitscan") != std::string::npos ? _mouse_pressed : cheat::game::get_key_press(1,2));
	//auto bState = &combo_opened[combo_idx];

	std::string items_selected = "";

	//Draw->DrawOutlinedRect(MX, MY, MWidth, MHeight, Color(30, 30, 30, 200)); // Tabs main //OUTLINE

	//RECT textsizeñombo = Drawing::GetTextSize(F::MenuV2, items_selected.c_str());

	//if (_mouse_pressed)
	//{
	//	if (bHover)
	//		bState = !bState;

	//	combobox_opened = bState;

	//	if (bState)
	//	{
	//		if (!bHover)
	//		{
	//			for (int i = 0; i < nSize; i++)
	//			{
	//				if (mouse_in_pos(Vector(menu_pos.x, menu_pos.y + 14 + i * 14, 0), Vector(menu_pos.x + 180, menu_pos.y + 14 + i * 14 + 13, 0))) {
	//					auto old_val = *items[i].value;
	//					*items[i].value = !old_val;
	//					value_changed = true;
	//				}
	//			}
	//		}
	//		
	//		if (!mouse_in_pos(menu_pos + Vector(0,nSize,0), max_menu_pos + Vector(0, nSize * 14,0)))
	//			bState = combobox_opened = false;
	//	}

	//	items_selected = 0;

	//	for (int i = 0; i < nSize; i++)// drawing all we got
	//	{
	//		if (*items[i].value == true)
	//			items_selected++;
	//	}
	//}

	auto combo_idx = (line != 0 ? ((max(n, 1) * (name.c_str()[0])) * max(line, 1)) : n);

	bool did_set = false;

	const bool was_closed = !combo_opened[combo_idx];


	if (combobox_items.empty() && bHover && _mouse_pressed && ignore_items < n * max(line, 1) && !combobox_had_items)
		combo_opened[combo_idx] = true;

	if (combo_opened[combo_idx]) {
		ignore_items = (n + nSize) * max(line, 1);
		did_set = true;
	}

	for (int i = 0; i < nSize; i++)// drawing all we got
	{
		if (*items[i].value == true)
			items_selected += items[i].name;

		if ((i + 1) < nSize && items_selected.size() > 1 && *items[i + 1].value == true)
			items_selected += ", ";

		if (Drawing::GetTextSize(F::MenuV2, items_selected.c_str()).right > 170) {
			items_selected.resize(32);
			items_selected += "...";
			break;
		}
	}

	const auto is_empty = items_selected.size() <= 0;

	if (is_empty)
		items_selected = "empty";

	//if (ignore_items < n * line || did_set)
	//{
		Drawing::DrawString(F::MenuV2, menu_pos.x, menu_pos.y - 15, Color(255, 255, 255, (alpha * 0.9f)), false, name.c_str());

		Drawing::DrawRect(menu_pos.x, menu_pos.y, 180, 16, Color(37, 37, 37, alpha));
		Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 16, Color(22, 22, 22, alpha));
		Drawing::DrawOutlinedRect(menu_pos.x+1, menu_pos.y+1, 178, 14, Color(55, 55, 55, alpha));

	//	if (!did_set)
	//		ignore_items = -1;
	//}

	if (combo_opened[combo_idx]) {
		if (bHover)
		{
			if (_mouse_pressed && !was_closed)
				combo_opened[combo_idx] = !combo_opened[combo_idx];
		}
		else
		{
			if (_mouse_pressed && !was_closed) {

				if (combo_opened[combo_idx]) {
					for (int i = 0; i < nSize; i++)
					{
						if (mouse_in_pos(Vector(menu_pos.x, menu_pos.y + 14 + i * 14, 0), Vector(menu_pos.x + 180, menu_pos.y + 14 + i * 14 + 13, 0))) {
							auto old_val = *items[i].value;
							*items[i].value = !old_val;
							value_changed = true;
						}
					}
				}

				if (!mouse_in_pos(menu_pos + Vector(0, nSize, 0), max_menu_pos + Vector(0, nSize * 14, 0)))
					combo_opened[combo_idx] = false;
			}
		}

		//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 14, bHover ? main_color.alpha(0.78431372549 * alpha) : Color(30, 30, 30, 0.78431372549 * alpha));

		if (nSize > 0)
		{
			if (combo_opened[combo_idx])
			{
				//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y + 14, 180, nSize * 14, Color(30, 30, 30, 0.78431372549 * alpha));
				//Drawing::DrawRect(menu_pos.x, menu_pos.y + 14, 179, nSize * 14, Color(45, 45, 45, alpha));

				for (int i = 0; i < nSize; i++)
				{
					auto hovered_item = mouse_in_pos(Vector(menu_pos.x, menu_pos.y + 14 + i * 14, 0), Vector(menu_pos.x + 180, menu_pos.y + 14 + i * 14 + 13, 0));

					combobox_items.emplace_back(items[i].name, hovered_item || *items[i].value, Vector(menu_pos.x + 2, menu_pos.y + 14, 0));
					//Drawing::DrawString(F::MenuV2, menu_pos.x + 2, menu_pos.y + 14 + i * 14, (hovered_item || *items[i].value) ? Color(255, 255, 255, alpha * 0.7) : Color(255, 255, 255, 0.3 * alpha), FONT_LEFT, items[i].name.c_str());
				}
			}
		}

		Drawing::DrawOutlinedRectA(menu_pos.x + 182 - 10, menu_pos.y + 16 - 8, menu_pos.x + 182 - 5, menu_pos.y + 16 - 7, Color(255, 255, 255, alpha * 0.7));
		Drawing::DrawOutlinedRectA(menu_pos.x + 182 - 9, menu_pos.y + 16 - 9, menu_pos.x + 182 - 6, menu_pos.y + 16 - 8, Color(255, 255, 255, alpha * 0.7));
		Drawing::DrawOutlinedRectA(menu_pos.x + 182 - 8, menu_pos.y + 16 - 10, menu_pos.x + 182 - 7, menu_pos.y + 16 - 9, Color(255, 255, 255, alpha * 0.7));

		if (!did_set)
			ignore_items = -1;
	}
	else
	{

		Drawing::DrawOutlinedRectA(menu_pos.x + 182 - 8, menu_pos.y + 16 - 8, menu_pos.x + 182 - 7, menu_pos.y + 16 - 7, Color(255, 255, 255, alpha * 0.4));
		Drawing::DrawOutlinedRectA(menu_pos.x + 182 - 9, menu_pos.y + 16 - 9, menu_pos.x + 182 - 6, menu_pos.y + 16 - 8, Color(255, 255, 255, alpha * 0.4));
		Drawing::DrawOutlinedRectA(menu_pos.x + 182 - 10, menu_pos.y + 16 - 10, menu_pos.x + 182 - 5, menu_pos.y + 16 - 9, Color(255, 255, 255, alpha * 0.4));

	}

	Drawing::DrawString(F::MenuV2, menu_pos.x + 3, menu_pos.y + 1, Color(255, 255, 255, is_empty ? (alpha * 0.4f) : (alpha * 0.9f)), FONT_LEFT, /*"%d items selected", */items_selected.c_str());

	n++;

	return value_changed;
}

void c_menu::colorpicker(int& n, std::string name, float* value, bool parent)
{
	n++;

	auto menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * n), 0);
	if (line != 0) menu_pos.x += 200 * line;
	auto max_menu_pos = Vector(menu_pos.x + 180, menu_pos.y + 14, 0);

	//if (ignore_items < n * (line + 1) {
		Color color = Color(40, 210, 116);

		Drawing::DrawString(F::MenuV2, menu_pos.x + 2, menu_pos.y - 16, Color(255, 255, 255), false, name.c_str());
		Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 14, Color(40, 40, 40));

		if (value[0] >= 0.97f)
			value[2] = 0.f;
		else
			value[2] = 1.f;

		if (!mouse_in_pos(menu_pos, max_menu_pos) || !ctx.pressed_keys[1])
			Drawing::DrawRect(menu_pos.x + 2, menu_pos.y + 2, value[0] * ((180.f - 4.f) / 1.f), 10, Color().FromHSB(value[0], value[2], 1.f).alpha(value[1] * 255));

		if (mouse_in_pos(menu_pos, max_menu_pos) && ignore_items < n * max(line, 1))
		{
			int start = value[0] * ((180.f - 4.f) / 1.f);
			float flColor = std::clamp((_cursor_position.x - menu_pos.x) * 1.f / (180.f - 4.f), 0.f, 1.f);
			Color newColor = Color().FromHSB(flColor, value[2], 1.f);

			if (ctx.pressed_keys[1] && !combo_was_opened) {
				value[0] = std::clamp((_cursor_position.x - menu_pos.x) * 1.f / (180.f - 4.f), 0.f, 1.f);
				//if (_cursor_position.x > (menu_pos.x + 1 + start))
				//	Drawing::DrawRect(menu_pos.x + 2 + start, menu_pos.y + 2, _cursor_position.x - menu_pos.x - start - 3.f, 10, newColor.alpha(alpha * 0.7));
				//else
				Drawing::DrawRect(menu_pos.x + 2, menu_pos.y + 2, _cursor_position.x - menu_pos.x - 3.f, 10, newColor.alpha(value[1] * 255));
			}
			else if (ctx.pressed_keys[2] && !combo_was_opened) {
				value[1] = std::clamp((_cursor_position.x - menu_pos.x) * 1.f / (180.f - 4.f), 0.f, 1.f);
				Drawing::DrawRect(menu_pos.x + 2, menu_pos.y + 2, _cursor_position.x - menu_pos.x - 3.f, 10, Color().FromHSB(value[0], 1.f, value[1]));
			}

			Drawing::DrawRect(_cursor_position.x + 12, _cursor_position.y + 2, 312, 16, Color::Black(200));
			Drawing::DrawString(F::MenuV2, _cursor_position.x + 14, _cursor_position.y + 4, Color(255, 255, 255), false, "hold mouse1 to change color / hold mouse2 to change alpha.");
		}

		ignore_items = -1;
	//}

	n++;
}

void c_menu::colorpicker2(int& n, std::string name, Color* value, bool parent, bool sameline, bool edit_alpha)
{
	//n++;

	auto menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * max(0, (n-1))), 0);
	if (line != 0) menu_pos.x += 200 * line;
	auto max_menu_pos = Vector(menu_pos.x + 180, menu_pos.y + 8, 0);

	bool value_changed = false;

	//if (ignore_items < n * (line + 1) {

	Color picker_col = *value;
	Drawing::DrawRect(max_menu_pos.x - 17, max_menu_pos.y - 3, 16, 8, picker_col.alpha(alpha));
	//Drawing::DrawRectGradientVerticalA(max_menu_pos.x - 17, max_menu_pos.y - 2, max_menu_pos.x - 2, max_menu_pos.y + 5, Color(0, 0, 0, 0), Color(0, 0, 0, alpha));
	
	Drawing::DrawOutlinedRect(max_menu_pos.x - 16, max_menu_pos.y - 2, 14, 6, Color(55, 55, 55, alpha));
	Drawing::DrawOutlinedRect(max_menu_pos.x - 17, max_menu_pos.y - 3, 16, 8, Color(22, 22, 22, alpha));

	const auto color_idx = int(n * int(name.size() > 1 ? (int)name[0] : 360));

	auto hovered = mouse_in_pos(max_menu_pos - Vector(19, 2, 0), max_menu_pos + Vector(2, 10 , 0 )) && ignore_items < n* max(line, 1);
	auto hovered_down = mouse_in_pos(picker_start_pos, picker_start_pos + Vector2D(150, edit_alpha ? 147 : 135)) && ignore_items < n* max(line, 1);

	if (render_color_picker_index == color_idx)
		m_allow_alpha = edit_alpha;

	if (hovered && _mouse_pressed && (!render_color_picker || render_color_picker_index == color_idx)) {
		render_color_picker = !render_color_picker;
		value_changed = true;
		if (render_color_picker) {
			render_color_picker_index = color_idx;
			m_col_value = value;
		}
		else {
			render_color_picker_index = 0;
			m_col_value = nullptr;
		}

		picker_start_pos = Vector2D(max_menu_pos.x + 1, contents_posY + (parent ? 19 : 0) + 10 + (16 * max(0, (n - 1))));
	}
	/*else if ( g_menuinput.is_key_released( VK_LBUTTON ) ) {
		g_menu.focus( nullptr );
		opened = false;
	}*/

	if (render_color_picker_index == color_idx && _mouse_pressed && !hovered && !mouse_in_pos(picker_start_pos, picker_start_pos + Vector2D(150, edit_alpha ? 147 : 135))) {
		render_color_picker = false;
		value_changed = true;
	}

	if (render_color_picker_index == color_idx && render_color_picker && ctx.pressed_keys[1] && mouse_in_pos(picker_start_pos, picker_start_pos + Vector2D(130, 130))) {
		const auto h = (_cursor_position.y - picker_start_pos.y) * (1.0f / 130);
		const auto s = 1.f;
		const auto l = 1.0f - (_cursor_position.x - picker_start_pos.x) * (1.0f / 130);
		static Color temp = Color(0, 0, 0);

		last_color_pos = { Math::clamp(_cursor_position.x, picker_start_pos.x, picker_start_pos.x + 130) , Math::clamp(_cursor_position.y, picker_start_pos.y, picker_start_pos.y + 130) };

		const auto prev_alpha = value->a();

		*value = temp.FromHSL(h, s, l).alpha(prev_alpha);
		_mouse_pressed = false;
		//value_changed = true;
	}

	bool dragging_alpha = false;

	bool hovered_alpha;

	// hovered
	if (render_color_picker_index == color_idx && render_color_picker && edit_alpha) {
		hovered_alpha = mouse_in_pos(picker_start_pos + Vector2D(0, 135), picker_start_pos + Vector2D(130, 140));

		if (hovered_alpha && _mouse_pressed)
			dragging_alpha = true;
		else if (!_mouse_pressed)
			dragging_alpha = false;

		if (dragging_alpha) {
			const auto drag_x = Math::clamp(int(_cursor_position.x - (picker_start_pos.x + 1)), 0, 130);

			value->_a() = int(255 * float(drag_x / 130.f));
		}

		value->_a() = Math::clamp(value->a(), 0, 255);
	}
	else
		hovered_alpha = false;

	if (value_changed) {
		ctx.pressed_keys[1] = _mouse_pressed = false;
	}

	//Drawing::DrawString(F::MenuV2, menu_pos.x + 2, menu_pos.y - 16, Color(255, 255, 255), false, name.c_str());
	//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 14, Color(40, 40, 40));

	//if (!mouse_in_pos(menu_pos, max_menu_pos) || !ctx.pressed_keys[1])
	//	Drawing::DrawRect(menu_pos.x + 2, menu_pos.y + 2, value[0] * ((180.f - 4.f) / 1.f), 10, Color().FromHSB(value[0], value[2], 1.f).alpha(value[1] * 255));

	//if (mouse_in_pos(menu_pos, max_menu_pos) && ignore_items < n * max(line, 1))
	//{
	//	int start = value[0] * ((180.f - 4.f) / 1.f);
	//	float flColor = std::clamp((_cursor_position.x - menu_pos.x) * 1.f / (180.f - 4.f), 0.f, 1.f);
	//	Color newColor = Color().FromHSB(flColor, value[2], 1.f);

	//	if (ctx.pressed_keys[1] && !combo_was_opened) {
	//		value[0] = std::clamp((_cursor_position.x - menu_pos.x) * 1.f / (180.f - 4.f), 0.f, 1.f);
	//		//if (_cursor_position.x > (menu_pos.x + 1 + start))
	//		//	Drawing::DrawRect(menu_pos.x + 2 + start, menu_pos.y + 2, _cursor_position.x - menu_pos.x - start - 3.f, 10, newColor.alpha(alpha * 0.7));
	//		//else
	//		Drawing::DrawRect(menu_pos.x + 2, menu_pos.y + 2, _cursor_position.x - menu_pos.x - 3.f, 10, newColor.alpha(value[1] * 255));
	//	}
	//	else if (ctx.pressed_keys[2] && !combo_was_opened) {
	//		value[1] = std::clamp((_cursor_position.x - menu_pos.x) * 1.f / (180.f - 4.f), 0.f, 1.f);
	//		Drawing::DrawRect(menu_pos.x + 2, menu_pos.y + 2, _cursor_position.x - menu_pos.x - 3.f, 10, Color().FromHSB(value[0], 1.f, value[1]));
	//	}

	//	Drawing::DrawRect(_cursor_position.x + 12, _cursor_position.y + 2, 312, 16, Color::Black(200));
	//	Drawing::DrawString(F::MenuV2, _cursor_position.x + 14, _cursor_position.y + 4, Color(255, 255, 255), false, "hold mouse1 to change color / hold mouse2 to change alpha.");
	//}

	//ignore_items = -1;
	////}
}


void c_menu::menugroupbox(int& n, int size, std::string text, int lines_size, int extra_y)
{
	Vector menu_pos = Vector(contents_posX, contents_posY + (12 * n), 0);
	if (line != 0) menu_pos.x += 200 * line;
	float extra_pixels = lines_size > 1 ? 5 * lines_size : 0.f;
	Vector max_menu_pos = Vector(menu_pos.x + (190 * max(1, lines_size) + extra_pixels), menu_pos.y + 16, 0);

	//if (ignore_items < n * (line + 1) {
		//Drawing::DrawRect(menu_pos.x, menu_pos.y, max_menu_pos.x - menu_pos.x, (size + 1) * 12, Color(28, 28, 28, alpha - 60));

		//Drawing::DrawRect(menu_pos.x, menu_pos.y, max_menu_pos.x - menu_pos.x, 15, Color(53, 53, 53, alpha - 40));
		Drawing::DrawRect(menu_pos.x, menu_pos.y, max_menu_pos.x - menu_pos.x, ((size + 1) * 12 + extra_y), Color(33, 33, 33, alpha - 10));

		//Drawing::Texture(menu_pos.x, menu_pos.y, max_menu_pos.x - menu_pos.x, ((size + 1) * 12 + extra_y), 0, const Color tint);

		Drawing::DrawOutlinedRect(menu_pos.x - 1, menu_pos.y - 1, (max_menu_pos.x - menu_pos.x) + 2, ((size + 1) * 12 + extra_y) + 2, Color(22, 22, 22, alpha - 10));
		Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, (max_menu_pos.x - menu_pos.x), ((size + 1) * 12 + extra_y), Color(55, 55, 55, alpha - 10));

		//Drawing::DrawRectGradientHorizontal(menu_pos.x, menu_pos.y, (190 * max(1, lines_size) + extra_pixels) / 2, 1, Color(12, 12, 12, alpha), ctx.m_settings.menu_color.alpha(alpha));
		//Drawing::DrawRectGradientHorizontal(menu_pos.x + (190 * max(1, lines_size) + extra_pixels) / 2 , menu_pos.y, (190 * max(1, lines_size) + extra_pixels) / 2, 1, ctx.m_settings.menu_color.alpha(alpha), Color(12, 12, 12, alpha));

		Drawing::DrawRectGradientHorizontal(menu_pos.x + (190 * max(1, lines_size) + extra_pixels) / 2 + 1, menu_pos.y + (size + 1) * 12 + extra_y - 1, (190 * max(1, lines_size) + extra_pixels) / 2, 2, ctx.m_settings.menu_color.alpha(10), ctx.m_settings.menu_color.alpha(alpha));
		Drawing::DrawRectGradientHorizontal(menu_pos.x - 1, menu_pos.y - 1, (190 * max(1, lines_size) + extra_pixels) / 2 + 1, 2, ctx.m_settings.menu_color.alpha(alpha), ctx.m_settings.menu_color.alpha(10));

		Drawing::DrawRectGradientVertical(menu_pos.x-1, menu_pos.y, /*menu_pos.x*/2, /*menu_pos.y +*/ (size * 12 + extra_y) / 2 + 1, ctx.m_settings.menu_color.alpha(alpha), ctx.m_settings.menu_color.alpha(10)); // left
		Drawing::DrawRectGradientVertical(max_menu_pos.x - 1, menu_pos.y + ((size + 1) * 12 + extra_y) / 2, 2, /*menu_pos.y + */((size + 1) * 12 + extra_y) / 2, ctx.m_settings.menu_color.alpha(10), ctx.m_settings.menu_color.alpha(alpha)); // right

		auto tsize = Drawing::GetTextSize(F::Menu, text.c_str());
		Drawing::DrawString(F::Menu, /*menu_pos.x + 150 / 2 - tsize.right / 2*/menu_pos.x + (190 * max(1, lines_size) + extra_pixels) / 2, menu_pos.y + 2, Color(255, 255, 255, alpha * 0.8f), FONT_CENTER, text.c_str());

		Drawing::DrawRect(menu_pos.x + 1, menu_pos.y + tsize.bottom + 3, max_menu_pos.x - menu_pos.x - 2, 1, Color(55,55,55));

		//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, max_menu_pos.x - menu_pos.x, (size + 1) * 12, Color(53, 53, 53, alpha));

		//if (!text.empty()) {
		//	auto tsize = Drawing::GetTextSize(F::MenuV2, text.c_str());
		//	Drawing::DrawString(F::MenuV2, /*menu_pos.x + 150 / 2 - tsize.right / 2*/menu_pos.x + 6, menu_pos.y - 7, Color(200, 200, 200, alpha* 0.7), FONT_LEFT, text.c_str());

		//	//Drawing::DrawRectGradientVertical(menu_pos.x, menu_pos.y, /*menu_pos.x*/1, /*menu_pos.y +*/ (size + 1) * 12, Color::White(alpha * 0.6), Color(15, 15, 15, alpha - 20)); // left

		//	Drawing::DrawLine(menu_pos.x, menu_pos.y + (size + 1) * 12, max_menu_pos.x, menu_pos.y + (size + 1) * 12, Color(53, 53, 53, alpha)); // down

		//	Drawing::DrawLine(menu_pos.x, menu_pos.y, menu_pos.x, menu_pos.y + (size + 1) * 12, Color(53, 53, 53, alpha)); // left

		//	//Drawing::DrawRectGradientVertical(max_menu_pos.x, menu_pos.y, 1, /*menu_pos.y + */(size + 1) * 12, Color::White(alpha * 0.6), Color(15, 15, 15, alpha - 20)); // right

		//	Drawing::DrawLine(max_menu_pos.x, menu_pos.y, max_menu_pos.x ,menu_pos.y + (size + 1) * 12, Color(53, 53, 53, alpha)); // right

		//	Drawing::DrawLine(menu_pos.x, menu_pos.y, menu_pos.x + 2, menu_pos.y, Color(53, 53, 53, alpha)); // top

		//	Drawing::DrawLine(menu_pos.x + tsize.right + 8, menu_pos.y, max_menu_pos.x, menu_pos.y, Color(53, 53, 53, alpha)); // top

		//	
		//}
		//else
		//	Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, max_menu_pos.x - menu_pos.x, (size + 1) * 12, Color::White(alpha * 0.6));
	//	ignore_items = -1;
	//}
	//return groupbox(line);
}

void c_menu::separator(int& n, std::string text, int line_size)
{
	Vector menu_pos = Vector(contents_posX, contents_posY + 19 + (16 * n), 0);
	if (line != 0) menu_pos.x += 200 * line;
	const float fixed_size = (line_size > 1 ? 5.f * line_size : 0.f);
	Vector max_menu_pos = Vector(menu_pos.x + (190 * line_size) + fixed_size - 1.f, menu_pos.y + 16, 0);
	//if (ignore_items < n * (line + 1) {
		if (!text.empty()) {
			auto tsize = Drawing::GetTextSize(F::MenuV2, text.c_str());
			Drawing::DrawString(F::MenuV2, /*menu_pos.x + 150 / 2 - tsize.right / 2*/menu_pos.x + 6, menu_pos.y - 7, Color(200, 200, 200, alpha * 0.7f), FONT_LEFT, text.c_str());

			Drawing::DrawLine(menu_pos.x, menu_pos.y, menu_pos.x + 2, menu_pos.y, Color(55, 55, 55, alpha)); // top

			Drawing::DrawLine(menu_pos.x + tsize.right + 8, menu_pos.y, max_menu_pos.x, menu_pos.y, Color(55, 55, 55, alpha)); // top
		}
		else
			Drawing::DrawLine(menu_pos.x, menu_pos.y, max_menu_pos.x, menu_pos.y, Color(55, 55, 55, alpha));

	//	ignore_items = -1;
	//}
	n++;
	//return groupbox(line);
}

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> split(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

bool c_menu::menutab(int& tab, std::string text, bool current_selected)
{
	std::vector<std::string> lmao_wtf = {};

	bool value_changed = false;

	if (text.find(";") != std::string::npos)
		lmao_wtf = split(text, ';');
	else
		lmao_wtf.push_back(text);

	//auto text_size = Drawing::GetTextSize(F::MenuIcons, lmao_wtf.front().c_str());

	//Vector menu_pos = Vector(contents_posX, contents_posY + (parent ? 12 : 5) + tab * 36, 0);
	//Vector menu_pos = Vector(contents_posX + width_menu - tab - 5, menu_posY, 0);
	float width = (width_menu / 6) - 1;
	float height = 25.f;
	Vector menu_pos = Vector(menu_posX - 8 + tab, tabs_posY, 0);

	Vector max_menu_pos = Vector(menu_pos.x + width, menu_pos.y + height, 0);

	auto hovered = mouse_in_pos(menu_pos, max_menu_pos);

	auto &hover_a = hover_alpha[tab + (int)text.c_str()[0]];
	auto &hover_shit_a = tab_hover_shit_alpha[tab + (int)text.c_str()[0]];

	if (hovered)
	{
		if (hover_a < 1.f)
			hover_a += min(1.f - hover_a, csgo.m_globals()->frametime * 1.6f);

		if (hover_shit_a < 0.7f)
			hover_shit_a += min(0.7f - hover_shit_a, csgo.m_globals()->frametime * 1.5f);
	}
	else
	{
		if (hover_a > 0.3f)
			hover_a -= min(hover_a - 0.3f, csgo.m_globals()->frametime * 1.6f);

		if (hover_shit_a > 0)
			hover_shit_a -= min(hover_shit_a, csgo.m_globals()->frametime * 1.6f);

		if (hover_a < 0.3f)
			hover_a += min(0.3f - hover_a, csgo.m_globals()->frametime * 1.6f);
	}

	hover_a = Math::clamp(hover_a, 0.f, 1.f);
	hover_shit_a = Math::clamp(hover_shit_a, 0.f, 0.35f);

	/*if (hover_shit_a > 0.f) 
	{
		float val = (width * hover_shit_a) / 3;
		float hval = (height * hover_shit_a) / 3;

		if ((tab + 6 + width * 2) > width_menu)
			menu_pos.x -= val;
		else if ((tab + 6 + width * 3) > width_menu)
			menu_pos.x -= val / 2;
		
		width += val;

		if ((tabs_posY - contents_posY + height * 2) > height_menu)
			menu_pos.y -= hval;
		
		height += hval;
	}*/

	//else
	//	fill_rgba(menu_pos.x, menu_pos.y, text_size + 5, 11, 36, 36, 36, 255);

	//draw_quad(menu_pos.x, menu_pos.y, menu_pos.x + text_size + 5, menu_pos.y + 11, 255, 255, 255, 50);

	//Drawing::DrawString(F::LBY, menu_pos.x + 4, max_menu_pos.y - 35 / 2 - text_size.bottom / 2, Color::White(alpha), FONT_LEFT, text.c_str());

	//if (selected)
	//	Drawing::DrawRect(menu_pos.x, menu_pos.y - 2, text_size.right + 4, 16, Color(255, 255, 255, 0.15 * alpha));
	//else if (hovered)
	//	Drawing::DrawRect(menu_pos.x, menu_pos.y - 2, text_size.right + 4, 16, Color(192, 192, 192, 0.15 * alpha));

	//Drawing::DrawString(F::MenuV2, menu_pos.x + 2, menu_pos.y, Color::White(alpha* (selected ? 1 : (hovered ? 0.8 : 0.5))), FONT_LEFT, lmao_wtf.front().c_str());

	if (current_selected && hover_a < 1.f)
		hover_a = 1.f;

	Drawing::DrawRect(menu_pos.x, menu_pos.y, width, height, Color(42, 42, 42, alpha - (current_selected ? 0 : 20)));

	//if ((hovered || hover_shit_a > 0.f) && is_tabs_opened)
	//{
	//	*(bool*)((DWORD)csgo.m_surface() + 0x280) = true;
	//	int x, y, x1, y1;
	//	Drawing::GetDrawingArea(x, y, x1, y1);
	//	Drawing::LimitDrawingArea(menu_pos.x - 2, menu_pos.y - 2, 125 + 3, 159 + 2);

	//	Drawing::DrawFilledCircle(_cursor_position.x, _cursor_position.y, 30, 40, Color(63, 63, 63,alpha * (hover_shit_a * 0.7f)));
	//	Drawing::DrawCircle(_cursor_position.x, _cursor_position.y, 30, 40, Color(12, 12, 12, alpha * hover_shit_a));

	//	Drawing::LimitDrawingArea(x, y, x1, y1);
	//	//*(bool*)((DWORD)csgo.m_surface() + 0x280) = false;
	//}

	Drawing::DrawOutlinedRect(menu_pos.x - 1, menu_pos.y - 1, width + 2, height + 2, Color(22, 22, 22, alpha - (current_selected ? 0 : 20)));
	Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, width, height, Color(55, 55, 55, alpha - (current_selected ? 0 : 20)));

	if (current_selected) {
		Drawing::DrawRect(menu_pos.x + 2, menu_pos.y - 4 + height, -4 + width, 2, ctx.m_settings.menu_color.alpha(alpha * 0.5f));
	}
	//Drawing::DrawRectGradientHorizontal(menu_pos.x - 2, menu_pos.y + height + 2, width / 2 + 3, 1, Color(12, 12, 12, alpha * hover_a), ctx.m_settings.menu_color.alpha(alpha * hover_a));
	//Drawing::DrawRectGradientHorizontal(menu_pos.x + width / 2, menu_pos.y + height + 2, width / 2 + 3, 1, ctx.m_settings.menu_color.alpha(alpha * hover_a), Color(12, 12, 12, alpha * hover_a));

	//Drawing::DrawRectGradientHorizontal(menu_pos.x - 3, menu_pos.y + height + 3, width / 2 + 4, 1, Color(12, 12, 12, alpha * (hover_a * 0.3f)), ctx.m_settings.menu_color.alpha(alpha * (hover_a * 0.3f)));
	//Drawing::DrawRectGradientHorizontal(menu_pos.x + width / 2, menu_pos.y + height + 3, width / 2 + 4, 1, ctx.m_settings.menu_color.alpha(alpha * (hover_a * 0.3f)), Color(12, 12, 12, alpha * (hover_a * 0.3f)));

	//Drawing::DrawString(F::MenuIcons, menu_pos.x + width / 2, menu_pos.y + height / 2 - text_size.bottom + 10, Color::White(alpha * min(1.f, hover_a + 0.2f)), FONT_CENTER, lmao_wtf.front().c_str());

	//if (lmao_wtf.size() > 1)
		Drawing::DrawString(F::Menu, menu_pos.x + width / 2, menu_pos.y + height - 20, Color::White(alpha * min(1.f, hover_a + 0.2f)), FONT_CENTER, lmao_wtf.at(1).c_str());

	if (hovered && _mouse_pressed && !combo_was_opened) {
		value_changed = true;
	}

	tab += width + 2/*width + 2*/;

	lmao_wtf.clear();

	return value_changed;
}

bool c_menu::menubuttons(int& n, std::vector < std::string > items, int& var, bool parent, int adjustY, int line_size, bool has_icons)
{
	bool value_changed = false;
	static float avg_size = 0.f;
	//Vector menu_pos = Vector(contents_posX, contents_posY + (parent ? 12 : 5) + tab * 36, 0);
	//if (ignore_items < n * (line + 1) {

	const float extra_fixed_size = line_size > 1 ? line_size * 5.f : 0.f;

	const auto maxsize = ((180 * line_size + extra_fixed_size) / items.size());

		for (auto i = 0; i < items.size(); i++) {
			auto text_size = Drawing::GetTextSize(F::MenuV2, items.at(i).c_str());

			auto& hover_a = subtab_icons_alpha[(n + i + (items.empty() ? 2 : items.front().c_str()[0])) * max(line, 1)];

			Vector menu_pos = Vector(contents_posX + 5 + maxsize * i, contents_posY + (parent ? 19 : 0) + (16 * n) + adjustY, 0);
			if (line != 0) menu_pos.x += 200 * line;
			Vector max_menu_pos = Vector(menu_pos.x + maxsize, menu_pos.y + 14, 0);

			const auto hovered = mouse_in_pos(menu_pos, max_menu_pos);

			//if (var == i)
			//	Drawing::DrawRect(menu_pos.x, menu_pos.y, maxsize, 14, main_color_fade.alpha(0.15 * alpha));
			//else if (hovered)
			//	Drawing::DrawRect(menu_pos.x, menu_pos.y, maxsize, 14, Color(192, 192, 192, 0.15 * alpha));

			if (has_icons)
			{
				if (hovered)
				{
					if (hover_a.alpha < 1)
						hover_a.alpha += min(1.f - hover_a.alpha, csgo.m_globals()->frametime * 2.f);
				}
				else
				{
					if (hover_a.alpha > 0.2f)
						hover_a.alpha -= min(hover_a.alpha - 0.2f, csgo.m_globals()->frametime * 3.f);
				}

				hover_a.alpha = Math::clamp(hover_a.alpha, 0.2f, 1.f);

				//auto non_icon_string = items.at(i);
				//non_icon_string.erase(0, non_icon_string.find_first_of(";") + 1);

				//avg_size += 

				if (hover_a.icon.empty()) {
					std::vector<std::string> lmao_wtf = {};

					bool value_changed = false;

					if (items.at(i).find(";") != std::string::npos)
						lmao_wtf = split(items.at(i), ';');
					else
						lmao_wtf.push_back(items.at(i));

					hover_a.name = lmao_wtf.at(1);
					hover_a.icon = lmao_wtf.at(0);
				}

				if (hover_a.alpha > 0.5f)
					Drawing::DrawString(F::MenuV2, menu_pos.x + maxsize / 2, menu_pos.y, Color::White(min(hover_a.alpha, 0.65f) * alpha), FONT_CENTER, hover_a.name.c_str());
				else
					Drawing::DrawString(F::Icons, menu_pos.x + maxsize / 2, menu_pos.y + 1, (var == i) ? Color::White(alpha) : Color::White(hover_a.alpha * alpha), FONT_CENTER, hover_a.icon.c_str());
			}
			else
				Drawing::DrawString(F::MenuV2, menu_pos.x + maxsize / 2 - text_size.right / 2, menu_pos.y + 1, (hovered || var == i) ? (Color::White(var == i ? alpha * 0.9f : alpha * 0.7f)) : Color::White(0.4f * alpha), FONT_LEFT, items.at(i).c_str());

			/*if (i > 0)
			{
				Drawing::DrawRectGradientVertical(menu_pos.x, menu_pos.y - 4, 2, 11, Color(12, 12, 12, alpha * 0.5), ctx.m_settings.menu_color.alpha(alpha * 0.5));
				Drawing::DrawRectGradientVertical(menu_pos.x, menu_pos.y + 7, 2, 9, ctx.m_settings.menu_color.alpha(alpha * 0.5), Color(12, 12, 12, alpha * 0.5));
			}*/

			if (hovered && _mouse_pressed && ignore_items < n * max(line, 1) && !combo_was_opened) {
				var = i;
				value_changed = true;
			}
		}
		ignore_items = -1;
	//}

	n++;

	return value_changed;
}

int is_already_holding = -1;

void c_menu::menu_slider(int& n, const char* name, float min, float max, float& value, bool parent, const int& display_decimal, const char* mark)
{
	n++;
	Vector menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * n) - 4, 0);
	if (line != 0) menu_pos.x += 200 * line;
	Vector max_menu_pos = Vector(menu_pos.x + 180, menu_pos.y + 14, 0);
	//if (ignore_items < n * (line + 1) {
		//g_Drawing.MenuStringNormal(false, true, columns[column] + (tabs[3].w / 2) + tabs[2].w - 43, itemheight[column] + 7, Color(255, 255, 255, 200), "%s: %.1f", name, value);

		std::string noob = "%.1f";

		if (display_decimal <= 9)
			noob[2] = std::to_string(display_decimal).c_str()[0];

		if ((max - min) == 0)
			max += 0.001f;

		char text[255]; sprintf(text, noob.c_str(), value);

		if (strlen(mark) > 0)
			sprintf(text, "%s %s", text, mark);

		if (value > max) value = max;
		if (value < min) value = min;

		if (mouse_in_pos(menu_pos, max_menu_pos) && !combo_was_opened && ctx.pressed_keys[1] && !save_pos && !was_moved && ignore_items < n * max(line, 1) || is_already_holding && was_holding[n * name[0]])
		{
			value = min + (((min(max(_cursor_position.x, menu_pos.x + 2), menu_pos.x + 180) - menu_pos.x + 2) / 180) * (max - min));//(_cursor_position.x - menu_pos.x) * (max - min) / (180 - 7);

			if (is_already_holding == -1) {
				was_holding[n * name[0]] = true;
				is_already_holding = n * name[0];
			}

			if (value > max) value = max;
			if (value < min) value = min;
		}

		if (!ctx.pressed_keys[1] && was_holding[n * name[0]] && is_already_holding && ignore_items < n * max(line, 1)) {
			was_holding[n * name[0]] = false;
			is_already_holding = -1;
		}

		int val = ((value - min) * 180 / (max - min));

		float text_size = Drawing::GetTextSize(F::MenuV2, text).right;

		//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y + 1, 180, 7, Color(65, 65, 65, alpha));
		Drawing::DrawRect(menu_pos.x, menu_pos.y + 1, 180, 7, Color(37, 37, 37, alpha));

		if (val > 0)
			Drawing::DrawRectGradientVertical(menu_pos.x + 2, menu_pos.y + 1, val - 4, 6, ctx.m_settings.menu_color.malpha(0.70588235294f), ctx.m_settings.menu_color.alpha(alpha));

		Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y + 5, 178, 7, Color(22, 22, 22, alpha));
		/*int col = 80;
		g_Drawing.FilledRect(x, itemheight[column], width, 1, Color(col, col, col, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 1, width, 1, Color(col - 2, col - 2, col - 2, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 2, width, 1, Color(col - 4, col - 4, col - 4, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 3, width, 1, Color(col - 8, col - 8, col - 8, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 4, width, 1, Color(col - 12, col - 12, col - 12, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 5, width, 1, Color(col - 14, col - 14, col - 14, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 6, width, 9, Color(col - 16, col - 16, col - 16, 255));

		g_Drawing.GradientHorizontal(x, itemheight[column], val, barheight, Colors.maincolor, Colors.maincolorfade, 128);

		g_Drawing.OutlinedRect(x, itemheight[column], width, barheight, Color(51, 51, 51, 255));
		g_Drawing.FilledRect(x, itemheight[column], width - 1, 1, Color(91, 91, 91, 255));
		g_Drawing.FilledRect(x, itemheight[column], 1, 14, Color(91, 91, 91, 255));*/

		Drawing::DrawString(F::MenuV2i, menu_pos.x + (180 + 1) / 2 - text_size / 2, menu_pos.y + 8, Color::White(alpha * 0.65f), FONT_LEFT, text);

		Drawing::DrawString(F::MenuV2, menu_pos.x /*+ 110 + 2*/, menu_pos.y - 11, Color::White(alpha * 0.9f), FONT_LEFT, name);

	//	ignore_items = -1;
	///}

	n++;
}

void c_menu::menu_slider(int& n, const char* name, int min, int max, int& value, bool parent, const char* mark, bool if_zero_display_text, const char* ntext)
{
	if (strlen(name) > 1)
	n++;

	//if (ignore_items < n * (line + 1) {
		Vector menu_pos = Vector(contents_posX + 5, contents_posY + (parent ? 19 : 0) + (16 * n) - 4, 0);
		if (line != 0) menu_pos.x += 200 * line;
		Vector max_menu_pos = Vector(menu_pos.x + 180, menu_pos.y + 18, 0);

		auto hover = mouse_in_pos(menu_pos, max_menu_pos) && ignore_items < n * max(line, 1);
		//g_Drawing.MenuStringNormal(false, true, columns[column] + (tabs[3].w / 2) + tabs[2].w - 43, itemheight[column] + 7, Color(255, 255, 255, 200), "%s: %.1f", name, value);

		char text[255]; sprintf(text, "%i %s", value, mark);

		if (value > max) value = max;
		if (value < min) value = min;

		bool no_name = false;

		if (strlen(name) <= 1)
			no_name = true;

		if (hover && !combo_was_opened && ctx.pressed_keys[1] && !save_pos && !was_moved || is_already_holding && was_holding[n * (no_name ? 200 : name[0])])
		{
			value = min + (((min(max(_cursor_position.x, menu_pos.x), menu_pos.x + 176) - menu_pos.x) / 176) * (max - min));//(_cursor_position.x - menu_pos.x) * (max - min) / (180 - 7);

			if (is_already_holding == -1) {
				was_holding[no_name ? 200 : n * (name[0])] = true;
				is_already_holding = n * (no_name ? 200 : name[0]);
			}

			if (value > max) value = max;
			if (value < min) value = min;
		}
		
		if (!ctx.pressed_keys[1] && was_holding[n * (no_name ? 200 : name[0])] && is_already_holding) {
			was_holding[n * (no_name ? 200 : name[0])] = false;
			is_already_holding = -1;
		}

		//int valueX = menu_pos.x + ((value - min) * 180 / (max - min));
		//int val = valueX - menu_pos.x;
		int val = ((value - min) * 176 / (max - min));

		float text_size = Drawing::GetTextSize(F::MenuV2, text).right;

		Drawing::DrawRect(menu_pos.x, menu_pos.y + 5, 180, 7, Color(37, 37, 37, alpha));

		if (val > 0)
			Drawing::DrawRectGradientVertical(menu_pos.x + 2, menu_pos.y + 6, val, 4, ctx.m_settings.menu_color.malpha(0.70588235294f), ctx.m_settings.menu_color.alpha(alpha));

		Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y + 5, 180, 7, Color(22, 22, 22, alpha));
		Drawing::DrawOutlinedRect(menu_pos.x+1, menu_pos.y + 6, 178, 5, Color(55, 55, 55, alpha));

		//Drawing::DrawOutlinedRect(menu_pos.x, menu_pos.y, 180, 7, hover ? main_color.alpha(0.78431372549 * alpha) : Color(30, 30, 30, 0.78431372549 * alpha));

		/*int col = 80;
		g_Drawing.FilledRect(x, itemheight[column], width, 1, Color(col, col, col, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 1, width, 1, Color(col - 2, col - 2, col - 2, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 2, width, 1, Color(col - 4, col - 4, col - 4, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 3, width, 1, Color(col - 8, col - 8, col - 8, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 4, width, 1, Color(col - 12, col - 12, col - 12, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 5, width, 1, Color(col - 14, col - 14, col - 14, 255));
		g_Drawing.FilledRect(x, itemheight[column] + 6, width, 9, Color(col - 16, col - 16, col - 16, 255));

		g_Drawing.GradientHorizontal(x, itemheight[column], val, barheight, Colors.maincolor, Colors.maincolorfade, 128);

		g_Drawing.OutlinedRect(x, itemheight[column], width, barheight, Color(51, 51, 51, 255));
		g_Drawing.FilledRect(x, itemheight[column], width - 1, 1, Color(91, 91, 91, 255));
		g_Drawing.FilledRect(x, itemheight[column], 1, 14, Color(91, 91, 91, 255));*/

		if (if_zero_display_text && val == 0)
			strcpy(text, ntext);

		Drawing::DrawString(F::MenuV2i, menu_pos.x + (180) / 2 - text_size / 2, menu_pos.y + 8, Color::White(alpha * 0.65f), FONT_OUTLINE, text);

		if (!no_name)
		Drawing::DrawString(F::MenuV2, menu_pos.x /*+ 110 + 2*/, menu_pos.y - 9, Color::White(alpha * 0.9f), FONT_LEFT, name);

	//	ignore_items = -1;
	//}

	n++;
}

void c_menu::draw_gui()
{
	//hint_item.name.clear();
	//hint_item.time = 0.f;

	_mouse_pressed = false;
	//is_using_slider = false;
	ctx.scroll_value = 0;
}

void c_menu::render()
{
	static int pX, pY, framecount_prev = csgo.m_globals()->framecount;

	if (csgo.m_globals()->framecount != framecount_prev) {
		csgo.m_input_system()->GetCursorPosition(&pX, &pY);
		framecount_prev = csgo.m_globals()->framecount;
	}

	_cursor_position.x = (float)pX;
	_cursor_position.y = (float)pY;

	//if (ignored.size() > 5)
	//	ignored.clear();

	return;

	//if (_menu_opened) {
	//	if (alpha < 255)
	//		alpha += min(255 - alpha, 20);
	//}
	//else {
	//	if (alpha > 0)
	//		alpha -= min(alpha, 5);
	//}

	//if (alpha > 10) {
	//	draw_gui();

	//	Color clr = Color(3, 6, 26, alpha - 20);

	//	Drawing::DrawRect(_cursor_position.x + 1, _cursor_position.y, 1, 17, clr);

	//	for (int i = 0; i < 11; i++)
	//		Drawing::DrawRect(_cursor_position.x + 2.f + i, _cursor_position.y + 1.f + i, 1, 1, clr);
	//	Drawing::DrawRect(_cursor_position.x + 7.f, _cursor_position.y + 12.f, 6, 1, clr);
	//	Drawing::DrawRect(_cursor_position.x + 6.f, _cursor_position.y + 12.f, 1, 1, clr);
	//	Drawing::DrawRect(_cursor_position.x + 5.f, _cursor_position.y + 13.f, 1, 1, clr);
	//	Drawing::DrawRect(_cursor_position.x + 4.f, _cursor_position.y + 14.f, 1, 1, clr);
	//	Drawing::DrawRect(_cursor_position.x + 3.f, _cursor_position.y + 15.f, 1, 1, clr);
	//	Drawing::DrawRect(_cursor_position.x + 2.f, _cursor_position.y + 16.f, 1, 1, clr);

	//	Color mclr = Color(255, 255, 255, alpha);

	//	for (int i = 0; i < 4; i++)
	//		Drawing::DrawRect(_cursor_position.x + 2.f + i, _cursor_position.y + 2.f + i, 1, 14 - (i * 2), mclr);
	//	Drawing::DrawRect(_cursor_position.x + 6.f, _cursor_position.y + 6.f, 1, 6, mclr);
	//	Drawing::DrawRect(_cursor_position.x + 7.f, _cursor_position.y + 7.f, 1, 5, mclr);
	//	Drawing::DrawRect(_cursor_position.x + 8.f, _cursor_position.y + 8.f, 1, 4, mclr);
	//	Drawing::DrawRect(_cursor_position.x + 9.f, _cursor_position.y + 9.f, 1, 3, mclr);
	//	Drawing::DrawRect(_cursor_position.x + 10.f, _cursor_position.y + 10.f, 1, 2, mclr);
	//	Drawing::DrawRect(_cursor_position.x + 11.f, _cursor_position.y + 11.f, 1, 1, mclr);
	//}
}