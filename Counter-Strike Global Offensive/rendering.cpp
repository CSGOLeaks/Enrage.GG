#include "rendering.hpp"
#include "source.hpp"
#include "i_menu.hpp"
#include "icon_font.hpp"
#include "menu_icons.hpp"

#define POINTERCHK( pointer ) ( pointer  && pointer != 0 && HIWORD( pointer ) )

namespace renderer
{
	bool m_initialized = false;

	void create(IDirect3DDevice9* device)
	{
		ImGui_ImplDX9_CreateDeviceObjects();
		d::create(device);
	}

	void lost(IDirect3DDevice9* device)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		d::lost();
	}

	void run(IDirect3DDevice9* device)
	{
		if (!m_initialized)
		{
			ImGui::CreateContext();
			ImGui_ImplDX9_Init(FindWindowA(sxor("Valve001"), NULL), device);
			menu::init();
			d::create(device);

			m_initialized = true;
		}

		// render
		ImGui_ImplDX9_NewFrame();

		if (d::begin())
		{
			d::end();
			d::render();
		}

		menu::draw(device);

		ImGui::Render();
	}

	void unload()
	{
		ImGui_ImplDX9_Shutdown();
	}
}

namespace d
{
	IDirect3DDevice9* m_device;
	ImDrawList* m_drawlist;
	ImDrawData				m_drawdata;
	bool					m_initialized;

	namespace fonts
	{
		c_font main = c_font();
		c_font small_esp = c_font();
		c_font icons = c_font();
		c_font menu_icons = c_font();
		c_font menu_smaller_icons = c_font();
		c_font menu_tab = c_font();

		IDirect3DTexture9* _texture = nullptr;
		ImFontAtlas			_atlas;
	}

	bool world_to_screen(Vector vOrigin, Vector& vScreen)
	{
		static uintptr_t g_matrix = 0;
		if (g_matrix == 0)
			g_matrix = (DWORD)&csgo.m_engine()->WorldToScreenMatrix() + 0x40;

		const w2smatrix4x4& matrix = *(w2smatrix4x4*)(*(PDWORD)(g_matrix)+988);

		if (!POINTERCHK(matrix))
			return false;

		int ScrW, ScrH;
		csgo.m_engine()->GetScreenSize(ScrW, ScrH);

		float w = matrix[3][0] * vOrigin[0] + matrix[3][1] * vOrigin[1] + matrix[3][2] * vOrigin[2] + matrix[3][3];
		if (w > 0.01)
		{
			float inverseWidth = 1 / w;
			vScreen.x = (ScrW / 2) + (0.5 * ((matrix[0][0] * vOrigin[0] + matrix[0][1] * vOrigin[1] + matrix[0][2] * vOrigin[2] + matrix[0][3]) * inverseWidth) * ScrW + 0.5);
			vScreen.y = (ScrH / 2) - (0.5 * ((matrix[1][0] * vOrigin[0] + matrix[1][1] * vOrigin[1] + matrix[1][2] * vOrigin[2] + matrix[1][3]) * inverseWidth) * ScrH + 0.5);
			vScreen.z = 0;
			return true;
		}
		return false;
	}

	void line(int x1, int y1, int x2, int y2, Color col, float w)
	{
		m_drawlist->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), col, w);
	}

	auto corners = [](ImVec2 left_up, ImVec2 right_up, ImVec2 left_down, ImVec2 right_down, float size, float w, Color col, bool is_outline)
	{
		const auto o = is_outline ? w / 2.f : 0;
		const auto v_size = (right_up.x - left_up.x) * size + o;
		const auto h_size = (left_down.y - left_up.y) * size + o;

		// left up corner
		line(left_up.x - o, left_up.y, left_up.x + v_size, left_up.y, col, w);
		line(left_up.x, left_up.y - o, left_up.x, left_up.y + h_size, col, w);

		// right up corner 
		line(right_up.x + o, right_up.y, right_up.x - v_size, right_up.y, col, w);
		line(right_up.x, right_up.y - o, right_up.x, right_up.y + h_size, col, w);

		// right down corner 
		line(right_down.x + o, right_down.y, right_down.x - v_size, right_down.y, col, w);
		line(right_down.x, right_down.y + o, right_down.x, right_down.y - h_size, col, w);

		// left down corner 
		line(left_down.x - o, left_down.y, left_down.x + v_size, left_down.y, col, w);
		line(left_down.x, left_down.y + o, left_down.x, left_down.y - h_size, col, w);
	};

	void rect(int x, int y, int size, Color col, float w)
	{
		m_drawlist->AddRect(ImVec2(x - size, y - size), ImVec2(x + size, y + size), col, 0, 15, w);
	}

	void rect(int x1, int y1, int x2, int y2, Color col, float w)
	{
		m_drawlist->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), col, 0, 15, w);
	}

	void rect_filled(int x1, int y1, int x2, int y2, Color col)
	{
		m_drawlist->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), col);
	}

	void rect(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color col, float w)
	{
		m_drawlist->AddQuad(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImVec2(x4, y4), col, w);
	}

	void rect_filled(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, Color col)
	{
		m_drawlist->AddQuadFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImVec2(x4, y4), col);
	}

	void circle(int x, int y, int r, int segments, float w, Color col)
	{
		m_drawlist->AddCircle(ImVec2(x, y), r, col, segments, w);
	}

	void circle_filled(int x, int y, int r, int segments, Color col)
	{
		m_drawlist->AddCircleFilled(ImVec2(x, y), r, col, segments);
	}

	void circle_3d(Vector vPosition, float flRadius, Color ccolor, bool bCrosshair)
	{
		static float flStep = M_PI * 2.0f / 40.f;
		Vector vPrev;

		vPosition.z -= 64.f;
		Vector vCenter;
		if (!world_to_screen(vPosition, vCenter))
			return;

		// TODO: Implement m_drawlist->AddLine
		for (float flLat = 0, i = 0; flLat <= M_PI * 2.0f + flStep; flLat += flStep, i++)
		{
			float flSin1 = sin(flLat);
			float flCos1 = cos(flLat);

			float flSin3 = sin(0.0);
			float flCos3 = cos(0.0);

			Vector vPoint1;
			vPoint1 = Vector(flSin1 * flCos3, flCos1, flSin1 * flSin3) * flRadius;

			Vector vPoint3 = vPosition + vPoint1;
			if (world_to_screen(vPoint3, vPoint3))
			{
				if (flLat != 0)
				{
					d::line(vPrev.x, vPrev.y, vPoint3.x, vPoint3.y, ccolor, 2);

					if (bCrosshair && i >= 4)
					{
						i = -1;
						d::line(vCenter.x, vCenter.y, vPoint3.x, vPoint3.y, ccolor, 1);
					}
				}
			}
			else return;

			vPrev = vPoint3;
		}
	}

	void horizontal_line(int x1, int y1, int x2, int y2,
		float min_value, float max_value, float cur_value,
		Color value_col, Color background_col,
		int height)
	{
		// background first
		d::line(x1 - 1, y1, x2 + 1, y2, background_col, height + 1);

		//
		/*max_value -= min_value;
		cur_value -= min_value;*/
		auto a = cur_value - min_value;
		auto b = max_value - min_value;
		auto c = a / b;
		auto d = x1 + ((c * (x2 - x1)));

		// Line now
		d::line(x1, y1, d, y2, value_col, height);
	}

	ImVec2 calc_text_size(ImFont* font, const char* format)
	{
		return font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, format);
	}

	ImVec2 text(ImFont* font, int x, int y, Color color, int flags, const char* format, ...)
	{
		static const auto MAX_BUFFER_SIZE = 1024;
		static char buffer[MAX_BUFFER_SIZE] = "";

		m_drawlist->PushTextureID(fonts::_atlas.TexID);

		va_list va;
		va_start(va, format);
		vsnprintf_s(buffer, MAX_BUFFER_SIZE, format, va);
		va_end(va);

		if (flags & text_render_flag_centered_all) flags = text_render_flag_centered_x | text_render_flag_centered_y;
		auto text_size = font->CalcTextSizeA(font->FontSize, FLT_MAX, 0.0f, buffer);
		if (flags & text_render_flag_centered_x)
			x -= text_size.x / 2;
		if (flags & text_render_flag_centered_y)
			y -= text_size.y / 2;

		/*if (flags & outline) {
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x - 1, point.y - 1 }, 0xFF000000, buffer);
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x + 1, point.y }, 0xFF000000, buffer);
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x    , point.y + 1 }, 0xFF000000, buffer);
		_drawList->AddText(font, font->FontSize, ImVec2{ point.x - 1, point.y }, 0xFF000000, buffer);
		}*/

		//if (flags & text_render_flag_drop_shadow) {
		m_drawlist->AddText(font, font->FontSize, ImVec2(x + 1, y + 1), ImColor(0, 0, 0, color.a()), buffer);
		//}

		m_drawlist->AddText(font, font->FontSize, ImVec2(x, y), color, buffer);
		m_drawlist->PopTextureID();
		return text_size;
	}

	ImVec2 calc_text_size(LPD3DXFONT font, const char* format)
	{
		RECT rcRect = { 0,0,0,0 };
		if (font)
		{
			// calculate required rect
			font->DrawTextA(NULL, format, strlen(format), &rcRect, DT_CALCRECT,
				D3DCOLOR_XRGB(0, 0, 0));
		}

		// return size
		return ImVec2(rcRect.right - rcRect.left, rcRect.bottom - rcRect.top);
	}

	ImVec2 text(ID3DXFont* font, int x, int y, Color col, int flags, const char* format, ...)
	{
		if (font == nullptr)
			return ImVec2(0, 0);

		static const auto MAX_BUFFER_SIZE = 1024;
		static char buffer[MAX_BUFFER_SIZE] = "";

		va_list va;
		va_start(va, format);
		vsnprintf_s(buffer, MAX_BUFFER_SIZE, format, va);
		va_end(va);

		if (flags & text_render_flag_centered_all) flags = text_render_flag_centered_x | text_render_flag_centered_y;

		auto text_size = calc_text_size(font, buffer);

		RECT font_pos = {};
		font_pos.left = x;
		font_pos.top = y;

		if (flags & text_render_flag_centered_x)
			font_pos.left -= text_size.x / 2;
		if (flags & text_render_flag_centered_y)
			font_pos.top -= text_size.y / 2;
		if (flags & text_render_flag_right_x)
			font_pos.left -= text_size.y;

		//if (flags & text_render_flag_drop_shadow) {
		RECT shadow_pos = {};
		shadow_pos.left = font_pos.left + 1;
		shadow_pos.top = font_pos.top + 1;
		font->DrawTextA(0, buffer, strlen(buffer), &shadow_pos, DT_NOCLIP, D3DCOLOR_ARGB(col.a() / 2 + 30, 0, 0, 0));
		//}

		font->DrawTextA(0, buffer, strlen(buffer), &font_pos, DT_NOCLIP, D3DCOLOR_ARGB(col.a(), col.r(), col.g(), col.b()));

		return text_size;
	}

	bool util_get_font_path(const std::string& name, std::string& path)
	{
		char buffer[MAX_PATH];
		HKEY registryKey;

		GetWindowsDirectoryA(buffer, MAX_PATH);
		std::string fontsFolder = buffer + std::string(sxor("\\Fonts\\"));

		if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, sxor("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"), 0, KEY_READ, &registryKey)) {
			return false;
		}

		uint32_t valueIndex = 0;
		char valueName[MAX_PATH];
		uint8_t valueData[MAX_PATH];
		std::wstring wsFontFile;

		do {
			uint32_t valueNameSize = MAX_PATH;
			uint32_t valueDataSize = MAX_PATH;
			uint32_t valueType;

			auto error = RegEnumValueA(
				registryKey,
				valueIndex,
				valueName,
				reinterpret_cast<DWORD*>(&valueNameSize),
				0,
				reinterpret_cast<DWORD*>(&valueType),
				valueData,
				reinterpret_cast<DWORD*>(&valueDataSize));

			valueIndex++;

			if (error == ERROR_NO_MORE_ITEMS) {
				RegCloseKey(registryKey);
				return false;
			}

			if (error || valueType != REG_SZ) {
				continue;
			}

			if (_strnicmp(name.data(), valueName, name.size()) == 0) {
				path = fontsFolder + std::string((char*)valueData, valueDataSize);
				RegCloseKey(registryKey);
				return true;
			}
		} while (true);

		return false;
	}

	void create_font(c_font* font, IDirect3DDevice9* device, std::string name, ImFontConfig* cfg, const ImWchar* ranges, float size, int weight, int quality)
	{
		std::string path;

		if (!util_get_font_path(name, path) || path.empty()) return;

		font->imgui = fonts::_atlas.AddFontFromFileTTF(path.data(), size, cfg, ranges);

		if (FAILED(D3DXCreateFontA(device, (int)size, 0, weight, 0, false, ANSI_CHARSET | RUSSIAN_CHARSET, OUT_CHARACTER_PRECIS, quality, DEFAULT_PITCH, name.c_str(), &font->directx)))
			assert(0);
	}

	bool create(IDirect3DDevice9* device)
	{
		if (!device)
			return false;

		m_device = device;
		m_drawlist = new ImDrawList(ImGui::GetDrawListSharedData());

		// Setup fonts
		ImFontConfig cfg;
		cfg.PixelSnapH = 0;
		cfg.OversampleH = 5;
		cfg.OversampleV = 5;
		cfg.RasterizerFlags = 2;
		cfg.RasterizerMultiply = 1.2f;
		static const ImWchar ranges[] =
		{
			0x0020, 0x00FF, // Basic Latin + Latin Supplement
			0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
			0x2DE0, 0x2DFF, // Cyrillic Extended-A
			0xA640, 0xA69F, // Cyrillic Extended-B
			0xE000, 0xE226, // icons
			0,
		};
		cfg.GlyphRanges = ranges;

		create_font(&fonts::main, device, sxor("Verdana"), &cfg, ranges, 12.f, 400, DRAFT_QUALITY);
		create_font(&fonts::small_esp, device, sxor("Smallest Pixel-7"), &cfg, ranges, 10.f, 100);
		create_font(&fonts::menu_tab, device, sxor("Segoe UI"), &cfg, ranges, 20.f, FW_LIGHT);
		//create_font(&fonts::menu_icons, device, _("eaglefinal_0"), &cfg, ranges, 16.f, 100);
		//fonts::small_esp.imgui = fonts::_atlas.AddFontFromMemoryTTF((void*)small_font_compressed, 25600, 10.f, &cfg, ranges);
		fonts::icons.imgui = fonts::_atlas.AddFontFromMemoryCompressedTTF(icons_font_compressed, icons_font_compressed_size, 14.0f, &cfg, ranges);
		fonts::menu_icons.imgui = fonts::_atlas.AddFontFromMemoryCompressedTTF(eagle_compressed_data, eagle_compressed_size, 32.0f, &cfg, ranges);
		fonts::menu_smaller_icons.imgui = fonts::_atlas.AddFontFromMemoryCompressedTTF(eagle_compressed_data, eagle_compressed_size, 14.0f, &cfg, ranges);
		//fonts::icons = fonts::_atlas.AddFontFromMemoryCompressedBase85TTF(icons_font_compressed, 14.0f, &cfg, ranges);

		// font shit
		uint8_t* pixel_data;
		int width, height, BYTEs_per_pixel;
		fonts::_atlas.GetTexDataAsRGBA32(&pixel_data, &width, &height, &BYTEs_per_pixel);
		auto hr = m_device->CreateTexture(
			width, height,
			1,
			D3DUSAGE_DYNAMIC,
			D3DFMT_A8R8G8B8,
			D3DPOOL_DEFAULT,
			&fonts::_texture,
			NULL);
		if (FAILED(hr)) return false;
		D3DLOCKED_RECT tex_locked_rect;
		if (fonts::_texture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
			return false;
		for (int y = 0; y < height; y++)
			memcpy((uint8_t*)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixel_data + (width * BYTEs_per_pixel) * y, (width * BYTEs_per_pixel));
		fonts::_texture->UnlockRect(0);
		fonts::_atlas.TexID = fonts::_texture;

		m_initialized = true;
		return true;
	}

	bool begin()
	{
		if (!m_initialized)
			return false;

		m_drawdata.Valid = false;
		m_drawlist->Clear();
		m_drawlist->PushClipRectFullScreen();
		return true;
	}

	bool end()
	{
		return true;
	}

	bool render()
	{
		if (!m_drawlist->VtxBuffer.empty()) {
			m_drawdata.Valid = true;
			m_drawdata.CmdLists = &m_drawlist;
			m_drawdata.CmdListsCount = 1;
			m_drawdata.TotalVtxCount = m_drawlist->VtxBuffer.Size;
			m_drawdata.TotalIdxCount = m_drawlist->IdxBuffer.Size;
		}
		ImGui_ImplDX9_RenderDrawLists(&m_drawdata);
		//ImGui::Render();
		//ImGui::EndFrame();

		return true;
	}

	bool lost()
	{
		if (fonts::_texture) { fonts::_texture->Release(); } fonts::_texture = nullptr;
		fonts::_atlas.Clear();
		if (m_drawlist) { delete m_drawlist; } m_drawlist = nullptr;
		//Fonts::watermark = nullptr;
		//Fonts::esp = nullptr;

		return true;
	}
}