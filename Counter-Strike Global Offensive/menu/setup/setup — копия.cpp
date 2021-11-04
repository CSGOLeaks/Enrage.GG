#include "../menu/menu_v2.h"
#include "settings.h"
#include "../../source.hpp"
#include "../../misc.hpp"
#include "../../music_player.hpp"
//void test( ) { printf( "pog\n" ); }

static std::vector<std::string> models_to_change = {
	sxor("none"),
	sxor("Local СT Agent"),
	sxor("Local T Agent"),
	sxor("Blackwolf | Sabre"),
	sxor("Rezan The Ready | Sabre"),
	sxor("Maximus | Sabre"),
	sxor("Dragomir | Sabre"),
	sxor("Lt. Commander Ricksaw | NSWC SEAL"),
	sxor("'Two Times' McCoy | USAF TACP"),
	sxor("Seal Team 6 Soldier | NSWC SEAL"),
	sxor("3rd Commando Company | KSK"),
	sxor("'The Doctor' Romanov | Sabre"),
	sxor("Michael Syfers  | FBI Sniper"),
	sxor("Markus Delrow | FBI HRT"),
	sxor("Operator | FBI SWAT"),
	sxor("Slingshot | Phoenix"),
	sxor("Enforcer | Phoenix"),
	sxor("Soldier | Phoenix"),
	sxor("The Elite Mr. Muhlik | Elite Crew"),
	sxor("Prof. Shahmat | Elite Crew"),
	sxor("Osiris | Elite Crew"),
	sxor("Ground Rebel  | Elite Crew"),
	sxor("Special Agent Ava | FBI"),
	sxor("B Squadron Officer | SAS"),
};


void mmenu::setup( ) {
//	VIRTUALIZER_FISH_LITE_START;
//	VIRTUALIZER_STR_ENCRYPT_START;
//
//	const auto main_window = new window( Vector2D( 50, 50), Vector2D( 630, 500 ) ); {
//		const auto aimbot = new tab( "aimbot" ); {
//			const auto aimbot_section_1 = new section(); {
//				aimbot_section_1->add_checkbox("enable", &ctx.m_settings.aimbot_enabled);
//				aimbot_section_1->add_checkbox("silent aimbot", &ctx.m_settings.aimbot_silent_aim);
//				aimbot_section_1->add_checkbox("auto revolver", &ctx.m_settings.aimbot_auto_revolver);
//				aimbot_section_1->add_combobox("target selection", &ctx.m_settings.aimbot_target_selection, { "distance", "crosshair", "damage", "health", "height" });
//				//aimbot_section_1->add_checkbox( "angle limit", &g_settings.fake.angle_limit );
//				//aimbot_section_1->add_slider( "", 0, 180, &g_settings.fake.angle_limit_value, L"°", &g_settings.fake.angle_limit );
//				aimbot_section_1->add_multibox("hitbox", ctx.m_settings.aimbot_hitboxes, { "head", "chest", "body", "arms", "legs", "foot" });
//				//aimbot_section_1->add_multibox( "hitbox history", g_settings.fake.hitbox_history, { "head", "chest", "body", "arms", "legs", "foot" } );
//				aimbot_section_1->add_multibox("multi-point", ctx.m_settings.aimbot_multipoint, { "head", "chest", "body", "legs", "foot" });
//				aimbot_section_1->add_slider("head hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[0], L"%", &ctx.m_settings.aimbot_multipoint[0]);
//				aimbot_section_1->add_slider("chest hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[1], L"%", &ctx.m_settings.aimbot_multipoint[1]);
//				aimbot_section_1->add_slider("body hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[2], L"%", &ctx.m_settings.aimbot_multipoint[2]);
//				aimbot_section_1->add_slider("legs hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[3], L"%", &ctx.m_settings.aimbot_multipoint[3]);
//				aimbot_section_1->add_slider("foot hitbox scale", 0, 100, &ctx.m_settings.aimbot_pointscale[4], L"%", &ctx.m_settings.aimbot_multipoint[4]);
//				aimbot_section_1->add_slider("minimal damage", 0, 150, &ctx.m_settings.aimbot_min_damage_viable, L"");
//				aimbot_section_1->add_checkbox("scale damage on hp", &ctx.m_settings.aimbot_scale_damage_on_hp);
//				aimbot_section_1->add_checkbox("penetrate walls", &ctx.m_settings.aimbot_autowall);
//				aimbot_section_1->add_slider("", 0, 150, &ctx.m_settings.aimbot_min_damage, L"", &ctx.m_settings.aimbot_autowall);
//				aimbot_section_1->add_checkbox("scale penetration damage on hp", &ctx.m_settings.aimbot_wall_scale_damage_on_hp, &ctx.m_settings.aimbot_autowall);
//				//aimbot_section_1->add_checkbox( "aimbot with knife", &g_settings.fake.aimbot_with_knife );
//				aimbot_section_1->add_checkbox("aimbot with taser", &ctx.m_settings.aimbot_allow_taser);
//			}
//			aimbot->add( aimbot_section_1 );
//			
//			const auto aimbot_section_2 = new section( ); {
//				//aimbot_section_2->add_combobox( "auto scope", &ctx.m_settings.aimbot_autoscope, { "off", "always", "hitchance fail" } );
//				aimbot_section_2->add_checkbox("automatic scope", &ctx.m_settings.aimbot_autoscope);
//				aimbot_section_2->add_checkbox("compensate spread", &ctx.m_settings.aimbot_no_spread);
//				//aimbot_section_2->add_checkbox( "compensate recoil", &g_settings.fake.compensate_recoil );
//				aimbot_section_2->add_checkbox( "hitchance", &ctx.m_settings.aimbot_hitchance);
//				aimbot_section_2->add_slider( "", 0, 100, &ctx.m_settings.aimbot_hitchance_val, L"%", &ctx.m_settings.aimbot_hitchance);
//				//aimbot_section_2->add_slider("trace validation", 0, 100, &ctx.m_settings.aimbot_accuracy_boost, L"%", &ctx.m_settings.aimbot_hitchance);
//				//aimbot_doubletap_hitchance_val
//				//aimbot_section_2->add_checkbox( "position adjustment", &ctx.m_settings.aimbot_position_adjustment );
//				//aimbot_section_2->add_checkbox( "extended silent shot", &ctx.m_settings.extended_silent_shot);
//				//aimbot_section_2->add_checkbox( "fakeangles corrections", &ctx.m_settings.aimbot_position_adjustment_old );
//				//aimbot_section_2->add_checkbox("delay shot", &ctx.m_settings.aimbot_fakelag_prediction);
//				aimbot_section_2->add_checkbox( "prefer body aim", &ctx.m_settings.aimbot_prefer_body);
//				aimbot_section_2->add_multibox( "accuracy options", ctx.m_settings.aimbot_extra_scan_aim, { "baim lethal", "rapidfire baim", "baim spread", "safe limbs", "prefer safepoints", "delay shot" } );
//				//aimbot_section_2->add_multibox( "only body aim", g_settings.fake.only_body_aim, { "always", "health", "fake", "in air" } );
//				aimbot_section_2->add_keybind("only body aim", &ctx.m_settings.aimbot_bodyaim_key);
//				aimbot_section_2->add_combobox("doubletap", &ctx.m_settings.aimbot_doubletap_method, { "defensive", "instant" });
//				aimbot_section_2->add_multibox("instant options", ctx.m_settings.aimbot_extra_doubletap_options, { "extend teleport", "stop before 2nd shot", "break lc", "fakelag" });
//				//aimbot_section_2->add_slider("teleport distance", 0, 100, &ctx.m_settings.aimbot_tickbase_teleport_speed, L"%");
//				aimbot_section_2->add_keybind("doubletap key", &ctx.m_settings.aimbot_doubletap_exploit_toggle);
//				aimbot_section_2->add_keybind("suppress shot key", &ctx.m_settings.aimbot_hideshots_exploit_toggle);
//				aimbot_section_2->add_slider("doubletap hitchance", 0, 100, &ctx.m_settings.aimbot_doubletap_hitchance_val, L"%", &ctx.m_settings.aimbot_hitchance);
//				aimbot_section_2->add_keybind("min damage override", &ctx.m_settings.aimbot_min_damage_override);
//				aimbot_section_2->add_slider("", 0, 101, &ctx.m_settings.aimbot_min_damage_override_val, L"");
//				//aimbot_section_2->add_multibox("aimbot optimizations", ctx.m_settings.aimbot_low_fps_optimizations, { "optimize enemy scan" });
//				//aimbot_section_2->add_slider( "", 0, 100, &g_settings.fake.body_aim_min_hp, L"hp", &g_settings.fake.only_body_aim[ 1 ] );
//			}
//			aimbot->add( aimbot_section_2 );
//		}
//		main_window->add( aimbot );
//
//		const auto antiaim = new tab( "anti-aim" ); {
//			const auto antiaim_section_1 = new section(); {
//				antiaim_section_1->add_checkbox("enable", &ctx.m_settings.anti_aim_enabled);
//				//antiaim_section_1->add_checkbox("follow targets", &ctx.m_settings.anti_aim_at_target);
//				//antiaim_section_1->add_multibox("at targets", ctx.m_settings.anti_aim_at_target, { "follow by crosshair", "follow by distance", "ignore when manual" });
//				antiaim_section_1->add_keybind("freestanding base yaw", &ctx.m_settings.anti_aim_freestanding_key);
//				antiaim_section_1->add_combobox("pitch", &ctx.m_settings.anti_aim_typex, { "none", "down", "up", "zero" });
//				antiaim_section_1->add_combobox("yaw", &ctx.m_settings.anti_aim_typey, { "none", "180", "zero", "lby twist", "spin" });
//				antiaim_section_1->add_combobox("switch method", &ctx.m_settings.anti_aim_jittering_type, { "none", "jitter", "rotate" });
//				antiaim_section_1->add_slider("angle", 0, 180, &ctx.m_settings.anti_aim_jittering, L"°"/*, &ctx.m_settings.anti_aim_jittering_type*/);
//				antiaim_section_1->add_slider("speed", 0, 100, &ctx.m_settings.anti_aim_jittering_speed, L"%"/*, &ctx.m_settings.anti_aim_jittering_type*/);
//				antiaim_section_1->add_combobox("body yaw", &ctx.m_settings.anti_aim_typelby, { "opposite", "eye yaw" });
//				//antiaim_section_1->add_checkbox("freestanding body yaw", &ctx.m_settings.anti_aim_freestanding_fake);
//				antiaim_section_1->add_combobox("freestanding body yaw", &ctx.m_settings.anti_aim_freestanding_fake_type, { "none", "peeking", "hiding" });
//				antiaim_section_1->add_slider("left side limit", 0, 60, &ctx.m_settings.anti_aim_fake_left_limit, L"°");
//				//antiaim_section_1->add_slider("right side limit", 0, 60, &ctx.m_settings.anti_aim_fake_right_limit, L"°");
//			}
//			antiaim->add(antiaim_section_1);
//
//			const auto antiaim_section_2 = new section(); {
//				antiaim_section_2->add_combobox("fake yaw", &ctx.m_settings.anti_aim_typeyfake, { "none", "fake out", "real out", "jitter"/*, "alternative"*/ });
//				//antiaim_section_2->add_checkbox("avoid overlap", &ctx.m_settings.anti_aim_fake_static_real);
//				antiaim_section_2->add_multibox("switch side if", ctx.m_settings.anti_aim_automatic_side, { "hurt"/*, "shoot cycle"*/, "exploits", "high speed", "overlap" });
//				antiaim_section_2->add_combobox("on-shot fake yaw", &ctx.m_settings.anti_aim_typeyfake_shot, { "default", "opposite", "cycle", "alternative"/*, "alternative"*/ });
//
//				antiaim_section_2->add_keybind("fake side switch", &ctx.m_settings.anti_aim_fake_switch, false);
//				antiaim_section_2->add_blank();
//				antiaim_section_2->add_checkbox("fake lag", &ctx.m_settings.fake_lag_enabled);
//				antiaim_section_2->add_combobox("", &ctx.m_settings.fake_lag_type, { "maximum", "switch", "break" });
//				antiaim_section_2->add_slider("variance", 0, 100, &ctx.m_settings.fake_lag_variance, L"%");
//				antiaim_section_2->add_slider("limit", 0, 16, &ctx.m_settings.fake_lag_value, L"");
//				antiaim_section_2->add_checkbox("desync client-side", &ctx.m_settings.fake_lag_special);
//				antiaim_section_2->add_checkbox("max choke on peek", &ctx.m_settings.fake_lag_peek);
//				antiaim_section_2->add_checkbox("lag compensation breaker", &ctx.m_settings.fake_lag_lag_compensation);
//				antiaim_section_2->add_checkbox("fakelag while shooting", &ctx.m_settings.fake_lag_shooting);
//			}
//			antiaim->add(antiaim_section_2);
//		}
//		main_window->add( antiaim );
//
//		const auto players = new tab( "players" ); {
//			const auto players_section_1 = new section( ); {
//				//players_section_1->add_multibox( "boxes", ctx.m_settings.esp_box, { "enemy", "friendly" } );
//				//players_section_1->add_colorpicker( "box enemy color", &ctx.m_settings.box_enemy_color, &ctx.m_settings.esp_box[0]);
//				//players_section_1->add_colorpicker( "box friendly color", &ctx.m_settings.box_friendly_color, &ctx.m_settings.esp_box[1]);
//				//players_section_1->add_checkbox( "dormant enemies", &g_settings.player_esp.dormant_enemies );
//				players_section_1->add_checkbox( "enemy offscreen esp", &ctx.m_settings.esp_offscreen );
//			//	players_section_1->add_colorpicker( "", &ctx.m_settings.colors_esp_offscreen);
//				players_section_1->add_slider("offscreen esp size", 0, 60, &ctx.m_settings.esp_arrows_size, L"");
//				players_section_1->add_slider("offscreen esp distance", 0, 100, &ctx.m_settings.esp_arrows_distance, L"%");
//				//players_section_1->add_multibox("name", ctx.m_settings.esp_name, { "enemy", "friendly" });
//				//players_section_1->add_colorpicker("name color", &ctx.m_settings.colors_esp_name);
//				//players_section_1->add_multibox("health", ctx.m_settings.esp_health, { "enemy", "friendly" });
//				//players_section_1->add_multibox("flags enemy", ctx.m_settings.esp_flags[0], { "armor", "last place", "scope", "debug", "pin", "c4/hostage" });
//				//players_section_1->add_multibox("flags friendly", ctx.m_settings.esp_flags[1], { "armor", "last place", "scope", "c4/hostage" });
//				//players_section_1->add_multibox("weapon", ctx.m_settings.esp_weapon, { "enemy", "friendly" });
//				players_section_1->add_combobox("", &ctx.m_settings.esp_weapon_type, { "text", "icon" });
//				players_section_1->add_checkbox("ammo", &ctx.m_settings.esp_weapon_ammo);
//				//players_section_1->add_colorpicker("", &ctx.m_settings.colors_esp_ammo);
//			}
//			players->add( players_section_1 );
//
//			const auto players_section_2 = new section(); {
//				//players_section_2->add_multibox("skeleton", ctx.m_settings.esp_skeleton, { "enemy", "friendly" });
//				//players_section_2->add_colorpicker("enemy color", &ctx.m_settings.colors_skeletons_enemy, &ctx.m_settings.esp_skeleton[0]);
//				//players_section_2->add_colorpicker("friendly color", &ctx.m_settings.colors_skeletons_teammate, &ctx.m_settings.esp_skeleton[1]);//esp_glow
//				//players_section_2->add_multibox("glow", ctx.m_settings.esp_glow, { "enemy", "friendly", "local" });
//				//players_section_2->add_colorpicker("enemy color", &ctx.m_settings.colors_glow_enemy, &ctx.m_settings.esp_glow[0]);
//				//players_section_2->add_colorpicker("friendly color", &ctx.m_settings.colors_glow_teammate, &ctx.m_settings.esp_glow[1]);
//				//players_section_2->add_colorpicker("local color", &ctx.m_settings.colors_glow_local, &ctx.m_settings.esp_glow[2]);
//				//players_section_2->add_colorpicker("fake yaw color", &ctx.m_settings.colors_glow_fake_yaw, &ctx.m_settings.esp_glow[3]);
//				//players_section_2->add_slider("", 0, 100, &ctx.m_settings.esp_glow_alpha, L"%");
//
//				players_section_2->add_blank();
//
//				//menu_chams_type
//				players_section_2->add_combobox("chams options", &ctx.m_settings.menu_chams_type, { "enemy", "friendly", "local", "fake yaw", "hands", "weapon", "backtrack" });
//
//				//players_section_2->add_multibox("", ctx.m_settings.chams_enemy, { "visible", "invisible" }, &ctx.m_settings.changing_chams[0]);
//				
//				//players_section_2->add_slider("", 0, 100, &ctx.m_settings.esp_chams_enemy_alpha, L"%");
//				//players_section_2->add_multibox("", ctx.m_settings.chams_teammates, { "visible", "invisible" }, & ctx.m_settings.changing_chams[1]);
//
//				//players_section_2->add_checkbox("enable", &ctx.m_settings.chams_local_player[0], &ctx.m_settings.changing_chams[2]);
//				//players_section_2->add_checkbox("enable", &ctx.m_settings.chams_local_player[1], &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_checkbox("enable", &ctx.m_settings.chams_backtrack, &ctx.m_settings.changing_chams[6]);
//
//				players_section_2->add_checkbox("apply only when knife", &ctx.m_settings.chams_weapon_apply_only_on[0], &ctx.m_settings.changing_chams[5]);
//
//				players_section_2->add_checkbox("enable", &ctx.m_settings.chams_misc[0], &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_checkbox("enable", &ctx.m_settings.chams_misc[1], &ctx.m_settings.changing_chams[5]);
//				//players_section_2->add_slider("", 0, 100, &ctx.m_settings.esp_chams_friendly_alpha, L"%");
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_type[0], { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[0]);
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_type[1], { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[1]);
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_type[2], { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[2]);
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_type[3], { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[3]);
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_type[4], { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[4]);
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_type[5], { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[5]);
//				players_section_2->add_combobox("", &ctx.m_settings.chams_material_backtrack, { "default", "flat", "metallic", "metallic anim", "outline", "ghost", "glass outlined", "glass", "glass colored", "leaves", "plastic", "water anim", "glowing", "ghost anim", "flow anim" }, & ctx.m_settings.changing_chams[6]);
//
//				//players_section_2->add_colorpicker("color visible", &ctx.m_settings.colors_chams_enemy_viable, &ctx.m_settings.changing_chams[0]);
//				//players_section_2->add_colorpicker("color invisible", &ctx.m_settings.colors_chams_enemy_hidden, &ctx.m_settings.changing_chams[0]);
//
//				//players_section_2->add_colorpicker("color visible", &ctx.m_settings.colors_chams_teammate_viable, &ctx.m_settings.changing_chams[1]);
//				//players_section_2->add_colorpicker("color invisible", &ctx.m_settings.colors_chams_teammate_hidden, &ctx.m_settings.changing_chams[1]);
//
//				players_section_2->add_colorpicker("color", &ctx.m_settings.colors_chams_hands_viable, &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_colorpicker("color", &ctx.m_settings.colors_chams_backtrack, &ctx.m_settings.changing_chams[6]);
//				//players_section_2->add_colorpicker("color invisible", &ctx.m_settings.colors_chams_hands_hidden, &ctx.m_settings.changing_chams[4]);
//
//				players_section_2->add_colorpicker("color", &ctx.m_settings.colors_chams_weapon_viable, &ctx.m_settings.changing_chams[5]);
//				//players_section_2->add_colorpicker("color invisible", &ctx.m_settings.colors_chams_weapon_hidden, &ctx.m_settings.changing_chams[5]);
//
//				players_section_2->add_colorpicker("color", &ctx.m_settings.colors_chams_local, &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_slider("chams scope alpha", 0, 100, &ctx.m_settings.local_chams_scope_trans, L"%", &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_slider("chams scope alpha", 0, 100, &ctx.m_settings.local_chams_fake_scope_trans, L"%", &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_colorpicker("color fake yaw", &ctx.m_settings.colors_chams_local_desync, &ctx.m_settings.changing_chams[3]);
//
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe[0], &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe[1], &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe[2], &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe[3], &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe[4], &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe[5], &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_checkbox("wireframe", &ctx.m_settings.chams_wireframe_backtrack, &ctx.m_settings.changing_chams[6]);
//
//				/*players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_discoball[0], &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_discoball[1], &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_discoball[2], &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_discoball[3], &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_discoball[4], &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_discoball[5], &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_checkbox("discoball", &ctx.m_settings.chams_wireframe_backtrack, &ctx.m_settings.changing_chams[6]);*/
//
//				players_section_2->add_checkbox("redraw", &ctx.m_settings.chams_redraw[0], &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_checkbox("redraw", &ctx.m_settings.chams_redraw[1], &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_checkbox("redraw", &ctx.m_settings.chams_redraw[2], &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_checkbox("redraw", &ctx.m_settings.chams_redraw[4], &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_checkbox("redraw", &ctx.m_settings.chams_redraw[5], &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_checkbox("redraw", &ctx.m_settings.chams_redraw_backtrack, &ctx.m_settings.changing_chams[6]);
//
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity[0], L"%", &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity[1], L"%", &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity[2], L"%", &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity[3], L"%", &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity[4], L"%", &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity[5], L"%", &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_slider("reflectivity", 0, 100, &ctx.m_settings.chams_reflectivity_backtrack, L"%", &ctx.m_settings.changing_chams[6]);
//
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c[0], &ctx.m_settings.changing_chams[0], false);
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c[1], &ctx.m_settings.changing_chams[1], false);
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c[2], &ctx.m_settings.changing_chams[2], false);
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c[3], &ctx.m_settings.changing_chams[3], false);
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c[4], &ctx.m_settings.changing_chams[4], false);
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c[5], &ctx.m_settings.changing_chams[5], false);
//				players_section_2->add_colorpicker("reflectivity color", &ctx.m_settings.chams_reflectivity_c_backtrack, &ctx.m_settings.changing_chams[6], false);
//
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent[0], L"%", &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent[1], L"%", &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent[2], L"%", &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent[3], L"%", &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent[4], L"%", &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent[5], L"%", &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_slider("pearlescent", 0, 100, &ctx.m_settings.chams_pearlescent_backtrack, L"%", &ctx.m_settings.changing_chams[6]);
//
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim[0], L"%", &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim[1], L"%", &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim[2], L"%", &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim[3], L"%", &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim[4], L"%", &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim[5], L"%", &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_slider("rim", 0, 100, &ctx.m_settings.chams_rim_backtrack, L"%", &ctx.m_settings.changing_chams[6]);
//
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine[0], L"%", &ctx.m_settings.changing_chams[0]);
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine[1], L"%", &ctx.m_settings.changing_chams[1]);
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine[2], L"%", &ctx.m_settings.changing_chams[2]);
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine[3], L"%", &ctx.m_settings.changing_chams[3]);
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine[4], L"%", &ctx.m_settings.changing_chams[4]);
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine[5], L"%", &ctx.m_settings.changing_chams[5]);
//				players_section_2->add_slider("shine", 0, 100, &ctx.m_settings.chams_shine_backtrack, L"%", &ctx.m_settings.changing_chams[6]);
//
//
//				//players_section_2->add_slider("chams material", 0, 18, &ctx.m_settings.chams_material_type, L"", &ctx.m_settings.changing_chams[3]);
//
//			}
//			players->add(players_section_2);
//		}
//		main_window->add( players );
//
//		const auto visuals = new tab( "visuals" ); {
//			const auto visuals_section_1 = new section(); {
//				//visuals_section_1->add_multibox("dropped weapons", ctx.m_settings.esp_world_weapons, { "box", "ammo", "name" });
//				visuals_section_1->add_colorpicker("color", &ctx.m_settings.world_esp_color, false);
//
//				visuals_section_1->add_checkbox("bomb", &ctx.m_settings.esp_world_bomb);
//				visuals_section_1->add_checkbox("projectiles", &ctx.m_settings.esp_world_nades);
//	
//				//visuals_section_1->add_checkbox( "dormant enemies", &g_settings.player_esp.dormant_enemies );
//				visuals_section_1->add_checkbox("do not render teammates", &ctx.m_settings.visuals_no_teammates);
//				visuals_section_1->add_checkbox("do not render sleeves", &ctx.m_settings.visuals_no_sleeves);
//				visuals_section_1->add_multibox("world", ctx.m_settings.misc_visuals_world_modulation, { "fullbright", "nightmode" });
//				visuals_section_1->add_slider("props transparency", 0, 100, &ctx.m_settings.visuals_props_alpha, L"%");
//
//				visuals_section_1->add_multibox("show hitbox if", ctx.m_settings.misc_visuals_hitboxes, { "aimbot", "hurt" });
//				visuals_section_1->add_slider("", 0, 10, &ctx.m_settings.misc_visuals_hitboxes_time, L"s");
//				//visuals_section_1->add_slider("", 0, 100, &ctx.m_settings.misc_visuals_hitboxes_alpha, L"%");
//				visuals_section_1->add_colorpicker("color", &ctx.m_settings.misc_visuals_hitboxes_color);
//
//				visuals_section_1->add_multibox("extra windows", ctx.m_settings.visuals_extra_windows, { "spectators", "key binds" });
//				visuals_section_1->add_slider("spectators window transparency", 0, 100, &ctx.m_settings.visuals_spectators_alpha, L"%", &ctx.m_settings.visuals_extra_windows[0]);
//				visuals_section_1->add_slider("keybinds window transparency", 0, 100, &ctx.m_settings.visuals_keybinds_alpha, L"%", &ctx.m_settings.visuals_extra_windows[1]);
//			}
//			visuals->add(visuals_section_1);
//
//			const auto visuals_section_2 = new section(); {
//				//visuals_section_2->add_combobox("autowall crosshair", &ctx.m_settings.visuals_penetration_crosshair_type, { "none", "standalone", "2d circle" });
//				visuals_section_2->add_checkbox("remove visual recoil", &ctx.m_settings.visuals_no_recoil);
////				visuals_section_2->add_checkbox("remove flash/smoke", &ctx.m_settings.visuals_no_flashsmoke);
//				visuals_section_2->add_checkbox("remove post process", &ctx.m_settings.visuals_no_postprocess);
//				visuals_section_2->add_checkbox("remove scope", &ctx.m_settings.visuals_no_scope);
//				visuals_section_2->add_checkbox("override fov", &ctx.m_settings.misc_override_fov);
//				visuals_section_2->add_slider("", -30, 110, &ctx.m_settings.misc_override_fov_val, L"°");
//				visuals_section_2->add_checkbox("ignore on first zoom lvl", &ctx.m_settings.visuals_no_first_scope);
//				visuals_section_2->add_multibox("indicators", ctx.m_settings.misc_visuals_indicators_2, { "hitmarker", "fake", "lag compensation", "bomb timer", "defuse timer", "aimbot settings", "min damage key", "side control", "current exploit", "desync side control"});
//				visuals_section_2->add_checkbox("local bullet impacts", &ctx.m_settings.visuals_draw_local_impacts);
//				visuals_section_2->add_checkbox("local bullet tracers", &ctx.m_settings.visuals_draw_local_beams);
//				visuals_section_2->add_colorpicker("", &ctx.m_settings.local_beams_color);
//				visuals_section_2->add_checkbox("force third person", &ctx.m_settings.visuals_tp_force);
//				visuals_section_2->add_keybind("", &ctx.m_settings.visuals_tp_key);
//				visuals_section_2->add_slider("third person distance", 30, 400, &ctx.m_settings.visuals_tp_dist, L"");
//				visuals_section_2->add_checkbox("force third person if dead", &ctx.m_settings.visuals_tp_force_dead);
//				visuals_section_2->add_checkbox("force crosshair", &ctx.m_settings.visuals_force_crosshair);
//				visuals_section_2->add_checkbox("force engine radar", &ctx.m_settings.misc_engine_radar);
//				visuals_section_2->add_checkbox("force grenade preview", &ctx.m_settings.misc_grenade_preview);
//				visuals_section_2->add_checkbox("clan tag", &ctx.m_settings.visuals_clantag);
//				//visuals_section_2->add_checkbox("hold aim angle", &ctx.m_settings.visuals_tp_hold_aim_angle);
//			}
//			visuals->add(visuals_section_2);
//		}
//		main_window->add( visuals );
//
//		const auto movement = new tab( "movement" ); {
//			const auto movement_section_1 = new section(); {
//				movement_section_1->add_checkbox("automatic jump", &ctx.m_settings.misc_bhop);
//				movement_section_1->add_checkbox("duck in air", &ctx.m_settings.aimbot_anti_aim_air_duck);
//				movement_section_1->add_checkbox("automatic strafe", &ctx.m_settings.misc_autostrafer);
//				movement_section_1->add_checkbox("wasd strafer", &ctx.m_settings.misc_autostrafer_wasd);
//				movement_section_1->add_slider("wasd speed", 0, 100, &ctx.m_settings.misc_autostrafer_retrack, L"%");
//
//				//movement_section_1->add_blank();
//				//movement_section_1->add_checkbox("auto stop", &ctx.m_settings.aimbot_autostop);
//				//movement_section_1->add_multibox("auto stop options", ctx.m_settings.aimbot_autostop_options, { "min walk", /*"duck",*/ "move between shots", "ignore molotov", "force accuracy" });
//
//				movement_section_1->add_combobox("slow walk options", &ctx.m_settings.anti_aim_slow_walk_type, { "favor desync", "favor random" });
//				movement_section_1->add_slider("slow walk limit", 0, 100, &ctx.m_settings.anti_aim_slow_walk_speed, L"%");
//				movement_section_1->add_slider("speed desync amount", 0, 100, &ctx.m_settings.anti_aim_slow_walk_desync, L"%");
//
//				movement_section_1->add_combobox("leg movement options", &ctx.m_settings.anti_aim_leg_movement, { "default", "always slide", "never slide", "LEDSYNC" });
//			}
//			movement->add(movement_section_1);
//
//			const auto movement_section_2 = new section(); {
//				movement_section_2->add_keybind("auto peek", &ctx.m_settings.anti_aim_autopeek_key);
//				movement_section_2->add_keybind("fake duck", &ctx.m_settings.anti_aim_fakeduck_key);
//				movement_section_2->add_keybind("slow walk", &ctx.m_settings.anti_aim_slowwalk_key);
//				movement_section_2->add_keybind("stop time", &ctx.m_settings.anti_aim_timestop_key);
//				movement_section_2->add_keybind("manual anti-aim left", &ctx.m_settings.anti_aim_yaw_left_switch, false);
//				movement_section_2->add_keybind("manual anti-aim right", &ctx.m_settings.anti_aim_yaw_right_switch, false);
//				movement_section_2->add_keybind("manual anti-aim 180", &ctx.m_settings.anti_aim_yaw_backward_switch, false);
//			}
//			movement->add(movement_section_2);
//		}
//		main_window->add( movement );
//
//		const auto skins = new tab( "skins" ); {
//			const auto skins_section_1 = new section(); {
//				skins_section_1->add_checkbox("change skins", &ctx.m_settings.skinchanger_enabled);
//				skins_section_1->add_combobox("knife model", &ctx.m_settings.skinchanger_knife, &ctx.knifes);
//				skins_section_1->add_combobox("knife skin", &ctx.m_settings.skinchanger_knife_skin, &ctx.skins);
//
//				skins_section_1->add_checkbox("change player model", &ctx.m_settings.skins_player_model);
//				skins_section_1->add_combobox("T agent model", &ctx.m_settings.skins_player_model_type_t, models_to_change);
//				skins_section_1->add_combobox("CT agent model", &ctx.m_settings.skins_player_model_type_ct, models_to_change);
//			}
//			skins->add(skins_section_1);
//		}
//		main_window->add( skins );
//		
//		const auto misc = new tab( "misc" ); {
//			const auto misc_section_1 = new section(); {
//				misc_section_1->add_checkbox("autobuy", &ctx.m_settings.misc_autobuy_enabled);
//				misc_section_1->add_combobox("auto buy items", &ctx.m_settings.misc_autobuy_primary, { "none", "autosniper", "ssg-08", "awp", "negev", "ak-47/m4" });
//				misc_section_1->add_combobox("", &ctx.m_settings.misc_autobuy_secondary, { "none", "deagle/r8", "dualies", "usp/glock", "tec9/fiveseven" });
//				misc_section_1->add_multibox("", ctx.m_settings.misc_autobuy_etc, { "taser", "armor", "kit", "smoke", "hegrenade", "flashbang", "flashbang", "molotov" });
////				misc_section_1->add_checkbox("ignore if low money", &ctx.m_settings.misc_autobuy_money_limit);
//				misc_section_1->add_multibox("notifications", ctx.m_settings.misc_notifications, { "bomb info", "damage given", "missed shot", "shot debug", "purchases", "damage received" });
//				//ctx.m_settings.misc_extend_backtrack
//			}
//			misc->add(misc_section_1);
//
//			const auto misc_section_2 = new section(); {
//				misc_section_2->add_checkbox("unlock invertory in-game", &ctx.m_settings.misc_unlock_inventory);
//				misc_section_2->add_checkbox("preserve killfeed", &ctx.m_settings.misc_preserve_killfeed);
//				misc_section_2->add_checkbox("knife left hand", &ctx.m_settings.misc_knife_hand_switch);
//				misc_section_2->add_checkbox("remove revolver cock sound", &ctx.m_settings.misc_no_revolver_cock_sound);
//
//				misc_section_2->add_slider("viewmodel control xyz", -10, 10, &ctx.m_settings.visuals_viewmodel_control[0], L"");
//				misc_section_2->add_slider("", -10, 10, &ctx.m_settings.visuals_viewmodel_control[1], L"");
//				misc_section_2->add_slider("", -10, 10, &ctx.m_settings.visuals_viewmodel_control[2], L"");
//				misc_section_2->add_checkbox("viewmodel fov", &ctx.m_settings.misc_override_viewmodel);
//				misc_section_2->add_slider("", -40, 110, &ctx.m_settings.misc_override_viewmodel_val, L"°");
//
//				//misc_section_2->add_combobox("hit marker sound", &ctx.m_settings.misc_hitsound_type, { "none", "default", "bell" });
//				misc_section_2->add_checkbox("f12 kill sound", &ctx.m_settings.misc_f12_kill_sound);
//				misc_section_2->add_checkbox("ragdoll force", &ctx.m_settings.misc_ragdoll_force);
//				//misc_section_2->add_combobox("voice sound player", &ctx.m_settings.music_curtrack, &ctx.music_found);
//				//misc_section_2->add_keybind("", &ctx.m_settings.music_key);
//				//misc_section_1->add_checkbox("fake latency", &ctx.m_settings.misc_extend_backtrack);
//			}
//			misc->add(misc_section_2);
//		}
//		main_window->add( misc );
//
//		const auto config = new tab( "config" ); {
//			const auto config_section_1 = new section( ); {
//				config_section_1->add_colorpicker( "menu color", &ctx.m_settings.menu_color, false);
//				config_section_1->add_button("reveal hidden commands", [&] {feature::misc->unlock_cvars(); });
//				config_section_1->add_button("unlock clientside commands", [&] {feature::misc->unlock_cl_cvars(); });
//			}
//			config->add( config_section_1 );
//
//			const auto config_section_2 = new section(); {
//				//config_section_2->add_combobox("safety mode", &ctx.m_settings.security_safety_mode, { "none", "matchmaking" });
//				config_section_2->add_combobox("configuration", &g_settings.cur_cfg, { "1", "2", "3", "4", "5", "6", "7" });
//				config_section_2->add_button("save", [&] {feature::misc->save_cfg(); }, true);
//				config_section_2->add_button("load", [&] {feature::misc->load_cfg(); g_menu.on_cfg_load(); if (ctx.m_settings.skinchanger_enabled) ctx.updated_skin = true; });
//
////#ifndef AUTH
////				config_section_2->add_blank();
////
////				config_section_2->add_combobox("lua manager", &ctx.m_settings.lua_selected_lua, &ctx.lua_scripts_count);
////				config_section_2->add_button("load script", [&] {
////
////					//feature::music_player->play("csgo\\sound\\voice_input.wav");
////
////					if (ctx.m_settings.lua_selected_lua < 0 || ctx.lua_scripts_count.size() < ctx.m_settings.lua_selected_lua + 1)
////						return;
////
////					//const std::string& name = ctx.lua_scripts_count[ctx.m_settings.lua_selected_lua];
////					////Utils::DebugMSG(name.c_str());
////					//if (name != "")
////					//{
////					//	if (lua::pLuaEngine->LoadScript(name)) {
////					//		pGUI->m_pScriptList->SetStatus(name, lua::pLuaEngine->ExecuteScript(name));
////					//	}
////					//	else {
////					//		pGUI->m_pScriptList->SetStatus(name, 0);
////					//	}
////					//} 
////				});
////
////				config_section_2->add_button("unload script", [&] {
////					if (ctx.m_settings.lua_selected_lua < 0 || ctx.lua_scripts_count.size() < ctx.m_settings.lua_selected_lua + 1)
////						return;
////
////					//const std::string& name = ctx.lua_scripts_count[ctx.m_settings.lua_selected_lua];
////					////Utils::DebugMSG(name.c_str());
////					//if (name != "")
////					//{
////					//	lua::pLuaEngine->UnloadScript(name);
////					//	//bool result = lua::pLuaEngine->ExecuteScript(name);
////					//	pGUI->m_pScriptList->SetStatus(name, 2);
////					//} 
////				});
////#endif
//			}
//			config->add(config_section_2);
//		}
//		main_window->add( config );
//#ifndef AUTH
//		const auto lua = new tab("lua"); {
//
//		}
//		main_window->add(lua);
//#endif
//	}
//	children.push_back( main_window );
//
//	VIRTUALIZER_STR_ENCRYPT_END;
//	VIRTUALIZER_FISH_LITE_END;
}