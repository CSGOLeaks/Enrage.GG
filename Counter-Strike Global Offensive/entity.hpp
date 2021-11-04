#pragma once

#include "sdk.hpp"

#pragma region decl_indices
namespace Index
{
namespace IHandleEntity
{
enum
{
	SetRefEHandle = 1,
	GetRefEHandle = 2,
};
}
namespace IClientUnknown
{
enum
{
	GetCollideable = 3,
	GetClientNetworkable = 4,
	GetClientRenderable = 5,
	GetIClientEntity = 6,
	GetBaseEntity = 7,
};
}
namespace ICollideable
{
	enum
	{
		OBBMins = 1,
		OBBMaxs = 2,
		SolidType = 11,
	};
}
namespace IClientNetworkable
{
	enum
	{
		GetClientClass = 2,
		IsDormant = 9,
		entindex = 10,
		GetDataTableBasePtr = 12,
	};
}
namespace IClientRenderable
{
enum
{
	GetModel = 8,
	SetupBones = 13,
};
}
namespace C_BaseEntity
{
enum
{
	IsPlayer = 157,
};
}
}
#pragma endregion

class IHandleEntity
{
public:
	void SetRefEHandle( const CBaseHandle& handle );
	const CBaseHandle& GetRefEHandle();
};

class IClientUnknown : public IHandleEntity
{
public:
	ICollideable* GetCollideable();
	IClientNetworkable* GetClientNetworkable();
	IClientRenderable* GetClientRenderable();
	IClientEntity* GetIClientEntity();
	C_BaseEntity* GetBaseEntity();
};

enum solid_type : int
{
	solid_none = 0,
	solid_bsp = 1,
	solid_bbox = 2,
	solid_obb = 3,
	solid_obb_yaw = 4,
	solid_custom = 5,
	solid_vphysics = 6,
	solid_last
};

class ICollideable
{
public:
	Vector& OBBMins();
	Vector& OBBMaxs();
	solid_type GetSolidType();
};

class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;

	// Called by the engine when the server deletes the entity.
	virtual void			Release() = 0;

	// Supplied automatically by the IMPLEMENT_CLIENTCLASS macros.
	virtual ClientClass* GetClientClass() = 0;

	// This tells the entity what the server says for ShouldTransmit on this entity.
	// Note: This used to be EntityEnteredPVS/EntityRemainedInPVS/EntityLeftPVS.
	virtual void			NotifyShouldTransmit(int state) = 0;



	//
	// NOTE FOR ENTITY WRITERS: 
	//
	// In 90% of the cases, you should hook OnPreDataChanged/OnDataChanged instead of 
	// PreDataUpdate/PostDataUpdate.
	//
	// The DataChanged events are only called once per frame whereas Pre/PostDataUpdate
	// are called once per packet (and sometimes multiple times per frame).
	//
	// OnDataChanged is called during simulation where entity origins are correct and 
	// attachments can be used. whereas PostDataUpdate is called while parsing packets
	// so attachments and other entity origins may not be valid yet.
	//

	virtual void			OnPreDataChanged(int updateType) = 0;
	virtual void			OnDataChanged(int updateType) = 0;

	// Called when data is being updated across the network.
	// Only low-level entities should need to know about these.
	virtual void			PreDataUpdate(int updateType) = 0;
	virtual void			PostDataUpdate(int updateType) = 0;

	//ClientClass* GetClientClass();
	bool IsDormant();
	int entindex();
};

typedef unsigned short ClientShadowHandle_t;
typedef unsigned short ClientRenderHandle_t;
typedef unsigned short ModelInstanceHandle_t;
typedef unsigned char uint8_t;

class matrix3x4a_t;
class IClientUnknown;
struct model_t;

class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector const& GetRenderOrigin(void) = 0;
	virtual QAngle const& GetRenderAngles(void) = 0;
	virtual bool                       ShouldDraw(void) = 0;
	virtual int                        GetRenderFlags(void) = 0; // ERENDERFLAGS_xxx
	virtual void                       Unused(void) const {}
	virtual ClientShadowHandle_t       GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual const model_t* GetModel() const = 0;
	virtual int                        DrawModel(int flags, const int /*RenderableInstance_t*/& instance) = 0;
	virtual int                        GetBody() = 0;
	virtual void                       GetColorModulation(float* color) = 0;
	virtual bool                       LODTest() = 0;
	virtual bool                       SetupBones(matrix3x4a_t* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;
	virtual void                       SetupWeights(const matrix3x4a_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) = 0;
	virtual void                       DoAnimationEvents(void) = 0;
	virtual void* /*IPVSNotify*/       GetPVSNotifyInterface() = 0;
	virtual void                       GetRenderBounds(Vector& mins, Vector& maxs) = 0;
	virtual void                       GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;
	virtual void                       GetShadowRenderBounds(Vector& mins, Vector& maxs, int /*ShadowType_t*/ shadowType) = 0;
	virtual bool                       ShouldReceiveProjectedTextures(int flags) = 0;
	virtual bool                       GetShadowCastDistance(float* pDist, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       GetShadowCastDirection(Vector* pDirection, int /*ShadowType_t*/ shadowType) const = 0;
	virtual bool                       IsShadowDirty() = 0;
	virtual void                       MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual int /*ShadowType_t*/       ShadowCastType() = 0;
	virtual void                       CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t      GetModelInstance() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int                        LookupAttachment(const char* pAttachmentName) = 0;
	virtual   bool                     GetAttachment(int number, Vector& origin, QAngle& angles) = 0;
	virtual bool                       GetAttachment(int number, matrix3x4_t& matrix) = 0;
	virtual float* GetRenderClipPlane(void) = 0;
	virtual int                        GetSkin() = 0;
	virtual void                       OnThreadedDrawSetup() = 0;
	virtual bool                       UsesFlexDelayedWeights() = 0;
	virtual void                       RecordToolMessage() = 0;
	virtual bool                       ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual uint8_t                      OverrideAlphaModulation(uint8_t nAlpha) = 0;
	virtual uint8_t                      OverrideShadowAlphaModulation(uint8_t nAlpha) = 0;
};

class IClientEntity : public IClientUnknown
{
public:
	Vector& OBBMins();
	Vector& OBBMaxs();

	ClientClass* GetClientClass();
	bool IsDormant();
	int entindex();

	const model_t* GetModel();
	void GetWorldSpaceCenter(Vector& wSpaceCenter);
	bool is_breakable();
	bool SetupBones( matrix3x4a_t* pBoneToWorld, int nMaxBones, int boneMask, float currentTime );
};

class C_BaseEntity : public IClientEntity
{
public:
	bool IsPlayer();

	unsigned char& m_MoveType();
	unsigned char& m_MoveCollide();
	matrix3x4_t& m_rgflCoordinateFrame();

	int& m_iTeamNum();
	Vector& m_vecOrigin();

public:
	static void SetPredictionRandomSeed( CUserCmd* cmd );
	static void SetPredictionPlayer( C_BasePlayer* player );
};

class C_BaseCombatCharacter : public C_BaseEntity
{
public:
	CBaseHandle& m_hActiveWeapon();
};