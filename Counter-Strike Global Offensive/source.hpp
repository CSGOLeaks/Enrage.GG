#pragma once

#include "sdk.hpp"
#include "player.hpp"
#include <array>
#include "lag_comp.hpp"

namespace Source
{
	extern HWND Window;

	bool Create();
	void Destroy();
	void QueueJobs();

	void* CreateInterface(const std::string& image_name, const std::string& name, bool force = false);
	void* CreateInterface(ULONG64 offset);
}

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define pad(size) char TOKENPASTE2(padding_, __LINE__) [size]

#define XOR_VAL xor_val
#define ADD_VAL 0x54F52D73//0x54F12F43
//#define ADD_VAL2 0x54F52D73

#pragma  optimize( "", off ) 
template<typename T>
class Encrypted_t {
	__forceinline uintptr_t rotate_dec(uintptr_t c) const {
		return c;
	}
public:
	uintptr_t np;
	uintptr_t xor_val;

	__forceinline Encrypted_t(T* ptr) {
		auto p = &ptr;
		xor_val = rotate_dec(CRC32_GetTableEntry(*((uint8_t*)p + 1) + (((uintptr_t(ptr) >> 16) & 7) << 8)));
		np = rotate_dec(rotate_dec(xor_val) ^ (uintptr_t(ptr) + ADD_VAL));
	}

	__forceinline  T* get() const {
		return (T*)((uintptr_t)(rotate_dec(np) ^ rotate_dec(xor_val)) - ADD_VAL);
	}

	__forceinline  T* operator-> () {
		return get();
	}

	__forceinline bool IsValid() const {
		return ((uintptr_t)(rotate_dec(np) ^ rotate_dec(xor_val)) - ADD_VAL) != 0;
	}
};

template <typename t>
class c_interface_base
{
public:
	t* m_interface = nullptr;

	__forceinline t* operator()()
	{
		return get();
	}

	__forceinline virtual t* get()
	{
		return m_interface;
	}

	__forceinline void set(t* ur_shit)
	{
		m_interface = ur_shit;
	}
};
#pragma  optimize( "", on )

class c_variables
{
public:
	bool	aimbot_enabled = false;
	bool	aimbot_auto_fire = false;
	int		aimbot_target_selection = 0;
	int		aimbot_hitbox = 0;
	int		aimbot_hitscan = 0;
	int		aimbot_resolver = 0;
	bool	aimbot_hitchance = false;
	int		aimbot_hitchance_val = 0;
	int		aimbot_min_damage = 0;
	int		aimbot_min_damage_viable = 0;
	int		aimbot_pointscale[6];
	int		aimbot_accuracy_boost = 0;
	bool	aimbot_position_adjustment_old = false;
	bool	aimbot_autowall = false;
	bool	aimbot_scale_damage_on_hp = false;
	bool	aimbot_wall_scale_damage_on_hp = false;
	bool	aimbot_allow_taser = false;
	bool	aimbot_hitboxes[6];
	bool	aimbot_hitbox_history[6];
	bool	aimbot_multipoint[6];
	bool	aimbot_weapons[2];
	bool	aimbot_auto_revolver = false;
	bool	aimbot_autostop = false;
	int	aimbot_fov_limit = 180;
	bool	aimbot_position_adjustment = false;
	bool	aimbot_silent_aim = false;
	//bool	aimbot_psilent_aim = false;
	bool	aimbot_nasa_psilent = false; //XDDDD
	bool	aimbot_no_spread = false;
	bool	aimbot_anti_aim_air_duck = false;
	int		_aimbot_bodyaim_key = 0;

	bool	anti_aim_enabled = false;
	int		anti_aim_typex = 0;
	int		anti_aim_typey = 0;
	int		anti_aim_typelby = 0;
	int		anti_aim_typeyfake = 0;
	int		anti_aim_side_control = 0;
	int		anti_aim_jittering_type = 0;
	int		anti_aim_jittering = 0;
	int		anti_aim_jittering_speed = 0;
	bool	anti_aim_fake_static_real = false;
	int		anti_aim_fake_left_limit = 0;
	int		anti_aim_fake_limit = 60;

	int		_anti_aim_fake_switch = 0; //key is: Z
	int		_anti_aim_fakeduck_key = 0;
	int		_anti_aim_yaw_left_switch = 0;
	int		_anti_aim_yaw_right_switch = 0;
	int		_anti_aim_yaw_backward_switch = 0;

	bool	fake_lag_enabled = false;
	int		fake_lag_type = 0;
	int		fake_lag_value = 0;
	int		fake_lag_variance = 0;
	bool	fake_lag_special = false;

	bool	visuals_enabled = false;
	bool	visuals_no_recoil = false;
	bool	visuals_tp_force = false;
	int		visuals_tp_dist = 30.f;
	int		_visuals_tp_key = 0;
	bool	visuals_no_scope = false;
	bool	visuals_no_first_scope = false;
	bool	visuals_no_postprocess = false;
	bool	visuals_no_smoke = false;
	bool	visuals_no_flash = false;
	bool	visuals_no_sleeves = false;
	bool	visuals_no_teammates = false;
	bool	esp_box;
	bool	esp_name;
	bool	esp_dormant;
	bool	esp_health;
	bool	esp_weapon;
	int		esp_weapon_type = 0;
	bool	esp_weapon_ammo = false;
	bool	esp_flags[6];
	bool	esp_skeleton;
	int		esp_arrows_size = 13;
	int		esp_arrows_distance = 85;

	bool	chams_enemy;
	bool	chams_walls;
	bool	chams_teammates;
	bool	chams_local_player;

	bool	esp_world_weapons;
	bool	esp_world_nades = false;
	bool	esp_world_bomb;

	bool	dummy[5] = { false,false,false,false, false };

	bool	misc_enabled = false;
	bool	misc_bhop = false;
	bool	misc_autostrafer = false;
	bool	misc_autostrafer_wasd = false;
	int 	misc_autostrafer_retrack = 0;
	bool	misc_unlock_inventory = false;

	int		misc_autobuy_primary = 0;
	int		misc_autobuy_secondary = 0;
	bool	misc_autobuy_etc[8];
	int	misc_autobuy_money_limit = 1000;

	
	bool	misc_visuals_world_modulation[2];
	bool	misc_preserve_killfeed = false;

	bool	misc_override_fov = false;
	int		misc_override_fov_val = 0.f;
	int		misc_aspect_ratio = 0;
	int		esp_box_type = 0;
	bool    misc_extend_backtrack = false;
	bool	visuals_draw_local_beams = false;
	bool	visuals_draw_local_impacts = false;
	bool	visuals_tp_force_dead = false;
	int		visuals_props_alpha = 0;
	bool	fake_lag_between_shots = false;

	float	box_enemy_color[4] = { 1, 1, 1, 1};
	float	   colors_esp_name[4] = { 1, 1, 1, 1 };
	//Color   colors_esp_weapon = { 255, 255, 255, 230 };
	float	   colors_esp_offscreen[4 ]= { 1, 0, 0, 1 };
	float	   colors_esp_health[4 ]= { 1, 0, 0, 1 };

	bool	esp_offscreen = false;
	bool	esp_glow = false;
	int		esp_glow_alpha = 60;

	int		esp_chams_enemy_type = 0;
	int		esp_chams_friendly_type = 0;
	int		esp_chams_local_type = 0;
	int		esp_chams_desync_type = 0;

	float	colors_chams_enemy_viable[4] = { 1, 0, 0, 1 };
	float	colors_chams_enemy_hidden[4] = { 1, 0.25f, 0 , 1 };

	float   colors_skeletons_enemy[4] = { 1, 1, 1, 1 };
	//Color   colors_skeletons_teammate = { 255, 255, 255, 230 };

	float   colors_glow_enemy[4] = { 1, 0, 0, 1 };
	float   colors_world_color[4] = { 0.16f, 0.16f, 0.16f , 1 };
	//Color   colors_glow_teammate = { 60, 180, 220 };

	//Color   colors_chams_teammate_viable = { 250, 200, 0 };
	//Color   colors_chams_teammate_hidden = { 250, 50, 0 };

	float   colors_chams_local_desync[4] = { 1, 1, 1, 1 };
	float   colors_chams_local[4] = { 1, 1, 1, 1 };

	Color	world_esp_color = { 255, 255, 255 };
	//Color	projectiles_esp_color = { 255, 255, 255 };

	bool	aimbot_prefer_body = false;
	bool	aimbot_ignore_head = false;

	bool	aimbot_headaim[6] = { false,false,false,false, false,false };

	Color	menu_color = { 180, 1, 45 };
	//bool	aimbot_headaim_shot = false;
	//bool	aimbot_headaim_high_speed = false;
	//bool	aimbot_headaim_low_damage = false;
	//bool	aimbot_headaim_safe_shot = false;
	//bool	aimbot_headaim_resolved = false;
	//bool	aimbot_doubletap = false;
	//int		security_safety_mode = 1;
	//int		oldkontrol = 0;
	int		_aimbot_min_damage_override = 0; //key is: Z
	int		aimbot_min_damage_override_val = 0;
	bool	misc_visuals_hitboxes = false;
	int		misc_visuals_hitboxes_time = 3;
	float	misc_visuals_hitboxes_color[4] = { 255, 0, 0, 255 };

	bool	misc_visuals_indicators_2[10] = { false,false,false,false, false,false, false,false,false,false };

	float	local_beams_color[4] = { 250, 0, 0, 255 };
	int		misc_visuals_hitboxes_alpha = 100;

	int		_anti_aim_slowwalk_key = 0;

	bool	visuals_tp_hold_aim_angle = false;

	int		_aimbot_tickbase_exploit_toggle = 0;

	int		aimbot_doubletap_hitchance_val = 0;

	bool    pad_fr_fake = false;

	bool	anti_aim_automatic_side[5] = { false,false,false,false, false };

	bool	fake_lag_shooting = false;

	int		menu_chams_type = 0;

	int		chams_material_type[6] = { 0,0,0,0,0,0 };

	int		chams_pearlescent[6] = { 0,0,0,0 ,0,0 };
	int		chams_reflectivity[6] = { 0,0,0,0,0,0 };
	Color	chams_reflectivity_c[6] = { 255, 255, 255 };

	int		chams_rim[6] = { 0,0,0,0 ,0,0 };
	int		chams_shine[6] = { 0,0,0,0,0,0 };

	bool	chams_wireframe[6] = { false,false,false,false };

	bool	chams_redraw[6] = { false,false,false,false,false };

	Color   colors_glow_local = { 60, 180, 220 };

	bool	pp[6] = { true, false, false, false, false }; //0 - enemy 1 - teammates 2 - local 3 - fake?

	Color   colors_chams_hands_viable = { 150, 200, 60 };
	Color   colors_chams_hands_hidden = { 60, 180, 220 };

	Color   colors_chams_weapon_viable = { 150, 200, 60 };
	Color   colors_chams_weapon_hidden = { 60, 180, 220 };

	bool	chams_misc[2] = { false,false };

	//bool	aimbot_autostop_options[5] = { false,false,false,false, false };
	int autostop_type = 0;
	bool autostop_only_when_shooting = 0;

	c_keybind anti_aim_fake_switch = c_keybind{ 0, 1 , false, 0 }; //key is: Z
	c_keybind anti_aim_fakeduck_key = c_keybind{ 0, 1 , false, 0 };
	c_keybind anti_aim_yaw_left_switch = c_keybind{ 0, 1, false, 0 };
	c_keybind anti_aim_yaw_right_switch = c_keybind{ 0, 1, false, 0 };
	c_keybind anti_aim_yaw_backward_switch = c_keybind{ 0, 1 , false, 0 };
	c_keybind anti_aim_slowwalk_key = c_keybind{ 0, 1 , false, 0 };
	c_keybind aimbot_doubletap_exploit_toggle = c_keybind{ 0, 0, false, 0 };
	c_keybind aimbot_bodyaim_key = c_keybind{ 0, 0 , false, 0 };
	c_keybind aimbot_min_damage_override = c_keybind{ 0, 0, false, 0 };
	c_keybind visuals_tp_key = c_keybind{ 0, 0, false, 0 };

	int		anti_aim_slow_walk_type = 0;
	int		anti_aim_slow_walk_speed = 51;

	bool	extended_silent_shot = false;

	bool	aimbot_fakelag_prediction = false;

	bool	visuals_force_crosshair = false;
	//int		visuals_penetration_crosshair_type = 0;
	bool visuals_autowall_crosshair = false;

	bool	skinchanger_enabled = false;
	int		skinchanger_knife = 0;
	int		skinchanger_knife_skin = 0;

	bool	chams_weapon_apply_only_on[5];
	bool	misc_knife_hand_switch = false;
	
	int	visuals_viewmodel_control[3] = {0,0,0};

	bool	fake_lag_peek = false;

	bool		misc_hitsound_type = 0;

	bool	aimbot_low_fps_optimizations[5] = { false,false,false,false,false };

	bool	chams_backtrack = false;

	Color   colors_chams_backtrack = { 150, 200, 60 , 255};

	int		chams_material_backtrack = 0;

	int		chams_pearlescent_backtrack = 0;
	int		chams_reflectivity_backtrack = 0;
	Color	chams_reflectivity_c_backtrack = Color::White();

	int		chams_rim_backtrack = 0;
	int		chams_shine_backtrack = 0;

	bool	chams_wireframe_backtrack = false;

	bool	chams_redraw_backtrack = false;

	bool	changing_chams[7] = { false, false, false, false, false, false };

	int		lua_selected_lua = 0;

	bool	misc_f12_kill_sound = false;

	c_keybind music_key = c_keybind{ 0, 1, false, 0 };

	bool	visuals_extra_windows[5] = { false, false,false,false,false };
	int		visuals_spectators_alpha = 100;
	Vector2D visuals_spectators_pos = Vector2D(200, 200);
	int		visuals_keybinds_alpha = 100;
	Vector2D visuals_keybinds_pos = Vector2D(200, 400);

	bool	visuals_clantag = false;

	float   colors_esp_ammo[4] = { 100, 100, 255, 230 };

	int	local_chams_scope_trans = 100;
	int	local_chams_fake_scope_trans = 100;

	int		music_curtrack = 0;

	bool	misc_engine_radar = false;
	bool	misc_grenade_preview = false;

	//bool    anti_aim_at_target = false;

	c_keybind anti_aim_autopeek_key = c_keybind{ 0, 0 , false, 0 };

	int	anti_aim_at_target[4] = { false,false,false,false };

	bool	misc_notifications[8] = { false,false,false,false, false,false, false,false };

	bool 	aimbot_autoscope = false;

	int		anti_aim_leg_movement = 0;

	bool	aimbot_allow_knife = false;

	int		aimbot_tickbase_teleport_speed = 0;

	bool	skins_player_model = false;

	int		skins_player_model_type_t = 0;
	int		skins_player_model_type_ct = 0;

	bool	chams_discoball[8] = { false,false,false,false, false,false, false, false };

	bool	aimbot_extra_scan_aim[6] = { false,false,false,false, false,false };

	c_keybind anti_aim_freestanding_key = c_keybind{ 0, 1 , false, 0 };

	int    anti_aim_freestanding_fake_type = false;
	int		anti_aim_typeyfake_shot = 0;

	int		aimbot_doubletap_method = 0;

	c_keybind aimbot_hideshots_exploit_toggle = c_keybind{ 0, 0, false, 0 };

	int		anti_aim_slow_walk_desync = 10;

	bool	aimbot_extra_doubletap_options[4] = { false,false,false,false };

	bool	misc_no_revolver_cock_sound = false;
	bool	misc_autobuy_enabled = false;
	c_keybind anti_aim_timestop_key = c_keybind{ 0, 1 , false, 0 };

	bool	misc_override_viewmodel = false;
	int		misc_override_viewmodel_val = 0.f;
	bool	fake_lag_lag_compensation = false;

	bool	misc_ragdoll_force = false;

	bool anti_aim_override_stand_yaw = false;
	bool anti_aim_override_move_yaw = false;
	int anti_aim_override_stand_typey = 0;
	int anti_aim_override_move_typey = 0;

	bool anti_aim_override_stand_fyaw = false;
	bool anti_aim_override_move_fyaw = false;

	int		anti_aim_fake_stand_flimit = 0;
	int		anti_aim_fake_move_flimit = 0;
	int		anti_aim_fake_stand_frlimit = 0;
	int		anti_aim_fake_move_frlimit = 0;

	int anti_aim_override_stand_fake = 0;
	int anti_aim_override_move_fake = 0;

	int		anti_aim_yaw_add = 0;

	struct Aimbot {
		bool enabled{ false };
		bool onKey{ false };
		int key{ 0 };
		int keyMode{ 0 };
		bool aimlock{ false };
		bool autodelay{ false };
		bool silent{ false };
		bool friendlyFire{ false };
		bool visibleOnly{ true };
		bool scopedOnly{ true };
		bool ignoreFlash{ false };
		bool ignoreSmoke{ false };
		bool autoShot{ false };
		bool autoScope{ false };
		float fov{ 0.0f };
		float smooth{ 1.0f };
		int bone{ 0 };
		float maxAimInaccuracy{ 1.0f };
		float maxShotInaccuracy{ 1.0f };
		int minDamage{ 1 };
		bool killshot{ false };
		bool betweenShots{ true };
		float killdelay{ 0.0f };
		float rcsfov{ 0.0f };
		float silentfov{ 0.0f };
	};
	std::array<Aimbot, 40> aimbot;

	bool misc_status_list = false;
	int aimbot_key = 5;
	int scope_transparency = 100;
	bool colors_glow_hp = false;
	bool visuals_bloom_enabled = false;
	int visuals_bloom_exposure = 0;
	int visuals_bloom_scale = 0;
	bool misc_spectators_list = false;
	bool autostop_force_accuracy = 0;

};

class c_csgo
{
public:
	//interfaces
	c_interface_base<IBaseClientDLL>		m_client;
	c_interface_base<ISurface>				m_surface;
	c_interface_base<IClientEntityList>		m_entity_list;
	c_interface_base<IGameMovement>			m_movement;
	c_interface_base<IPrediction>			m_prediction;
	c_interface_base<IMoveHelper>			m_move_helper;
	c_interface_base<IInput>				m_input;
	c_interface_base<CGlobalVarsBase>		m_globals;
	c_interface_base<IVEngineClient>		m_engine;
	c_interface_base<IPanel>				m_panel;
	c_interface_base<IEngineVGui>			m_engine_vgui;
	c_interface_base<CClientState>			m_client_state;
	c_interface_base<ICvar>					m_engine_cvars;
	c_interface_base<IEngineTrace>			m_engine_trace;
	c_interface_base<IVModelInfo>			m_model_info;
	c_interface_base<CCSGameRules>          m_game_rules;
	c_interface_base<InputSystem>			m_input_system;
	c_interface_base<IMaterialSystem>       m_material_system;
	c_interface_base<IVModelRender>			m_model_render;
	c_interface_base<IVRenderView>			m_render_view;
	c_interface_base<IPhysicsSurfaceProps>  m_phys_props;//IVDebugOverlay
	c_interface_base<IVDebugOverlay>        m_debug_overlay;
	c_interface_base<IGameEventManager>     m_event_manager;
	c_interface_base<IViewRenderBeams>		m_beams;
	c_interface_base<ILocalize>				m_localize;
	c_interface_base<CGlowObjectManager>	m_glow_object;
	c_interface_base<IMDLCache>             m_mdl_cache;
	c_interface_base<IStaticPropMgr>		m_static_prop;
	c_interface_base<IEngineSound>          m_engine_sound;
	c_interface_base<C_PlayerResource>      m_player_resource;
	c_interface_base< IMemAlloc >           m_mem_alloc;
	c_interface_base< IPhysicsCollision >           m_physcollision;

	/*
		c_interface_base< c_base_client >            m_client;
        c_interface_base< c_client_mode >            m_client_mode;
        c_interface_base< c_entity_list >            m_entity_list;
        c_interface_base< c_var >                    m_engine_cvars;
        c_interface_base< CEngineClient >            m_engine;
        c_interface_base< CEngineTrace >             m_engine_trace;
        c_interface_base< c_global_vars >            m_globals;
        c_interface_base< IVModelInfoClient >        m_model_info;
        c_interface_base< ISurface >                 m_surface;
        c_interface_base< VPanel >                   m_panel;
        c_interface_base< c_input >                  m_input;
        c_interface_base< IGameMovement >            m_movement;
        c_interface_base< IPrediction >              m_prediction;
        c_interface_base< IMaterialSystem >          m_material_system;
        c_interface_base< IVRenderView >             m_render_view;
        c_interface_base< IVModelRender >            m_model_render;
        c_interface_base< c_physics_props >          m_phys_props;
        c_interface_base< c_client_state >           m_client_state;
        c_interface_base< c_localize >               m_localize;
        c_interface_base< IGameEventManager2 >       m_event_manager;
        c_interface_base< void >                     m_event_manager1;
        c_interface_base< IMDLCache >                m_mdl_cache;
        c_interface_base< void >                     m_hdn;
        c_interface_base< void >                     m_file_system;
        c_interface_base< IViewRenderBeams >         m_beams;
        c_interface_base< c_debug_overlay >          m_debug_overlay;
        c_interface_base< CEngineVGui >              m_engine_vgui;
        c_interface_base< engine_sound >             m_engine_sound;
        c_interface_base< c_move_helper >            m_move_helper;
        c_interface_base< void >                     m_soundservices;
        c_interface_base< mem_alloc >                m_mem_alloc;
	*/
};

class c_vmthooks
{
public:
	////interfaces
	//c_interface_base<IBaseClientDLL>		m_client;
	//c_interface_base<ISurface>				m_surface;
	//c_interface_base<IClientEntityList>		m_entity_list;
	//c_interface_base<IGameMovement>			m_movement;
	//c_interface_base<IPrediction>			m_prediction;
	//c_interface_base<IMoveHelper>			m_move_helper;
	//c_interface_base<IInput>				m_input;
	//c_interface_base<CGlobalVarsBase>		m_globals;
	//c_interface_base<IVEngineClient>		m_engine;
	//c_interface_base<IPanel>				m_panel;
	//c_interface_base<IEngineVGui>			m_engine_vgui;
	//c_interface_base<CClientState>			m_client_state;
	//c_interface_base<ICvar>					m_engine_cvars;
	//c_interface_base<IEngineTrace>			m_engine_trace;
	//c_interface_base<IVModelInfo>			m_model_info;
	//c_interface_base<CCSGameRules>          m_game_rules;
	//c_interface_base<InputSystem>			m_input_system;
	//c_interface_base<IMaterialSystem>       m_material_system;
	//c_interface_base<IVModelRender>			m_model_render;
	//c_interface_base<IVRenderView>			m_render_view;
	//c_interface_base<IPhysicsSurfaceProps>  m_phys_props;//IVDebugOverlay
	//c_interface_base<IVDebugOverlay>        m_debug_overlay;
	//c_interface_base<IGameEventManager>     m_event_manager;
	//c_interface_base<IViewRenderBeams>		m_beams;
	//c_interface_base<ILocalize>				m_localize;
	//c_interface_base<CGlowObjectManager>	m_glow_object;
	//c_interface_base<IMDLCache>             m_mdl_cache;
	//c_interface_base<IStaticPropMgr>		m_static_prop;
	//c_interface_base<IEngineSound>          m_engine_sound;
	//c_interface_base<C_PlayerResource>      m_player_resource;
	//c_interface_base< IMemAlloc >           m_mem_alloc;
	//c_interface_base< IPhysicsCollision >           m_physcollision;

	Memory::VmtSwap::Shared m_material_system;
	Memory::VmtSwap::Shared m_material;
	Memory::VmtSwap::Shared m_engine_trace;
	Memory::VmtSwap::Shared m_bsp_tree_query;
	Memory::VmtSwap::Shared m_client;
	Memory::VmtSwap::Shared m_clientstate;
	Memory::VmtSwap::Shared m_clientmode;
	Memory::VmtSwap::Shared m_surface;
	Memory::VmtSwap::Shared m_prediction;
	Memory::VmtSwap::Shared m_movement;
	Memory::VmtSwap::Shared m_panel;
	Memory::VmtSwap::Shared m_render_view;
	Memory::VmtSwap::Shared m_engine;
	Memory::VmtSwap::Shared m_fire_bullets;
	Memory::VmtSwap::Shared m_engine_vgui;
	Memory::VmtSwap::Shared m_model_render;
	Memory::VmtSwap::Shared m_net_channel;
	Memory::VmtSwap::Shared m_show_impacts;
	Memory::VmtSwap::Shared m_device;
	Memory::VmtSwap::Shared m_cl_clock_correction;
	Memory::VmtSwap::Shared m_cl_grenadepreview;
	Memory::VmtSwap::Shared cl_smooth;
	Memory::VmtSwap::Shared m_engine_sound;
};

static DWORD client = 0;
static DWORD m_surface;
static DWORD m_entity_list;
static DWORD m_movement;
static DWORD m_prediction;
static DWORD m_move_helper;
static DWORD m_input;
static DWORD m_globals;
static DWORD m_engine;
static DWORD m_panel;
static DWORD m_engine_vgui;
static DWORD m_client_state;
static DWORD m_engine_cvars;
static DWORD m_engine_trace;
static DWORD m_model_info;
static DWORD m_game_rules;
static DWORD m_input_system;
static DWORD m_material_system;
static DWORD m_model_render;
static DWORD m_render_view;
static DWORD m_phys_props;//IVDebugOverlay
static DWORD m_debug_overlay;
static DWORD m_event_manager;
static DWORD m_beams;
static DWORD m_localize;
static DWORD m_glow_object;
static DWORD m_mdl_cache;
static DWORD m_static_prop;

extern c_csgo	 csgo;
extern c_vmthooks	 vmt;

class C_WeaponCSBaseGun;
class C_BasePlayer;

class CCSGO_HudDeathNotice;

#define ANGLES_TOTAL 4
#define ANGLE_POSDELTA 3
#define ANGLE_SHOOTANGLE 2
#define ANGLE_FAKE 1
#define ANGLE_REAL 0

enum cycle_update_flags
{
	CYCLE_NONE = 0,
	CYCLE_PRE_UPDATE = 1 << 1,
	CYCLE_UPDATE = 1 << 2
};

enum fakeduck_flags
{
	FD_NONE = 0,
	FD_NEED_A_FIX = 1 << 2,
};

enum packet_flags
{
	PACKET_NONE = 0,
	PACKET_CHOKE = 1 << 1,
	PACKET_SEND = 1 << 2
};

struct timing_data
{
	timing_data(int _t, int _s, int _cm, int _tc)
	{
		tickbase = _t;
		shifted_shit = _s;
		cmd_num = _cm;
		tick_count = _tc;
	}
	int tickbase;
	int shifted_shit;
	int cmd_num;
	int tick_count;
};

struct c_keybindinfo
{
	c_keybindinfo()
	{
		index = 0;
		sort_index = 0;
		mode = 0;
		prev_state = false;
		name = "";
		type = "";
	}

	c_keybindinfo(int _i, const char* _n, const char* _t, int _si)
	{
		index = _i;
		name = _n;
		type = _t;
		sort_index = _si;
	}
	int index;
	int mode;
	const char* name;
	const char* type;
	bool prev_state;
	int sort_index;
};

struct s_local_damage
{
	s_local_damage(int _d, float _t)
	{
		time = _t;
		damage = _d;
	}
	s_local_damage()
	{
		damage = 0;
		time = 0.f;
	}
	int damage;
	float time;
};

struct multipoint_info_s
{
	Vector point;
	int damage;
};

struct shot_pre_event_info_s
{
	Vector point;
	bool hit;
	bool find;
	int entindex;
	int resolver_index;
	float last_time_hit;
};

struct COutgoingData {
	int command_nr;
	int prev_command_nr;

	bool is_outgoing;
	bool is_used;
};

enum m_eflags
{
	hook_should_return_cl_smooth = 0x4,
};

struct server_delay_s
{
	int tick_count;
	int cmd_num;
};

class c_context
{
public:
	c_variables m_settings;
	_MANUAL_INJECTEX32* data;
	FORCEINLINE C_BasePlayer* m_local()
	{
		//if (!csgo.m_engine()->IsInGame())
		//	return nullptr;
		
		auto local_player = csgo.m_engine()->GetLocalPlayer();

		//if (client == nullptr || *(void**)client == nullptr || !client->IsPlayer())
		//	return nullptr;

		return csgo.m_entity_list()->GetClientEntity(local_player);
	}
	FORCEINLINE Color flt2color(float c[])
	{
		return Color(c[0] * 255, c[1] * 255, c[2] * 255, c[3] * 255);
	}
	/*FORCEINLINE*/ CClientEffectRegistration* m_effects_head()
	{
		static CClientEffectRegistration* effcts = **reinterpret_cast<CClientEffectRegistration* **>(Memory::Scan(
			sxor("client.dll"), sxor("8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F ? 8B 3E")) + 2);
		return effcts;
	}
	std::vector<std::string> get_all_files_names_within_folder(std::string folder, std::string fmt = "*.2k17")
	{
		std::vector<std::string> names = {};
		char search_path[200] = { 0 };
		sprintf_s(search_path, "%s/%s", folder.c_str(), fmt.c_str());
		WIN32_FIND_DATAA fd;
		HANDLE hFind = ::FindFirstFileA(search_path, &fd);
		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				// read all (real) files in current folder
				// , delete '!' read other 2 default folder . and ..
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					names.emplace_back(fd.cFileName);
				}
			} while (::FindNextFileA(hFind, &fd));
			::FindClose(hFind);
		}
		return names;

	}
	bool pressed_keys[256] = {};
	Vector2D screen_size = Vector2D(0, 0);
	int side = -1; //manual anti-aim side
	int fside = -1; //anti-aim fake yaw side
	int previous_buttons = 0; 
	Vector old_move = Vector::Zero;
	bool pressed_before = false;
	bool fakeducking = false;
	bool fakeducking_prev_state = false;
	int fakeduck_will_choke = 0;
	bool in_tp = false;
	float fov = 0.f;
	float hurt_time = 0.f;
	QAngle last_angles = QAngle::Zero;
	CUserCmd* last_usercmd;
	bool setup_bones = false;
	bool allow_attachment_helper = false;
	bool start_switching = false;
	float lerp_time = 0.f;
	bool applied_tickbase = false;
	float latency[MAX_FLOWS] = { 0.f , 0.f };
	float avglatency[MAX_FLOWS] = { 0.f , 0.f };
	bool updating_anims = false;
	bool updating_resolver = false;
	int last_penetrated_count = 4;
	int last_hitgroup = -1;
	float angles[ANGLES_TOTAL] = { 0.f, 0.f, 0.f, 0.f };
	Vector abs_origin[ANGLES_TOTAL] = { Vector::Zero, Vector::Zero, Vector::Zero, Vector::Zero };
	std::array<float, 24> poses[ANGLES_TOTAL];
	//C_AnimationLayer layers[ANGLES_TOTAL][14];
	matrix3x4_t fake_matrix[128];
	//matrix3x4_t zero_matrix[128];
	server_delay_s m_arr_latency[150];
	std::array< std::array<std::vector<Vector>, 20u>, 64u> points;
	bool do_autostop = false;
	int onshot_desync = 0;
	bool allows_aimbot = false;
	int onshot_aa_cmd = 0;
	int original_model_index = 0;
	void* update_hud_weapons = nullptr;
	CCSGO_HudDeathNotice* hud_death_notice = nullptr;
	CUserCmd* last_predicted_command = nullptr;
	//std::deque<_shotinfo> fired_shot;
	int update_cycle = CYCLE_NONE;
	bool was_fakeducking_before = false;
	bool changed_fake_side = false;
	int last_frame_stage = 0;
	//bool fix_velocity_modifier = false;
	//float old_velocity_modifier = 0.f;
	bool is_predicting = false;
	std::deque<timing_data> m_corrections_data;
	float last_velocity_modifier = -1.f;
	int last_velocity_modifier_tick = -1;
	int last_velocity_modifier_update_tick = -1;
	bool fix_modify_eye_pos = false;
	bool fix_runcommand = false;
	bool can_aimbot = true;
	float max_weapon_speed;
	int last_command_number = -1;
	int last_time_command_arrived = -1;
	int last_cmd_delta = 0;
	std::deque<int> last_4_deltas;
	int ticks_allowed = 0;
	int out_sequence_nr;
	bool skip_communication = false;
	bool did_communicate = false;
	bool can_call_senddatagram = false;
	int did_recharge = false;
	bool can_store_netvars = false;
	bool run_cmd_got_called = false;
	int last_netvars_update_tick = -1;
	int send_next_tick = PACKET_NONE;
	bool left_side = false;
	float last_shot_time_clientside = 0.f;
	bool autopeek_back = false;
	bool is_in_teleport = false;
	float fps = 0.f;
	bool boost_fps = false;
	int scroll_value = 0;
	bool is_local_defusing = false;
	int shift_amount = 0;
	bool doubletap_now = false;
	bool doubletap_charged = 0;
	int last_sent_tick = 0;
	bool buy_weapons = false;
	QAngle shot_angles = QAngle::Zero;
	bool did_shot = false;
	CCSGOPlayerAnimState fake_state;
	bool in_hbp = false;
	int next_shift_amount = 0;
	int double_tapped = 0;
	bool return_ishltv = false;
	bool force_next_packet_choke = false;
	int speed_hack = 0;
	int allow_shooting = 0;
	int charged_commands = 0;
	int charged_tickbase = 0;
	bool is_able_to_shoot = false;
	int started_speedhack = 0;
	int speedhack_choke = 0;
	int shifted_cmd = 0;
	bool next_run_cmd_fix_tickbase = false;
	int max_shift_cmd = 0;
	int tickbase_started_teleport = 0;
	bool speed_hacking = false;
	bool was_teleporting = false;
	bool has_scar = false;
	int estimated_shift_amount = 0;
	bool init_finished = false;
	C_AnimationLayer local_layers[4][14];
	int original_tickbase = 0;
	bool did_set_shift = false;
	int exploit_tickbase_shift = 0;
	int tickbase_shift_nr = 0;
	bool exploit_allowed = false;
	std::deque<COutgoingData> command_numbers = {};
	int fix_senddatagram[150] = {};
	int shifted_command[150] = { 0 };
	bool in_send_datagram = false;
	bool air_stuck = false;
	float last_speedhack_time = 0.f;
	int last_aim_index = 0;
	int last_aim_state = 0;
	int autowall_crosshair = 0;
	std::vector<std::string> knifes;
	std::vector<std::string> skins;
	bool updated_skin = false;
	int knife_model_index = 0;
	std::vector<C_BasePlayer*> m_player_entities;
	//bool can_hit[65] = {};
	//matrix3x4_t matrix[128];
	int m_ragebot_shot_nr = 0;
	int m_last_shot_index = 0;
	int force_aimbot = 0;
	QAngle m_ragebot_shot_ang = QAngle(0,0,0);
	//std::vector<std::string> lua_scripts_count;
	float time_to_reset_sound = 0.f;
	std::vector<std::string> music_found;
	bool sound_valid = false;
	//std::vector<multipoint_info_s> multi_points[64][20] = {};
	float local_spawntime;
	c_keybindinfo active_keybinds[15] = { };
	int active_keybinds_visible = 0;
	float last_shot_time_fakeduck;
	bool is_updating_fake = false;
	int current_tickcount = 0;
	int previous_tickcount = 0;
	int tickrate = 0;
	int cmd_tickcount = 0;
	bool did_stop_before = false;
	int last_autostop_tick = 0;
	int accurate_max_previous_chocked_amt = 0;
	float current_realtime = 0.f;
	bool should_rotate_camera = false;
	Vector m_eye_position = Vector::Zero;
	Vector m_pred_eye_pos = Vector::Zero;
	int command_number = 0;
	//int prediction_tickbase = 0;
	ConVar* clantag_cvar = nullptr;
	ConVar* cv_console_window_open = NULL;
	//Vector abs_origin = Vector::Zero;
	bool force_hitbox_penetration_accuracy = false;
	weapon_info* latest_weapon_data = nullptr;
	bool force_low_quality_autowalling = false;
	Vector auto_peek_spot = Vector::Zero;
	bool breaks_lc = false;
	s_local_damage local_damage[64] = {};
	bool first_run_since_init[65] = {};
	float current_spread = 0.0f;
	bool process_movement_sound_fix = false;
	bool optimized_point_search = false;
	bool allow_freestanding = false;
	bool is_charging = false;
	float last_time_charged = 0.f;
	bool is_cocking = false;
	float r8_timer = 0.f;
	QAngle cmd_original_angles;
	int cmd_original_buttons = 0;
	bool has_exploit_toggled = false;
	shot_pre_event_info_s last_shot_info;
	int main_exploit = 0;
	bool prev_exploit_states[2] = { false, false };
	int cheat_option_flags = 0;
	int ticks_to_stop = 0;
	float last_time_layers_fixed = 0.f;

	bool hold_aim = false;
	QAngle hold_angles = QAngle::Zero;
	int hold_aim_ticks = 0;
	//FORCEINLINE bool check_crc(std::string file, CRC32_t original)
	//{
	//	auto hFile = CreateFileA(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); // Open the DLL

	//	if (hFile == INVALID_HANDLE_VALUE)
	//		return 0;

	//	auto FileSize = GetFileSize(hFile, NULL);
	//	auto buffer = VirtualAlloc(NULL, FileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	//	if (!buffer)
	//	{
	//		CloseHandle(hFile);
	//		return 0;
	//	}

	//	// Read the DLL
	//	DWORD read;

	//	if (!ReadFile(hFile, buffer, FileSize, &read, NULL))
	//	{
	//		VirtualFree(buffer, 0, MEM_RELEASE);
	//		CloseHandle(hFile);

	//		return 0;
	//	}

	//	CloseHandle(hFile);

	//	CRC32_t crc;

	//	CRC32_Init(&crc);
	//	CRC32_ProcessBuffer(&crc, &buffer, sizeof(unsigned char) * FileSize);
	//	CRC32_Final(&crc);

	//	VirtualFree(buffer, 0, MEM_RELEASE);
	//	CloseHandle(hFile);

	//	return (crc == original);
	//}
	/*FORCEINLINE*/ bool get_key_press(int key, int zticks = 1) const
	{
		static int ticks[256];
		bool returnvalue = false;



		if (pressed_keys[key] && !csgo.m_client()->IsChatRaised() && ((DWORD)cv_console_window_open < 0x200 || cv_console_window_open->GetInt() == 0))
		{
			ticks[key]++;

			if (ticks[key] <= zticks)
			{
				returnvalue = true;
			}
		}
		else
			ticks[key] = 0;

		return returnvalue;
	}
	/*FORCEINLINE*/ bool get_key_press(c_keybind &key, int zticks = 1) const
	{
		bool returnvalue = false;

		if (key.key > 255 || key.mode > 5)
			return false;

		if (key.mode == 3 && key.key <= 0)
			return true;

		if (key.key <= 0)
			return false;

		switch (key.mode)
		{
		case 0:
			if (!csgo.m_client()->IsChatRaised() && ((DWORD)cv_console_window_open < 0x200 || cv_console_window_open->GetInt() == 0) && pressed_keys[key.key])
			{
				key.time++;

				if (key.time <= zticks) {
					key.toggled = !key.toggled;
				}
			}
			else
				key.time = 0;

			returnvalue = key.toggled;
			break;
		case 1:

			if (csgo.m_client()->IsChatRaised() || (DWORD)cv_console_window_open > 0x200 && cv_console_window_open->GetInt() == 1)
				return false;

			returnvalue = pressed_keys[key.key];
			
			break;
		case 2:

			if (csgo.m_client()->IsChatRaised() || (DWORD)cv_console_window_open > 0x200 && cv_console_window_open->GetInt() == 1)
				return false;

			returnvalue = !pressed_keys[key.key];

			break;
		case 3:
			if (key.key != 0)
				returnvalue = true;
			break;
		}

		return returnvalue;
	}
	int host_frameticks()
	{
		static auto host_frameticks = *(int**)(Memory::Scan("engine.dll", "03 05 ? ? ? ? 83 CF 10") + 2);

		if (host_frameticks)
			return *host_frameticks;
		else
			return 1;
	}
	const char* base64_encode(const std::string& in)
	{
		std::string out = "";

		int val = 0, valb = -6;
		for (unsigned char c : in) {
			val = (val << 8) + c;
			valb += 8;
			while (valb >= 0) {
				out.push_back(sxor("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")[(val >> valb) & 0x3F]);
				valb -= 6;
			}
		}
		if (valb > -6) out.push_back(sxor("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")[((val << 8) >> (valb + 8)) & 0x3F]);
		while (out.size() % 4) out.push_back('=');
		return out.c_str();
	}
	std::string base64_decode(const std::string& in)
	{
		std::string out = "";

		std::vector<int> T(256, -1);
		for (int i = 0; i < 64; i++) T[sxor("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/")[i]] = i;

		int val = 0, valb = -8;
		for (unsigned char c : in) {
			if (T[c] == -1) break;
			val = (val << 6) + T[c];
			valb += 6;
			if (valb >= 0) {
				out.push_back(char((val >> valb) & 0xFF));
				valb -= 8;
			}
		}
		return out;
	}
	void ForceTermination()
	{
		char buffer[256] = { 0 };
		GetModuleFileNameA(NULL, buffer, 256);


		int random1 = RandomInt(15, 40);
		DWORD random2 = RandomInt(0x11000000, 0x12000000);


		std::string msg = sxor(
			"This application has encountered a critical error:"
			"\n\n"
			"FATAL ERROR!"
			"\n\n"
			"Program: %s"
			"\n"
			"Exception:        0xC0000005 (ACCESS_VIOLATION) at 00%d:%X"
			"\n\n"
			"The instruction at '0x%X' referenced memory at '0x%X'."
			"\n"
			"The memory could not be 'read'."
			"\n\n"
			"Press OK to terminate the application."

		);

		char formatted[512] = { 0 };

		sprintf_s(formatted, 512, msg.c_str(), buffer, random1, random2, random2, random2);

		MessageBoxA(0, formatted, sxor("csgo.exe"), MB_OK | MB_ICONERROR);
		exit(0xffffff);
		terminate();
	}
	std::array<int,128u> shots_fired;
	std::array<int, 128u> shots_total;
	int padd[128];
};

class c_menu;
class c_antiaimbot;
class c_misc;
class c_resolver;
class c_visuals;
class c_usercmd;
class c_lagcomp;
class c_chams;
class c_autowall;
class c_aimbot;
class c_dormant_esp;
class c_music_player;
class c_grenade_tracer;
class c_weather_controller;
class c_legitaimbot;

namespace feature
{
	extern Encrypted_t<c_menu> menu;
	extern Encrypted_t< c_misc> misc;
	extern Encrypted_t < c_antiaimbot > anti_aim;
	extern Encrypted_t < c_resolver > resolver;
	extern Encrypted_t < c_visuals > visuals;
	extern Encrypted_t < c_grenade_tracer > grenades;
	extern Encrypted_t < c_usercmd > usercmd;
	extern Encrypted_t < c_lagcomp > lagcomp;
	extern Encrypted_t < c_chams > chams;
	extern Encrypted_t < c_autowall > autowall;
	extern Encrypted_t < c_aimbot > ragebot;
	extern Encrypted_t < c_dormant_esp > sound_parser;
	extern Encrypted_t < c_music_player > music_player;
	extern Encrypted_t < c_weather_controller > weather;
	extern Encrypted_t < c_legitaimbot > legitbot;

	template <class T>
	T find_hud_element(const char* name)
	{
		static auto pThis = *reinterpret_cast<DWORD * *>(Memory::Scan("client.dll", "B9 ? ? ? ? E8 ? ? ? ? 8B 5D 08") + 1);

		static auto find_hud_element = reinterpret_cast<DWORD(__thiscall*)(void*, const char*)>(Memory::Scan("client.dll", "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28"));

		if (find_hud_element && pThis)
			return (T)find_hud_element(pThis, name);
		else
			return (T)nullptr;
	}
}

extern C_WeaponCSBaseGun* m_weapon();

extern c_context ctx;

#define MEMEBR_FUNC_ARGS(...) ( this, __VA_ARGS__ ); }
#define FUNCARGS(...) ( __VA_ARGS__ ); }
#define VFUNC( index, func, sig ) auto func { return util::get_vfunc< sig >( this, index ) MEMEBR_FUNC_ARGS
#define MFUNC(func, sig, offset) auto func { return reinterpret_cast< sig >( offset ) MEMEBR_FUNC_ARGS
#define FUNC(func, sig, offset) auto func { return reinterpret_cast< sig >( offset ) FUNCARGS

inline uint8_t* find_sig_ext(uint32_t offset, const char* signature, uint32_t range = 0u)
{
	static auto pattern_to_bytes = [](const char* pattern) -> std::vector<int>
	{
		auto bytes = std::vector<int32_t>{};
		auto start = const_cast<char*>(pattern);
		auto end = const_cast<char*>(pattern) + strlen(pattern);

		for (auto current = start; current < end; ++current)
		{
			if (*current == '?')
			{
				current++;

				if (*current == '?')
					current++;

				bytes.push_back(-1);
			}
			else
				bytes.push_back(static_cast<int32_t>(strtoul(current, &current, 0x10)));
		}

		return bytes;
	};

	const auto scan_bytes = reinterpret_cast<std::uint8_t*>(offset);
	auto pattern_bytes = pattern_to_bytes(signature);
	const auto s = pattern_bytes.size();
	const auto d = pattern_bytes.data();

	for (auto i = 0ul; i < range - s; ++i)
	{
		auto found = true;

		for (auto j = 0ul; j < s; ++j)
			if (scan_bytes[i + j] != d[j] && d[j] != -1)
			{
				found = false;
				break;
			}

		if (found)
			return &scan_bytes[i];
	}

	return nullptr;
}

__forceinline void erase_function(uint8_t * function)
{
	if (*function == 0xE9)
	{
		auto pdFollow = (PDWORD)(function + 1);
		function = ((PBYTE)(*pdFollow + (DWORD)function + 5));
	}
	else if (*function == 0xEB)
	{
		auto pbFollow = (PDWORD)(function + 1);
		function = ((PBYTE)((DWORD)* pbFollow + (DWORD)function + 2));
	}

	static const auto current_process = reinterpret_cast<HANDLE>(-1);

	const auto end = find_sig_ext(reinterpret_cast<uint32_t>(function), "90 90 90 90 90", 0x2000);
	size_t bytes = reinterpret_cast<DWORD>(end) - reinterpret_cast<DWORD>(function) + 6;

	void* fn = function;
	size_t size = bytes;
	DWORD old;
	VirtualProtect(fn, size, PAGE_EXECUTE_READWRITE, &old);
	fn = function;
	size = bytes;
	memset(fn, 0, size);
	VirtualProtect(fn, size, old, &old);
}

#define concat_impl(x, y) x##y
#define concat(x, y) concat_impl(x, y)

// NOLINTNEXTLINE
#define erase_fn(a) constexpr auto concat(w, __LINE__) = &a;\
    erase_function(reinterpret_cast<uint8_t*>((void*&)concat(w, __LINE__)))
#define erase_end  __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 __asm _emit 0x90 

#include "parser.hpp"