#include "checkbox.h"

checkbox::checkbox(std::string name, bool* value, bool* shown) {
	this->name = name;
	this->value = value;
	this->shown = shown;
}

void checkbox::draw( ) {
	if ( !*shown )
		return;

	// background
	Drawing::DrawOutlinedRectA( pos.x, pos.y, pos.x + 8, pos.y + 8, menu_colors.control_outline );
	Drawing::DrawRectGradientVerticalA( pos.x + 1, pos.y + 1, pos.x + 7, pos.y + 7, menu_colors.checkbox_gradient_top, menu_colors.checkbox_gradient_bottom );

	// selected
	if ( *value ) {
		Drawing::DrawRectA(pos.x + 1, pos.y + 1, pos.x + 7, pos.y + 7, ctx.m_settings.menu_color );
		Drawing::DrawRectGradientVerticalA( pos.x + 1, pos.y + 1, pos.x + 7, pos.y + 7, Color( 0, 0, 0, 0 ), Color( 0, 0, 0, menu_colors.slider_checkbox_gradient ) );
	}

	// text
	Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 3, menu_colors.text_color, 0, name.c_str());
}

void checkbox::think( ) {
	if ( !*shown )
		return;

	// hovered
	hovered = g_menuinput.inbounds( pos, pos + size );

	if ( hovered && g_menuinput.is_key_pressed( VK_LBUTTON ) && size.y > 3 )
		*value = !*value;
}

void checkbox::extra() {
	const auto text_size = Drawing::GetTextSize( F::MenuV2, name.c_str());
	
	if ( *shown )
		this->size = { 25 + float(text_size.right), 7 };
	else
		this->size = { 25 + float(text_size.right), -8 };
}

void checkbox::on_cfg_update()
{
}

void checkbox::on_unload()
{
	//delete this;
}

blank::blank(bool* shown) {
	this->name = "blank_shit";
	this->value = nullptr;
	this->shown = shown;
}

void blank::draw() {
}

void blank::think() {
}

void blank::extra() {
	const auto text_size = Drawing::GetTextSize(F::MenuV2, name.c_str());

	if (shown == nullptr || *shown)
		this->size = { 25 + float(text_size.right), 7 };
	else
		this->size = { 25 + float(text_size.right), -8 };
}

void blank::on_cfg_update()
{
}

void blank::on_unload()
{
	//delete this;
}
