#include "sdk.hpp"
#include "displacement.hpp"
#include "player.hpp"
#include "source.hpp"
#include "weapon.hpp"

const char* string_t::ToCStr() const
{
	return ( pszValue ) ? pszValue : "";
}

CUserCmd::CUserCmd()
{
	Reset();
}

CUserCmd::CUserCmd( const CUserCmd& cmd )
{
	( *this ) = cmd;
}

CUserCmd::~CUserCmd()
{

}

void CUserCmd::Reset()
{
	command_number = 0;
	tick_count = 0;
	viewangles.Set();
	aimdirection.Set();
	forwardmove = 0.0f;
	sidemove = 0.0f;
	upmove = 0.0f;
	buttons = 0;
	impulse = 0u;
	weaponselect = 0;
	weaponsubtype = 0;
	random_seed = 0;
	mousedx = 0;
	mousedy = 0;
	hasbeenpredicted = false;
	headangles.Set();
	headoffset.Set();
}

CRC32_t CUserCmd::GetChecksum() const
{
	CRC32_t crc;

	CRC32_Init( &crc );
	CRC32_ProcessBuffer( &crc, &command_number, sizeof( command_number ) );
	CRC32_ProcessBuffer( &crc, &tick_count, sizeof( tick_count ) );
	CRC32_ProcessBuffer( &crc, &viewangles, sizeof( viewangles ) );
	CRC32_ProcessBuffer( &crc, &aimdirection, sizeof( aimdirection ) );
	CRC32_ProcessBuffer( &crc, &forwardmove, sizeof( forwardmove ) );
	CRC32_ProcessBuffer( &crc, &sidemove, sizeof( sidemove ) );
	CRC32_ProcessBuffer( &crc, &upmove, sizeof( upmove ) );
	CRC32_ProcessBuffer( &crc, &buttons, sizeof( buttons ) );
	CRC32_ProcessBuffer( &crc, &impulse, sizeof( impulse ) );
	CRC32_ProcessBuffer( &crc, &weaponselect, sizeof( weaponselect ) );
	CRC32_ProcessBuffer( &crc, &weaponsubtype, sizeof( weaponsubtype ) );
	CRC32_ProcessBuffer( &crc, &random_seed, sizeof( random_seed ) );
	CRC32_ProcessBuffer( &crc, &mousedx, sizeof( mousedx ) );
	CRC32_ProcessBuffer( &crc, &mousedy, sizeof( mousedy ) );
	CRC32_Final( &crc );

	return crc;
}

CUserCmd& CUserCmd::operator = ( const CUserCmd& cmd )
{
	if( this == &cmd )
		return ( *this );
	
	command_number = cmd.command_number;
	tick_count = cmd.tick_count;
	viewangles = cmd.viewangles;
	aimdirection = cmd.aimdirection;
	forwardmove = cmd.forwardmove;
	sidemove = cmd.sidemove;
	upmove = cmd.upmove;
	buttons = cmd.buttons;
	impulse = cmd.impulse;
	weaponselect = cmd.weaponselect;
	weaponsubtype = cmd.weaponsubtype;
	random_seed = cmd.random_seed;
	mousedx = cmd.mousedx;
	mousedy = cmd.mousedy;
	hasbeenpredicted = cmd.hasbeenpredicted;
	headangles = cmd.headangles;
	headoffset = cmd.headoffset;

	return ( *this );
}

ClientClass* IBaseClientDLL::GetAllClasses()
{
	using Fn = ClientClass* ( __thiscall* )( void* );
	return Memory::VCall<Fn>( this, Index::IBaseClientDLL::GetAllClasses )( this );
}

bool IBaseClientDLL::WriteUsercmdDeltaToBuffer(int nSlot, void* buf,
	int from, int to, bool isNewCmd)
{
	using Fn = bool(__thiscall*)(void*, int, void*, int, int, bool);
	return Memory::VCall<Fn>(this, 24)(this, nSlot, buf, from, to, isNewCmd);
}

bool IBaseClientDLL::IsChatRaised() {
	using Fn = bool(__thiscall*)(void*);
	return Memory::VCall<Fn>(this, 90)(this);
}

//void IPrediction::SetupMove( C_BasePlayer* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move )
//{
//	using Fn = void ( __thiscall* )( void*, C_BasePlayer*, CUserCmd*, IMoveHelper*, CMoveData* );
//	return Memory::VCall<Fn>( this, Index::IPrediction::SetupMove )( this, player, ucmd, pHelper, move );
//}
//
//void IPrediction::FinishMove( C_BasePlayer* player, CUserCmd* ucmd, CMoveData* move )
//{
//	using Fn = void ( __thiscall* )( void*, C_BasePlayer*, CUserCmd*, CMoveData* );
//	return Memory::VCall<Fn>( this, Index::IPrediction::FinishMove )( this, player, ucmd, move );
//}

CUserCmd* IInput::GetUserCmd( int sequence_number )
{
	return &m_pCommands[sequence_number % MULTIPLAYER_BACKUP];
}

CVerifiedUserCmd* IInput::GetVerifiedUserCmd( int sequence_number )
{
	return &m_pVerifiedCommands[sequence_number % MULTIPLAYER_BACKUP];
}

KeyValues* KeyValues::FindKey(const char* keyName, bool bCreate)
{
	static auto key_values_find_key = reinterpret_cast<KeyValues * (__thiscall*)(void*, const char*, bool)>(Memory::Scan("client.dll", "55 8B EC 83 EC 1C 53 8B D9 85 DB"));
	return key_values_find_key(this, keyName, bCreate);
}

void KeyValues::SetString(const char* keyName, const char* value)
{
	auto key = FindKey(keyName, true);
	if (key)
	{
		static auto key_values_set_string = reinterpret_cast<void(__thiscall*)(void*, const char*)>(Memory::Scan("client.dll", "55 8B EC A1 ? ? ? ? 53 56 57 8B F9 8B 08 8B 01"));
		key_values_set_string(key, value);
	}
}
void KeyValues::InitKeyValues(const char* name)
{
	static auto key_values = reinterpret_cast<void(__thiscall*)(void*, const char*)>(Memory::Scan("client.dll", "55 8B EC 51 33 C0 C7 45 ? ? ? ? ? 56 8B F1 81 26 ? ? ? ? C6 46 03 ? 89 46 10 89 46 18 89 46 14 89 46 1C 89 46 04 89 46 08 89 46 0C FF 15 ? ? ? ? 6A 01 FF 75 08 8D 4D FC 8B 10 51 8B C8 FF 52 24 8B 0E 33 4D FC 81 E1 ? ? ? ? 31 0E 88 46 03"));
	key_values(this, name);
}
void KeyValues::SetUint64(const char* keyName, int value, int value2)
{
	static auto key_values_set_uint64 = reinterpret_cast<void(__thiscall*)(void*, const char*, int, int)>(Memory::Scan("client.dll", "55 8B EC 56 6A 01 FF 75 08"));
	key_values_set_uint64(this, keyName, value, value2);
}
const char* KeyValues::GetString(const char* keyName, const char* defaultValue)
{
	static auto key_values_get_string = reinterpret_cast<const char* (__thiscall*)(void*, const char*, const char*)>(Memory::Scan("client.dll", "55 8B EC 83 E4 C0 81 EC ? ? ? ? 53 8B 5D 08"));
	return key_values_get_string(this, keyName, defaultValue);
}
int KeyValues::GetInt(const char* keyName, int defaultValue)
{
	static auto key_values_get_int = reinterpret_cast<int(__thiscall*)(void*, const char*, int)>(Memory::Scan("client.dll", "55 8B EC 6A ? FF 75 08 E8 ? ? ? ? 85 C0 74 45"));
	return key_values_get_int(this, keyName, defaultValue);
}
void KeyValues::SetInt(const char* keyName, int Value)
{
	auto key_int = FindKey(keyName, true);
	if (key_int)
	{
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(key_int) + 0xC) = Value;
		*reinterpret_cast<char*>(reinterpret_cast<uintptr_t>(key_int) + 0x10) = 2;
	}
}

IMaterial* IMaterialSystem::CreateMaterial(bool flat, bool ignorez, bool wireframed)
{
	static int created = 0;

	static const char tmp[] =
	{
		"\"%s\"\
		\n{\
		\n\t\"$basetexture\" \"vgui/white_additive\"\
		\n\t\"$envmap\" \"\"\
		\n\t\"$model\" \"1\"\
		\n\t\"$receiveflashlight\" \"1\"\
		\n\t\"$singlepassflashlight\" \"1\"\
		\n\t\"$flat\" \"1\"\
		\n\t\"$nocull\" \"0\"\
		\n\t\"$selfillum\" \"1\"\
		\n\t\"$halflambert\" \"1\"\
		\n\t\"$nofog\" \"0\"\
		\n\t\"$ignorez\" \"%i\"\
		\n\t\"$znearer\" \"0\"\
		\n\t\"$wireframe\" \"%i\"\
		\n}\n"
	};

	char material[512];
	sprintf_s(material, sizeof(material), tmp, (flat) ? "VertexLitGeneric" : "UnlitGeneric", (ignorez) ? 1 : 0, (wireframed) ? 1 : 0);

	char name[512];
	sprintf_s(name, sizeof(name), "mat_%i.vmt", created);
	++created;

	KeyValues* pKeyValues = new KeyValues();
	pKeyValues->InitKeyValues((flat) ? "VertexLitGeneric" : "UnlitGeneric");
	//LoadFromBuffer(name, tmp);

	typedef IMaterial* (__thiscall * OriginalFn)(void*, const char* pMaterialName, KeyValues * pVMTKeyValues);
	IMaterial* createdMaterial = Memory::VCall<OriginalFn>(this, 83)(this, name, pKeyValues);

	createdMaterial->IncrementReferenceCount();
	return createdMaterial;
}

#define VirtualFn( cast )typedef cast( __thiscall* OriginalFn )

IMaterial* IMaterialSystem::FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain, const char* pComplainPrefix)
{
	VirtualFn(IMaterial*)(PVOID, const char*, const char*, bool, const char*);
	return Memory::VCall< OriginalFn >(this, 84)(this, pMaterialName, pTextureGroupName, complain, pComplainPrefix);
}

MaterialHandle_t  IMaterialSystem::FirstMaterial()
{
	VirtualFn(MaterialHandle_t)(PVOID);
	return Memory::VCall< OriginalFn >(this, 86)(this);
}

MaterialHandle_t  IMaterialSystem::NextMaterial(MaterialHandle_t h)
{
	VirtualFn(MaterialHandle_t)(PVOID, MaterialHandle_t);
	return Memory::VCall< OriginalFn >(this, 87)(this, h);
}

MaterialHandle_t  IMaterialSystem::InvalidMaterial()
{
	VirtualFn(MaterialHandle_t)(PVOID);
	return Memory::VCall< OriginalFn >(this, 88)(this);
}

IMaterial* IMaterialSystem::GetMaterial(MaterialHandle_t h)
{
	VirtualFn(IMaterial*)(PVOID, MaterialHandle_t);
	return Memory::VCall< OriginalFn >(this, 89)(this, h);
}


void* IMaterialSystem::GetRenderContext() {
	typedef void* (*oGetRenderContext)(void*);
	return Memory::VCall<oGetRenderContext>(this, 115)(this);
}

const char* IPanel::GetName( vgui::VPANEL vguiPanel )
{
	using Fn = const char* ( __thiscall* )( void*, vgui::VPANEL );
	return Memory::VCall<Fn>( this, Index::IPanel::GetName )( this, vguiPanel );
}

auto IMemAlloc::Alloc(int nSize) -> void* {
	using Fn = void* (__thiscall*)(void*, int);
	return Memory::VCall< Fn >(this, 1)(this, nSize);
}

auto IMemAlloc::Realloc(void* pMem, int nSize) -> void* {
	using Fn = void* (__thiscall*)(void*, void*, int);
	return Memory::VCall< Fn >(this, 3)(this, pMem, nSize);
}

auto IMemAlloc::Free(void* pMem) -> void {
	using Fn = void(__thiscall*)(void*, void*);
	return Memory::VCall< Fn >(this, 5)(this, pMem);
}

#pragma region impl_functions
void RandomSeed( unsigned int seed )
{
	using Fn = void ( * )( unsigned int );
	auto Procedure = ( Fn )( Engine::Displacement::Function::m_uRandomSeed );

	if( Procedure )
		Procedure( seed );
}

float RandomFloat( float min, float max )
{
	using Fn = float ( * )( float, float );
	auto Procedure = ( Fn )( Engine::Displacement::Function::m_uRandomFloat );

	return ( Procedure ? Procedure( min, max ) : 0.0f );
}

int RandomInt( int min, int max )
{
	using Fn = int ( * )( int, int );
	auto Procedure = ( Fn )( Engine::Displacement::Function::m_uRandomInt );

	return ( Procedure ? Procedure( min, max ) : 0 );
}

#pragma region impl_crc32
#define CRC32_INIT_VALUE 0xFFFFFFFFUL
#define CRC32_XOR_VALUE  0xFFFFFFFFUL

#define NUM_BYTES 256

static const CRC32_t pulCRCTable[NUM_BYTES] =
{
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
	0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
	0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
	0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
	0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
	0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
	0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
	0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
	0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
	0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
	0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
	0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
	0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
	0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
	0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
	0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
	0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
	0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
	0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
	0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

void CRC32_Init( CRC32_t* pulCRC )
{
	*pulCRC = CRC32_INIT_VALUE;
}

void CRC32_ProcessBuffer( CRC32_t* pulCRC, const void* p, int len )
{
	CRC32_t ulCrc = *pulCRC;
	unsigned char *pb = (unsigned char *)p;
	unsigned int nFront;
	int nMain;

JustAfew:

	switch (len)
	{
	case 7:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

	case 6:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

	case 5:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

	case 4:
		ulCrc ^= ( *(CRC32_t *)pb );
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		*pulCRC = ulCrc;
		return;

	case 3:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

	case 2:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

	case 1:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

	case 0:
		*pulCRC = ulCrc;
		return;
	}

	nFront = ((unsigned int)pb) & 3;
	len -= nFront;
	switch (nFront)
	{
	case 3:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);
	case 2:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);
	case 1:
		ulCrc  = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);
	}

	nMain = len >> 3;
	while (nMain--)
	{
		ulCrc ^= ( *(CRC32_t *)pb );
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc ^= ( *(CRC32_t *)(pb + 4) );
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		ulCrc  = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
		pb += 8;
	}

	len &= 7;
	goto JustAfew;
}

void CRC32_Final( CRC32_t* pulCRC )
{
	*pulCRC ^= CRC32_XOR_VALUE;
}

CRC32_t CRC32_GetTableEntry( unsigned int slot )
{
	return pulCRCTable[ ( unsigned char )slot ];
}
#pragma endregion
#pragma endregion

bool& CClientState::m_bIsHLTV()
{
	return *(bool*)((uintptr_t)this + Engine::Displacement::Data::m_bIsHLTV);
	// TODO: insert return statement here
}

//CCSGOPlayerAnimState::CCSGOPlayerAnimState(C_BasePlayer* m_player)
//{
//	this->m_player = m_player;
//
//	m_cached_model_index = -1;
//	m_animstate_model_version = 2;
//	reset();
//}

void CCSGOPlayerAnimState::reset(void)
{
	m_last_update_frame = 0;
	m_step_height_left = 0;
	m_step_height_right = 0;

	//m_weapon = m_player->get_weapon();
	m_weapon_last = m_weapon;

	m_weapon_last_bone_setup = m_weapon;
	m_eye_position_smooth_lerp = 0;
	m_strafe_change_weight_smooth_fall_off = 0;
	m_first_foot_plant_since_init = true;

	m_last_update_time = 0;
	m_last_update_increment = 0;

	m_eye_yaw = 0;
	m_eye_pitch = 0;
	m_abs_yaw = 0;
	m_abs_yaw_last = 0;
	m_move_yaw = 0;
	m_move_yaw_ideal = 0;
	m_move_yaw_current_to_ideal = 0;

	m_stand_walk_how_long_to_wait_until_transition_can_blend_in = 0.4f;
	m_stand_walk_how_long_to_wait_until_transition_can_blend_out = 0.2f;
	m_stand_run_how_long_to_wait_until_transition_can_blend_in = 0.2f;
	m_stand_run_how_long_to_wait_until_transition_can_blend_out = 0.4f;
	m_crouch_walk_how_long_to_wait_until_transition_can_blend_in = 0.3f;
	m_crouch_walk_how_long_to_wait_until_transition_can_blend_out = 0.3f;

	m_primary_cycle = 0;
	m_move_weight = 0;
	m_move_weight_smoothed = 0;
	m_anim_duck_amount = 0;
	m_duck_additional = 0; // for when we duck a bit after landing from a jump
	m_recrouch_weight = 0;

	m_position_current.clear();
	m_position_last.clear();

	m_velocity.clear();
	m_velocity_normalized.clear();
	m_velocity_normalized_non_zero.clear();
	m_velocity_length_xy = 0;
	m_velocity_length_z = 0;

	m_speed_as_portion_of_run_top_speed = 0;
	m_speed_as_portion_of_walk_top_speed = 0;
	m_speed_as_portion_of_crouch_top_speed = 0;

	m_duration_moving = 0;
	m_duration_still = 0;

	m_on_ground = true;

	m_land_anim_multiplier = 1.0f;
	m_left_ground_height = 0;
	m_landing = false;
	m_jump_to_fall = 0;
	m_duration_in_air = 0;

	m_walk_run_transition = 0;

	m_landed_on_ground_this_frame = false;
	m_left_the_ground_this_frame = false;
	m_in_air_smooth_value = 0;

	m_on_ladder = false;
	m_ladder_weight = 0;
	m_ladder_speed = 0;

	m_walk_to_run_transition_state = 0;

	m_defuse_started = false;
	m_plant_anim_started = false;
	m_twitch_anim_started = false;
	m_adjust_started = false;

	m_next_twitch_time = 0;

	m_time_of_last_known_injury = 0;

	m_last_velocity_test_time = 0;
	m_velocity_last.clear();
	m_target_acceleration.clear();
	m_acceleration.clear();
	m_acceleration_weight = 0;

	m_aim_matrix_transition = 0;
	m_aim_matrix_transition_delay = 0;

	m_flashed = 0;

	m_strafe_change_weight = 0;
	m_strafe_change_target_weight = 0;
	m_strafe_change_cycle = 0;
	m_strafe_sequence = -1;
	m_strafe_changing = false;
	m_duration_strafing = 0;

	m_foot_lerp = 0;

	m_feet_crossed = false;

	m_player_is_accelerating = false;

	m_duration_move_weight_is_too_high = 0;
	m_static_approach_speed = 80;

	m_stutter_step = 0;
	m_previous_move_state = 0;

	m_action_weight_bias_remainder = 0;

	m_aim_yaw_min = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN;
	m_aim_yaw_max = CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX;
	m_aim_pitch_min = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN;
	m_aim_pitch_max = CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX;

	memset(&m_activity_modifiers_server[0], 0, 20);

	m_first_run_since_init = true;

	m_camera_smooth_height = 0;
	m_smooth_height_valid = false;
	m_last_time_velocity_over_ten = 0;
}

//----------------------------------------------------------------------------------
int animstate_pose_param_cache_t::get_index(void)
{
	if (!m_initialized)
		return -1;

	return m_index;
}

////these correlate to the CSWeaponType enum
//const char* const g_szWeaponPrefixLookupTable[] = {
//	"knife",
//	"pistol",
//	"smg",
//	"rifle",
//	"shotgun",
//	"sniper",
//	"heavy",
//	"c4",
//	"grenade",
//	"knife"
//};

// —œ¿—»¡Œ œ¿œ¿ÿ¿( ƒ∆ŒÕ  ¿–Ã¿  ) «¿ ›“”  ¬¿ Œ¬— ”ﬁ Ã¿“≈Ã¿“» ” 20 ¬≈ ¿ ( Õ” “”“ Õ≈ —œ»«ƒ»À –≈¿À‹ÕŒ 20 ¬≈  )
matrix3x4_t matrix3x4_t::ConcatTransforms(matrix3x4_t in) const {
	auto& m = m_flMatVal;
	matrix3x4_t out;
	out[0][0] = m[0][0] * in[0][0] + m[0][1] * in[1][0] + m[0][2] * in[2][0];
	out[0][1] = m[0][0] * in[0][1] + m[0][1] * in[1][1] + m[0][2] * in[2][1];
	out[0][2] = m[0][0] * in[0][2] + m[0][1] * in[1][2] + m[0][2] * in[2][2];
	out[0][3] = m[0][0] * in[0][3] + m[0][1] * in[1][3] + m[0][2] * in[2][3] + m[0][3];
	out[1][0] = m[1][0] * in[0][0] + m[1][1] * in[1][0] + m[1][2] * in[2][0];
	out[1][1] = m[1][0] * in[0][1] + m[1][1] * in[1][1] + m[1][2] * in[2][1];
	out[1][2] = m[1][0] * in[0][2] + m[1][1] * in[1][2] + m[1][2] * in[2][2];
	out[1][3] = m[1][0] * in[0][3] + m[1][1] * in[1][3] + m[1][2] * in[2][3] + m[1][3];
	out[2][0] = m[2][0] * in[0][0] + m[2][1] * in[1][0] + m[2][2] * in[2][0];
	out[2][1] = m[2][0] * in[0][1] + m[2][1] * in[1][1] + m[2][2] * in[2][1];
	out[2][2] = m[2][0] * in[0][2] + m[2][1] * in[1][2] + m[2][2] * in[2][2];
	out[2][3] = m[2][0] * in[0][3] + m[2][1] * in[1][3] + m[2][2] * in[2][3] + m[2][3];
	return out;
}

Vector matrix3x4_t::operator*(const Vector& vVec) const {
	auto& m = m_flMatVal;
	Vector vRet;
	vRet.x = m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z + m[0][3];
	vRet.y = m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z + m[1][3];
	vRet.z = m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z + m[2][3];

	return vRet;
}

matrix3x4_t matrix3x4_t::operator+(const matrix3x4_t& other) const {
	matrix3x4_t ret;
	auto& m = m_flMatVal;
	for (int i = 0; i < 12; i++) {
		((float*)ret.m_flMatVal)[i] = ((float*)m)[i] + ((float*)other.m_flMatVal)[i];
	}
	return ret;
}

matrix3x4_t matrix3x4_t::operator-(const matrix3x4_t& other) const {
	matrix3x4_t ret;
	auto& m = m_flMatVal;
	for (int i = 0; i < 12; i++) {
		((float*)ret.m_flMatVal)[i] = ((float*)m)[i] - ((float*)other.m_flMatVal)[i];
	}
	return ret;
}

matrix3x4_t matrix3x4_t::operator*(const float& other) const {
	matrix3x4_t ret;
	auto& m = m_flMatVal;
	for (int i = 0; i < 12; i++) {
		((float*)ret.m_flMatVal)[i] = ((float*)m)[i] * other;
	}
	return ret;
}


const char* CCSGOPlayerAnimState::GetWeaponPrefix(void)
{
	//<const char* (__thiscall*)(void*)>;
	using GetWeaponPrefixFn = const char*(__thiscall*)(void*);
	static auto get_weapon_prefix = (GetWeaponPrefixFn)Memory::Scan(sxor("client.dll"), "53 56 57 8B F9 33 F6 8B 4F 60 8B 01");
	return get_weapon_prefix(this);
	//int nWeaponType = 0; // knife

	//m_weapon = m_player->get_weapon();
	//if (m_weapon)
	//{
	//	auto weapon_info = m_weapon->GetCSWeaponData();
	//	nWeaponType = (int)m_weapon->GetWeaponType();

	//	int nWeaponID = m_weapon->m_iItemDefinitionIndex();
	//	if (nWeaponID == WEAPON_MAG7)
	//	{
	//		nWeaponType = WEAPONTYPE_RIFLE;
	//	}
	//	else if (nWeaponID == WEAPON_TASER)
	//	{
	//		nWeaponType = WEAPONTYPE_PISTOL;
	//	}

	//	if (nWeaponType == WEAPONTYPE_STACKABLEITEM)
	//	{
	//		nWeaponType = WEAPONTYPE_GRENADE; // redirect healthshot, adrenaline, etc to the grenade archetype
	//	}
	//}

	//return g_szWeaponPrefixLookupTable[Math::clamp(nWeaponType, 0, 9)];
	//return "";
}

float animstate_pose_param_cache_t::get_value(C_BasePlayer* pPlayer)
{
	//if (!m_initialized)
	//{
	//	init(pPlayer, m_name);
	//}
	//if (m_initialized && pPlayer)
	//{
	//	return pPlayer->GetPoseParameter(m_index);
	//}
	return 0;
}

void animstate_pose_param_cache_t::set_value(C_BasePlayer* pPlayer, float flValue)
{
	//if (!m_initialized)
	//{
		//init(pPlayer, m_name);
	//}
	if (m_initialized && pPlayer && flValue >= 0)
	{
		pPlayer->set_pose_param(m_index, flValue);
	}
}

//bool animstate_pose_param_cache_t::init(C_BasePlayer* pPlayer, const char* szPoseParamName)
//{
//	//MDLCACHE_CRITICAL_SECTION();
//	m_name = szPoseParamName;
//	m_index = pPlayer->LookupPoseParameter(szPoseParamName);
//	if (m_index != -1)
//	{
//		m_initialized = true;
//	}
//	return m_initialized;
//}
//----------------------------------------------------------------------------------

const mstudioposeparamdesc_t& CStudioHdr::pPoseParameter(int i)
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalPoseParameter(i);
	}

	if (m_pVModel->m_pose[i].group == 0)
		return *m_pStudioHdr->pLocalPoseParameter(m_pVModel->m_pose[i].index);

	const studiohdr_t* pStudioHdr = GroupStudioHdr(m_pVModel->m_pose[i].group);

	return *pStudioHdr->pLocalPoseParameter(m_pVModel->m_pose[i].index);
}

int	CStudioHdr::GetNumPoseParameters(void) const
{
	if (m_pVModel == NULL)
	{
		if (m_pStudioHdr)
			return m_pStudioHdr->numlocalposeparameters;
		else
			return 0;
	}

	return m_pVModel->m_pose.Count();
}

const studiohdr_t* CStudioHdr::GroupStudioHdr(int i)
{
	if (!this)
	{
		//ExecuteNTimes(5, Warning("Call to NULL CStudioHdr::GroupStudioHdr()\n"));
		return nullptr;
	}

	if (!m_pStudioHdrCache.IsValidIndex(i))
	{
		const char* pszName;
		pszName = (m_pStudioHdr) ? m_pStudioHdr->pszName() : "<<null>>";
		//ExecuteNTimes(5, Warning("Invalid index passed to CStudioHdr(%s)::GroupStudioHdr(): %d [%d]\n", pszName, i, m_pStudioHdrCache.Count()));
		//DebuggerBreakIfDebugging();
		return m_pStudioHdr; // return something known to probably exist, certainly things will be messed up, but hopefully not crash before the warning is noticed
	}

	const studiohdr_t* pStudioHdr = m_pStudioHdrCache[i];

	if (pStudioHdr == NULL)
	{
		virtualgroup_t* pGroup = &m_pVModel->m_group[i];
		pStudioHdr = pGroup->GetStudioHdr();
		m_pStudioHdrCache[i] = pStudioHdr;
	}

	//Assert(pStudioHdr);
	return pStudioHdr;
}

inline MDLHandle_t VoidPtrToMDLHandle(void* ptr)
{
	return (MDLHandle_t)(int)(intptr_t)ptr;
}

const studiohdr_t* virtualgroup_t::GetStudioHdr(void) const
{
	return csgo.m_mdl_cache()->GetStudioHdr(VoidPtrToMDLHandle(cache));
}