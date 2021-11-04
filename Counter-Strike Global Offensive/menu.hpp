#pragma once

#include "sdk.hpp"

struct MultiSelectable
{
	std::string		name;
	bool* value;

	MultiSelectable(const std::string& name_, bool* value_)
	{
		name = name_;
		value = value_;
	}
};

struct combobox_item
{
	std::string name = "";
	Vector left_top = Vector::Zero;
	bool selected = false;

	combobox_item(const std::string _name, const bool _selected, const Vector _left_top)
	{
		name = _name;
		selected = _selected;
		left_top = _left_top;
	}
};

struct hint_item
{
	std::string name = "";
	float time = 0.f;
};

class c_menu
{
public:
	///* main calls */
	//void render();
	//void add_subtabs(float& tabsz, int& i, std::vector<std::string> v);
	//void finish_tab();
	//void draw_gui();
	//bool mouse_in_pos(Vector start, Vector end);
	//bool menuitem(int& n, std::string text, bool& var, bool parent);
	//bool add_bool(std::string text, bool& var, bool has_colorpicker = false);
	//bool add_key(std::string text, bool center = false);
	//bool add_int(std::string name, std::vector<std::string> items, int& var);
	//void finish_subtabs();
	//bool menutab(float& tab, std::string text, bool selected = false, bool lol = false, float tabs_size = 0.f);
	//bool menusubtab(float& pos, std::string text, bool selected, float tab_size);
	//void menu_colorpicker(const char* name, float* value, bool prev_item_pos = false);
	//void menu_slider(const char* name, float min, float max, float& value, const int& display_decimal = 1, const char* mark = "");
	////bool combobox(std::string name, std::vector<std::string> items, int& value);
	//bool add_groupbox(std::string text);
	//void end_groupbox();
	///* ENDOF: main calls */

	//bool _menu_opened = true;
	//Vector2D _cursor_position;
	//bool _mouse_pressed = false;

	///* menu main & misc things */
	//bool	_save_pos = false;
	//int		_saved_x = 0;
	//int		_saved_y = 0;
	//int		_menu_posX = 100;
	//int		_menu_posY = 100;
	//int		_contents_posX = 100;
	//int		_contents_posY = 100;

	//int		_nonscroll_contents_posY = 100;

	//int		_scroll_value = 0;

	//int		_prev_i = 0;
	//int		_i = 0;
	//int		_curtab = 0;
	//int		_alpha = 0;
	//int		_parent = 0;
	//int		_parents_count = 0;
	//int		_last_parent_i = 0;
	//int		_multiplier = 1;
	///* ENDOF: menu main & misc things */

	//int x, y, w, h;

	///* spectator list position & move handlers */
	//int		_saved_spec_x = 0;
	//int		_saved_spec_y = 0;
	//bool	_saved_spec_pos = false;
	///* ENDOF: spectator list position & move handlers */

	//bool _is_using_slider = false;
	virtual void render();

	virtual bool mouse_in_pos(Vector start, Vector end);

	virtual bool mouse_in_pos(Vector2D start, Vector2D end);

	//menu items below
	virtual bool menuitem(int& n, std::string text, bool& var, bool parent = false);
	virtual bool keybind(int& n, std::string text, c_keybind* var, bool parent, bool same_line);
	virtual void scrollbar(Vector pos, float sizeY, int& n);
	virtual bool combobox(int& n, std::string name, std::vector<std::string> items, int& selected, bool parent = false);
	virtual bool multicombo(int& n, std::string name, std::vector<MultiSelectable> items, bool parent);
	virtual void colorpicker(int& n, std::string name, float* value, bool parent = false);
	virtual void colorpicker2(int& n, std::string name, Color* value, bool parent, bool sameline, bool edit_alpha);
	virtual void menugroupbox(int& n, int size, std::string text = "", int lines_size = 1, int extra_y = 0);
	virtual void separator(int& n, std::string text = "", int line_size = 1);
	//bool menuitem(int & n, std::string name, std::vector<std::string> items, int & var, bool parent = false);
	/*virtual void menu_slider(int & n, const char * name, float min, float max, float & value, const int & display_decimal, const char * mark);
	virtual void menu_slider(int & n, const char * name, int min, int max, int & value, const char * mark);
	virtual void change_line(int _line, int & i, int & old);
	virtual bool menubutton(int & n, std::string text);*/
	virtual bool menutab(int& n, std::string text, bool is_tabs_opened);
	/*virtual bool menuitem(int & n, std::string text_t, int & var, int min, int max);
	virtual void get_keys(bool & IsGettingKey, std::string & text, bool secondtry);
	virtual void get_key(bool & IsGettingKey, int & get);
	virtual bool menuinput(int & n, const char * name, char * text, int buf_size);
	virtual bool menuinput(int & n, const char * name, std::string & text);
	virtual bool menukeybind(int & n, int * var);*/

	virtual bool menubuttons(int& tab, std::vector<std::string> items, int& var, bool parent = false, int adjustY = 0, int line_size = 1, bool has_icons = false);

	virtual void menu_slider(int& n, const char* name, float min, float max, float& value, bool parent = false, const int& display_decimal = 1, const char* mark = "");

	virtual void menu_slider(int& n, const char* name, int min, int max, int& value, bool parent = false, const char* mark = "", bool ll = false, const char* nn = "");

	//boxes to render
	virtual void draw_gui();
	//virtual void draw_speclist_box();

	//misc shit
	bool _menu_opened = true;
	bool _keybind_toggled = true;
	bool _keybind_toggled_this_tick = true;
	Vector2D _cursor_position;
	bool _mouse_pressed = false;
	bool save_pos = false;
	int saved_x = 0;
	int saved_y = 0;
	int line = 0/*left*/;
	int contents_posX = 100;
	int contents_posY = 100;
	int tabs_posY = 100;
	int menu_posX = 100;
	int menu_posY = 100;
	bool combobox_had_items = false;
	bool combobox_opened = false;

	std::vector<std::string> keybind_states = { sxor("toggle"), sxor("on key"), sxor("off key"), sxor("always on") };

	std::vector<Vector> ignored;
	std::vector<combobox_item> combobox_items;
	bool render_color_picker = false;
	int render_color_picker_index = 0;
	Vector2D picker_start_pos = Vector2D::Zero;
	Vector2D last_color_pos = Vector2D::Zero;
	Color* m_col_value = nullptr;
	bool m_allow_alpha = false;
	hint_item hint_item;
	bool combo_was_opened = false;

	int alpha = 0;

	int saved_box_x = 0;
	int saved_box_y = 0;
	bool save_box_pos = false;

	int saved_spec_x = 0;
	int saved_spec_y = 0;
	bool save_spec_pos = false;

	int _scroll_value = 0;
	virtual std::string get_key_name(int key);
	
	DWORD XOR_KEY = 0;
};

inline std::string c_menu::get_key_name(int key) {
	switch (key) {
	case VK_LBUTTON:
		return "m1";
	case VK_RBUTTON:
		return "m2";
	case VK_CANCEL:
		return "brk";
	case VK_MBUTTON:
		return "m3";
	case VK_XBUTTON1:
		return "m4";
	case VK_XBUTTON2:
		return "m5";
	case VK_BACK:
		return "back";
	case VK_TAB:
		return "tab";
	case VK_CLEAR:
		return "clear";
	case VK_RETURN:
		return "enter";
	case VK_SHIFT:
		return "shift";
	case VK_CONTROL:
		return "ctrl";
	case VK_MENU:
		return "alt";
	case VK_PAUSE:
		return "[19]";
	case VK_CAPITAL:
		return "caps";
	case VK_SPACE:
		return "space";
	case VK_PRIOR:
		return "pgup";
	case VK_NEXT:
		return "pgdwn";
	case VK_END:
		return "end";
	case VK_HOME:
		return "home";
	case VK_LEFT:
		return "left";
	case VK_UP:
		return "up";
	case VK_RIGHT:
		return "right";
	case VK_DOWN:
		return "down";
	case VK_SELECT:
		return "sel";
	case VK_INSERT:
		return "ins";
	case VK_DELETE:
		return "del";
	case '0':
		return "0";
	case '1':
		return "1";
	case '2':
		return "2";
	case '3':
		return "3";
	case '4':
		return "4";
	case '5':
		return "5";
	case '6':
		return "6";
	case '7':
		return "7";
	case '8':
		return "8";
	case '9':
		return "9";
	case 'A':
		return "a";
	case 'B':
		return "b";
	case 'C':
		return "c";
	case 'D':
		return "d";
	case 'E':
		return "e";
	case 'F':
		return "f";
	case 'G':
		return "g";
	case 'H':
		return "h";
	case 'I':
		return "i";
	case 'J':
		return "j";
	case 'K':
		return "k";
	case 'L':
		return "l";
	case 'M':
		return "m";
	case 'N':
		return "n";
	case 'O':
		return "o";
	case 'P':
		return "p";
	case 'Q':
		return "q";
	case 'R':
		return "r";
	case 'S':
		return "s";
	case 'T':
		return "t";
	case 'U':
		return "u";
	case 'V':
		return "v";
	case 'W':
		return "w";
	case 'X':
		return "x";
	case 'Y':
		return "y";
	case 'Z':
		return "z";
	case VK_LWIN:
		return "lwin";
	case VK_RWIN:
		return "rwin";
	case VK_NUMPAD0:
		return "num 0";
	case VK_NUMPAD1:
		return "num 1";
	case VK_NUMPAD2:
		return "num 2";
	case VK_NUMPAD3:
		return "num 3";
	case VK_NUMPAD4:
		return "num 4";
	case VK_NUMPAD5:
		return "num 5";
	case VK_NUMPAD6:
		return "num 6";
	case VK_NUMPAD7:
		return "num 7";
	case VK_NUMPAD8:
		return "num 8";
	case VK_NUMPAD9:
		return "num 9";
	case VK_MULTIPLY:
		return "*";
	case VK_ADD:
		return "+";
	case VK_SEPARATOR:
		return "|";
	case VK_SUBTRACT:
		return "-";
	case VK_DECIMAL:
		return ".";
	case VK_DIVIDE:
		return "/";
	case VK_F1:
		return "f1";
	case VK_F2:
		return "f2";
	case VK_F3:
		return "f3";
	case VK_F4:
		return "f4";
	case VK_F5:
		return "f5";
	case VK_F6:
		return "f6";
	case VK_F7:
		return "f7";
	case VK_F8:
		return "f8";
	case VK_F9:
		return "f9";
	case VK_F10:
		return "f10";
	case VK_F11:
		return "f11";
	case VK_F12:
		return "f12";
	case VK_NUMLOCK:
		return "num";
	case VK_SCROLL:
		return "brk";
	case VK_LSHIFT:
		return "shift";
	case VK_RSHIFT:
		return "shift";
	case VK_LCONTROL:
		return "ctrl";
	case VK_RCONTROL:
		return "ctrl";
	case VK_LMENU:
		return "alt";
	case VK_RMENU:
		return "alt";
	case VK_OEM_COMMA:
		return "),";
	case VK_OEM_PERIOD:
		return ".";
	case VK_OEM_1:
		return ";";
	case VK_OEM_MINUS:
		return "-";
	case VK_OEM_PLUS:
		return "=";
	case VK_OEM_2:
		return "/";
	case VK_OEM_3:
		return "grave";
	case VK_OEM_4:
		return "[";
	case VK_OEM_5:
		return "\\";
	case VK_OEM_6:
		return "]";
	case VK_OEM_7:
		return "[222]";
	default:
		return "";
	}
}
