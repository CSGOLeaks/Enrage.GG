#pragma once
#include "../../menu/menu_v2.h"

class checkbox : public element {
public:
	checkbox( std::string name, bool* value, bool* shown );

	void draw( ) override;
	void think( ) override;
	void extra( ) override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	bool* value;
	bool* shown;
	bool hovered;
};

class blank : public element {
public:
	blank(bool* shown);

	void draw() override;
	void think() override;
	void extra() override;
	void on_cfg_update() override;
	void on_unload() override;
private:
	bool* value;
	bool* shown;
	bool hovered = false;
};