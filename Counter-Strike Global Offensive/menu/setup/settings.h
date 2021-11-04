#pragma once
// containers
#include "../gui/container/window.h"
#include "../gui/container/tab.h"
#include "../gui/container/section.h"

//controls
#include "../gui/controls/checkbox.h"
#include "../gui/controls/slider.h"
#include "../gui/controls/combobox.h"
#include "../gui/controls/multibox.h"
#include "../gui/controls/button.h"
#include "../gui/controls/keybind.h"
#include "../gui/controls/colorpicker.h"

// settings for everything
class settings {
public:
	bool menu_fullfade = false;
	bool shown = true;
	
	//Color test_color = { 255, 0, 0 };

	//class menu {
	//public:
	//	Color menu_color = { 180, 1, 45 };
	//	std::wstring username = L"L3D451R7"; //� temp!
	//};
	//menu menu;

	//class fake {
	//public:
	//	bool enable;
	//	bool silent_aimbot;
	//	int target_selection;
	//	bool angle_limit;
	//	int angle_limit_value = 180;
	//	bool hitboxes[ 5 ];
	//	bool hitbox_history[ 5 ];
	//	bool multipoint[ 5 ];
	//	int head_multipoint = 90;
	//	int body_hitbox_scale = 50;
	//	int minimal_damage = 40;
	//	bool scale_damage_on_hp;
	//	bool penetrate_walls;
	//	int penetration_damage = 30;
	//	bool scale_penetration_damage_on_hp;
	//	bool aimbot_with_knife;
	//	bool aimbot_with_taser;

	//	int autoscope;
	//	bool compensate_recoil;
	//	bool hitchance;
	//	int hitchance_value = 50;
	//	bool predict_fake_lag;
	//	bool correct_anti_aim;
	//	bool prefer_body_aim[ 5 ];
	//	bool only_body_aim[ 5 ];
	//	int body_aim_key = 0;
	//	int body_aim_min_hp = 20;
	//};
	//fake fake;

	//class player_esp {
	//public:
	//	bool boxes[ 2 ];
	//	Color box_enemy_color = { 148, 197, 56 };
	//	Color box_friendly_color = { 250, 197, 2 };
	//	bool dormant_enemies;
	//	bool enemy_offscreen_esp;
	//	Color enemy_offscreen_esp_color = { 255, 255, 255 };
	//};
	//player_esp player_esp;
	int cur_cfg;
};

extern settings g_settings;