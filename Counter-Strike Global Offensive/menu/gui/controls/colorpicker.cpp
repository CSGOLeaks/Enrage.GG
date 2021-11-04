#include "colorpicker.h"

colorpicker::colorpicker(std::string name, Color* colour, bool show_alpha, bool* shown) {
	this->name = name;
	this->colour = colour;
	this->shown = shown;
	this->allow_alpha = show_alpha;
	last_color_pos = Vector2D::Zero;
}

void colorpicker::draw() {
	if ( !*shown )
		return;

	Color picker_col = *colour;
	Drawing::DrawRectA( pos.x + size.x - 18 , pos.y + size.y - 6 , pos.x + size.x, pos.y + size.y, picker_col.alpha( menu_colors.menu_alpha ) );
	Drawing::DrawRectGradientVerticalA( pos.x + size.x - 19, pos.y + size.y - 7, pos.x + size.x + 1, pos.y + size.y, Color( 0, 0, 0, 0 ), Color( 0, 0, 0, menu_colors.color_picker_gradient ) );
	//Drawing::DrawOutlinedRectA( pos.x + size.x - 19, pos.y + size.y - 7 , pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline );
	Drawing::DrawOutlinedRectA( pos.x + size.x - 19, pos.y + size.y - 7 , pos.x + size.x + 1, pos.y + size.y + 1, Color(80, 80, 80, 100));

	if ( !name.empty() )
		Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 2, menu_colors.text_color, 0 , name.c_str());

	if ( opened ) 
	{
		Drawing::DrawRectA(picker_start_pos.x, picker_start_pos.y + 130, picker_start_pos.x + 130, picker_start_pos.y + (allow_alpha ? 147 : 135), menu_colors.window_outline_dark_gray.alpha(menu_colors.menu_alpha));

		Drawing::DrawOutlinedRectA(picker_start_pos.x - 5, picker_start_pos.y - 5, picker_start_pos.x + 135, picker_start_pos.y + (allow_alpha ? 147 : 135), menu_colors.window_outline_black);
		Drawing::DrawOutlinedRectA(picker_start_pos.x - 4, picker_start_pos.y - 4, picker_start_pos.x + 134, picker_start_pos.y + (allow_alpha ? 147 : 135) - 1, menu_colors.window_outline_light_gray);
		Drawing::DrawOutlinedRectA(picker_start_pos.x - 3, picker_start_pos.y - 3, picker_start_pos.x + 133, picker_start_pos.y + (allow_alpha ? 147 : 135) - 2, menu_colors.window_outline_dark_gray);
		Drawing::DrawOutlinedRectA(picker_start_pos.x - 2, picker_start_pos.y - 2, picker_start_pos.x + 132, picker_start_pos.y + (allow_alpha ? 147 : 135) - 3, menu_colors.window_outline_dark_gray);
		Drawing::DrawOutlinedRectA(picker_start_pos.x - 1, picker_start_pos.y - 1, picker_start_pos.x + 131, picker_start_pos.y + (allow_alpha ? 147 : 135) - 4, menu_colors.window_outline_dark_gray);

		Drawing::ColorPicker( picker_start_pos, 130, 130, menu_colors.menu_alpha);
		Drawing::DrawOutlinedRectA( picker_start_pos.x, picker_start_pos.y, picker_start_pos.x + 130, picker_start_pos.y + 130, menu_colors.window_outline_light_gray);

		if (allow_alpha)
		{
			Drawing::DrawRectGradientVerticalA(picker_start_pos.x + 1, picker_start_pos.y + 135, picker_start_pos.x + 129, picker_start_pos.y + 140, menu_colors.slider_gradient_top, menu_colors.slider_gradient_bottom);

			const auto slider_pos = 129.f * (int(colour->a()) / 255.f);
			std::wstring current_value = { std::to_wstring(int(colour->a())) };
			const auto addon_size = Drawing::GetTextSize(F::MenuV2, current_value.c_str());

			Drawing::DrawRectA(picker_start_pos.x + 1, picker_start_pos.y + 135, picker_start_pos.x + slider_pos, picker_start_pos.y + 140, colour->alpha(menu_colors.menu_alpha));
			Drawing::DrawRectGradientVerticalA(picker_start_pos.x + 1, picker_start_pos.y + 135, picker_start_pos.x + slider_pos, picker_start_pos.y + 140, Color(0, 0, 0, 0), Color(0, 0, 0, menu_colors.slider_checkbox_gradient));

			Drawing::DrawOutlinedRectA(picker_start_pos.x, picker_start_pos.y + 134, picker_start_pos.x + 130, picker_start_pos.y + 141, menu_colors.control_outline);

			const auto text_pos_x = Math::clamp(picker_start_pos.x + slider_pos - addon_size.right / 2, picker_start_pos.x, picker_start_pos.x + 129 - addon_size.right);

			Drawing::DrawStringUnicode(F::MenuV2, text_pos_x, picker_start_pos.y + 134, menu_colors.slider_addon_color, 0, current_value.c_str());
		}

		auto invalidate_pos = last_color_pos.IsZero()
			|| last_color_pos.x < picker_start_pos.x
			|| last_color_pos.x > (picker_start_pos.x + 130)
			|| last_color_pos.y < picker_start_pos.y
			|| last_color_pos.y > (picker_start_pos.y + 130);

		if (!invalidate_pos)
		{
			Drawing::DrawOutlinedRect(last_color_pos.x-2, last_color_pos.y-2, 5, 5, Color::Black(180));
			Drawing::DrawOutlinedRect(last_color_pos.x-1, last_color_pos.y-1, 3, 3, Color::White(180));
		}
		else
		{
			float paste[] = { colour->ToHSV().x, colour->Brightness() };

			last_color_pos = { Math::clamp(picker_start_pos.x + ((1.f - paste[1]) * 130), picker_start_pos.x, picker_start_pos.x + 130),
						Math::clamp(picker_start_pos.y + ((paste[0] / 360.f) * 130), picker_start_pos.y, picker_start_pos.y + 130) };
		}
	}
}

void colorpicker::think() {
	if (!*shown)
		return;

	// hovered
	hovered = g_menuinput.inbounds(pos + size - Vector2D(19, 7), pos + size + Vector2D(1, 1));

	if (hovered && g_menuinput.is_key_pressed(VK_LBUTTON)) {
		opened = !opened;
		g_menu.focus(opened ? this : nullptr);
	}
	/*else if ( g_menuinput.is_key_released( VK_LBUTTON ) ) {
		g_menu.focus( nullptr );
		opened = false;
	}*/

	if (opened && g_menuinput.is_key_pressed(VK_LBUTTON) && !hovered && !g_menuinput.inbounds(picker_start_pos, picker_start_pos + Vector2D(150, allow_alpha ? 147 : 135))) {
		g_menu.focus(nullptr);
		opened = false;
	}

	if (opened && g_menuinput.is_key_down(VK_LBUTTON) && g_menuinput.inbounds(picker_start_pos, picker_start_pos + Vector2D(130, 130))) {
		const auto h = (g_menuinput.get_pos().y - picker_start_pos.y) * (1.0f / 130);
		const auto s = 1.f;
		const auto l = 1.0f - (g_menuinput.get_pos().x - picker_start_pos.x) * (1.0f / 130);
		static Color temp = Color(0, 0, 0);

		last_color_pos = { Math::clamp(g_menuinput.get_pos().x, picker_start_pos.x, picker_start_pos.x + 130) , Math::clamp(g_menuinput.get_pos().y, picker_start_pos.y, picker_start_pos.y + 130) };

		const auto prev_alpha = colour->a();

		*colour = temp.FromHSL(h, s, l).alpha(prev_alpha);
	}

	// hovered
	if (opened && allow_alpha) {
		hovered_alpha = g_menuinput.inbounds(picker_start_pos + Vector2D(0, 135), picker_start_pos + Vector2D(130, 140));

		if (hovered_alpha && g_menuinput.is_key_pressed(1))
			dragging_alpha = true;
		else if (g_menuinput.is_key_released(1))
			dragging_alpha = false;

		if (dragging_alpha) {
			const auto drag_x = Math::clamp(int(g_menuinput.get_pos().x - (picker_start_pos.x + 1)), 0, 130);

			colour->_a() = int(255 * float(drag_x / 130.f));
		}

		colour->_a() = Math::clamp(colour->a(), 0, 255);
	}
	else
		hovered_alpha = false;
}

void colorpicker::extra() {
	if ( *shown )
		this->size = { 174, static_cast<float>( !name.empty( ) ? 7 : -8 ) };
	else
		this->size = { 174, -8 };

	picker_start_pos = pos + Vector2D( size.x - 14, size.y + 8 );
}

void colorpicker::on_cfg_update()
{
	float paste[] = { colour->ToHSV().x, colour->Brightness() };

	last_color_pos = { Math::clamp(picker_start_pos.x + ((1.f - paste[1]) * 130), picker_start_pos.x, picker_start_pos.x + 130),
				Math::clamp(picker_start_pos.y + ((paste[0] / 360.f) * 130), picker_start_pos.y, picker_start_pos.y + 130) };
}

void colorpicker::on_unload()
{
	//delete this;
}
