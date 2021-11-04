#include "sdk.hpp"
#include "hooked.hpp"
#include <Windows.h>
#include <cstdio>
#include <TlHelp32.h>
#include "menu.hpp"
#include "menu/input/input.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


WNDPROC Hooked::oldWindowProc;
void OpenMenu();

LRESULT __stdcall Hooked::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	bool lol = false;

	switch (uMsg) {
	case WM_LBUTTONDOWN:
		ctx.pressed_keys[VK_LBUTTON] = true;
		feature::menu->_mouse_pressed = true;
		lol = true;
		break;
	case WM_LBUTTONUP:
		ctx.pressed_keys[VK_LBUTTON] = false;
		feature::menu->_mouse_pressed = false;
		lol = true;
		break;
	case WM_RBUTTONDOWN:
		ctx.pressed_keys[VK_RBUTTON] = true;
		lol = true;
		break;
	case WM_RBUTTONUP:
		ctx.pressed_keys[VK_RBUTTON] = false;
		lol = true;
		break;
	case WM_MBUTTONDOWN:
		ctx.pressed_keys[VK_MBUTTON] = true;
		lol = true;
		break;
	case WM_MBUTTONUP:
		ctx.pressed_keys[VK_MBUTTON] = false;
		lol = true;
		break;
	case WM_MOUSEMOVE:
		feature::menu->_cursor_position.x = static_cast<signed short>(lParam);
		feature::menu->_cursor_position.y = static_cast<signed short>(lParam >> 16);
		lol = true;
		break;
	case WM_XBUTTONDOWN:
	{
		const UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			ctx.pressed_keys[VK_XBUTTON1] = true;
		}
		else if (button == XBUTTON2)
		{
			ctx.pressed_keys[VK_XBUTTON2] = true;
		}
		lol = true;
		break;
	}
	case WM_XBUTTONUP:
	{
		const UINT button = GET_XBUTTON_WPARAM(wParam);
		if (button == XBUTTON1)
		{
			ctx.pressed_keys[VK_XBUTTON1] = false;
		}
		else if (button == XBUTTON2)
		{
			ctx.pressed_keys[VK_XBUTTON2] = false;
		}
		lol = true;
		break;
	}
	case WM_MOUSEWHEEL:
	{
		//DWORD x = GET_WHEEL_DELTA_WPARAM(wParam);

		ctx.scroll_value = static_cast<short>(HIWORD(wParam)) < 0 ? -1 : 1;

		lol = true;
		break;
	}
	case WM_KEYDOWN:
		ctx.pressed_keys[wParam] = true;
		lol = true;
		break;
	case WM_KEYUP:
		ctx.pressed_keys[wParam] = false;
		lol = true;
		break;
	/*case WM_SYSKEYDOWN:
		ctx.pressed_keys[18] = true;
		lol = true;
		break;
	case WM_SYSKEYUP:
		ctx.pressed_keys[18] = false;
		lol = true;
		break;*/
	default: break;
	}

	ctx.pressed_keys[18] = GetAsyncKeyState(VK_LMENU) || GetAsyncKeyState(VK_RMENU);

	OpenMenu();

	if (feature::menu->_menu_opened && lol) {
		//return ((csgo.m_engine()->IsInGame() && ctx.m_local() && !ctx.m_local()->IsDead()) ? CallWindowProc(oldWindowProc, hWnd, uMsg, wParam, lParam) : true);
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
			return true;
	}

	g_menuinput.wndproc(uMsg, wParam, lParam);

	return CallWindowProc(oldWindowProc, hWnd, uMsg, wParam, lParam);
}

void OpenMenu()
{
	static bool is_down = false;
	static bool is_clicked = false;

	if (ctx.pressed_keys[VK_INSERT])
	{
		is_clicked = false;
		is_down = true;
	}
	else if (!ctx.pressed_keys[VK_INSERT] && is_down)
	{
		is_clicked = true;
		is_down = false;
	}
	else
	{
		is_clicked = false;
		is_down = false;
	}

	if (is_clicked)
	{
		feature::menu->_menu_opened = !feature::menu->_menu_opened;

		if (feature::menu->_menu_opened)
			csgo.m_input_system()->EnableInput(feature::menu->_menu_opened);
	}
}