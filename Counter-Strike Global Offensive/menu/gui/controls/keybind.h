#pragma once
#include "../../menu/menu_v2.h"

class keybind : public element {
public:
	keybind( std::string name, c_keybind* key, bool can_be_changed, bool* shown );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;

	std::string get_name( int key );
private:
	int focused();
	c_keybind* key;
	std::string key_name;
	bool pressed = false;
	bool opened_settings = false;
	float combobox_size = 0;
	float combo_size_factor = 0;
	bool hovered = false;
	bool can_be_changed;
	std::vector<std::string> elements = { sxor("toggle"), sxor("on key"), sxor("off key"), sxor("always on") };
	bool* shown;
};

// large brain
inline std::string keybind::get_name( int key ) {
	switch( key ) {
	case VK_LBUTTON:
		return "mouse1";
	case VK_RBUTTON:
		return "mouse2";
	case VK_CANCEL:
		return "break";
	case VK_MBUTTON:
		return "mouse3";
	case VK_XBUTTON1:
		return "mouse4";
	case VK_XBUTTON2:
		return "mouse5";
	case VK_BACK:
		return "backspace";
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
		return "capslock";
	case VK_SPACE:
		return "space";
	case VK_PRIOR:
		return "pgup";
	case VK_NEXT:
		return "pgdown";
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
		return "select";
	case VK_INSERT:
		return "insert";
	case VK_DELETE:
		return "delete";
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
		return "left win";
	case VK_RWIN:
		return "right win";
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
		return "num mult";
	case VK_ADD:
		return "num add";
	case VK_SEPARATOR:
		return "|";
	case VK_SUBTRACT:
		return "num sub";
	case VK_DECIMAL:
		return "num decimal";
	case VK_DIVIDE:
		return "num divide";
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
		return "num lock";
	case VK_SCROLL:
		return "break";
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