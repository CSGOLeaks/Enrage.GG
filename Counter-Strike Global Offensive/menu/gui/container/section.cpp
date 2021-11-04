#include "section.h"

section::section( ) {  };

void section::draw( ) {
	if ( !parent )
		return;

	for ( auto* i : children )
		i->draw( );
}

void section::think( ) {
	size = Vector2D( ( parent->parent->size.x - 162 ) / 2, parent->parent->size.y - 42 );
	
	for ( auto* i : children )
		i->think( );
}

void section::extra( ) {
	this->setup_positions( );

	for ( auto* i : children )
		i->extra( );
}

void section::on_cfg_update()
{
	for (auto* i : children)
		i->on_cfg_update();
}

void section::on_unload()
{
	for (auto* i : children) {

		if ((DWORD)i < 0x100)
			break;

		i->on_unload();
		//delete i;
		//continue;
	}
}

void section::setup_positions() {
	auto used_area = 0;

	for ( auto *i : children ) {
		const auto pos_x = int( pos.x ) + 19;
		const auto pos_y = int( pos.y ) + 19 + used_area;

		i->set_position(Vector2D{static_cast<float>(pos_x), static_cast<float>(pos_y)} );

		used_area += int( i->size.y ) + 8;
	}
}

// checkbox
void section::add_checkbox( std::string name, bool* value ) { this->add( new checkbox( name, value, &g_settings.shown ) ); }
void section::add_checkbox( std::string name, bool* value, bool* shown ) { this->add( new checkbox( name, value, shown ) ); }

void section::add_blank(bool* shown) { this->add(new blank(shown)); }

// slider
void section::add_slider( std::string name, int min, int max, int* value, std::wstring addon ) { this->add( new slider( name, min, max, value, addon, &g_settings.shown, 1 ) ); }
void section::add_slider( std::string name, int min, int max, int* value, std::wstring addon, bool* shown ) { this->add( new slider( name, min, max, value, addon, shown, 1 ) ); }

void section::add_slider( std::string name, int min, int max, int* value, std::wstring addon, int increase_by ) { this->add( new slider( name, min, max, value, addon, &g_settings.shown, increase_by ) ); }
void section::add_slider( std::string name, int min, int max, int* value, std::wstring addon, bool* shown, int increase_by ) { this->add( new slider( name, min, max, value, addon, shown, increase_by ) ); }

// combobox
void section::add_combobox( std::string name, int* value, std::vector<std::string> elements ) { this->add( new combobox( name, value, elements, &g_settings.shown ) ); }
void section::add_combobox( std::string name, int* value, std::vector<std::string> elements, bool* shown ) { this->add( new combobox( name, value, elements, shown ) ); }

void section::add_combobox(std::string name, int* value, std::vector<std::string> *elements) { this->add(new combobox(name, value, elements, &g_settings.shown)); }

// multibox
void section::add_multibox( std::string name, bool* enabled, std::vector<std::string> elements ) { this->add( new multibox( name, enabled, elements, &g_settings.shown ) ); }
void section::add_multibox( std::string name, bool* enabled, std::vector<std::string> elements, bool* shown ) { this->add( new multibox( name, enabled, elements, shown ) ); }

// button
void section::add_button( std::string name, std::function< void() > callback) { this->add( new button( name, callback, &g_settings.shown ) ); }
void section::add_button( std::string name, std::function< void() > callback, bool ask_for_confirmation) { this->add( new button( name, callback, &g_settings.shown, ask_for_confirmation ) ); }
void section::add_button( std::string name, std::function< void() > callback, bool* shown ) { this->add( new button( name, callback, shown ) ); }

// keybind
void section::add_keybind( std::string name, c_keybind* key ) { this->add( new keybind( name, key, true, &g_settings.shown ) ); }
void section::add_keybind( std::string name, c_keybind* key, bool* shown ) { this->add( new keybind( name, key, true, shown ) ); }
// keybind
void section::add_keybind(std::string name, c_keybind* key, bool can_be_changed) { this->add(new keybind(name, key, can_be_changed, &g_settings.shown)); }
void section::add_keybind(std::string name, c_keybind* key, bool can_be_changed, bool* shown) { this->add(new keybind(name, key, can_be_changed, shown)); }

// color picker
void section::add_colorpicker( std::string name, Color* colour, bool allow_alpha ) { this->add( new colorpicker( name, colour, allow_alpha, &g_settings.shown) ); }
void section::add_colorpicker( std::string name, Color* colour, bool* shown, bool allow_alpha) { this->add( new colorpicker( name, colour, allow_alpha, shown) ); }