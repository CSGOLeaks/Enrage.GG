#include "combobox.h"

combobox::combobox(std::string name, int* value, std::vector<std::string> elements, bool* shown) {
	this->name = name;
	this->value = value;
	this->t_elements = nullptr;
	this->elements = elements;
	this->shown = shown;
}

combobox::combobox(std::string name, int* value, std::vector<std::string> *elements, bool* shown) {
	this->name = name;
	this->value = value;
	this->t_elements = elements;
	this->elements = std::vector<std::string>{"error", "parsing in process", "or failed"};
	this->shown = shown;
}

void combobox::draw() {
	if ( !*shown )
		return;

	if (t_elements != nullptr && t_elements->size() > 0 && (elements.size() != t_elements->size() || t_elements->size() > 0 && elements[0] != t_elements->at(0)) )
	{
		elements.clear();
		auto elem = *t_elements;
		for (auto el : elem)
		{
			elements.push_back(el);
		}

		*value = Math::clamp(*value, 0, (int)t_elements->size() - 1);
	}
	else
		*value = Math::clamp(*value, 0, (int)elements.size() - 1);

	// background
	Drawing::DrawOutlinedRectA( pos.x + 20, pos.y + size.y - 19, pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline );
	Drawing::DrawRectA(pos.x + 21, pos.y + size.y - 18, pos.x + size.x, pos.y + size.y, menu_colors.control_background );

	// selected name
	Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 15, menu_colors.box_text_color, 0, elements.at(*value).c_str());
	
	// if theres a name, draw the name
	if ( !name.empty( ) )
		Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 2, menu_colors.text_color, 0, name.c_str());

	combo_size_factor = csgo.m_globals()->frametime * float( elements.size( ) * elements.size( ) + 70 );

	// opened stuff
	if ( opened ) {
		// triangle (POG)
		Drawing::DrawOutlinedRectA(pos.x + size.x - 10, pos.y + size.y - 8, pos.x + size.x - 5, pos.y + size.y - 7, menu_colors.box_text_color );
		Drawing::DrawOutlinedRectA(pos.x + size.x - 9, pos.y + size.y - 9, pos.x + size.x - 6, pos.y + size.y - 8, menu_colors.box_text_color );
		Drawing::DrawOutlinedRectA(pos.x + size.x - 8, pos.y + size.y - 10, pos.x + size.x - 7, pos.y + size.y - 9, menu_colors.box_text_color );
		
		combobox_size += combo_size_factor;	
		combobox_size = Math::clamp( combobox_size, 0.f, 3.f + 16.f * (int)elements.size( ) );

		// background
		Drawing::DrawRectA(pos.x + 21, pos.y + size.y + 3, pos.x + size.x, pos.y + size.y + int( combobox_size ), menu_colors.control_background );

		// outline
		Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + size.y + 3, pos.x + size.x + 1, pos.y + size.y + combobox_size, menu_colors.control_outline );
		
		// text
		size_t n{ };
		for ( ; n < elements.size( ); ++n ) {
			const auto selected = n == *value;
			const auto seen = 16 * ( n + 1 ) - 8 < combobox_size;
			
			if ( seen )
				Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 11 + 16 * ( n + 1 ),
					selected ? ctx.m_settings.menu_color : menu_colors.box_text_color, 0 , elements.at(n).c_str());
		}
	}
	else {
		combobox_size = 0;
		
		// triangle (POG)
		Drawing::DrawOutlinedRectA( pos.x + size.x - 8, pos.y + size.y - 8, pos.x + size.x - 7, pos.y + size.y - 7, menu_colors.box_text_color );
		Drawing::DrawOutlinedRectA( pos.x + size.x - 9, pos.y + size.y - 9, pos.x + size.x - 6, pos.y + size.y - 8, menu_colors.box_text_color );
		Drawing::DrawOutlinedRectA( pos.x + size.x - 10, pos.y + size.y - 10, pos.x + size.x - 5, pos.y + size.y - 9, menu_colors.box_text_color );
	}
}

int combobox::focused( ) {
	static const auto original_pos = pos;

	for ( auto n = 0; n < elements.size( ); ++n ) {
		pos.y += 16;

		// catch the bag (index)
		if ( g_menuinput.inbounds( Vector2D( pos.x + 21, pos.y + size.y - 16 ), pos + Vector2D( size.x, size.y + 3 ) ) ) {
			pos = original_pos;
			return n;
		}
	}

	pos = original_pos;
	
	return -1;
}

void combobox::think() {
	if ( !*shown )
		return;

	*value = Math::clamp(*value, 0, (int)elements.size() - 1);

	// hovered
	hovered = g_menuinput.inbounds( pos + Vector2D( 21, size.y - 18 ), pos + size );

	if ( g_menuinput.is_key_pressed( VK_LBUTTON ) ) {
		if ( hovered && !opened && size.y > 3 ) { // hovered and not opened
			g_menu.focus( this ); // focus this!
			opened = true;
		}
		else if ( hovered && opened ) { // hovered and opened
			g_menu.focus( nullptr ); // unfocus!
			opened = false;
		}
		else {
			if ( opened ) {
				const auto part = focused( );
				if ( part != -1 ) {
					*value = part;
					opened = false;
					g_menu.focus( nullptr );
				}
				else {
					g_menu.focus( nullptr );
					opened = false;
				}
			}
		}
	}
}

void combobox::extra() {
	if ( *shown )
		size = { 174, static_cast<float>( !name.empty( ) ? 34 : 16 ) };
	else
		size = { 174, -8 };
}

void combobox::on_cfg_update()
{
}

void combobox::on_unload()
{
	//delete this;
}
