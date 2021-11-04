#include "button.h"

button::button(std::string name, std::function< void() > callback, bool* shown, bool ask_for_confirmation) {
	this->name = name;
	this->callback = callback;
	this->shown = shown;
	this->should_confirm = ask_for_confirmation;

	this->hovered = false;
	this->last_time_pressed = 0.f;
	this->show_confirm_msg = false;
}

void button::draw() {
	if ( !*shown )
		return;

	const auto text_size = Drawing::GetTextSize( F::MenuV2, name.c_str() );
	
	const auto ppos = pos + Vector2D(21, size.y - 18);

	// background
	Drawing::DrawOutlinedRectA(ppos.x - 1, ppos.y - 1, pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline );
	Drawing::DrawRectA(ppos.x, ppos.y, pos.x + size.x, pos.y + size.y, menu_colors.control_background );

	if (should_confirm && show_confirm_msg && (csgo.m_globals()->realtime - last_time_pressed) > 1.f)
		show_confirm_msg = false;

	// selected name
	if (!show_confirm_msg) {
		Drawing::DrawString(F::MenuV2n, pos.x + 20 + 154 / 2 - text_size.right / 2, pos.y + size.y - 15, menu_colors.text_color, 0, name.c_str());
	}
	else
	{
		Drawing::DrawString(F::MenuV2n, pos.x + 20 + 154 / 2, pos.y + size.y - 15, menu_colors.text_color.malpha(1.f - Math::clamp(csgo.m_globals()->realtime - last_time_pressed, 0.f, 1.f)), FONT_CENTER, sxor("press again to confirm."));
		Drawing::DrawRectA(ppos.x, ppos.y + size.y - 1, pos.x + (size.x * (1.f - Math::clamp(csgo.m_globals()->realtime - last_time_pressed, 0.f, 1.f))), pos.y + size.y - 1, ctx.m_settings.menu_color);
	}
}

void button::think() {
	if ( !*shown )
		return;

	// hovered
	hovered = g_menuinput.inbounds( pos + Vector2D( 21, size.y - 18 ), pos + size );

	if (hovered && g_menuinput.is_key_pressed(VK_LBUTTON)) {
		const auto previous_time = last_time_pressed;

		last_time_pressed = csgo.m_globals()->realtime;

		if (!should_confirm)
			this->callback();
		else
		{
			auto time_delta = (csgo.m_globals()->realtime - previous_time);

			if (time_delta > 0.0f && time_delta <= 1.0f) {
				this->callback();
				show_confirm_msg = false;
				last_time_pressed = 0.f;
			}
			else
				show_confirm_msg = true;
		}
	}
}

void button::extra() {
	if ( *shown )
		size = { 174, 16 };
	else
		size = { 174, -8 };
}

void button::on_cfg_update()
{
}

void button::on_unload()
{
	//delete this;
}
