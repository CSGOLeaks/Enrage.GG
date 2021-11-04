#pragma once
#include "../../menu/menu_v2.h"

class section : public element {
public:
	section( );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
	void setup_positions( );

	void add_checkbox( std::string name, bool* value );
	void add_checkbox( std::string name, bool* value, bool* shown );

	void add_blank(bool* shown = nullptr);

	void add_slider( std::string name, int min, int max, int* value, std::wstring addon );
	void add_slider( std::string name, int min, int max, int* value, std::wstring addon, bool* shown );
	void add_slider( std::string name, int min, int max, int* value, std::wstring addon, int increase_by );
	void add_slider( std::string name, int min, int max, int* value, std::wstring addon, bool* shown, int increase_by );

	void add_combobox( std::string name, int* value, std::vector< std::string > elements );
	void add_combobox( std::string name, int* value, std::vector< std::string > elements, bool* shown );

	void add_combobox(std::string name, int* value, std::vector<std::string>* elements);

	void add_multibox( std::string name, bool* enabled, std::vector< std::string > elements );
	void add_multibox( std::string name, bool* enabled, std::vector< std::string > elements, bool* shown );

	void add_button( std::string name, std::function< void() >);
	void add_button(std::string name, std::function<void()> callback, bool ask_for_confirmation);
	void add_button( std::string name, std::function< void() >, bool* shown );

	void add_keybind( std::string name, c_keybind* key );
	void add_keybind( std::string name, c_keybind* key, bool* shown );
	void add_keybind( std::string name, c_keybind* key, bool can_be_changed);
	void add_keybind( std::string name, c_keybind* key, bool can_be_changed, bool* shown );

	void add_colorpicker( std::string name, Color* colour, bool allow_alpha = true);
	void add_colorpicker( std::string name, Color* colour, bool* shown, bool allow_alpha = true);
};