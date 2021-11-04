#include "entity.hpp"
#include "displacement.hpp"
#include "prop_manager.hpp"

void IHandleEntity::SetRefEHandle( const CBaseHandle& handle )
{
	using Fn = void ( __thiscall* )( void*, const CBaseHandle& );
	return Memory::VCall<Fn>( this, Index::IHandleEntity::SetRefEHandle )( this, handle );
}

const CBaseHandle& IHandleEntity::GetRefEHandle()
{
	using Fn = const CBaseHandle& ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::IHandleEntity::GetRefEHandle )( this );
}

ICollideable* IClientUnknown::GetCollideable()
{
	static auto m_Collision = Engine::PropManager::Instance()->GetOffset("DT_BaseEntity", "m_Collision");

	return (ICollideable*)(uintptr_t(this) + m_Collision);
	//using Fn = ICollideable* ( __thiscall* )( void* );
	//return Memory::VCall<Fn>( this, Index::IClientUnknown::GetCollideable )( this );
}

IClientNetworkable* IClientUnknown::GetClientNetworkable()
{
	//using Fn = IClientNetworkable* ( __thiscall* )( void* );
	//return Memory::VCall<Fn>( this, Index::IClientUnknown::GetClientNetworkable )( this );
	return (IClientNetworkable*)(uintptr_t(this) + 0x8);
}

IClientRenderable* IClientUnknown::GetClientRenderable()
{
	/*if (Source::m_pClientState->m_iDeltaTick == -1 || Source::m_pCvar->FindVar("mat_norendering")->GetInt())
		return nullptr;*/
		//using Fn = IClientRenderable* ( __thiscall* )( void* );
		//return Memory::VCall<Fn>( this, Index::IClientUnknown::GetClientRenderable )( this );
	return (IClientRenderable*)(uintptr_t(this) + 0x4);
}

IClientEntity* IClientUnknown::GetIClientEntity()
{
	using Fn = IClientEntity* ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::IClientUnknown::GetIClientEntity )( this );
}

C_BaseEntity* IClientUnknown::GetBaseEntity()
{
	using Fn = C_BaseEntity* ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::IClientUnknown::GetBaseEntity )( this );
}

Vector& ICollideable::OBBMins()
{
	using Fn = Vector& ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::ICollideable::OBBMins )( this );
}

Vector& ICollideable::OBBMaxs()
{
	using Fn = Vector& ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::ICollideable::OBBMaxs )( this );
}

solid_type ICollideable::GetSolidType()
{
	using Fn = solid_type(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, Index::ICollideable::SolidType)(this);
}

//ClientClass* IClientNetworkable::GetClientClass()
//{
//	using Fn = ClientClass* ( __thiscall* )( void* );
//	return Memory::VCall<Fn>( this, Index::IClientNetworkable::GetClientClass )( this );
//}

bool IClientNetworkable::IsDormant()
{
	using Fn = bool ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::IClientNetworkable::IsDormant )( this );
}

int IClientNetworkable::entindex()
{
	using Fn = int(__thiscall*)(void*);

	if (this && *(void**)this != nullptr)
		return Memory::VCall<Fn>(this, Index::IClientNetworkable::entindex)(this);
	else
		return 0;
}

Vector& IClientEntity::OBBMins()
{
	return GetCollideable()->OBBMins();
}

Vector& IClientEntity::OBBMaxs()
{
	return GetCollideable()->OBBMaxs();
}

ClientClass* IClientEntity::GetClientClass()
{
	return GetClientNetworkable()->GetClientClass();
}

bool IClientEntity::IsDormant()
{
	if (this == nullptr || *(void**)this == nullptr)
		return true;

	//auto networkable = GetClientNetworkable();
	return *(bool*)(uintptr_t(this) + 0xED);//networkable->IsDormant();
}

int IClientEntity::entindex()
{
	//auto networkable = GetClientNetworkable();
	//return networkable->entindex();
	if (!this)
		return 0;

	return *(int*)(uintptr_t(this) + 0x64);
}

const model_t* IClientEntity::GetModel()
{
	//if (!GetClientRenderable())
	//	return nullptr;
	return GetClientRenderable()->GetModel();
}

void IClientEntity::GetWorldSpaceCenter(Vector& wSpaceCenter) {
	void* cRender = (void*)(this + 0x4);
	typedef void(__thiscall * fn)(void*, Vector&, Vector&);
	Vector va, vb;
	Memory::VCall<fn>(cRender, 17)(cRender, va, vb); // GetRenderBounds : 17
	wSpaceCenter.z += (va.z + vb.z) * 0.5f;
}

bool IClientEntity::is_breakable()
{
	//if (!this)
	//	return false;

	static auto is_breakable_fn = reinterpret_cast<bool(__thiscall*)(IClientEntity*)>(Engine::Displacement::Signatures[c_signatures::BREAKABLE]);

	//const auto result = is_breakable_fn(this);

	//if (!result && GetClientClass() != nullptr &&
	//	(GetClientClass()->m_ClassID == class_ids::CBaseDoor ||
	//		GetClientClass()->m_ClassID == class_ids::CBreakableSurface ||
	//		(GetClientClass()->m_ClassID == class_ids::CBaseEntity && GetCollideable() != nullptr && GetCollideable()->GetSolidType() == solid_bsp/*solid_bsp*/)))
	//	return true;

	//return result;

	if (!this || !GetCollideable() || !GetClientClass())
		return false;

	auto client_class = GetClientClass();

	if (this->entindex() > 0) {
		if (client_class)
		{
			auto v3 = (int)client_class->m_pNetworkName;
			if (*(DWORD*)v3 == 0x65724243)
			{
				if (*(DWORD*)(v3 + 7) == 0x53656C62)
					return 1;
			}
			if (*(DWORD*)v3 == 0x73614243)
			{
				if (*(DWORD*)(v3 + 7) == 0x79746974)
					return 1;
			}

			return is_breakable_fn(this);
		}

		return is_breakable_fn(this);
	}
	return 0;

	//auto m_take_damage = *(uintptr_t*)((uintptr_t)Engine::Displacement::Signatures[c_signatures::BREAKABLE] + 38);
	//auto backup = *(uint8_t*)(uintptr_t(this) + m_take_damage);

	//// fix world desync between server and client ds

	//auto hash_name = hash_32_fnv1a_const(client_class->m_pNetworkName);
	//if (hash_name == hash_32_fnv1a_const("CBreakableSurface"))
	//	* (uint8_t*)(uintptr_t(this) + m_take_damage) = 2; // DAMAGE_YES
	//else if (hash_name == hash_32_fnv1a_const("CBaseDoor") || hash_name == hash_32_fnv1a_const(("CDynamicProp")))
	//	* (uint8_t*)(uintptr_t(this) + m_take_damage) = 0; // DAMAGE_NO	

	//using fn_t = bool(__thiscall*)(IClientEntity*);
	//auto result = ((fn_t)Engine::Displacement::Signatures[c_signatures::BREAKABLE])(this);
	//*(uint8_t*)(uintptr_t(this) + m_take_damage) = backup;

	//return result;
}

bool IClientEntity::SetupBones( matrix3x4a_t* pBoneToWorld, int nMaxBones, int boneMask, float currentTime )
{
	auto renderable = GetClientRenderable();
	return renderable->SetupBones( pBoneToWorld, nMaxBones, boneMask, currentTime );
}

bool C_BaseEntity::IsPlayer()
{
	using Fn = bool ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::C_BaseEntity::IsPlayer )( this );
}

unsigned char& C_BaseEntity::m_MoveType()
{
	return *(unsigned char* )(uintptr_t(this) + Engine::Displacement::C_BaseEntity::m_MoveType );
}

unsigned char& C_BaseEntity::m_MoveCollide()
{
	return *(unsigned char* )(uintptr_t(this) + Engine::Displacement::C_BaseEntity::m_MoveType + 1 );
}

matrix3x4_t& C_BaseEntity::m_rgflCoordinateFrame()
{
	return *( matrix3x4_t* )(uintptr_t(this) + Engine::Displacement::C_BaseEntity::m_rgflCoordinateFrame );
}

int& C_BaseEntity::m_iTeamNum()
{
	return *( int* )(uintptr_t(this) + Engine::Displacement::DT_BaseEntity::m_iTeamNum );
}

Vector& C_BaseEntity::m_vecOrigin()
{
	return *( Vector* )(uintptr_t(this) + Engine::Displacement::DT_BaseEntity::m_vecOrigin );
}

void C_BaseEntity::SetPredictionRandomSeed( CUserCmd* cmd )
{
	*( int* )(Engine::Displacement::Signatures[c_signatures::PREDRANDOMSEED]) = cmd ? cmd->random_seed : -1;
}

void C_BaseEntity::SetPredictionPlayer( C_BasePlayer* player )
{
	*( C_BasePlayer** )(Engine::Displacement::Signatures[c_signatures::PREDPLAYER]) = player;
}

CBaseHandle& C_BaseCombatCharacter::m_hActiveWeapon()
{
	return *( CBaseHandle* )(uintptr_t(this) + Engine::Displacement::DT_BaseCombatCharacter::m_hActiveWeapon );
}