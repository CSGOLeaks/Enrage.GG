#include "tab.h"

static int count{ };
static int selected{ };

tab::tab( std::string name ) {
	this->name = name;
	tabs = count++;
}

void tab::draw( ) {
	if ( !parent )
		return;

	// draw the text
	Drawing::DrawString(F::MenuV2, pos.x + 9, pos.y,
		selected == tabs ? ctx.m_settings.menu_color : menu_colors.tab_unselected, 0, name.c_str());
	
	if ( selected == tabs )
		for ( auto* i : children )
			i->draw( );
}

void tab::think( ) {
	size = Vector2D( 98, 16 );
	pos = parent->pos + Vector2D( 21, 25 + size.y * tabs );

	// hovered
	hovered = g_menuinput.inbounds( pos, pos + size );

	if ( hovered && g_menuinput.is_key_pressed( VK_LBUTTON ) )
		selected = tabs;

	if ( selected == tabs )
		for ( auto* i : children )
			i->think( );
}

void tab::extra( ) {
	this->setup_positions( );

	for ( auto* i : children )
		i->extra( );
}

void tab::on_cfg_update()
{
	for (auto* i : children)
		i->on_cfg_update();
}

void tab::on_unload()
{
	for (auto* i : children)
		i->on_unload();
}

void tab::setup_positions( ) {
	auto used_area_y = 0;
	auto old_size = 0;

	for ( auto *i : children ) {
		auto pos_x = int( parent->pos.x + 141 );
		auto pos_y = int( parent->pos.y + 21 + used_area_y );

		if ( pos_y + i->size.y > parent->pos.y + parent->size.y ) {
			pos_x += i->size.x + 1;
			pos_y -= old_size;
		}
		else
			old_size = int( i->size.y ) + 8;

		i->set_position( Vector2D{static_cast<float>(pos_x), static_cast<float>(pos_y)} );
		used_area_y += i->size.y + 8;
	}
}