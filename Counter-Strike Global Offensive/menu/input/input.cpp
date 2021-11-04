#include "input.h"
#include "../../sdk.hpp"
#include "../../source.hpp"
#include "../../menu.hpp"

// menu input mango
input g_menuinput;

#define GET_X_LPARAM(lp)                        (int(short(LOWORD(lp))))
#define GET_Y_LPARAM(lp)                        (int(short(HIWORD(lp))))

void input::wndproc( UINT message, WPARAM wparam, LPARAM lparam ) {
	// literally only using this for the mouse pos
	switch ( message )
	{
	case WM_MOUSEMOVE:
	case WM_NCMOUSEMOVE:
		mouse_pos = Vector2D( float( GET_X_LPARAM( lparam ) ), float( GET_Y_LPARAM( lparam ) ) );
		break;
	case WM_MOUSEWHEEL:
		scroll_amount = int( float( GET_WHEEL_DELTA_WPARAM( wparam ) ) / float( WHEEL_DELTA ) );
		break;
	}
}

bool input::inbounds( const Vector2D& pos, const Vector2D& pos2 ) {
	return  get_pos( ).x > pos.x &&
		get_pos( ).y > pos.y &&
		get_pos( ).x < pos2.x &&
		get_pos( ).y < pos2.y;
}

void input::pre()
{
	/*static const auto window_handle = FindWindowA(nullptr, "Counter-Strike: Global Offensive");

	RECT client_size;
	GetClientRect(window_handle, &client_size);
	const auto screen_width = client_size.right - client_size.left, screen_height = client_size.bottom - client_size.top;*/

	/*POINT mouse_position;
	GetCursorPos(&mouse_position);
	ScreenToClient(window_handle, &mouse_position);*/

	// the scuffed way to only work inside csgo
	tabbed_out = false;// mouse_position.x < 0L || mouse_position.x > screen_width || mouse_position.y < 0L || mouse_position.y > screen_height;

	mouse_pos = feature::menu->_cursor_position;
}

void input::think() {
	// thanks alpha lol

	static int lol = csgo.m_globals()->tickcount;

	if (lol != csgo.m_globals()->tickcount && feature::menu->_menu_opened) {
		for (int i = 0; i < 256; i++)
			this->previous_key_state[i] = ctx.pressed_keys[i];
	}

	feature::menu->_mouse_pressed = false;
}

// our scuffed input system ------
bool input::is_key_down( int key ) {
	if ( tabbed_out )
		return false;
	
	return ctx.pressed_keys[ key ];
}

bool input::is_key_pressed( int key ) {
	if ( tabbed_out )
		return false;
	
	return !this->previous_key_state[ key ] && ctx.pressed_keys[ key ];
}

bool input::is_key_released( int key ) {
	if ( tabbed_out )
		return false;
	
	return this->previous_key_state[ key ] && !ctx.pressed_keys[ key ];
}