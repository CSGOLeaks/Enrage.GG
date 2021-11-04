#pragma once
#include "Windows.h"
#include "../../sdk.hpp"

class input {
public:
	void wndproc( UINT message, WPARAM wparam, LPARAM lparam );

	virtual bool inbounds( const Vector2D& pos, const Vector2D& pos2 );

	void pre();

	Vector2D mouse_pos;
	Vector2D get_pos( ) const { return mouse_pos; };

	void think( );

	bool is_key_down( int key );
	bool is_key_pressed( int key );
	bool is_key_released( int key );

	int scroll_amount;

private:
	bool previous_key_state[ 256 ];

	bool tabbed_out;
};

extern input g_menuinput;