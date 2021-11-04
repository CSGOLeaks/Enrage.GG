#pragma once
#include "sdk.hpp"

class c_player_records;

class C_Hitbox
{
public:
	int hitboxID = 0;
	bool isOBB;
	Vector mins;
	Vector maxs;
	Vector start_scaled;
	float radius;
	mstudiobbox_t* hitbox;
	int bone;
	int hitgroup;
};

//static int seeds[] = {
//	10, 15, 31, 38, 45, 55, 59, 65, 72, 81, 84, 90, 96,
//	104, 114, 118, 121, 135, 143, 152, 170, 193, 211, 214,
//	218, 223, 225, 239, 248, 254, 255, 4, 7, 9, 20, 23,
//	26, 30, 34, 50, 54, 62, 70, 78, 93, 98, 115, 123, 130,
//	136, 144, 150, 158, 163, 167, 175, 178, 182, 185, 188,
//	192, 206, 226, 230, 234, 244, 12, 17, 22, 36, 57, 64,
//	73, 86, 89, 94, 100, 107, 109, 116, 124, 126, 131, 140,
//	148, 164, 168, 177, 194, 197, 202, 207, 210, 229, 236,
//	241, 247, 0, 2, 16, 27, 37, 43, 46, 49, 52, 69, 74,
//	77, 101, 129, 139, 146, 154, 157, 160, 174, 181, 189,
//	200, 203, 213, 221, 233, 238, 246, 250, 253
//};

static int seeds[] = {
0x2 ,
0x3 ,
0x4 ,
0x0A,
0x0D,
0x0F,
0x1B,
0x1D,
0x25,
0x26,
0x29,
0x2E,
0x2F,
0x31,
0x32,
0x34,
0x38,
0x39,
0x3A,
0x3E,
0x42,
0x44,
0x46,
0x48,
0x4C,
0x53,
0x57,
0x61,
0x6C,
0x6D,
0x6E,
0x6F,
0x71,
0x79,
0x7A,
0x7D,
0x87,
0x1 ,
0x5 ,
0x6 ,
0x8 ,
0x11,
0x19,
0x1A,
0x1C,
0x1F,
0x20,
0x24,
0x27,
0x2C,
0x33,
0x3D,
0x43,
0x47,
0x4A,
0x4D,
0x4E,
0x4F,
0x50,
0x51,
0x54,
0x59,
0x5A,
0x5B,
0x5C,
0x60,
0x62,
0x65,
0x66,
0x68,
0x6B,
0x72,
0x73,
0x77,
0x9 ,
0x0B,
0x10,
0x13,
0x14,
0x16,
0x1E,
0x30,
0x36,
0x3B,
0x3C,
0x41,
0x45,
0x49,
0x4B,
0x52,
0x55,
0x56,
0x58,
0x5E,
0x63,
0x67,
0x69,
0x70,
0x74,
0x75,
0x76,
0x7C,
0x0 ,
0x7 ,
0x0C,
0x0E,
0x12,
0x15,
0x17,
0x18,
0x21,
0x22,
0x23,
0x28,
0x2A,
0x2B,
0x2D,
0x35,
0x37,
0x3F,
0x40,
0x5D,
0x5F,
0x64,
0x6A,
0x78,
0x85,
0x89
};

class C_Tickrecord;

struct TargetListing_t
{
	TargetListing_t(C_BasePlayer* ent);
	//{ "distance", "crosshair", "damage", "health", "lag", "height" }
	TargetListing_t()
	{
		hp = 0;
		idx = -1;
		//distance = 0;
		//fov = 0;
		//strange = false;
		entity = nullptr;
		//angle.clear();
	}

	//float height = 0.f;
	//float damage = 0.f;
	float hp = -1;
	int idx = -1;
	//float distance = -1;
	//float fov = -1;
	//bool strange = false;
	//QAngle angle = QAngle::Zero;
	C_BasePlayer* entity = nullptr;
};

enum HitscanMode : int {
	NORMAL = 0,
	LETHAL = 1,
	LETHAL2 = 3,
	PREFER = 4
};

class C_Tickrecord;

struct HitscanData_t {
	float  m_damage;
	int m_hitchance;
	Vector m_pos;
	int m_hitbox;
	bool prefer;
	bool m_safepoint[19] = {};

	__forceinline HitscanData_t() : m_damage{ 0.f }, m_pos{} {}
};

struct HitscanBox_t {
	int         m_index;
	HitscanMode m_mode;

	__forceinline HitscanBox_t(int id, HitscanMode m)
	{
		m_index = id;
		m_mode = m;
	}

	__forceinline bool operator==(const HitscanBox_t& c) const {
		return m_index == c.m_index && m_mode == c.m_mode;
	}
};

class c_aimbot
{
public:
	virtual void get_hitbox_data(C_Hitbox* rtn, C_BasePlayer* ent, int ihitbox, matrix3x4_t* matrix);
	//bool safe_static_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx);
	//virtual bool safe_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx, bool maxdamage);
	//virtual bool safe_static_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx, bool maxdamage);
	//virtual void draw_capsule(C_BasePlayer* ent, int ihitbox);
	virtual Vector get_hitbox(C_BasePlayer* ent, int ihitbox, matrix3x4_t mat[]);
	//float can_hit(int hitbox, C_BasePlayer* Entity, matrix3x4_t mx[], bool* was_viable = nullptr, bool force_pointscale = false);
	virtual void build_seed_table();
	virtual void build_mini_hc_table();
	virtual bool mini_hit_chance(Vector vhitbox, C_BasePlayer* ent, int hitbox, int& hc);
	virtual int hitbox2hitgroup(C_BasePlayer* m_player, int ihitbox);
	virtual int safe_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx, C_Tickrecord* record);
	virtual bool safe_side_point(C_BasePlayer* entity, Vector eye_pos, Vector aim_point, int hitboxIdx, C_Tickrecord* record);
	virtual bool hit_chance(QAngle angle, Vector point, C_BasePlayer* ent, float chance, int hitbox, float damage, float* hc);
	virtual void visualize_hitboxes(C_BasePlayer* entity, matrix3x4_t* mx, Color color, float time);
	virtual void autostop(CUserCmd* cmd, C_WeaponCSBaseGun* local_weapon/*, C_BasePlayer * best_player, float dmg, bool hitchanced*/);
	//bool work(CUserCmd* cmd, bool* send_packet);

	virtual void OnRoundStart(C_BasePlayer* player);

	virtual void SetupHitboxes(C_BasePlayer* ent, C_Tickrecord* record, bool history);

	virtual void init();

	virtual void StripAttack(CUserCmd* cmd);

	virtual bool think(CUserCmd* cmd, bool* send_packet);

	virtual void find(CUserCmd* cmd);

	virtual bool GetBestAimPosition(C_BasePlayer* player, Vector& aim, float& damage, int& hitbox, C_Tickrecord* record, int players_iterated);

	virtual bool SelectTarget(C_BasePlayer* player, C_Tickrecord* record, const Vector& aim, float damage);

	virtual void apply(CUserCmd* cmd, bool* send_packet);

	//// essential data.
	using hitboxcan_t = std::vector< HitscanBox_t >;

	// aimbot data.
	hitboxcan_t m_hitboxes;

	/*std::vector< */TargetListing_t/*>*/   m_targets[64];
	Vector     m_aim;
	QAngle      m_angle;
	int m_targets_count = 0;
	// target selection stuff.
	float m_best_dist;
	float m_best_fov;
	float m_best_damage;
	int   m_best_hp;
	float m_best_lag;
	float m_best_height;

	// found target stuff.
	C_BasePlayer* m_target;
	float m_best_hc[2];
	int m_hitbox;
	bool m_damage_key;
	bool m_baim_key;
	float      m_damage;
	//int previous_player = 0;
	//bool will_shoot_2nd_wit_r8 = false;
	//float r8cock_time = 0.f;
	//bool is_cocking = false;
	//C_BasePlayer* best_player = nullptr;
	//Vector best_hitbox = Vector::Zero;
	//int	best_hitboxid = -1;
	//float last_shoot_time = 0.f;
	//int skip_player[64] = {};
	//static bool must_baim_player[64] = {};
	//int players_skipped = 0;

	struct table_t {
		uint8_t swing[2][2][2]; // [ first ][ armor ][ back ]
		uint8_t stab[2][2];		  // [ armor ][ back ]
	};

	const table_t m_knife_dmg{ { { { 25, 90 }, { 21, 76 } }, { { 40, 90 }, { 34, 76 } } }, { { 65, 180 }, { 55, 153 } } };

	std::array< QAngle, 12 > m_knife_ang{
		QAngle{ 0.f, 0.f, 0.f }, QAngle{ 0.f, -90.f, 0.f }, QAngle{ 0.f, 90.f, 0.f }, QAngle{ 0.f, 180.f, 0.f },
		QAngle{ -80.f, 0.f, 0.f }, QAngle{ -80.f, -90.f, 0.f }, QAngle{ -80.f, 90.f, 0.f }, QAngle{ -80.f, 180.f, 0.f },
		QAngle{ 80.f, 0.f, 0.f }, QAngle{ 80.f, -90.f, 0.f }, QAngle{ 80.f, 90.f, 0.f }, QAngle{ 80.f, 180.f, 0.f }
	};

	std::array<std::tuple<float, float, float, float, float>, 150> precomputed_seeds;
	//std::array<std::tuple<float, float, float, float, float>, 150> precomputed_r8_seeds;
	int seeds_filled = 0;
	/*int r8_seeds_filled = 0;*/
	std::array<std::tuple<float, float, float, float, float>, 64> precomputed_mini_seeds;
	bool knife(bool* send_packet, std::vector<TargetListing_t>& m_entities, CUserCmd* cmd);
	bool can_knife(C_BasePlayer* m_player, C_Tickrecord* record, QAngle angle, bool& stab);
	bool knife_trace(Vector dir, bool stab, CGameTrace* trace);
	bool knife_is_behind(C_Tickrecord* record);
};