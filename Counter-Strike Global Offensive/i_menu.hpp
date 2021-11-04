#pragma once
#include "sdk.hpp"
#include "parser.hpp"

namespace menu
{
	extern LPDIRECT3DTEXTURE9 flag_ewropi;
	extern LPDIRECT3DTEXTURE9 m_tplayer_with_glow;
	extern LPDIRECT3DTEXTURE9 m_tplayer_no_glow;
	extern IDirect3DDevice9* m_device;

	extern int category;
	extern int new_category;


	extern void init(const float& alpha = 1.f);

	extern void skins_listbox(const char* label, std::string filter, int selected_rarity, int* current_item, skin_list_t& skins, int height_in_items, const float& alpha);

	extern void draw(IDirect3DDevice9* device);

}