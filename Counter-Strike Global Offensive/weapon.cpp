#include "weapon.hpp"
#include "displacement.hpp"
#include "source.hpp"
#include "prop_manager.hpp"
#include "prediction.hpp"
#include "rage_aimbot.hpp"

float& C_BaseCombatWeapon::m_flNextPrimaryAttack()
{
	return *( float* )(uintptr_t(this) + Engine::Displacement::DT_BaseCombatWeapon::m_flNextPrimaryAttack );
}

float& C_BaseCombatWeapon::m_flNextSecondaryAttack()
{
	return *( float* )(uintptr_t(this) + Engine::Displacement::DT_BaseCombatWeapon::m_flNextSecondaryAttack );
}

CBaseHandle& C_BaseCombatWeapon::m_hOwner()
{
	return *( CBaseHandle* )(uintptr_t(this) + Engine::Displacement::DT_BaseCombatWeapon::m_hOwner );
}

int& C_BaseCombatWeapon::m_iClip1()
{
	return *( int* )(uintptr_t(this) + Engine::Displacement::DT_BaseCombatWeapon::m_iClip1 );
}

int& C_BaseCombatWeapon::m_iPrimaryReserveAmmoCount()
{
	static auto m_iPrimaryReserveAmmoCount = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");

	return *( int* )(uintptr_t(this) + m_iPrimaryReserveAmmoCount);
}

short& C_BaseCombatWeapon::m_iItemDefinitionIndex()
{
	return *(short* )(uintptr_t(this) + Engine::Displacement::DT_BaseCombatWeapon::m_iItemDefinitionIndex );
}

int& C_BaseCombatWeapon::m_iItemIDHigh()
{
	static auto m_iItemIDHigh = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_iItemIDHigh");

	return *(int*)(uintptr_t(this) + m_iItemIDHigh);
}

float& C_BaseCombatWeapon::m_flMaxspeed()
{
	static auto m_flMaxspeed = Engine::PropManager::Instance()->GetOffset("DT_BasePlayer", "m_flMaxspeed");

	return *(float*)(uintptr_t(this) + m_flMaxspeed);
}

float& C_BaseCombatWeapon::m_flThrowStrength()
{
	static auto m_flThrowStrength = Engine::PropManager::Instance()->GetOffset("DT_BaseCSGrenade", "m_flThrowStrength");

	return *(float*)(uintptr_t(this) + m_flThrowStrength);
}

int& C_BaseCombatWeapon::m_nFallbackPaintKit()
{
	static auto m_nFallbackPaintKit = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_nFallbackPaintKit");

	return *(int*)(uintptr_t(this) + m_nFallbackPaintKit);
}

int& C_BaseCombatWeapon::m_nFallbackStatTrak()
{
	static auto m_nFallbackStatTrak = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_nFallbackStatTrak");

	return *(int*)(uintptr_t(this) + m_nFallbackStatTrak);
}

float& C_BaseCombatWeapon::m_flFallbackWear()
{
	static auto m_flFallbackWear = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_flFallbackWear");

	return *(float*)(uintptr_t(this) + m_flFallbackWear);
}

float& C_BaseCombatWeapon::m_nFallbackSeed()
{
	static auto m_nFallbackSeed = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_nFallbackSeed");

	return *(float*)(uintptr_t(this) + m_nFallbackSeed);
}

float& C_BaseCombatWeapon::m_flAccuracyPenalty()
{
	static auto m_flAccuracyPenalty = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBase", "m_fAccuracyPenalty");

	return *(float*)(uintptr_t(this) + m_flAccuracyPenalty);
}

int& C_BaseCombatWeapon::m_iAccountID()
{
	static auto m_iAccountID = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_iAccountID");

	return *(int*)(uintptr_t(this) + m_iAccountID);
}

int& C_BaseCombatWeapon::m_OriginalOwnerXuidLow()
{
	static auto m_OriginalOwnerXuidLow = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_OriginalOwnerXuidLow");

	return *(int*)(uintptr_t(this) + m_OriginalOwnerXuidLow);
}

int& C_BaseCombatWeapon::m_OriginalOwnerXuidHigh()
{
	static auto m_OriginalOwnerXuidHigh = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_OriginalOwnerXuidHigh");

	return *(int*)(uintptr_t(this) + m_OriginalOwnerXuidHigh);
}

int& C_BaseCombatWeapon::m_iEntityQuality()
{
	static auto m_iEntityQuality = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_iEntityQuality");

	return *(int*)(uintptr_t(this) + m_iEntityQuality);
}

int& C_BaseCombatWeapon::m_iWorldModelIndex()
{
	static auto m_iWorldModelIndex = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_iWorldModelIndex");

	return *(int*)(uintptr_t(this) + m_iWorldModelIndex);
}

CBaseHandle& C_BaseCombatWeapon::m_hWeaponWorldModel()
{
	static auto m_hWeaponWorldModel = Engine::PropManager::Instance()->GetOffset("DT_BaseCombatWeapon", "m_hWeaponWorldModel");

	return *(CBaseHandle*)(uintptr_t(this) + m_hWeaponWorldModel);
}

void C_BaseCombatWeapon::set_model_index(int index)
{
	typedef void(__thiscall * OriginalFn)(PVOID, int);
	return Memory::VCall<OriginalFn>(this, 75)(this, index);
}

float& C_WeaponCSBaseGun::m_flRecoilIndex()
{
	return *( float* )(uintptr_t(this) + Engine::Displacement::DT_WeaponCSBase::m_flRecoilIndex );
}

int& C_WeaponCSBaseGun::m_zoomLevel()
{
	static auto m_zoomLevel = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBaseGun", "m_zoomLevel");
	return *(int*)(uintptr_t(this) + m_zoomLevel);
}

float C_WeaponCSBaseGun::GetSpread()
{
	using Fn = float ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, 452)( this );
}

float C_WeaponCSBaseGun::GetInaccuracy()
{
	using Fn = float ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, 482)( this );
}

float C_WeaponCSBaseGun::GetMaxSpeed()
{
	//8B  01  8B  80  ??  ??  ??  ??  FF  D0  F3  0F  ??  ??  ??  ??  ??  ??  D9  ??  ??  ??  F3  ??  ??  ??  ??  ??  0F  2F  C3
	
	using Fn = float(__thiscall*)(void*);
	if (this == m_weapon())
	{
		if (ctx.latest_weapon_data != nullptr)
			return (!ctx.m_local()->m_bIsScoped() ? *(float*)(uintptr_t(ctx.latest_weapon_data) + 0x130) : *(float*)(uintptr_t(ctx.latest_weapon_data) + 0x134));
		else
			return 260.f;
	}

	return Memory::VCall<Fn>(this, 441)(this);
}

weapon_info* C_WeaponCSBaseGun::GetCSWeaponData()
{
	using Fn = weapon_info * (__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 460)(this);
}

void C_WeaponCSBaseGun::UpdateAccuracyPenalty()
{
	using Fn = void ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, 483)( this );
}

bool C_WeaponCSBaseGun::is_weapon()
{
	using Fn = bool(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 165)(this);
}

CSWeaponType C_WeaponCSBaseGun::GetWeaponType()
{
	using Fn = CSWeaponType(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 454)(this);
}

bool C_WeaponCSBaseGun::can_fire_bullet()
{
	using Fn = bool(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 418)(this);
}

bool C_WeaponCSBaseGun::IsFireTime()
{
	return ( csgo.m_globals()->curtime >= m_flNextPrimaryAttack() );
}

int& C_WeaponCSBaseGun::m_weaponMode()
{
	static auto m_weaponMode = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBase", "m_weaponMode");
	return *(int*)(uintptr_t(this) + m_weaponMode);
}

int& C_WeaponCSBaseGun::m_reloadState()
{
	//static auto m_reloadState = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBaseGun", "m_reloadState");
	return *(int*)(uintptr_t(this) + 0x000033D4);
}

bool& C_WeaponCSBaseGun::m_reload()
{
	//static auto m_reloadState = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBase", "m_reloadState");
	return *(bool*)(uintptr_t(this) + 0x32A5);
}

bool C_WeaponCSBaseGun::can_shoot()
{
	if (!this)
		return false;

	auto is_local_weapon = this == m_weapon();

	if (is_local_weapon && csgo.m_game_rules() != nullptr && csgo.m_game_rules()->IsFreezeTime())
		return false;

	float curtime = csgo.m_globals()->curtime;

	if (is_local_weapon) {
		auto tickbase = ctx.m_local()->m_nTickBase();
		
		if (!ctx.fakeducking && ctx.has_exploit_toggled && ctx.exploit_allowed && !ctx.applied_tickbase) {
			auto shift = 0;

			if (ctx.next_shift_amount > 0 || ctx.shift_amount > 0)
			{
				if (ctx.shift_amount == 0)
				{
					if (ctx.next_shift_amount > 0)
						shift = ctx.next_shift_amount;
				}
				else
					shift = ctx.shift_amount;
			}

			if (ctx.main_exploit == 2)
				shift = 0;

			if (ctx.main_exploit == 2 && ctx.force_aimbot < 1 && abs(ctx.current_tickcount - ctx.started_speedhack) >= 16 && ctx.ticks_allowed > 13)
				shift = 13;

			tickbase -= shift;
		}

		curtime = TICKS_TO_TIME(tickbase);

		if (ctx.m_local()->m_fFlags() & 0x40)
			return false;
		if (ctx.m_local()->m_bWaitForNoAttack())
			return false;
		if (ctx.m_local()->m_bIsDefusing())
			return false;
		if (m_reload()/* && ctx.m_local()->m_flNextAttack() >= curtime*/)
			return false;
		if (ctx.m_local()->m_iPlayerState() > 0)
			return false;
		//if (ctx.m_local()->m_iShotsFired() > 0 && (!can_fire_bullet() || !m_iClip1()))
		//	return false;
		if (m_iClip1() <= 0 && !is_knife() && m_iItemDefinitionIndex() != WEAPON_TASER)
			return false;
	}
	
	if (m_iItemDefinitionIndex() == 64)
	{
		auto can_attack = IsFireTime();

		if ((can_attack || is_local_weapon && IsSecondaryFireTime() && ctx.m_local()->m_iShotsFired() <= 0)
			&& (m_weaponMode() == 1
				|| m_Activity() != 208
				|| m_flPostponeFireReadyTime() >= TICKS_TO_TIME(ctx.m_local()->m_nTickBase())))
		{
			can_attack = false;
		}

		return can_attack;
	}

	if (curtime < ctx.m_local()->m_flNextAttack())
		return false;

	return (curtime >= m_flNextPrimaryAttack());

	//return (IsFireTime() || IsSecondaryFireTime());
}

bool C_WeaponCSBaseGun::can_exploit(int tickbase_shift)
{
	//if (ctx.ticks_allowed < tickbase_shift)
	//	return false;

	auto tickbase = ctx.m_local()->m_nTickBase();
	if (tickbase_shift == 0) {
		if (ctx.next_shift_amount > 0)
			tickbase += -1 - ctx.next_shift_amount;
	}
	else {
		tickbase -= tickbase_shift;
	}

	float curtime = TICKS_TO_TIME(tickbase);

	if (curtime < ctx.m_local()->m_flNextAttack())
		return false;

	if (curtime < m_flNextPrimaryAttack())
		return false;

	return true;
}

bool C_WeaponCSBaseGun::can_cock()
{
	if (ctx.m_local()->m_fFlags() & 0x40)
		return 0;
	if (csgo.m_game_rules())
	{
		if (csgo.m_game_rules()->IsFreezeTime())
			return 0;
	}
	if (!ctx.m_local()->get_weapon())
		return 0;
	if (ctx.m_local()->m_bIsDefusing())
		return false;
	if (m_iClip1() <= 0)
		return 0;
	auto v11 = ctx.m_local()->m_nTickBase();
	if (ctx.next_shift_amount > 0 && !ctx.applied_tickbase)
		v11 += -1 - ctx.next_shift_amount;
	auto v13 = (float)v11 * csgo.m_globals()->interval_per_tick;
	
	/*if (m_iItemDefinitionIndex() == 64 && a2)
	{
		if (!WasShootingLastTick)
		{
			v14 = m_flNextAttack;
			if (!m_flNextAttack)
			{
				v14 = GetNetvarOffset(0x97F07A62, 0x3DFDCDEA);
				m_flNextAttack = v14;
			}
			if (v13 >= *(float*)&v3[v14])
			{
				v15 = dword_4473B7B4;
				if (!dword_4473B7B4)
				{
					v15 = GetNetvarOffset(0xF2F193B3, -1237057684);
					dword_4473B7B4 = v15;
				}
				if (v13 > * (float*)((char*)v6 + v15))
					return 1;
			}
		}
		return 0;
	}*/
	auto v17 = ctx.m_local()->m_flNextAttack();
	if (v13 < v17)
		return 0;
	auto v18 = m_flNextPrimaryAttack();
	if (v13 < v18)
		return 0;
	if (ctx.m_settings.aimbot_enabled)
		return ctx.is_cocking;

	return v13 >= m_flPostponeFireReadyTime();
}

float& C_WeaponCSBaseGun::m_flLastShotTime()
{
	static auto m_fLastShotTime = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBase", "m_fLastShotTime");

	return *(float*)(uintptr_t(this) + m_fLastShotTime);
}

bool C_BaseCombatWeapon::IsGrenade()
{
	if (!this)
		return false;

	int item = this->m_iItemDefinitionIndex();

	if (item == WEAPON_FLASHBANG
		|| item == WEAPON_HEGRENADE
		|| item == WEAPON_SMOKEGRENADE
		|| item == WEAPON_MOLOTOV
		|| item == WEAPON_DECOY
		|| item == WEAPON_INCGRENADE
		|| item == WEAPON_TAGRENADE)
		return true;
	else
		return false;
};

bool C_BaseCombatWeapon::IsShotgun()
{
	if (!this)
		return false;

	switch (this->m_iItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_MP5SD:
	case WEAPON_UMP45:
		return false;
	case WEAPON_XM1014:
		return true;
	case WEAPON_BIZON:
		return false;
	case WEAPON_MAG7:
		return true;
	case WEAPON_NEGEV:
		return false;
	case WEAPON_SAWEDOFF:
		return true;
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
		return false;
	case WEAPON_NOVA:
		return true;
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return false;
	default:
		return false;
	}
}

float& C_WeaponCSBaseGun::m_fAccuracyPenalty()
{
	static auto m_fAccuracyPenalty = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBase", "m_fAccuracyPenalty");
	return *(float*)(uintptr_t(this) + m_fAccuracyPenalty);
}

bool C_BaseCombatWeapon::IsGun()
{
	if (!this)
		return false;

	int id = this->m_iItemDefinitionIndex();

	switch (id)
	{
	case WEAPON_DEAGLE:
	case WEAPON_ELITE:
	case WEAPON_FIVESEVEN:
	case WEAPON_GLOCK:
	case WEAPON_AK47:
	case WEAPON_AUG:
	case WEAPON_AWP:
	case WEAPON_FAMAS:
	case WEAPON_G3SG1:
	case WEAPON_GALILAR:
	case WEAPON_M249:
	case WEAPON_M4A1:
	case WEAPON_MAC10:
	case WEAPON_P90:
	case WEAPON_MP5SD:
	case WEAPON_UMP45:
	case WEAPON_XM1014:
	case WEAPON_BIZON:
	case WEAPON_MAG7:
	case WEAPON_NEGEV:
	case WEAPON_SAWEDOFF:
	case WEAPON_TEC9:
	case WEAPON_TASER:
	case WEAPON_HKP2000:
	case WEAPON_MP7:
	case WEAPON_MP9:
	case WEAPON_NOVA:
	case WEAPON_P250:
	case WEAPON_SCAR20:
	case WEAPON_SG556:
	case WEAPON_SSG08:
		return true;
	case WEAPON_KNIFE:
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_DECOY:
	case WEAPON_INCGRENADE:
	case WEAPON_C4:
	case WEAPON_KNIFE_T:
		return false;
	case WEAPON_M4A1_SILENCER:
	case WEAPON_USP_SILENCER:
	case WEAPON_CZ75A:
	case WEAPON_REVOLVER:
		return true;
	default:
		return false;
	}
}

std::string C_BaseCombatWeapon::get_icon()
{
	if (!this)
		return "";

	int id = this->m_iItemDefinitionIndex();

	switch (id)
	{
	case WEAPON_DEAGLE:
		return sxor("A");
	case WEAPON_ELITE:
		return sxor("B");
	case WEAPON_FIVESEVEN:
		return sxor("C");
	case WEAPON_GLOCK:
		return sxor("D");
	case WEAPON_AK47:
		return sxor("W");
	case WEAPON_AUG:
		return sxor("U");
	case WEAPON_AWP:
		return sxor("Z");
	case WEAPON_FAMAS:
		return sxor("R");
	case WEAPON_G3SG1:
		return sxor("X");
	case WEAPON_GALILAR:
		return sxor("Q");
	case WEAPON_M249:
		return sxor("g");
	case WEAPON_M4A1:
		return sxor("S");
	case WEAPON_MAC10:
		return sxor("K");
	case WEAPON_P90:
		return sxor("P");
	case WEAPON_MP5SD:
		return sxor("K");
	case WEAPON_UMP45:
		return sxor("L");
	case WEAPON_XM1014:
		return sxor("b");
	case WEAPON_BIZON:
		return sxor("M");
	case WEAPON_MAG7:
		return sxor("d");
	case WEAPON_NEGEV:
		return sxor("f");
	case WEAPON_SAWEDOFF:
		return sxor("c");
	case WEAPON_TEC9:
		return sxor("H");
	case WEAPON_TASER:
		return sxor("h");
	case WEAPON_HKP2000:
		return sxor("E");
	case WEAPON_MP7:
		return sxor("N");
	case WEAPON_MP9:
		return sxor("O");
	case WEAPON_NOVA:
		return sxor("e");
	case WEAPON_P250:
		return sxor("F");
	case WEAPON_SCAR20:
		return sxor("Y");
	case WEAPON_SG556:
		return sxor("V");
	case WEAPON_SSG08:
		return sxor("a");
	case WEAPON_KNIFE:
		return sxor("]");
	case WEAPON_FLASHBANG:
		return sxor("i");
	case WEAPON_HEGRENADE:
		return sxor("j");
	case WEAPON_SMOKEGRENADE:
		return sxor("k");
	case WEAPON_MOLOTOV:
		return sxor("l");
	case WEAPON_DECOY:
		return sxor("m");
	case WEAPON_INCGRENADE:
		return sxor("n");
	case WEAPON_C4:
		return sxor("o");
	case WEAPON_KNIFE_T:
		return sxor("[");
	case WEAPON_M4A1_SILENCER:
		return sxor("T");
	case WEAPON_USP_SILENCER:
		return sxor("G");
	case WEAPON_CZ75A:
		return sxor("I");
	case WEAPON_REVOLVER:
		return sxor("J");
	default:
		return sxor("]");
	}

	return "";
}

bool C_WeaponCSBaseGun::IsSniper()
{
	if (!this)
		return false;

	auto id = m_iItemDefinitionIndex();

	if (id == WEAPON_AWP || id == WEAPON_SSG08 || id == WEAPON_SCAR20 || id == WEAPON_G3SG1)
		return true;
	else
		return false;
}

bool C_BaseCombatWeapon::is_default_knife()
{
	return (m_iItemDefinitionIndex() == WEAPON_KNIFE || m_iItemDefinitionIndex() == WEAPON_KNIFE_T);
}

void C_BaseCombatWeapon::post_data_update(int updateType)
{
	if (!this) return;

	PVOID pNetworkable = (PVOID)(uintptr_t(this) + 0x8);

	if (!pNetworkable)
		return;

	typedef void(__thiscall * OriginalFn)(PVOID, int);
	return Memory::VCall<OriginalFn>(pNetworkable, 7)(pNetworkable, updateType);
}

void C_BaseCombatWeapon::on_data_changed(int updateType)
{
	if (!this) return;

	PVOID pNetworkable = (PVOID)(uintptr_t(this) + 0x8);

	if (!pNetworkable)
		return;

	typedef void(__thiscall * OriginalFn)(PVOID, int);
	return Memory::VCall<OriginalFn>(pNetworkable, 5)(pNetworkable, updateType);
}

void C_BaseCombatWeapon::pre_data_update(int updateType)
{
	if (!this) return;

	PVOID pNetworkable = (PVOID)(uintptr_t(this) + 0x8);

	if (!pNetworkable)
		return;

	typedef void(__thiscall * OriginalFn)(PVOID, int);
	return Memory::VCall<OriginalFn>(pNetworkable, 6)(pNetworkable, updateType);
}

bool C_BaseCombatWeapon::is_knife()
{
	return (is_default_knife() || m_iItemDefinitionIndex() == WEAPON_KNIFEGG || m_iItemDefinitionIndex() == WEAPON_BAYONET || m_iItemDefinitionIndex() == WEAPON_KNIFE_BUTTERFLY
		|| m_iItemDefinitionIndex() == WEAPON_KNIFE_FALCHION || m_iItemDefinitionIndex() == WEAPON_KNIFE_FLIP || m_iItemDefinitionIndex() == WEAPON_KNIFE_GUT
		|| m_iItemDefinitionIndex() == WEAPON_KNIFE_KARAMBIT || m_iItemDefinitionIndex() == WEAPON_KNIFE_M9_BAYONET || m_iItemDefinitionIndex() == WEAPON_KNIFE_PUSH
		|| m_iItemDefinitionIndex() == WEAPON_KNIFE_SURVIVAL_BOWIE || m_iItemDefinitionIndex() == WEAPON_KNIFE_TACTICAL || m_iItemDefinitionIndex() == WEAPON_KNIFE_URSUS
		|| m_iItemDefinitionIndex() == WEAPON_KNIFE_CANIS || m_iItemDefinitionIndex() == WEAPON_KNIFE_CORD || m_iItemDefinitionIndex() == WEAPON_KNIFE_CSS
		|| m_iItemDefinitionIndex() == WEAPON_KNIFE_GYPSY_JACKKNIFE || m_iItemDefinitionIndex() == WEAPON_KNIFE_STILETTO || m_iItemDefinitionIndex() == WEAPON_KNIFE_WIDOWMAKER || m_iItemDefinitionIndex() == WEAPON_KNIFE_SKELETON);
}

float& C_WeaponCSBaseGun::m_flPostponeFireReadyTime()
{
	static auto m_flPostponeFireReadyTime = Engine::PropManager::Instance()->GetOffset("DT_WeaponCSBase", "m_flPostponeFireReadyTime");
	return *(float*)(uintptr_t(this) + m_flPostponeFireReadyTime);
}

int& C_WeaponCSBaseGun::m_Activity()
{
	static auto nigger = ((C_BasePlayer*)this)->FindInDataMap(((C_BasePlayer*)this)->GetPredDescMap(), "m_Activity");
	return *(int*)(uintptr_t(this) + nigger);
}

bool C_WeaponCSBaseGun::IsSecondaryFireTime()
{
	return (csgo.m_globals()->curtime >= m_flNextSecondaryAttack() );
}