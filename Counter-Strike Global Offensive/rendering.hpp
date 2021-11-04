#pragma once

#include "sdk.hpp"

class c_font {
public:
	ImFont* imgui;
	ID3DXFont* directx;

	c_font()
	{
		imgui = nullptr; directx = nullptr;
	}
};

namespace d
{
	namespace fonts
	{
		extern c_font main;
		extern c_font small_esp;
		extern c_font icons;
		extern c_font menu_icons;
		extern c_font menu_smaller_icons;
		extern c_font menu_tab;

		extern IDirect3DTexture9* _texture;
		extern ImFontAtlas			_atlas;
	}

	extern IDirect3DDevice9* m_device;
	extern ImDrawList* m_drawlist;
	extern ImDrawData				m_drawdata;
	extern bool					m_initialized;

	extern void line(int x1, int y1, int x2, int y2, Color col, float w = 1.0f);

	//extern auto corners = [](ImVec2 left_up, ImVec2 right_up, ImVec2 left_down, ImVec2 right_down, float size, float w, Color col, bool is_outline = false);

	extern void rect(int x, int y, int size, Color col, float w = 1.0f);

	extern void rect(int x1, int y1, int x2, int y2, Color col, float w = 1.0f);

	extern void rect_filled(int x1, int y1, int x2, int y2, Color col);

	extern void rect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color col, float w = 1.0f);

	extern void rect_filled(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color col);

	extern void circle(int x, int y, int r, int segments, float w, Color col);

	extern void circle_filled(int x, int y, int r, int segments, Color col);

	extern void circle_3d(Vector vPosition, float flRadius, Color ccolor, bool bCrosshair = false);

	extern void horizontal_line(int x1, int y1, int x2, int y2,
		float min_value, float max_value, float cur_value,
		Color value_col, Color background_col = Color(255, 255, 255),
		int height = 4);

	extern ImVec2 calc_text_size(ImFont* font, const char* format);

	enum text_render_flags
	{
		text_render_flags_empty = 0,
		text_render_flag_centered_x = 1 << 1,
		text_render_flag_centered_y = 1 << 2,
		text_render_flag_centered_all = 1 << 3,
		text_render_flag_right_x = 1 << 4,
		text_render_flag_outline = 1 << 5,
		text_render_flag_drop_shadow = 1 << 6
	};

	extern ImVec2 text(ImFont* font, int x, int y, Color color, int flags, const char* format, ...);

	extern ImVec2 calc_text_size(LPD3DXFONT font, const char* format);

	extern ImVec2 text(ID3DXFont* font, int x, int y, Color col, int flags, const char* format, ...);

	extern void create_font(c_font* font, IDirect3DDevice9* device, std::string name, ImFontConfig* cfg, const ImWchar* ranges, float size, int weight = 500, int quality = DEFAULT_QUALITY);

	extern bool create(IDirect3DDevice9* device);

	extern bool begin();

	extern bool end();

	extern bool render();

	extern bool lost();
}

namespace renderer
{
	extern bool m_initialized;

	extern void create(IDirect3DDevice9* device);
	extern void lost(IDirect3DDevice9* device);
	extern void run(IDirect3DDevice9* device);
	extern void unload();
}