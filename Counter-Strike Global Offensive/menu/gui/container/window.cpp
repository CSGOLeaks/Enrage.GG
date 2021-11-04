#include "window.h"
#include "../../../sdk.hpp"

window::window(const Vector2D pos, const Vector2D size ) {
	this->pos = pos;
	this->size = size;
}

void window::draw( ) {
	// outline
	Drawing::DrawOutlinedRectA( pos.x, pos.y, pos.x + size.x, pos.y + size.y, menu_colors.window_outline_black );
	Drawing::DrawOutlinedRectA( pos.x + 1, pos.y + 1, pos.x + size.x - 1, pos.y + size.y - 1, menu_colors.window_outline_light_gray );
	Drawing::DrawOutlinedRectA(pos.x + 2, pos.y + 2, pos.x + size.x - 2, pos.y + size.y - 2, menu_colors.window_outline_dark_gray );
	Drawing::DrawOutlinedRectA(pos.x + 3, pos.y + 3, pos.x + size.x - 3, pos.y + size.y - 3, menu_colors.window_outline_dark_gray );
	Drawing::DrawOutlinedRectA(pos.x + 4, pos.y + 4, pos.x + size.x - 4, pos.y + size.y - 4, menu_colors.window_outline_dark_gray );
	
	// background
	Drawing::DrawRectA( pos.x + 6, pos.y + 6, pos.x + size.x - 6, pos.y + size.y - 6, menu_colors.window_background );
	Drawing::DrawOutlinedRectA(pos.x + 5, pos.y + 5, pos.x + size.x - 5, pos.y + size.y - 5, menu_colors.window_outline_light_gray );
	
	// tab spot
	Drawing::DrawRectA( pos.x + 22, pos.y + 22, pos.x + 118, pos.y + size.y - 22, menu_colors.field_background );
	Drawing::DrawOutlinedRectA(pos.x + 21, pos.y + 21, pos.x + 119, pos.y + size.y - 21, menu_colors.field_outline );
	Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + 20, pos.x + 120, pos.y + size.y - 20, menu_colors.field_outer_outline );
	
	// section spot
	Drawing::DrawRectA( pos.x + 142, pos.y + 22, pos.x + size.x - 22, pos.y + size.y - 22, menu_colors.field_background );
	Drawing::DrawOutlinedRectA( pos.x + 141, pos.y + 21, pos.x + size.x - 21, pos.y + size.y - 21, menu_colors.field_outline );
	Drawing::DrawOutlinedRectA(pos.x + 140, pos.y + 20, pos.x + size.x - 20, pos.y + size.y - 20, menu_colors.field_outer_outline );

#ifdef AUTH
	static std::string date = std::string(__DATE__ + std::string(" | ") + std::string(ctx.data->username));
#else
	static char username[64] = { 'L','3','D','4','5','1','R','7' };

	static std::string date = std::string(__DATE__ + std::string(" | ") + std::string(username));
#endif // AUTH
	
	static const std::wstring name = std::wstring( date.begin( ), date.end( ) );

	const auto text_size = Drawing::GetTextSize( F::MenuV2, std::string( name.begin( ), name.end( ) ).c_str());
	
	// text in bottom right
	Drawing::DrawStringUnicode(F::MenuV2, pos.x + size.x - ( 25 + text_size.right ), pos.y + size.y - ( 23 + text_size.bottom ), menu_colors.text_color, 0, name.c_str());

	for ( auto* i : children )
		i->draw( );
}

void window::think( ) {
	if ( g_menuinput.inbounds( pos - Vector2D( 1, 1 ), pos + size ) 
		&& !g_menuinput.inbounds( pos + Vector2D( 10, 10 ), pos + size - Vector2D( 10, 10 ) ) 
		&& g_menuinput.is_key_pressed( VK_LBUTTON ) )
		dragging = true;
	else if ( g_menuinput.is_key_released( VK_LBUTTON ) )
		dragging = false;

	// old cursor pos
	static auto old_cursor{ g_menuinput.get_pos( ) };

	if ( dragging )
		this->pos = g_menuinput.get_pos( ) - old_cursor;
	else
		old_cursor = g_menuinput.get_pos( ) - this->pos;

	for ( auto* i : children )
		i->think( );
}

void window::extra( ) {
	for ( auto* i : children )
		i->extra( );
}

void window::on_cfg_update()
{
	for (auto* i : children)
		i->on_cfg_update();
}

void window::on_unload()
{
	for (auto* i : children)
		i->on_unload();
}
