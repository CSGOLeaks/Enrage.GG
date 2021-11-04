#include "horizon.hpp"
#include "i_menu.hpp"
#include "parser.hpp"
#include "source.hpp"
//#include "background_img.hpp"
//#include "imgui_extension.hpp"
#include "rendering.hpp"
#include "menu.hpp"
#include "misc.hpp"
#include "menu/setup/settings.h"
#include "rage_aimbot.hpp"

#pragma region "Key"
inline std::string get_kname(int key) {
	switch (key) {
	case VK_LBUTTON:
		return "mouse1";
	case VK_RBUTTON:
		return "mouse2";
	case VK_CANCEL:
		return "break";
	case VK_MBUTTON:
		return "mouse3";
	case VK_XBUTTON1:
		return "mouse4";
	case VK_XBUTTON2:
		return "mouse5";
	case VK_BACK:
		return "backspace";
	case VK_TAB:
		return "tab";
	case VK_CLEAR:
		return "clear";
	case VK_RETURN:
		return "enter";
	case VK_SHIFT:
		return "shift";
	case VK_CONTROL:
		return "ctrl";
	case VK_MENU:
		return "alt";
	case VK_PAUSE:
		return "[19]";
	case VK_CAPITAL:
		return "capslock";
	case VK_SPACE:
		return "space";
	case VK_PRIOR:
		return "pgup";
	case VK_NEXT:
		return "pgdown";
	case VK_END:
		return "end";
	case VK_HOME:
		return "home";
	case VK_LEFT:
		return "left";
	case VK_UP:
		return "up";
	case VK_RIGHT:
		return "right";
	case VK_DOWN:
		return "down";
	case VK_SELECT:
		return "select";
	case VK_INSERT:
		return "insert";
	case VK_DELETE:
		return "delete";
	case '0':
		return "0";
	case '1':
		return "1";
	case '2':
		return "2";
	case '3':
		return "3";
	case '4':
		return "4";
	case '5':
		return "5";
	case '6':
		return "6";
	case '7':
		return "7";
	case '8':
		return "8";
	case '9':
		return "9";
	case 'A':
		return "a";
	case 'B':
		return "b";
	case 'C':
		return "c";
	case 'D':
		return "d";
	case 'E':
		return "e";
	case 'F':
		return "f";
	case 'G':
		return "g";
	case 'H':
		return "h";
	case 'I':
		return "i";
	case 'J':
		return "j";
	case 'K':
		return "k";
	case 'L':
		return "l";
	case 'M':
		return "m";
	case 'N':
		return "n";
	case 'O':
		return "o";
	case 'P':
		return "p";
	case 'Q':
		return "q";
	case 'R':
		return "r";
	case 'S':
		return "s";
	case 'T':
		return "t";
	case 'U':
		return "u";
	case 'V':
		return "v";
	case 'W':
		return "w";
	case 'X':
		return "x";
	case 'Y':
		return "y";
	case 'Z':
		return "z";
	case VK_LWIN:
		return "left win";
	case VK_RWIN:
		return "right win";
	case VK_NUMPAD0:
		return "num 0";
	case VK_NUMPAD1:
		return "num 1";
	case VK_NUMPAD2:
		return "num 2";
	case VK_NUMPAD3:
		return "num 3";
	case VK_NUMPAD4:
		return "num 4";
	case VK_NUMPAD5:
		return "num 5";
	case VK_NUMPAD6:
		return "num 6";
	case VK_NUMPAD7:
		return "num 7";
	case VK_NUMPAD8:
		return "num 8";
	case VK_NUMPAD9:
		return "num 9";
	case VK_MULTIPLY:
		return "num mult";
	case VK_ADD:
		return "num add";
	case VK_SEPARATOR:
		return "|";
	case VK_SUBTRACT:
		return "num sub";
	case VK_DECIMAL:
		return "num decimal";
	case VK_DIVIDE:
		return "num divide";
	case VK_F1:
		return "f1";
	case VK_F2:
		return "f2";
	case VK_F3:
		return "f3";
	case VK_F4:
		return "f4";
	case VK_F5:
		return "f5";
	case VK_F6:
		return "f6";
	case VK_F7:
		return "f7";
	case VK_F8:
		return "f8";
	case VK_F9:
		return "f9";
	case VK_F10:
		return "f10";
	case VK_F11:
		return "f11";
	case VK_F12:
		return "f12";
	case VK_NUMLOCK:
		return "num lock";
	case VK_SCROLL:
		return "break";
	case VK_LSHIFT:
		return "shift";
	case VK_RSHIFT:
		return "shift";
	case VK_LCONTROL:
		return "ctrl";
	case VK_RCONTROL:
		return "ctrl";
	case VK_LMENU:
		return "alt";
	case VK_RMENU:
		return "alt";
	case VK_OEM_COMMA:
		return "),";
	case VK_OEM_PERIOD:
		return ".";
	case VK_OEM_1:
		return ";";
	case VK_OEM_MINUS:
		return "-";
	case VK_OEM_PLUS:
		return "=";
	case VK_OEM_2:
		return "/";
	case VK_OEM_3:
		return "grave";
	case VK_OEM_4:
		return "[";
	case VK_OEM_5:
		return "\\";
	case VK_OEM_6:
		return "]";
	case VK_OEM_7:
		return "[222]";
	default:
		return "";
	}
}
#pragma endregion

template <typename T>
static bool items_getter(void* data, int idx, const char** out_text)
{
	auto items = (T*)data;
	if (out_text) {
		*out_text = items[idx].item_name.data();//std::string(items[idx].name.begin(), items[idx].translated_name.end()).data();
	}
	return true;
};

int tabcount = 0;

struct s_tab
{
	const char* name = "";
	const char* icon = 0;
	ImVec2 size = ImVec2(0, 0);
	int num = 0;

	s_tab(const char* _name, const char* _icon = "", ImVec2 _size = ImVec2(0, 0))
	{
		name = _name;
		icon = _icon;
		size = _size;
		num = tabcount;
		tabcount++;
	}

	~s_tab()
	{
		/*tabcount--;
		name = "";
		icon = 0;
		size = ImVec2(0, 0);
		num = 0;*/
	}
};

namespace ImGui
{
	bool ColorButton(const char* desc_id, const float col[], ImGuiColorEditFlags flags, ImVec2 size)
	{
		return ImGui::ColorButton(desc_id, ImColor(col[0], col[1], col[2], col[3]), flags, size);
	}

	ImGuiID Colorpicker_Close = 0;
	__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }
	bool ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_ct_invis[], float col_t_invis[], bool alpha = true, bool use_buttons = false)
	{
		bool ret = false;

		ImGui::SameLine();
		static bool switch_entity_teams = false;
		static bool switch_color_vis = false;

		float* col = use_buttons ? (switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis)) : col_ct;

		const auto cursor_pos_y = GetCursorPosY();

		SetCursorPosY(cursor_pos_y + 5);
		bool open_popup = ImGui::ColorButton(picker_idname, col, ImGuiColorEditFlags_NoSidePreview 
			| ImGuiColorEditFlags_NoSmallPreview 
			| ImGuiColorEditFlags_AlphaPreview
			| ImGuiColorEditFlags_NoTooltip
			| ImGuiColorEditFlags_ColorPicker 
			| ImGuiColorEditFlags_ColorPickerSameline, ImVec2(18, 10));

		SetCursorPosY(GetCursorPosY() - 5);

		/*SetCursorPosY(cursor_pos_y);*/
		if (open_popup) {
			ImGui::OpenPopup(picker_idname);
			Colorpicker_Close = ImGui::GetID(picker_idname);
		}

		if (ImGui::BeginPopup(picker_idname))
		{
			if (use_buttons) {
				const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
				if (ImGui::Button(button_name0, ImVec2(-1, 0)))
					switch_entity_teams = !switch_entity_teams;

				const char* button_name1 = switch_color_vis ? "Visible" : "Invisible";
				if (ImGui::Button(button_name1, ImVec2(-1, 0)))
					switch_color_vis = !switch_color_vis;
			}

			std::string id_new = picker_idname;
			id_new += "##pickeritself_";

			ret = ImGui::ColorPicker(id_new.c_str(), col, (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
			ImGui::EndPopup();
		}

		return ret;
	}

	bool mouse_in_pos(ImRect minmaxs)
	{
		ImGuiContext& g = *GImGui;
		return (g.IO.MousePos.x >= minmaxs.Min.x && g.IO.MousePos.y >= minmaxs.Min.y && g.IO.MousePos.x <= minmaxs.Max.x && g.IO.MousePos.y <= minmaxs.Max.y);
	}

	bool Tab(const char* label, const ImVec2& size_arg, bool selected)
	{
		return ButtonEx(label, size_arg, (selected ? ImGuiItemFlags_TabButton : 0));
	}

	// NB: This is an internal helper. The user-facing IsItemHovered() is using data emitted from ItemAdd(), with a slightly different logic.
	bool IsHovered(const ImRect& bb, ImGuiID id, bool flatten_childs)
	{
		ImGuiContext& g = *GImGui;
		if (g.HoveredId == 0 || g.HoveredId == id || g.HoveredIdAllowOverlap)
		{
			ImGuiWindow* window = GetCurrentWindowRead();
			if (g.HoveredWindow == window || (flatten_childs && g.HoveredRootWindow == window->RootWindow))
				if ((g.ActiveId == 0 || g.ActiveId == id || g.ActiveIdAllowOverlap) && IsMouseHoveringRect(bb.Min, bb.Max))
					return true;
		}
		return false;
	}


	bool Bind(const char* label, int* key, const ImVec2& size_arg)
	{
		ImGuiWindow* window = GetCurrentWindow();

		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiIO& io = g.IO;
		const ImGuiStyle& style = g.Style;

		const ImGuiID id = window->GetID(label);

		ImVec2 label_size = CalcTextSize(label, NULL, true);
		ImVec2 size = CalcItemSize(size_arg, CalcItemWidth() / 2, label_size.y + style.FramePadding.y);// *2.0f);

		const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + size);
		const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? (style.ItemInnerSpacing.x + label_size.x) : 0.0f, 0.0f));

		ItemSize(total_bb, style.FramePadding.y);

		if (!ItemAdd(total_bb, id))
			return false;

		bool value_changed = false;

		std::string text = "None";

		bool hovered = false, held = false;

		if (g.ActiveId == id)
		{
			text = "Press a Key";

			if (!g.ActiveIdIsJustActivated)
			{
				for (int i = 0; i < 255; i++)
				{
					if (ctx.pressed_keys[i])//g.IO.KeysReleased[i])
					{
						SetActiveID(0, window);
						*key = (i == 0x1B/*VK_ESCAPE*/) ? 0 : i;
						value_changed = true;
						break;
					}
				}

				/*for (int i = 0; i < 6; i++)
				{
					if (g.IO.MouseReleased[i])
					{
						SetActiveID(0);
						*key = i + 1;
						value_changed = true;
						break;
					}
				}*/
			}
		}
		else
		{
			hovered = IsHovered(frame_bb, id, false);

			if (hovered)
			{
				SetHoveredID(id);

				if (g.IO.MouseDown[0])
				{
					held = true;
					FocusWindow(window);
				}
				else if (g.IO.MouseReleased[0])
				{
					SetActiveID(id, window);
				}
			}

			text = get_kname(*key);
		}

		const ImU32 col = GetColorU32((hovered && held) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
		RenderFrame(frame_bb.Min, frame_bb.Max, col, true, style.FrameRounding); // main frame
		label_size = CalcTextSize(text.c_str(), NULL, true);
		ImVec2 pos_min = frame_bb.Min;
		ImVec2 pos_max = frame_bb.Max;
		ImVec2 label_pos = pos_min;
		label_pos.x = ImMax(label_pos.x, (label_pos.x + pos_max.x - label_size.x) * 0.5f);
		label_pos.y = ImMax(label_pos.y, (label_pos.y + pos_max.y - label_size.y) * 0.5f);
		RenderText(label_pos, text.c_str(), NULL);
		RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y - 3), label); // render item lable

		return value_changed;
	}
}

namespace menu
{
	LPDIRECT3DTEXTURE9 flag_ewropi = NULL;
	LPDIRECT3DTEXTURE9 m_tplayer_with_glow = NULL;
	LPDIRECT3DTEXTURE9 m_tplayer_no_glow = NULL;
	IDirect3DDevice9* m_device = nullptr;

	int category = -1;
	int new_category = -1;

	void init(const float& alpha)
	{
		auto style = &ImGui::GetStyle();
		//ImGui::StyleColorsDark();

		ImVec4* colors = style->Colors;

		//if (m_device != nullptr)
		//{
			/*if (flag_ewropi == NULL) {
				D3DXCreateTextureFromFileInMemoryEx(m_device, &menu_background, sizeof(menu_background), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &flag_ewropi);
				m_device->SetTexture(0, flag_ewropi);
			}*/

			/*if (m_tplayer_with_glow == NULL) {
				D3DXCreateTextureFromFileInMemoryEx(m_device, &player_with_glow, sizeof(player_with_glow), 1920, 1080, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_tplayer_with_glow);
				m_device->SetTexture(0, m_tplayer_with_glow);
			}

			if (m_tplayer_no_glow == NULL) {
				D3DXCreateTextureFromFileInMemoryEx(m_device, &player_no_glow, sizeof(player_no_glow), 1920, 1080, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_tplayer_no_glow);
				m_device->SetTexture(0, m_tplayer_no_glow);
			}*/
		//}

		const auto main_color = ImVec4(0.24f, 0.52f, 0.88f, alpha);
		const auto main_color_fade = ImVec4(0.26f, 0.59f, 0.98f, alpha);

		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, alpha);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f * alpha);
		colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f * alpha);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f * alpha);
		colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f * alpha);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f * alpha);
		colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f * alpha);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f * alpha);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f * alpha);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, alpha);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.29f, 0.48f, alpha);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f * alpha);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, alpha);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f * alpha);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, alpha);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, alpha);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, alpha);
		colors[ImGuiCol_CheckMark] = main_color/*ImVec4(0.26f, 0.59f, 0.98f, alpha)*/;
		colors[ImGuiCol_SliderGrab] = main_color;
		colors[ImGuiCol_SliderGrabActive] = main_color_fade;
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f * alpha);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, alpha);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, alpha);
		colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f * alpha);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f * alpha);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, alpha);
		colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];//ImVec4(0.61f, 0.61f, 0.61f, alpha);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f * alpha);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, alpha);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f * alpha);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f * alpha);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f * alpha);
		colors[ImGuiCol_CloseButton] = ImVec4(0.41f, 0.41f, 0.41f, 0.50f * alpha);
		colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, alpha);
		colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, alpha);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, alpha);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, alpha);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, alpha);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, alpha);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f * alpha);
		colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f * alpha);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f * alpha);
		colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, alpha);
		colors[ImGuiCol_TitleBg] = ImVec4(0.2f, 0.2f, 0.2f, alpha);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.2f, alpha);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.2f, 0.2f, 0.2f, alpha);
		colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 1.0f, 1.0f, alpha * 0.06f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, alpha * 0.1f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, alpha * 0.2f);
		colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, alpha * 0.5f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.3f, alpha * 0.5f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.4f, 0.4f, 0.4f, alpha * 0.5f);
		colors[ImGuiCol_ScrollbarBg] = colors[ImGuiCol_WindowBg];

		colors[ImGuiCol_Tab] = ImVec4(0.16f, 0.29f, 0.48f, 0.54f * alpha);
		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f * alpha);
		colors[ImGuiCol_TabActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f * alpha);
		colors[ImGuiCol_TabText] = ImVec4(0.50f, 0.50f, 0.50f, alpha);
		colors[ImGuiCol_TabTextActive] = ImVec4(1.00f, 1.00f, 1.00f, alpha);
		colors[ImGuiCol_TabSelected] = ImVec4(0.06f, 0.53f, 0.98f, alpha);

		style->ScrollbarSize = 14.f;
		style->FrameRounding = 0;
		style->WindowRounding = 0.0f;
		style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style->FramePadding = ImVec2(8, 4);

		//ImGui::GetIO().Fonts->AddFontDefault();
	}

	void skins_listbox(const char* label, std::string filter, int selected_rarity, int* current_item, skin_list_t& skins, int height_in_items, const float& alpha)
	{
		if (!ImGui::ListBoxHeader(label, skins.list.size(), height_in_items))
			return;

		for (int i = 0; i < skins.list.size(); i++)
		{
			const auto item_selected = (i == *current_item);
			const auto item_text = skins.list[i].translated_name;
			auto rarity = parser::rarities.get_by_id(skins.list[i].rarity);
			auto color = rarity.Color.alpha(alpha * 255);

			if (selected_rarity != 0 &&
				selected_rarity != rarity.id)
				continue;

			std::string pstr = std::string(item_text.begin(), item_text.end());

			if (!filter.empty() &&
				parser::to_lower(pstr).find(parser::to_lower(filter)) == std::string::npos)
				continue;

			ImGui::PushID(i);
			ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)color);
			if (ImGui::Selectable(pstr.data(), item_selected))
			{
				*current_item = i;
			}
			ImGui::PopStyleColor();
			ImGui::PopID();
		}

		ImGui::ListBoxFooter();
	}

	//void draw_skinchanger(const float& alpha)
	//{
	//	static auto selected_item = 0;
	//	static auto fake_wear = 1.f;
	//	static auto fake_seed = 0;
	//	static char fake_name[256] = "";
	//	static auto fake_stattrak = false;
	//	static auto fake_stattrak_value = 0;

	//	//if (ImGui::Begin("Skin changer")
	//	//{
	//	ImGui::Page::Begin(3);
	//	ImGui::PushItemWidth(ImGui::GetCurrentWindow()->Size.x);

	//	ImGui::ListBox(sxor("##weapons"), &selected_item, items_getter<weapon_t>, (void*)parser::weapons.list.data(), parser::weapons.list.size(), 22);

	//	ImGui::PopItemWidth();
	//	ImGui::Page::Next();
	//	ImGui::PushItemWidth(ImGui::GetCurrentWindow()->Size.x);

	//	static bool show_all_skins = false;
	//	//ImGui::Checkbox("All skins", &show_all_skins);

	//	static char filter_buffer[64] = "";
	//	ImGui::InputText(sxor("##filter"), filter_buffer, 64);

	//	// draw rarities
	//	static auto selected_rarity = 0;
	//	{
	//		static auto draw_rarity = [](rarity_t& rarity, int& selected_rarity, const float& alpha) {

	//			bool selected = false;
	//			auto size = ImVec2(12, 8);

	//			auto rarity_col = rarity.Color.alpha(alpha * 255);

	//			ImGui::PushStyleColor(ImGuiCol_Button, selected_rarity == rarity.id ? (ImVec4)rarity_col : (ImVec4)rarity_col.malpha(0.3f)); //40 доху€ имхо
	//			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)rarity_col.malpha(0.7f));
	//			ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)rarity_col.malpha(0.7f));

	//			if (ImGui::Button(("##" + rarity.name).data(), size))
	//				selected_rarity = rarity.id;

	//			ImGui::PopStyleColor(3);
	//			ImGui::SameLine(/*0.f, 10.4f*/);

	//		};

	//		for (auto& rarity : parser::rarities.list) {
	//			draw_rarity(rarity, selected_rarity, alpha);
	//		}

	//		ImGui::NewLine();
	//	}

	//	auto selected_weapon = parser::weapons.list[selected_item].id;
	//	auto selected_skin = 0;
	//	/*for (auto i = 0u; i < parser::weapons.list[selected_item].skins.list.size(); i++)
	//	{
	//		auto skin = parser::weapons.list[selected_item].skins.list[i];
	//		if (skin.id == cheat::settings.paint[selected_weapon])
	//			selected_skin = i;
	//	}*/
	//	auto old_skin = selected_skin;

	//	if (!parser::weapons.list.empty())
	//		skins_listbox(sxor("##skins"), std::string(filter_buffer), selected_rarity, &selected_skin, parser::weapons.list[selected_item].skins, 20, alpha);

	//	//if (selected_skin != old_skin) {
	//	//	ctx.m_settings.paint[selected_weapon] = parser::weapons.list[selected_item].skins.list[selected_skin].id;
	//	//	//game::full_update();
	//	//}

	//	ImGui::PopItemWidth();
	//	ImGui::Page::Next();
	//	ImGui::PushItemWidth(ImGui::GetCurrentWindow()->Size.x);

	//	ImGui::SliderFloat(sxor("wear"), &fake_wear, 0.f, 1.f);
	//	ImGui::Spacing();

	//	ImGui::Text(sxor("seed"));
	//	ImGui::InputInt(sxor("##_seed"), &fake_seed);
	//	ImGui::Spacing();

	//	ImGui::Checkbox(sxor("stat trak"), &fake_stattrak);
	//	if (fake_stattrak) {
	//		ImGui::InputInt(sxor("##_stattrak_value"), &fake_stattrak_value);
	//	}
	//	ImGui::Spacing();

	//	ImGui::Text(sxor("name"));
	//	ImGui::InputText(sxor("##_name"), fake_name, 256);
	//	ImGui::Spacing();

	//	static auto selected_knife = 0;
	//	static std::vector<knife_t> knife_list;
	//	if (knife_list.empty())
	//	{
	//		knife_list = parser::knifes.list;

	//		knife_t default_knife;
	//		default_knife.id = 0;
	//		default_knife.translated_name = L"Default";
	//		knife_list.insert(knife_list.begin(), default_knife);
	//	}

	//	if (ImGui::ListBox("##knife_changer", &selected_knife, items_getter<knife_t>, (void*)knife_list.data(), knife_list.size(), 6))
	//	{
	//		auto new_id = knife_list[selected_knife].id;
	//		ctx.m_settings.skinchanger_knife = new_id;
	//		//game::full_update();
	//	}

	//	ImGui::PopItemWidth();
	//	ImGui::Page::End();

	//	//}
	//	//ImGui::End();
	//}

	bool add_tab(s_tab tab, int* var/*, bool is_clickable = true*/, ImVec2 override_size = ImVec2(-1, -1), bool hovered = true)
	{
		bool ret = false;
		const auto allowed_to_use_icon = strlen(tab.icon) > 0;

		if (allowed_to_use_icon && !hovered)
			ImGui::PushFont(d::fonts::menu_icons.imgui);
		else
			ImGui::PushFont(d::fonts::menu_tab.imgui);

		if (ImGui::Tab((allowed_to_use_icon && !hovered ? tab.icon : tab.name), override_size == ImVec2(-1, -1) ? tab.size : override_size, *var == tab.num))
		{
			ret = true;

			if (/*is_clickable && */var != nullptr)
				*var = tab.num;
		}

		ImGui::PopFont();

		//ImGui::SameLine(0);
		return ret;
	}

	std::list<int> get_spec(int playerId)
	{
		std::list<int> list;

		if (!csgo.m_engine()->IsInGame())
			return list;

		auto player = csgo.m_entity_list()->GetClientEntity(playerId);
		if (!player)
			return list;

		if (player->IsDead())
		{
			auto observerTarget = player->m_hObserverTarget() ? csgo.m_entity_list()->GetClientEntityFromHandle(*player->m_hObserverTarget()) : nullptr;
			if (!observerTarget)
				return list;

			player = observerTarget;
		}

		for (int i = 1; i < csgo.m_globals()->maxClients; i++)
		{
			auto pPlayer = csgo.m_entity_list()->GetClientEntity(i);

			if (!pPlayer)
				continue;

			if (pPlayer->IsDormant() || !pPlayer->IsDead())
				continue;

			auto target = pPlayer->m_hObserverTarget() ? csgo.m_entity_list()->GetClientEntityFromHandle(*pPlayer->m_hObserverTarget()) : nullptr;
			if (player != target)
				continue;

			list.push_back(i);
		}

		return list;
	}

	void speclist()
	{
		if (!ctx.m_settings.misc_spectators_list)
			return;

		ImGuiStyle& style = ImGui::GetStyle();
		//float oldAlpha = style.Colors[ImGuiCol_WindowBg].w;
		//style.Colors[ImGuiCol_WindowBg].w = Vars.Visuals.SpectatorListAlpha / 255.0f;

		if (ImGui::Begin(sxor("spectator list"), &ctx.m_settings.misc_spectators_list, ImVec2(300, 300), 1.f, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_ShowBorders))// |  ImGuiWindowFlags_ShowBorders))
		{
			ImGui::TextColored(ImVec4(style.Colors[ImGuiCol_Text].x - 0.3f, style.Colors[ImGuiCol_Text].y - 0.3f, style.Colors[ImGuiCol_Text].z - 0.3f, style.Colors[ImGuiCol_Text].w), sxor("spectators:"));
			ImGui::Separator();
			if (csgo.m_engine()->IsInGame()) {
				for (int playerId : get_spec(csgo.m_engine()->GetLocalPlayer()))
				{
					auto player = csgo.m_entity_list()->GetClientEntity(playerId);

					player_info entityInformation;
					csgo.m_engine()->GetPlayerInfo(playerId, &entityInformation);

					std::string name(entityInformation.name);

					if(name.size()>31)
					name.resize(31);

					ImGui::Text(name.c_str());
				}
			}
			//ImGui::Separator();

			ImGui::End();
		}
		//style.WindowPadding = oldPadding;
		//style.Colors[ImGuiCol_WindowBg].w = oldAlpha;
	}

	void draw(IDirect3DDevice9* device)
	{
		ImGui::is_input_allowed = true;
		m_device = device;

		ImGuiIO& io = ImGui::GetIO();
		io.MousePos.x = feature::menu->_cursor_position.x;
		io.MousePos.y = feature::menu->_cursor_position.y;
		//ImGui::GetIO().MouseDrawCursor = m_opened; // makes cursor dissapear when menu is closed ///кл€тий курсор, куди ти под≥вс€

		if (ctx.m_settings.misc_status_list)
		{
			static bool tr = true;
			float cursor_pos_y;
			ImGui::Begin("status list", &ctx.m_settings.misc_status_list, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders);
			{
				if (csgo.m_engine()->IsInGame() && !ctx.m_local()->IsDead())
				{
					ImGui::is_input_allowed = false;
					int desync_range = fabs(Math::angle_diff(ctx.angles[ANGLE_REAL], ctx.angles[ANGLE_FAKE]));
					int fakelag_choke = csgo.m_client_state()->m_iChockedCommands;

					ImGui::Text("desync delta:");
					ImGui::SameLine();

					cursor_pos_y = ImGui::GetCursorPosY();

					ImGui::SetCursorPosY(cursor_pos_y + 8);
					ImGui::SliderInt("##lmao", &desync_range, 0, 60); 
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
					
					//ImGui::TextColored(lby_broken ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f), "[is broken]");

					//ImGui::Text("lagcomp:");
					//ImGui::SameLine();
					//ImGui::TextColored(Vars.LC ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f), "[is broken]");

					ImGui::Text("speed: %.0f", ctx.m_local()->m_vecVelocity().Length2D());
			
					ImGui::Text("fakelag:");
					ImGui::SameLine();
					cursor_pos_y = ImGui::GetCursorPosY();

					ImGui::SetCursorPosY(cursor_pos_y + 8);
					ImGui::SliderInt("##flmao", &fakelag_choke, 0, 15);
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

					if (ctx.has_exploit_toggled)
					{
						ImGui::Text("exploit:");
						
						if (ctx.exploit_allowed && ctx.ticks_allowed > 13 && ctx.main_exploit != 0) {
							ImGui::SameLine();

							if (ctx.main_exploit == 1)
								ImGui::Text("hide shots");
							else if (ctx.main_exploit > 1)
								ImGui::Text("doubletap");
						}
						else
						{
							ImGui::SameLine();
							ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "none");
						}
					}

					if (ctx.m_settings.aimbot_key != 0)
					{
						ImGui::Text("rage aimbot: ");
						ImGui::SameLine();
					
						if (ctx.allows_aimbot)
							ImGui::Text("enabled");
						else
							ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "disabled");
					
					}

					if (ctx.allows_aimbot)
					{
						ImGui::Text("min damage override: ");
						ImGui::SameLine();

						if (feature::ragebot->m_damage_key)
							ImGui::Text("enabled");
						else
							ImGui::Text("disabled");

					}

					/*ImGui::Text("lagcomp:");
					ImGui::SameLine();
					ImGui::TextColored(ctx.breaks_lc ? ImVec4(0.f, 1.f, 0.f, 1.f) : ImVec4(1.f, 0.f, 0.f, 1.f), "%.f", ctx.last_origin_delta);*/
				}
			}
			ImGui::End();
		}

		speclist();

		if (!feature::menu->_menu_opened)
			return;

		ImGui::is_input_allowed = true;

		static auto alpha = 1.f;
		static auto pos = 0.4f;
		static auto switch_p = false;
		static auto switch_category = false;
		static auto was_hovered = false;
		static auto tab_size = 150.f;

		/*
		main.RegisterTab(&legitbot, "legit bot", "a", 480, 660, { "global", "A", "P", "e", "W", "Z" }, F::Weapons);
		main.RegisterTab(&ragebot, "rage bot", "c", 0, 623, {"aimbot", "anti-aim"});
		main.RegisterTab(&visuals, "visuals", "d", 530, 0, { "players","other" });
		main.RegisterTab(&misc, "miscellaneous", "f", 0, 623, {"misc", "config"});
		main.RegisterTab(&skins, "skins", "e", 0, 623);
		main.RegisterTab(&Lists, "players", "k", 530, 660);
		*/

		static auto t_ragebot = s_tab(("Ragebot"), "c");
		static auto t_legitbot = s_tab(("Legitbot"), "a");
		static auto t_visuals = s_tab(("Visuals"), "d");
		//static auto t_skins = s_tab(("Skins"), "e");
		static auto t_misc = s_tab(("Misc"), "f");
		static auto	curtab = 0;

		//bool pressed_return = false;

		if (alpha < 1.f)
			init(1.f); //н≥чого кр≥м меню не повинно зм≥нювати прозор≥сть

		ImGui::SetNextWindowSize(ImVec2(700, 550));

		if (ImGui::Begin(sxor("birdie v4"), (bool*)0, ImGuiWindowFlags_NoCollapse | ImGuiColumnsFlags_NoResize | ImGuiWindowFlags_NoTitleBar/*| ImGuiWindowFlags_CoolStyle*/ | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse, 0))
		{
			auto window = ImGui::GetCurrentWindow();
			const auto cursorpos = ImGui::GetCursorPos();

			/*pos = Math::clamp(pos, -1.f, 0.f);
			ImGui::PushClipRect(window->Pos + ImVec2(window->WindowPadding.x, 40 + window->WindowPadding.y), window->Pos + ImVec2(0, 40 + window->WindowPadding.y) + ImVec2(window->Size.x - window->WindowPadding.x, window->Size.y - 40 - window->WindowPadding.y * 2.f), false);
			ImGui::SetCursorPosY(cursorpos.y + window->Size.y * pos);
			ImGui::SetCursorPosX(cursorpos.x - 500);
			ImGui::Image((void*)flag_ewropi, ImVec2(1600, 900), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), ImVec4(1.f, 1.f, 1.f, 0.15f));
			ImGui::PopClipRect();

			if (fabs(pos) >= 1.f || pos == 0.f)
				switch_p = !switch_p;

			if (switch_p)
				pos -= 0.0005f;
			else
				pos += 0.0005f;

			ImGui::SetCursorPos(cursorpos);*/

			if (alpha < 1.f) //ЅЋя“№ ну вот честно не ебу зачем € поставил эту хуйню два раза LINE#308, похуй.
				init(alpha); //н≥чого кр≥м меню не повинно зм≥нювати прозор≥сть

			auto is_hovered = ImGui::mouse_in_pos(ImRect(window->Pos, window->Pos + ImVec2(static_cast<float>(tab_size * 0.45f) + window->WindowPadding.x * 2.f, window->Size.y)));

			if (is_hovered || was_hovered && ImGui::mouse_in_pos(ImRect(window->Pos, window->Pos + ImVec2(tab_size + window->WindowPadding.x * 2.f, window->Size.y)))) {
				if (pos < 1.f)
					pos += 0.08f;

				is_hovered = true;
			}
			else {
				if (pos > 0.5f)
					pos -= 0.08f;
			}

			pos = Math::clamp(pos, 0.5f, 1.f);

			const auto show_names = (pos > 0.65f);

			/*if (pos > 0.8f)
			{
				ImGui::SameLine();
				ImGui::SetCursorPosY(cursorpos.y + window->Size.y - window->WindowPadding.y*2.f - 22.5f);
				ImGui::Text("getze.us v4");
			}*/
			ImGui::SetCursorPos(cursorpos);

			ImGui::PushClipRect(window->Pos, window->Pos + ImVec2(tab_size * pos + window->WindowPadding.x * 2.f, window->Size.y), false);
			add_tab(t_ragebot, &curtab, ImVec2(tab_size * pos, static_cast<float>(tab_size * 0.45f)), show_names);
			add_tab(t_legitbot, &curtab, ImVec2(tab_size * pos, static_cast<float>(tab_size * 0.45f)), show_names);
			add_tab(t_visuals, &curtab, ImVec2(tab_size * pos, static_cast<float>(tab_size * 0.45f)), show_names);
			//add_tab(t_skins, &curtab, ImVec2(tab_size * pos, static_cast<float>(tab_size * 0.45f)), show_names);
			add_tab(t_misc, &curtab, ImVec2(tab_size * pos, static_cast<float>(tab_size * 0.45f)), show_names);
			ImGui::PopClipRect();

			ImGui::SetCursorPos(ImVec2(tab_size * pos + window->WindowPadding.x * 2.f, cursorpos.y));

			//if (switch_category) {
			//	if (alpha > 0.01f) {
			//		init(alpha); //оновлюють кольору
			//		alpha -= 0.025f;
			//	}
			//	else {
			//		switch_category = false;
			//		category = new_category; //зак≥нчили м≥н€ти прозор≥сть, пора-б застосувати категор≥ю
			//	}
			//}
			//else
			//{
			//	if (alpha < 1.f) {
			//		init(alpha); //оновлюють кольору
			//		alpha += 0.025f;
			//	}
			//}

			alpha = Math::clamp(alpha, 0.f, 1.f);

			static int currentCategory{ 0 };
			static int currentWeapon{ 0 };

			ImGui::BeginChild(sxor("##mainshit"), ImVec2(0, window->Size.y - window->WindowPadding.y * 2.f), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
			{
				float cursor_pos_y;

				switch (curtab)
				{
				case 0:
					ImGui::Columns(2, NULL, true);
					{
						ImGui::BeginChild("##ragebotmain");
						{
							ImGui::Checkbox(sxor("Ragebot enabled"), &ctx.m_settings.aimbot_enabled);
							ImGui::SameLine();
							ImGui::Bind(sxor("##ROnkey"), &ctx.m_settings.aimbot_key, ImVec2(100, 20));

							ImGui::Checkbox(sxor("Automatic revolver"), &ctx.m_settings.aimbot_auto_revolver);
							ImGui::Checkbox(sxor("Silent aim"), &ctx.m_settings.aimbot_silent_aim);
							ImGui::Checkbox(sxor("No Recoil/Spread"), &ctx.m_settings.aimbot_no_spread);
							ImGui::Checkbox(sxor("Automatic scope"), &ctx.m_settings.aimbot_autoscope);
							ImGui::SliderInt(sxor("Field of view"), &ctx.m_settings.aimbot_fov_limit, 0, 180);
							ImGui::Checkbox(sxor("Hitchance"), &ctx.m_settings.aimbot_hitchance);
							//ImGui::SameLine();
							//cursor_pos_y = ImGui::GetCursorPosY();

							//ImGui::SetCursorPosY(cursor_pos_y + 8);
							ImGui::SliderInt(sxor("##hcv"), &ctx.m_settings.aimbot_hitchance_val, 0, 100);
							ImGui::Spacing();
							//ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SliderInt(sxor("Min damage"), &ctx.m_settings.aimbot_min_damage_viable, 0, 101);
							ImGui::Spacing();
							ImGui::Checkbox(sxor("Autowall"), &ctx.m_settings.aimbot_autowall);
							ImGui::Text(sxor("Wall min damage"));
							ImGui::SliderInt(sxor("##Wallmindamage"), &ctx.m_settings.aimbot_min_damage, 0, 101);
							ImGui::Spacing();

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Columns(2, NULL, true);
							ImGui::BeginChild("##ragebothitbox");
							{
								ImGui::Checkbox(sxor("Scan head"), &ctx.m_settings.aimbot_hitboxes[0]);
								ImGui::Checkbox(sxor("Scan chest"), &ctx.m_settings.aimbot_hitboxes[1]);
								ImGui::Checkbox(sxor("Scan body"), &ctx.m_settings.aimbot_hitboxes[2]);
								ImGui::Checkbox(sxor("Scan arms"), &ctx.m_settings.aimbot_hitboxes[3]);
								ImGui::Checkbox(sxor("Scan legs"), &ctx.m_settings.aimbot_hitboxes[4]);
								ImGui::Checkbox(sxor("Scan feet"), &ctx.m_settings.aimbot_hitboxes[5]);
								ImGui::Separator();
								ImGui::Spacing();
								ImGui::Checkbox(sxor("Prefer body aim"), &ctx.m_settings.aimbot_prefer_body);
								//ImGui::Checkbox(sxor("Prefer body-aim"), &ctx.m_settings.aimbot_hitboxes[5]);
							}
							ImGui::EndChild();
							ImGui::NextColumn();
							ImGui::BeginChild("##ragebotedges");
							{
								constexpr auto sliders_size = 135.f;

								ImGui::Checkbox(sxor("Head edges"), &ctx.m_settings.aimbot_multipoint[0]);
								if (ctx.m_settings.aimbot_multipoint[0])
								{
									cursor_pos_y = ImGui::GetCursorPosY();

									ImGui::SetCursorPosY(cursor_pos_y + 8);
									ImGui::PushItemWidth(sliders_size);
									ImGui::SliderInt(sxor("##Headscale"), &ctx.m_settings.aimbot_pointscale[0], 1, 100);
									ImGui::PopItemWidth();
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
									ImGui::Spacing();								
									ImGui::Spacing();
									ImGui::Spacing();

								}
								ImGui::Checkbox(sxor("Chest edges"), &ctx.m_settings.aimbot_multipoint[1]);
								if (ctx.m_settings.aimbot_multipoint[1])
								{
									cursor_pos_y = ImGui::GetCursorPosY();

									ImGui::SetCursorPosY(cursor_pos_y + 8);
									ImGui::PushItemWidth(sliders_size);
									ImGui::SliderInt(sxor("##chestscale"), &ctx.m_settings.aimbot_pointscale[1], 1, 100);
									ImGui::PopItemWidth();
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);	

									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
								}
								ImGui::Checkbox(sxor("Body edges"), &ctx.m_settings.aimbot_multipoint[2]);
								if (ctx.m_settings.aimbot_multipoint[2])
								{
									cursor_pos_y = ImGui::GetCursorPosY();

									ImGui::SetCursorPosY(cursor_pos_y + 8);
									ImGui::PushItemWidth(sliders_size);
									ImGui::SliderInt(sxor("##bodyscale"), &ctx.m_settings.aimbot_pointscale[2], 1, 100);
									ImGui::PopItemWidth();
									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
								

									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
								}
								ImGui::Checkbox(sxor("Arms edges"), &ctx.m_settings.aimbot_multipoint[3]);
								if (ctx.m_settings.aimbot_multipoint[3])
								{
									cursor_pos_y = ImGui::GetCursorPosY();

									ImGui::SetCursorPosY(cursor_pos_y + 8);
									ImGui::PushItemWidth(sliders_size);
									ImGui::SliderInt(sxor("##armsscale"), &ctx.m_settings.aimbot_pointscale[3], 1, 100);
									ImGui::PopItemWidth();

									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
								}
								ImGui::Checkbox(sxor("Legs edges"), &ctx.m_settings.aimbot_multipoint[4]);
								if (ctx.m_settings.aimbot_multipoint[4])
								{
									cursor_pos_y = ImGui::GetCursorPosY();

									ImGui::SetCursorPosY(cursor_pos_y + 8);
									ImGui::PushItemWidth(sliders_size);
									ImGui::SliderInt(sxor("##legsscale"), &ctx.m_settings.aimbot_pointscale[4], 1, 100);
									ImGui::PopItemWidth();

									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
								}
								ImGui::Checkbox(sxor("Feet edges"), &ctx.m_settings.aimbot_multipoint[5]);
								if (ctx.m_settings.aimbot_multipoint[5])
								{
									cursor_pos_y = ImGui::GetCursorPosY();

									ImGui::SetCursorPosY(cursor_pos_y + 8);
									ImGui::PushItemWidth(sliders_size);
									ImGui::SliderInt(sxor("##feetscale"), &ctx.m_settings.aimbot_pointscale[5], 1, 100);
									ImGui::PopItemWidth();

									ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
									ImGui::Spacing();
									ImGui::Spacing();
									ImGui::Spacing();
								}
							}
							ImGui::EndChild();
						}
						ImGui::EndChild();
					}
					ImGui::NextColumn();
					{
						ImGui::BeginChild("##ragebotaa");
						{
							ImGui::Checkbox(sxor("Anti-Aim enabled"), &ctx.m_settings.anti_aim_enabled);
							ImGui::Text("Pitch");
							ImGui::Combo("##typexAA", &ctx.m_settings.anti_aim_typex, "Off\0Ideal\0");
							ImGui::Text("Yaw");
							ImGui::Combo("##typeyAA", &ctx.m_settings.anti_aim_typey, "Off\0Back\0View\0Spin\0");
							ImGui::Text("Yaw Jitter");
							ImGui::Combo("##YawdJitter", &ctx.m_settings.anti_aim_jittering_type, "Off\0Random\0Static\0");
							if (ctx.m_settings.anti_aim_jittering_type != 0)
								ImGui::SliderInt("##jittering", &ctx.m_settings.anti_aim_jittering, -90, 90);
							ImGui::Text("Fake yaw");
							ImGui::Combo("##typeyFAA", &ctx.m_settings.anti_aim_typeyfake, "Off\0Static\0Jitter\0Tank\0");
							if (ctx.m_settings.anti_aim_typeyfake != 0)
								ImGui::SliderInt("", &ctx.m_settings.anti_aim_fake_limit, 0, 60);
							ImGui::Text("Switch fake side key");
							ImGui::SameLine();
							ImGui::Bind("##fakesidekey", &ctx.m_settings.anti_aim_fake_switch.key, ImVec2(100, 20));

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Checkbox(sxor("Fake lag enabled"), &ctx.m_settings.fake_lag_enabled);
							ImGui::Combo("##typeFakelag", &ctx.m_settings.fake_lag_type, "Maximum\0Break\0");
							ImGui::SliderInt("##fakelagval", &ctx.m_settings.fake_lag_value, 0, 15);
							ImGui::Spacing();
							ImGui::Checkbox(sxor("Choke while shooting"), &ctx.m_settings.fake_lag_shooting);

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Text("Hideshots exploit key");
							ImGui::SameLine();
							ImGui::Bind("##Hideshotskey", &ctx.m_settings.aimbot_hideshots_exploit_toggle.key, ImVec2(100, 20));
							
							ImGui::Text("Doubletap exploit key");
							ImGui::SameLine();
							ImGui::Bind("##DTkey", &ctx.m_settings.aimbot_doubletap_exploit_toggle.key, ImVec2(100, 20));

							ImGui::Text("Min damage key");
							ImGui::SameLine();
							ImGui::Bind("##DMGkey", &ctx.m_settings.aimbot_min_damage_override.key, ImVec2(100, 20));

							ImGui::SliderInt(sxor("##Keymindamage"), &ctx.m_settings.aimbot_min_damage_override_val, 0, 101);

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Checkbox(sxor("Quick stop enabled"), &ctx.m_settings.aimbot_autostop);
							ImGui::Checkbox(sxor("Stop only when shooting"), &ctx.m_settings.autostop_only_when_shooting);
							ImGui::Checkbox(sxor("Force Accuracy"), &ctx.m_settings.autostop_force_accuracy);
							ImGui::Combo("##autostop", &ctx.m_settings.autostop_type, "Slow motion\0Full Stop\0");

						}
						ImGui::EndChild();
					}
					ImGui::Columns(1);
					
					//const auto aimbot_section_1 = new section(); {
					//	aimbot_section_1->add_checkbox("enable", &ctx.m_settings.aimbot_enabled);
					//	aimbot_section_1->add_checkbox("silent aimbot", &ctx.m_settings.aimbot_silent_aim);
					//	aimbot_section_1->add_checkbox("auto revolver", &ctx.m_settings.aimbot_auto_revolver);
					//	aimbot_section_1->add_combobox("target selection", &ctx.m_settings.aimbot_target_selection, { "distance", "crosshair", "damage", "health", "height" });
					//	//aimbot_section_1->add_checkbox( "angle limit", &g_settings.fake.angle_limit );
					//	//aimbot_section_1->add_slider( "", 0, 180, &g_settings.fake.angle_limit_value, L"∞", &g_settings.fake.angle_limit );
					//	aimbot_section_1->add_multibox("hitbox", ctx.m_settings.aimbot_hitboxes, { "head", "chest", "body", "arms", "legs", "foot" });
					//	//aimbot_section_1->add_multibox( "hitbox history", g_settings.fake.hitbox_history, { "head", "chest", "body", "arms", "legs", "foot" } );
					//	aimbot_section_1->add_multibox("multi-point", ctx.m_settings.aimbot_multipoint, { "head", "chest", "body", "legs", "foot" });
					//	aimbot_section_1->add_slider("head hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[0], L"%", &ctx.m_settings.aimbot_multipoint[0]);
					//	aimbot_section_1->add_slider("chest hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[1], L"%", &ctx.m_settings.aimbot_multipoint[1]);
					//	aimbot_section_1->add_slider("body hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[2], L"%", &ctx.m_settings.aimbot_multipoint[2]);
					//	aimbot_section_1->add_slider("legs hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[3], L"%", &ctx.m_settings.aimbot_multipoint[3]);
					//	aimbot_section_1->add_slider("foot hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[4], L"%", &ctx.m_settings.aimbot_multipoint[4]);
					//	aimbot_section_1->add_slider("minimal damage", 0, 150, &ctx.m_settings.aimbot_min_damage_viable, L"");
					//	aimbot_section_1->add_checkbox("scale damage on hp", &ctx.m_settings.aimbot_scale_damage_on_hp);
					//	aimbot_section_1->add_checkbox("penetrate walls", &ctx.m_settings.aimbot_autowall);
					//	aimbot_section_1->add_slider("", 0, 150, &ctx.m_settings.aimbot_min_damage, L"", &ctx.m_settings.aimbot_autowall);
					//	aimbot_section_1->add_checkbox("scale penetration damage on hp", &ctx.m_settings.aimbot_wall_scale_damage_on_hp, &ctx.m_settings.aimbot_autowall);
					//	//aimbot_section_1->add_checkbox( "aimbot with knife", &g_settings.fake.aimbot_with_knife );
					//	aimbot_section_1->add_checkbox("aimbot with taser", &ctx.m_settings.aimbot_allow_taser);
					//}
					//aimbot->add(aimbot_section_1);

					//const auto aimbot_section_2 = new section(); {
					//	//aimbot_section_2->add_combobox( "auto scope", &ctx.m_settings.aimbot_autoscope, { "off", "always", "hitchance fail" } );
					//	aimbot_section_2->add_checkbox("automatic scope", &ctx.m_settings.aimbot_autoscope);
					//	aimbot_section_2->add_checkbox("compensate spread", &ctx.m_settings.aimbot_no_spread);
					//	//aimbot_section_2->add_checkbox( "compensate recoil", &g_settings.fake.compensate_recoil );
					//	aimbot_section_2->add_checkbox("hitchance", &ctx.m_settings.aimbot_hitchance);
					//	aimbot_section_2->add_slider("", 0, 100, &ctx.m_settings.aimbot_hitchance_val, L"%", &ctx.m_settings.aimbot_hitchance);
					//	//aimbot_section_2->add_slider("trace validation", 0, 100, &ctx.m_settings.aimbot_accuracy_boost, L"%", &ctx.m_settings.aimbot_hitchance);
					//	//aimbot_doubletap_hitchance_val
					//	//aimbot_section_2->add_checkbox( "position adjustment", &ctx.m_settings.aimbot_position_adjustment );
					//	//aimbot_section_2->add_checkbox( "extended silent shot", &ctx.m_settings.extended_silent_shot);
					//	//aimbot_section_2->add_checkbox( "fakeangles corrections", &ctx.m_settings.aimbot_position_adjustment_old );
					//	//aimbot_section_2->add_checkbox("delay shot", &ctx.m_settings.aimbot_fakelag_prediction);
					//	aimbot_section_2->add_checkbox("prefer body aim", &ctx.m_settings.aimbot_prefer_body);
					//	aimbot_section_2->add_multibox("accuracy options", ctx.m_settings.aimbot_extra_scan_aim, { "baim lethal", "rapidfire baim", "baim spread", "safe limbs", "prefer safepoints", "delay shot" });
					//	//aimbot_section_2->add_multibox( "only body aim", g_settings.fake.only_body_aim, { "always", "health", "fake", "in air" } );
					//	aimbot_section_2->add_keybind("only body aim", &ctx.m_settings.aimbot_bodyaim_key);
					//	aimbot_section_2->add_combobox("doubletap", &ctx.m_settings.aimbot_doubletap_method, { "defensive", "instant" });
					//	aimbot_section_2->add_multibox("instant options", ctx.m_settings.aimbot_extra_doubletap_options, { "extend teleport", "stop before 2nd shot", "break lc", "fakelag" });
					//	//aimbot_section_2->add_slider("teleport distance", 0, 100, &ctx.m_settings.aimbot_tickbase_teleport_speed, L"%");
					//	aimbot_section_2->add_keybind("doubletap key", &ctx.m_settings.aimbot_doubletap_exploit_toggle);
					//	aimbot_section_2->add_keybind("suppress shot key", &ctx.m_settings.aimbot_hideshots_exploit_toggle);
					//	aimbot_section_2->add_slider("doubletap hitchance", 0, 100, &ctx.m_settings.aimbot_doubletap_hitchance_val, L"%", &ctx.m_settings.aimbot_hitchance);
					//	aimbot_section_2->add_keybind("min damage override", &ctx.m_settings.aimbot_min_damage_override);
					//	aimbot_section_2->add_slider("", 0, 101, &ctx.m_settings.aimbot_min_damage_override_val, L"");
					//	//aimbot_section_2->add_multibox("aimbot optimizations", ctx.m_settings.aimbot_low_fps_optimizations, { "optimize enemy scan" });
					//	//aimbot_section_2->add_slider( "", 0, 100, &g_settings.fake.body_aim_min_hp, L"hp", &g_settings.fake.only_body_aim[ 1 ] );
					//}
					//aimbot->add(aimbot_section_2);

					break;
				case 1:
					//draw_skinchanger(alpha);
					
					ImGui::PushItemWidth(110.0f);
					ImGui::PushID(0);
					ImGui::Combo("", &currentCategory, "Global\0Pistols\0Heavy\0SMG\0Rifles\0");
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::PushID(1);

					switch (currentCategory) {
					case 0:
						currentWeapon = 0;
						ImGui::NewLine();
						break;
					case 1: {
						static int currentPistol{ 0 };
						static constexpr const char* pistols[]{ "Global", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver" };

						ImGui::Combo("", &currentPistol, [](void* data, int idx, const char** out_text) {
							if (ctx.m_settings.aimbot[idx ? idx : 35].enabled) {
								static std::string name;
								name = pistols[idx];
								*out_text = name.append(" *").c_str();
							}
							else {
								*out_text = pistols[idx];
							}
							return true;
							}, nullptr, IM_ARRAYSIZE(pistols));

						currentWeapon = currentPistol ? currentPistol : 35;
						break;
					}
					case 2: {
						static int currentHeavy{ 0 };
						static constexpr const char* heavies[]{ "Global", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev" };

						ImGui::Combo("", &currentHeavy, [](void* data, int idx, const char** out_text) {
							if (ctx.m_settings.aimbot[idx ? idx + 10 : 36].enabled) {
								static std::string name;
								name = heavies[idx];
								*out_text = name.append(" *").c_str();
							}
							else {
								*out_text = heavies[idx];
							}
							return true;
							}, nullptr, IM_ARRAYSIZE(heavies));

						currentWeapon = currentHeavy ? currentHeavy + 10 : 36;
						break;
					}
					case 3: {
						static int currentSmg{ 0 };
						static constexpr const char* smgs[]{ "Global", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon" };

						ImGui::Combo("", &currentSmg, [](void* data, int idx, const char** out_text) {
							if (ctx.m_settings.aimbot[idx ? idx + 16 : 37].enabled) {
								static std::string name;
								name = smgs[idx];
								*out_text = name.append(" *").c_str();
							}
							else {
								*out_text = smgs[idx];
							}
							return true;
							}, nullptr, IM_ARRAYSIZE(smgs));

						currentWeapon = currentSmg ? currentSmg + 16 : 37;
						break;
					}
					case 4: {
						static int currentRifle{ 0 };
						static constexpr const char* rifles[]{ "Global", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };

						ImGui::Combo("", &currentRifle, [](void* data, int idx, const char** out_text) {
							if (ctx.m_settings.aimbot[idx ? idx + 23 : 38].enabled) {
								static std::string name;
								name = rifles[idx];
								*out_text = name.append(" *").c_str();
							}
							else {
								*out_text = rifles[idx];
							}
							return true;
							}, nullptr, IM_ARRAYSIZE(rifles));

						currentWeapon = currentRifle ? currentRifle + 23 : 38;
						break;
					}
					}
					ImGui::PopID();
					ImGui::SameLine();
					ImGui::Checkbox("Enabled", &ctx.m_settings.aimbot[currentWeapon].enabled);
					ImGui::Separator();
					ImGui::Columns(2, nullptr, false);
					ImGui::SetColumnOffset(1, 220.0f);
					//ImGui::Checkbox("On key", &ctx.m_settings.aimbot[currentWeapon].onKey);
					//ImGui::SameLine();
					//hotkey(ctx.m_settings.aimbot[currentWeapon].key);
					//ImGui::SameLine();
					//ImGui::PushID(2);
					//ImGui::PushItemWidth(70.0f);
					//ImGui::Combo("", &ctx.m_settings.aimbot[currentWeapon].keyMode, "Hold\0Toggle\0");
					//ImGui::PopItemWidth();
					//ImGui::PopID();
					//ImGui::Checkbox("Aimlock", &ctx.m_settings.aimbot[currentWeapon].aimlock);
					ImGui::Checkbox("Silent", &ctx.m_settings.aimbot[currentWeapon].silent);
					ImGui::Checkbox("Auto Delay", &ctx.m_settings.aimbot[currentWeapon].autodelay);
					ImGui::Checkbox("Friendly fire", &ctx.m_settings.aimbot[currentWeapon].friendlyFire);
					ImGui::Checkbox("Visible only", &ctx.m_settings.aimbot[currentWeapon].visibleOnly);
					ImGui::Checkbox("Scoped only", &ctx.m_settings.aimbot[currentWeapon].scopedOnly);
					ImGui::Checkbox("Ignore flash", &ctx.m_settings.aimbot[currentWeapon].ignoreFlash);
					ImGui::Checkbox("Ignore smoke", &ctx.m_settings.aimbot[currentWeapon].ignoreSmoke);
					//ImGui::Checkbox("Auto shot", &ctx.m_settings.aimbot[currentWeapon].autoShot);
					//ImGui::Checkbox("Auto scope", &ctx.m_settings.aimbot[currentWeapon].autoScope);
					ImGui::Combo("Bone", &ctx.m_settings.aimbot[currentWeapon].bone, "Nearest\0Best damage\0Head\0Neck\0Sternum\0Chest\0Stomach\0Pelvis\0");
					ImGui::NextColumn();
					ImGui::PushItemWidth(240.0f);
					ImGui::SliderFloat("Fov", &ctx.m_settings.aimbot[currentWeapon].fov, 0.0f, 20.0f, "%.2f");
					ImGui::SliderFloat("Smooth", &ctx.m_settings.aimbot[currentWeapon].smooth, 1.0f, 100.0f, "%.2f");
					ImGui::SliderFloat("Rcs Fov", &ctx.m_settings.aimbot[currentWeapon].rcsfov, 1.0f, 100.0f, "%.2f");
					ImGui::SliderFloat("Silent Fov", &ctx.m_settings.aimbot[currentWeapon].silentfov, 1.0f, 100.0f, "%.2f");
					ImGui::SliderFloat("Kill Delay", &ctx.m_settings.aimbot[currentWeapon].killdelay, 0.0f, 5.0f, "%.2f");
					//ImGui::SliderFloat("Max aim inaccuracy", &ctx.m_settings.aimbot[currentWeapon].maxAimInaccuracy, 0.0f, 1.0f, "%.5f");
					//ImGui::SliderFloat("Max shot inaccuracy", &ctx.m_settings.aimbot[currentWeapon].maxShotInaccuracy, 0.0f, 1.0f, "%.5f");
					//ImGui::InputInt("Min damage", &ctx.m_settings.aimbot[currentWeapon].minDamage);
					//ctx.m_settings.aimbot[currentWeapon].minDamage = std::clamp(ctx.m_settings.aimbot[currentWeapon].minDamage, 0, 250);
					//ImGui::Checkbox("Killshot", &ctx.m_settings.aimbot[currentWeapon].killshot);
					//ImGui::Checkbox("Between shots", &ctx.m_settings.aimbot[currentWeapon].betweenShots);
					ImGui::Columns(1);

					break;
				case 2:
					//draw_skinchanger(alpha);
					ImGui::Columns(2, NULL, true);
					{
						ImGui::BeginChild("##visualsplayers");
						ImGui::Checkbox("Visuals enabled", &ctx.m_settings.visuals_enabled);
						ImGui::Checkbox("Esp box", &ctx.m_settings.esp_box);
						ImGui::ColorPickerBox("#nigbox", ctx.m_settings.box_enemy_color, nullptr, nullptr, nullptr);
						//players_section_1->add_colorpicker("box enemy color", &ctx.m_settings.box_enemy_color, &ctx.m_settings.esp_box[0]);
						//players_section_1->add_checkbox( "dormant enemies", &g_settings.player_esp.dormant_enemies );
						ImGui::Checkbox("Dormant", &ctx.m_settings.esp_dormant);
						ImGui::Checkbox("Offscreen arrows", &ctx.m_settings.esp_offscreen);
						ImGui::ColorPickerBox("#nigarrows", ctx.m_settings.colors_esp_offscreen, nullptr, nullptr, nullptr);

						//players_section_1->add_colorpicker("", &ctx.m_settings.colors_esp_offscreen);
						//players_section_1->add_slider("offscreen esp size", 0, 60, &ctx.m_settings.esp_arrows_size, L"");
						//players_section_1->add_slider("offscreen esp distance", 0, 100, &ctx.m_settings.esp_arrows_distance, L"%");
						ImGui::Checkbox("Name", &ctx.m_settings.esp_name);
						ImGui::ColorPickerBox("#nigname", ctx.m_settings.colors_esp_name, nullptr, nullptr, nullptr);

						ImGui::Checkbox("Health", &ctx.m_settings.esp_health);
						ImGui::ColorPickerBox("#nighealth", ctx.m_settings.colors_esp_health, nullptr, nullptr, nullptr);

						ImGui::Checkbox("Weapon", &ctx.m_settings.esp_weapon);

						ImGui::Checkbox("Weapon ammo", &ctx.m_settings.esp_weapon);
						ImGui::ColorPickerBox("#nigammo", ctx.m_settings.colors_esp_ammo, nullptr, nullptr, nullptr);

						//players_section_1->add_colorpicker("name color", &ctx.m_settings.colors_esp_name);
						//players_section_1->add_multibox("flags enemy", ctx.m_settings.esp_flags[0], { "armor", "last place", "scope", "debug", "pin", "c4/hostage" });
						//players_section_1->add_multibox("weapon", ctx.m_settings.esp_weapon, { "enemy", "friendly" });
						//players_section_1->add_combobox("", &ctx.m_settings.esp_weapon_type, { "text", "icon" });
						//players_section_1->add_checkbox("ammo", &ctx.m_settings.esp_weapon_ammo);
						//players_section_1->add_colorpicker("", &ctx.m_settings.colors_esp_ammo);

						ImGui::Checkbox("Skeleton", &ctx.m_settings.esp_skeleton);
						ImGui::ColorPickerBox("#nigton", ctx.m_settings.colors_skeletons_enemy, nullptr, nullptr, nullptr);

						ImGui::Checkbox("Glowing", &ctx.m_settings.esp_glow);
						ImGui::SameLine();
						ImGui::Checkbox("Color based on hp", &ctx.m_settings.colors_glow_hp);
						ImGui::ColorPickerBox("#nigglow", ctx.m_settings.colors_glow_enemy, nullptr, nullptr, nullptr);
						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();
						ImGui::Checkbox("Autowall crosshair", &ctx.m_settings.visuals_autowall_crosshair);
						ImGui::Checkbox("Bullet impacts", &ctx.m_settings.visuals_draw_local_impacts);
						ImGui::Checkbox("Grenade Prediction", &ctx.m_settings.misc_grenade_preview);
			
						ImGui::Checkbox("Hitmarker", &ctx.m_settings.misc_visuals_indicators_2[0]);
						ImGui::SameLine();
						ImGui::Checkbox("Sound", &ctx.m_settings.misc_hitsound_type);
						ImGui::Spacing();
						ImGui::Separator();
						ImGui::Spacing();
						ImGui::Checkbox("Remove visual recoil", &ctx.m_settings.visuals_no_recoil);
						ImGui::Checkbox("Remove ingame post processing", &ctx.m_settings.visuals_no_postprocess);
						ImGui::Checkbox("Remove scope", &ctx.m_settings.visuals_no_scope);
						ImGui::Checkbox("Remove 1st zoom level", &ctx.m_settings.visuals_no_first_scope);
						ImGui::Checkbox("Remove smoke", &ctx.m_settings.visuals_no_smoke);
						ImGui::Checkbox("Remove flash", &ctx.m_settings.visuals_no_flash);
						ImGui::Checkbox("Override fov", &ctx.m_settings.misc_override_fov);
						ImGui::SameLine();
						cursor_pos_y = ImGui::GetCursorPosY();

						ImGui::SetCursorPosY(cursor_pos_y + 8);
						ImGui::SliderInt("##oFov", &ctx.m_settings.misc_override_fov_val, -20, 60);
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::EndChild();
					}
					ImGui::NextColumn();
					{
						ImGui::BeginChild("##visualsworld");
						{
						//	ImGui::Spacing();
							ImGui::Checkbox("World weapons", &ctx.m_settings.esp_world_weapons);
							ImGui::Checkbox("World grenades", &ctx.m_settings.esp_world_nades);
							ImGui::Checkbox("World c4/hostage", &ctx.m_settings.esp_world_bomb);
							ImGui::Checkbox("World nightmode", &ctx.m_settings.misc_visuals_world_modulation[1]);
							ImGui::ColorPickerBox("#nigmode", ctx.m_settings.colors_world_color, nullptr, nullptr, nullptr);
							ImGui::Checkbox("World fullbright", &ctx.m_settings.misc_visuals_world_modulation[0]);
							ImGui::SliderInt("World props alpha", &ctx.m_settings.visuals_props_alpha, 0, 100);

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Checkbox("Local bullets tracer", &ctx.m_settings.visuals_draw_local_beams);//local_beams_color
							ImGui::ColorPickerBox("#nigtrace", ctx.m_settings.local_beams_color, nullptr, nullptr, nullptr);
							ImGui::Checkbox("Draw hitbox on aimbot", &ctx.m_settings.misc_visuals_hitboxes);//local_beams_color
							ImGui::ColorPickerBox("#nigaimbox", ctx.m_settings.misc_visuals_hitboxes_color, nullptr, nullptr, nullptr);

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Checkbox("Filled player models", &ctx.m_settings.chams_enemy);
							ImGui::ColorPickerBox("#nigchamviable", ctx.m_settings.colors_chams_enemy_viable, nullptr, nullptr, nullptr);
							ImGui::Checkbox("Visible behind wall", &ctx.m_settings.chams_walls);
							ImGui::ColorPickerBox("#nigchamshidden", ctx.m_settings.colors_chams_enemy_hidden, nullptr, nullptr, nullptr);
							ImGui::Checkbox("Filled local model", &ctx.m_settings.chams_local_player);
							ImGui::ColorPickerBox("#nigchamslocal", ctx.m_settings.colors_chams_local, nullptr, nullptr, nullptr);
							ImGui::Text(sxor("Override player material"));
							ImGui::Combo("##playamaterial", &ctx.m_settings.menu_chams_type, "Flat\0Filled\0Glowing\0Model Glowing\0");
							ImGui::Spacing();

							ImGui::SliderInt("Local scope alpha", &ctx.m_settings.scope_transparency, 0, 100);

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();
							//ImGui::Checkbox("Keybinds list", &ctx.m_settings.esp_world_weapons);
							ImGui::Checkbox("Status list", &ctx.m_settings.misc_status_list);
							ImGui::Checkbox("Spectators list", &ctx.m_settings.misc_spectators_list);
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();
							ImGui::Checkbox("Bloom enabled", &ctx.m_settings.visuals_bloom_enabled);

							ImGui::SliderInt("Bloom exposure", &ctx.m_settings.visuals_bloom_exposure, 0, 100);
							ImGui::SliderInt("Bloom scale", &ctx.m_settings.visuals_bloom_scale, 0, 100);

						}
						ImGui::EndChild();
					}
					ImGui::Columns(1);

					/*
					visuals_section_2->add_checkbox("remove visual recoil", &ctx.m_settings.visuals_no_recoil);
				visuals_section_2->add_checkbox("remove flash/smoke", &ctx.m_settings.visuals_no_flashsmoke);
				visuals_section_2->add_checkbox("remove post process", &ctx.m_settings.visuals_no_postprocess);
				visuals_section_2->add_checkbox("remove scope", &ctx.m_settings.visuals_no_scope);
				visuals_section_2->add_checkbox("override fov", &ctx.m_settings.misc_override_fov);
				visuals_section_2->add_slider("", -30, 110, &ctx.m_settings.misc_override_fov_val, L"∞");
				visuals_section_2->add_checkbox("ignore on first zoom lvl", &ctx.m_settings.visuals_no_first_scope);
				visuals_section_2->add_multibox("indicators", ctx.m_settings.misc_visuals_indicators_2, { "hitmarker", "fake", "lag compensation", "bomb timer", "defuse timer", "aimbot settings", "min damage key", "side control", "current exploit", "desync side control"});
				visuals_section_2->add_checkbox("local bullet impacts", &ctx.m_settings.visuals_draw_local_impacts);
				visuals_section_2->add_checkbox("local bullet tracers", &ctx.m_settings.visuals_draw_local_beams);
				visuals_section_2->add_colorpicker("", &ctx.m_settings.local_beams_color);
				visuals_section_2->add_checkbox("force third person", &ctx.m_settings.visuals_tp_force);
				visuals_section_2->add_keybind("", &ctx.m_settings.visuals_tp_key);
				visuals_section_2->add_slider("third person distance", 30, 400, &ctx.m_settings.visuals_tp_dist, L"");
				visuals_section_2->add_checkbox("force third person if dead", &ctx.m_settings.visuals_tp_force_dead);
				visuals_section_2->add_checkbox("force crosshair", &ctx.m_settings.visuals_force_crosshair);
				visuals_section_2->add_checkbox("force engine radar", &ctx.m_settings.misc_engine_radar);
				visuals_section_2->add_checkbox("force grenade preview", &ctx.m_settings.misc_grenade_preview);
				visuals_section_2->add_checkbox("clan tag", &ctx.m_settings.visuals_clantag);
					*/

					//visuals_section_1->add_colorpicker("color", &ctx.m_settings.world_esp_color, false);

					//players_section_2->add_colorpicker("enemy color", &ctx.m_settings.colors_skeletons_enemy, &ctx.m_settings.esp_skeleton[0]);
					//players_section_2->add_colorpicker("enemy color", &ctx.m_settings.colors_glow_enemy, &ctx.m_settings.esp_glow[0]);

					break;
				/*case 3:
					draw_skinchanger(alpha);
					break;*/
				case 3:
				{
					//const auto movement_section_1 = new section(); {
					//	movement_section_1->add_checkbox("automatic jump", &ctx.m_settings.misc_bhop);
					//	movement_section_1->add_checkbox("duck in air", &ctx.m_settings.aimbot_anti_aim_air_duck);
					//	movement_section_1->add_checkbox("automatic strafe", &ctx.m_settings.misc_autostrafer);
					//	movement_section_1->add_checkbox("wasd strafer", &ctx.m_settings.misc_autostrafer_wasd);
					//	movement_section_1->add_slider("wasd speed", 0, 100, &ctx.m_settings.misc_autostrafer_retrack, L"%");

					//	movement_section_1->add_blank();
					//	movement_section_1->add_checkbox("auto stop", &ctx.m_settings.aimbot_autostop);
					//	movement_section_1->add_multibox("auto stop options", ctx.m_settings.aimbot_autostop_options, { "min walk", /*"duck",*/ "move between shots", "ignore molotov", "force accuracy" });

					//	movement_section_1->add_combobox("slow walk options", &ctx.m_settings.anti_aim_slow_walk_type, { "favor desync", "favor random" });
					//	movement_section_1->add_slider("slow walk limit", 0, 100, &ctx.m_settings.anti_aim_slow_walk_speed, L"%");
					//	movement_section_1->add_slider("speed desync amount", 0, 100, &ctx.m_settings.anti_aim_slow_walk_desync, L"%");

					//	movement_section_1->add_combobox("leg movement options", &ctx.m_settings.anti_aim_leg_movement, { "default", "always slide", "never slide", "LEDSYNC" });
					//}
					//movement->add(movement_section_1);

					//const auto movement_section_2 = new section(); {
					//	movement_section_2->add_keybind("auto peek", &ctx.m_settings.anti_aim_autopeek_key);
					//	movement_section_2->add_keybind("fake duck", &ctx.m_settings.anti_aim_fakeduck_key);
					//	movement_section_2->add_keybind("slow walk", &ctx.m_settings.anti_aim_slowwalk_key);
					//	movement_section_2->add_keybind("stop time", &ctx.m_settings.anti_aim_timestop_key);
					//	movement_section_2->add_keybind("manual anti-aim left", &ctx.m_settings.anti_aim_yaw_left_switch, false);
					//	movement_section_2->add_keybind("manual anti-aim right", &ctx.m_settings.anti_aim_yaw_right_switch, false);
					//	movement_section_2->add_keybind("manual anti-aim 180", &ctx.m_settings.anti_aim_yaw_backward_switch, false);
					//}
					//movement->add(movement_section_2);
					ImGui::Columns(2, NULL, true);
					{
						ImGui::BeginChild("##misclocal");
						{
							ImGui::Checkbox(sxor("Auto bhop"), &ctx.m_settings.misc_bhop);
							ImGui::Checkbox(sxor("Auto strafe"), &ctx.m_settings.misc_autostrafer);
							ImGui::SameLine();
							ImGui::Checkbox(sxor("WASD"), &ctx.m_settings.misc_autostrafer_wasd);
							ImGui::Spacing();
							ImGui::Checkbox(sxor("Force crosshair"), &ctx.m_settings.visuals_force_crosshair);
							ImGui::Checkbox(sxor("Force enemy on game radar"), &ctx.m_settings.misc_engine_radar);
							ImGui::Checkbox(sxor("Force inventory in-game"), &ctx.m_settings.misc_unlock_inventory);
							ImGui::Checkbox(sxor("Force knife left hand"), &ctx.m_settings.misc_knife_hand_switch);
							ImGui::Checkbox(sxor("Force killfeed"), &ctx.m_settings.misc_preserve_killfeed);
							ImGui::SliderInt(sxor("Force aspect ratio"), &ctx.m_settings.misc_aspect_ratio, 0, 100);
							ImGui::Spacing();
							ImGui::Checkbox(sxor("Force thirdperson"), &ctx.m_settings.visuals_tp_force);
							ImGui::SameLine();
							ImGui::Bind("##tpkey", &ctx.m_settings.visuals_tp_key.key, ImVec2(100, 20));
							
							if (ctx.m_settings.visuals_tp_force)
							ImGui::SliderInt(sxor("##tpdistance"), &ctx.m_settings.visuals_tp_dist, 30, 300);
							ImGui::Spacing();
							ImGui::Text(sxor("Auto peek"));
							//ImGui::SameLine();
							ImGui::Bind("##apkey", &ctx.m_settings.anti_aim_autopeek_key.key, ImVec2(100, 20));
							ImGui::Spacing();
							ImGui::Text(sxor("Fake duck"));
							//ImGui::SameLine();
							ImGui::Bind("##fdkey", &ctx.m_settings.anti_aim_fakeduck_key.key, ImVec2(100, 20));

							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Checkbox(sxor("Log misses"), &ctx.m_settings.misc_notifications[2]);
							ImGui::SameLine();
							ImGui::Checkbox(sxor("Log shots"), &ctx.m_settings.misc_notifications[3]);
							
							ImGui::Checkbox(sxor("Log damage"), &ctx.m_settings.misc_notifications[1]);
							ImGui::SameLine();
							ImGui::Checkbox(sxor("Log purchases"), &ctx.m_settings.misc_notifications[4]);
							//misc_section_1->add_multibox("notifications", ctx.m_settings.misc_notifications, { "bomb info", "damage given", "missed shot", "shot debug", "purchases", "damage received" });
							ImGui::Spacing();
							ImGui::Separator();
							ImGui::Spacing();

							ImGui::Checkbox(sxor("Autobuy"), &ctx.m_settings.misc_autobuy_enabled);
							if (ctx.m_settings.misc_autobuy_enabled) {
								ImGui::Text(sxor("Autobuy primary"));
								ImGui::Combo(sxor("##Autoprimary"), &ctx.m_settings.misc_autobuy_primary, "none\0AK47/M4A1\0AWP\0SCAR20/G3\0SSG-08\0\0");
								ImGui::Text(sxor("Autobuy secondary"));
								ImGui::Combo(sxor("##Autoecondary"), &ctx.m_settings.misc_autobuy_secondary, "none\0Deagle/R8\0Tec9/FiveSeven\0Dual berettas\0P250/CZ-74\0");
								ImGui::Separator();
							
								ImGui::Text(sxor("Autobuy minumum money"));
								ImGui::SliderInt(sxor("##musor"), &ctx.m_settings.misc_autobuy_money_limit, 0, 10000);
								ImGui::Spacing();
								ImGui::Separator();
								//misc_section_1->add_multibox("", ctx.m_settings.misc_autobuy_etc, { "taser", "armor", "kit", "smoke", "hegrenade", "flashbang", "flashbang", "molotov" });
								ImGui::Checkbox(sxor("HE"), &ctx.m_settings.misc_autobuy_etc[4]);
								ImGui::SameLine();
								ImGui::Checkbox(sxor("Smoke"), &ctx.m_settings.misc_autobuy_etc[3]);
								ImGui::SameLine();
								ImGui::Checkbox(sxor("Flash"), &ctx.m_settings.misc_autobuy_etc[5]);
								ImGui::SameLine();
								ImGui::Checkbox(sxor("Molotov"), &ctx.m_settings.misc_autobuy_etc[7]);
								ImGui::Separator();
								ImGui::Checkbox(sxor("Armor"), &ctx.m_settings.misc_autobuy_etc[1]);
								ImGui::SameLine();
								ImGui::Checkbox(sxor("Taser"), &ctx.m_settings.misc_autobuy_etc[0]);
								ImGui::SameLine();
								ImGui::Checkbox(sxor("Kit"), &ctx.m_settings.misc_autobuy_etc[2]);
							}
						}
						ImGui::EndChild();
					}
					ImGui::NextColumn();
					{
						ImGui::BeginChild("##cfgmisc");
						{
							/*ImGui::Text(sxor("Menu color"));
							if (ImGui::ColorPickerBox("##nigcolor", ctx.m_settings.colors_glow_enemy, nullptr, nullptr, nullptr))
							{

							}*/

							ImGui::Text(sxor("Config selection"));
							ImGui::Combo("##cfgselct", &g_settings.cur_cfg, "Legit\0Rage\0Hvh\0MM hvh\0Semi-rage\0");

							if (ImGui::Button(sxor("save config")))
								feature::misc->save_cfg();

							if (ImGui::Button(sxor("load config"))) {
								feature::misc->load_cfg();
								if (ctx.m_settings.skinchanger_enabled) ctx.updated_skin = true;
							}

							ImGui::Spacing();
							ImGui::Spacing();
							ImGui::Spacing();

							if (ImGui::Button(sxor("reset config")))
							{
								const size_t settings_size = sizeof(c_variables);
								for (int i = 0; i < settings_size; i++)
								{
									byte &current_byte = *reinterpret_cast<byte*>(uintptr_t(&ctx.m_settings) + i);
									current_byte = 0;
								}

								//#TODO def settings
							}
						}
						ImGui::EndChild();
					}
					ImGui::Columns(1);

					//ImGui::Checkbox(sxor("Force player model"), &ctx.m_settings.skins_player_model);
					//skins_section_1->add_checkbox("change player model", &ctx.m_settings.skins_player_model);
					//skins_section_1->add_combobox("T agent model", &ctx.m_settings.skins_player_model_type_t, models_to_change);
					//skins_section_1->add_combobox("CT agent model", &ctx.m_settings.skins_player_model_type_ct, models_to_change);
					break;
				}
				default:
					break;
				}

				ImGui::EndChild();
			}

			was_hovered = is_hovered;
		}

		if (alpha != 1.f)
			init(1.f); //н≥чого кр≥м меню не повинно зм≥нювати прозор≥сть

		ImGui::End();
	}

}