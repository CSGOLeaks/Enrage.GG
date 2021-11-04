#include "multibox.h"

multibox::multibox( std::string name, bool* enabled, std::vector<std::string> elements, bool* shown ) {
	this->name = name;
	this->enabled = enabled;
	this->elements = elements;
	this->shown = shown;
}

void multibox::draw( ) {
	if ( !*shown )
		return;

	std::string format;
	
	for ( const auto& i : elements_to_add ) {
		const std::size_t extras = format.find( "..." );
		
		if ( i.length() + format.length() <= 23 ) {
			if ( format.length( ) > 0 )
				format += ", ";

			format += i;
		}
		else if ( i.length( ) + format.length( ) > 23 && extras == std::string::npos )
			format.append( "..." );
	}
	
	if ( format.empty( ) )
		format += "none";
	
	// background
	Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + size.y - 19, pos.x + size.x + 1, pos.y + size.y + 1, menu_colors.control_outline);
	Drawing::DrawRectA(pos.x + 21, pos.y + size.y - 18, pos.x + size.x, pos.y + size.y, menu_colors.control_background);

	// selected name
	Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 15, menu_colors.box_text_color, 0, format.c_str());

	// if theres a name, draw the name
	if (!name.empty())
		Drawing::DrawString(F::MenuV2, pos.x + 20, pos.y - 2, menu_colors.text_color, 0, name.c_str());

	multi_size_factor = csgo.m_globals()->frametime * float(elements.size() * elements.size() + 70);

	// opened stuff
	if ( opened ) {
		// triangle (POG)
		Drawing::DrawOutlinedRectA(pos.x + size.x - 10, pos.y + size.y - 8, pos.x + size.x - 5, pos.y + size.y - 7, menu_colors.box_text_color);
		Drawing::DrawOutlinedRectA(pos.x + size.x - 9, pos.y + size.y - 9, pos.x + size.x - 6, pos.y + size.y - 8, menu_colors.box_text_color);
		Drawing::DrawOutlinedRectA(pos.x + size.x - 8, pos.y + size.y - 10, pos.x + size.x - 7, pos.y + size.y - 9, menu_colors.box_text_color);

		multibox_size += multi_size_factor;
		multibox_size = Math::clamp(multibox_size, 0.f, 3.f + 16.f * (int)elements.size( ) );
		
		// background
		Drawing::DrawRectA(pos.x + 21, pos.y + size.y + 3, pos.x + size.x, pos.y + size.y + int(multibox_size), menu_colors.control_background);

		// outline
		Drawing::DrawOutlinedRectA(pos.x + 20, pos.y + size.y + 3, pos.x + size.x + 1, pos.y + size.y + multibox_size, menu_colors.control_outline);
		
		// text
		size_t n{ };
		for ( ; n < elements.size( ); ++n ) {
			const auto seen = 16 * ( n + 1 ) - 8 < multibox_size;

			if ( seen )
				Drawing::DrawString(F::MenuV2n, pos.x + 30, pos.y + size.y - 11 + 16 * ( n + 1 ),
					enabled[ n ] ? ctx.m_settings.menu_color : menu_colors.box_text_color, 0, elements.at(n).c_str());
		}
	}
	else {
		multibox_size = 0;

		// triangle (POG)
		Drawing::DrawOutlinedRectA(pos.x + size.x - 8, pos.y + size.y - 8, pos.x + size.x - 7, pos.y + size.y - 7, menu_colors.box_text_color);
		Drawing::DrawOutlinedRectA(pos.x + size.x - 9, pos.y + size.y - 9, pos.x + size.x - 6, pos.y + size.y - 8, menu_colors.box_text_color);
		Drawing::DrawOutlinedRectA(pos.x + size.x - 10, pos.y + size.y - 10, pos.x + size.x - 5, pos.y + size.y - 9, menu_colors.box_text_color);
	}
}

int multibox::focused( ) {
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

void multibox::think( ) {
	if ( !*shown )
		return;

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
					enabled[ part ] = !enabled[ part ];

					if ( enabled[ part ] )
						elements_to_add.emplace_back( elements.at( part ) );
					else {
						const auto itr = std::find( elements_to_add.begin( ), elements_to_add.end( ), elements.at( part ) );
						if ( itr != elements_to_add.end( ) ) elements_to_add.erase( itr );
					}
				}
				else {
					g_menu.focus( nullptr );
					opened = false;
				}
			}
		}
	}
}

void multibox::extra( ) {
	if ( *shown )
		size = { 174, static_cast<float>( !name.empty( ) ? 34 : 16 ) };
	else
		size = { 174, -8 };
}

void multibox::on_cfg_update()
{
	elements_to_add.clear();

	for (int i = 0; i < (int)elements.size(); i++)
	{
		if (enabled[i])
			elements_to_add.emplace_back(elements.at(i));
		else {
			const auto itr = std::find(elements_to_add.begin(), elements_to_add.end(), elements.at(i));
			if (itr != elements_to_add.end()) elements_to_add.erase(itr);
		}
	}
}

void multibox::on_unload()
{
	//delete this;
}
