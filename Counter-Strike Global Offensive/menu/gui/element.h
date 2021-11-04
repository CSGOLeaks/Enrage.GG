#pragma once
#include <vector>
#include <string>
#include "../../sdk.hpp"

// element - controls, containers, anything gui based
class element {
public:
	// basic stuff all elements have
	std::string name{ };
	Vector2D pos, size;

	// element running shit
	virtual void draw( ) = 0;
	virtual void think( ) = 0;
	virtual void extra( ) = 0; // extra shit (basically useless)
	virtual void on_cfg_update() = 0;
	virtual void on_unload() = 0;

	// parent & children
	element* parent = nullptr;
	std::vector< element* > children{ };

	// oh yes antario best
	bool usable;

	// add a element
	void add( element* p ) {
		p->parent = this;
		children.emplace_back( p );
	}

	// set the element pos
	virtual void set_position( const Vector2D& position ) {
		this->pos = position;
	}

	// set the elements size
	virtual void set_size( const Vector2D& size ) {
		this->size = size;
	}
};