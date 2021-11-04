#pragma once

#include "core.hpp"
#include "math.hpp"

#define  Assert( _exp )										((void)0)

#define   DISPSURF_FLAG_SURFACE           (1<<0)
#define   DISPSURF_FLAG_WALKABLE          (1<<1)
#define   DISPSURF_FLAG_BUILDABLE         (1<<2)
#define   DISPSURF_FLAG_SURFPROP1         (1<<3)
#define   DISPSURF_FLAG_SURFPROP2         (1<<4)

class IClientEntity;
class IHandleEntity;

class VectorAligned : public Vector
{
public:
	VectorAligned() {}

	VectorAligned(const Vector& vec)
	{
		this->x = vec.x;
		this->y = vec.y;
		this->z = vec.z;
	}

	float w;
};

struct Ray_t
{
	Ray_t() { }

	VectorAligned		m_Start;
	VectorAligned		m_Delta;
	VectorAligned		m_StartOffset;
	VectorAligned		m_Extents;
	const matrix3x4_t* m_pWorldAxisTransform;
	bool				m_IsRay;
	bool				m_IsSwept;

	void Init(Vector vecStart, Vector vecEnd)
	{
		m_Delta = VectorAligned(vecEnd - vecStart);
		m_IsSwept = (m_Delta.LengthSquared() != 0);
		m_Extents.Zero;
		m_pWorldAxisTransform = NULL;
		m_IsRay = true;
		m_StartOffset.Zero;
		m_Start = vecStart;
	}

	void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
	{
		Assert(&end);
		Math::VectorSubtract(end, start, m_Delta);

		m_pWorldAxisTransform = NULL;
		m_IsSwept = (m_Delta.LengthSquared() != 0);

		Math::VectorSubtract(maxs, mins, m_Extents);
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSquared() < 1e-6);

		// Offset m_Start to be in the center of the box...
		Math::VectorAdd(mins, maxs, m_StartOffset);
		m_StartOffset *= 0.5f;
		Math::VectorAdd(start, m_StartOffset, m_Start);
		m_StartOffset *= -1.0f;
	}
};

struct cplane_t
{
	Vector normal;
	float dist;
	uint8_t type;   // for fast side tests
	uint8_t signbits;  // signx + (signy<<1) + (signz<<1)
	uint8_t pad[2];
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,				// NOTE: This does *not* test static props!!!
	TRACE_ENTITIES_ONLY,			// NOTE: This version will *not* test static props
	TRACE_EVERYTHING_FILTER_PROPS,	// NOTE: This version will pass the IHandleEntity for props through the filter, unlike all other filters
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}
	virtual TraceType_t	GetTraceType()
	{
		return TRACE_EVERYTHING;
	}
	void* pSkip;
};

class CTraceFilter : public ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
	void* pSkip;

	//bool ShouldHitEntity(IHandleEntity* pEntityHandle, int /*contentsMask*/)
	//{
	//	ClientClass* pEntCC = ((CBaseEntity*)pEntityHandle)->GetClientClass();
	//	if (pEntCC && strcmp(ccIgnore, ""))
	//	{
	//		if (pEntCC->m_pNetworkName == ccIgnore)
	//			return false;
	//	}

	//	return !(pEntityHandle == pSkip);
	//}

	//virtual TraceType_t GetTraceType() const
	//{
	//	return TraceType_t::TRACE_EVERYTHING;
	//}

	//inline void SetIgnoreClass(char* Class)
	//{
	//	ccIgnore = Class;
	//}

	//void* pSkip;
	//char* ccIgnore = "";
};

class CTraceFilterOneEntity : public ITraceFilter
{
public:
	bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle == pEntity);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pEntity;
};

class CTraceFilterPlayersOnlyNoWorld : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntityHandle, int contentsMask)
	{
		if (!pEntityHandle || pEntityHandle == pSkip)
			return false;

		if (((IClientEntity*)pEntityHandle) && AllowTeammates)
			return true;

		return false;
	}
	virtual TraceType_t	GetTraceType()
	{
		return TRACE_ENTITIES_ONLY;
	}
	bool AllowTeammates;
};

class CTraceCBaseEntity : public ITraceFilter
{
public:
	bool ShouldHitCBaseEntity(IClientEntity* pCBaseEntityHandle, int contentsMask)
	{
		return (pCBaseEntityHandle == pHit);
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_ENTITIES_ONLY;
	}
	void* pHit;
};

class CTraceFilterWorldOnly : public ITraceFilter
{
public:

	bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle && (*(int*)((uintptr_t)pEntityHandle + 0x64)) == 0);
	}

	TraceType_t GetTraceType()
	{
		return TRACE_WORLD_ONLY;
	}
};

class CTraceFilterWorldAndPropsOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientEntity* pServerEntity, int contentsMask)
	{
		return false;
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterSkipTwoEntities : public ITraceFilter {
public:
	CTraceFilterSkipTwoEntities(void* pPassEnt1, void* pPassEnt2) {
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(void* pEntityHandle, int contentsMask) {
		return !(pEntityHandle == passentity1 || pEntityHandle == passentity2);
	}

	virtual TraceType_t GetTraceType() const {
		return TRACE_EVERYTHING;
	}

	void* passentity1;
	void* passentity2;
};

typedef bool(*ShouldHitFunc_t)(IHandleEntity* pHandleEntity, int contentsMask);

class CTraceFilterSimple : public CTraceFilter
{
public:
	CTraceFilterSimple(const IHandleEntity* passedict, int collisionGroup,
		ShouldHitFunc_t pExtraShouldHitFunc = NULL)
	{
		m_pPassEnt = passedict;
		m_collisionGroup = collisionGroup;
		m_pExtraShouldHitCheckFunction = pExtraShouldHitFunc;
	}


	virtual bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}
	virtual void SetPassEntity(const IHandleEntity* pPassEntity) { m_pPassEnt = pPassEntity; }
	virtual void SetCollisionGroup(int iCollisionGroup) { m_collisionGroup = iCollisionGroup; }

	const IHandleEntity* GetPassEntity(void) { return m_pPassEnt; }

private:
	const IHandleEntity* m_pPassEnt;
	int m_collisionGroup;
	ShouldHitFunc_t m_pExtraShouldHitCheckFunction;

};

class CBaseTrace
{
public:
	bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:

	// these members are aligned!!
	Vector         startpos;            // start position
	Vector         endpos;              // final position
	cplane_t       plane;               // surface normal at impact

	float          fraction;            // time completed, 1.0 = didn't hit anything

	int            contents;            // contents on other side of surface hit
	unsigned short dispFlags;           // displacement flags for marking surfaces with data

	bool           allsolid;            // if true, plane is not valid
	bool           startsolid;          // if true, the initial point was in a solid area

	CBaseTrace() {}
};

struct csurface_t
{
	const char* name;
	short			surfaceProps;
	unsigned short	flags;
};

class CGameTrace : public CBaseTrace
{
public:
	bool                    DidHitWorld() const;
	bool                    DidHitNonWorldEntity() const;
	int                     GetEntityIndex() const;
	bool                    DidHit() const;
	bool					IsVisible() const;

public:

	float                   fractionleftsolid;
	csurface_t              surface;
	int                     hitgroup;
	short                   physicsbone;
	unsigned short          worldSurfaceIndex;
	IClientEntity* m_pEnt;
	int                     hitbox;

	CGameTrace() { }

private:
	CGameTrace(const CGameTrace& vOther);
};

inline bool CGameTrace::IsVisible() const
{
	return fraction > 0.97f;
}

inline bool CGameTrace::DidHit() const
{
	return fraction < 1.0f || allsolid || startsolid;
}

inline bool CGameTrace::DidHitWorld() const
{
	return m_pEnt && (*(int*)((uintptr_t)m_pEnt + 0x64)) == 0;
}

inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEnt != NULL && !DidHitWorld();
}

typedef CGameTrace trace_t;

class VMatrix
{
public:
	float* operator [] (const std::uint32_t index);
	const float* operator [] (const std::uint32_t index) const;

public:
	float m[4][4] = { };
};

typedef float w2smatrix4x4[4][4];