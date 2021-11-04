#include "slider.h"

slider::slider( std::string name, int min, int max, int* value, std::wstring addon, bool* shown, int increase_by ) {
	this->name = name;
	this->min = min;
	this->max = max;
	this->increase_by = increase_by;
	this->value = value;
	this->addon = addon;
	this->shown = shown;
}

void slider::draw( ) {
	if ( !*shown )
		return;

	// current value
	std::wstring current_value = { std::to_wstring(int( *value )) + addon };

	const auto addon_size = Drawing::GetTextSize( F::MenuV2, current_value.c_str());
	
	// background
	Drawing::DrawRectGradientVerticalA( pos.x + 21, pos.y + size.y - 6, pos.x + size.x, pos.y + size.y, menu_colors.slider_gradient_top, menu_colors.slider_gradient_bottom );

	// slider pos
	Drawing::DrawRectA( pos.x + 21, pos.y + size.y - 6, pos.x + slider_pos + 18, pos.y + size.y, ctx.m_settings.menu_color );
	Drawing::DrawRectGradientVerticalA(pos.x + 21, pos.y + size.y - 6, pos.x + slider_pos + 18, pos.y + size.y, Color( 0, 0, 0, 0 ), Color( 0, 0, 0, menu_colors.slider_checkbox_gradient ) );

	// if theres a name, draw the name
	if ( !name.empty( ) )
		Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 2, menu_colors.text_color, 0, name.c_str());

	// border
	Drawing::DrawOutlinedRectA( pos.x + 20, pos.y + size.y - 7, pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline );
	
	// current value
	Drawing::DrawStringUnicode(F::MenuV2, pos.x + slider_pos + 19 - addon_size.right / 2, pos.y + size.y - 6, menu_colors.slider_addon_color, 0, current_value.c_str());
}

void slider::think() {
	if ( !*shown )
		return;

	// hovered
	hovered = g_menuinput.inbounds( pos + Vector2D( 20, size.y - 7 ), pos + size );

	if ( hovered && g_menuinput.is_key_pressed( 1 ) && size.y > 3 )
		dragging = true;
	else if ( g_menuinput.is_key_released( 1 ) )
		dragging = false;

	if ( dragging ) {
		const auto drag_x = std::clamp<int>( int(g_menuinput.get_pos( ).x - int(pos.x + 16)), 0, int(size.x - 18));
		*value = min + ( max - min ) * ( drag_x / ( size.x - 18 ) );
	}

	*value = std::clamp( *value, min, max ) + increase_by / 2;
	*value -= int( *value ) % increase_by;
}

void slider::extra() {
	if ( *shown )
		size = { 174, static_cast<float>( !name.empty( ) ? 22 : 4 ) };
	else
		size = { 174, -8 };

	slider_pos = ( size.x - 18 ) * ( *value - min ) / ( max - min );
}

void slider::on_cfg_update()
{
}

void slider::on_unload()
{
	//delete this;
}
