#include "sdk.hpp"
#include "math.hpp"

#include <cmath>
#include <xmmintrin.h>
#include <pmmintrin.h>

static const float invtwopi = 0.1591549f;
static const float twopi = 6.283185f;
static const float threehalfpi = 4.7123889f;
static const float pi = 3.141593f;
static const float halfpi = 1.570796f;
static const __m128 signmask = _mm_castsi128_ps(_mm_set1_epi32(0x80000000));

static const __declspec(align(16)) float null[4] = { 0.f, 0.f, 0.f, 0.f };
static const __declspec(align(16)) float _pi2[4] = { 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f, 1.5707963267948966192f };
static const __declspec(align(16)) float _pi[4] = { 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f, 3.141592653589793238f };

#define fsel(c,x,y) ( (c) >= 0 ? (x) : (y) )

void inline Math::SinCos(float radians, float* sine, float* cosine)
{
	*sine = std::sinf(radians);
	*cosine = std::cosf(radians);
}

typedef float vec_t;

inline unsigned long& FloatBits(vec_t& f)
{
	return *reinterpret_cast<unsigned long*>(&f);
}


inline bool IsFinite(vec_t f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

bool CheckIfNonValidNumber(float num)
{
	return !IsFinite(num);
}

float Math::PickRandomAngle(float numangles, ...)
{
	va_list list;
	va_start(list, numangles);
	float ret;
	float* angles = new float[numangles];
	for (int i = 0; i < numangles; i++)
	{
		angles[i] = (float)va_arg(list, double);
	}
	va_end(list);
	ret = angles[rand() % (int)numangles];

	delete[]angles;
	return ret;
}

inline Vector CrossProduct(const Vector& a, const Vector& b)
{
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

float Math::GuwopNormalize(float flAngle)
{
	/*float flNormalizedAngle = 0;
	float flRevolutions = flAngle / 360;

	if (flAngle > 180 || flAngle < -180)
	{
		if (flRevolutions < 0) flRevolutions = -flRevolutions;

		flRevolutions = round(flRevolutions);

		if (flAngle < 0) flNormalizedAngle = (flAngle + 360 * flRevolutions);
		else flNormalizedAngle = (flAngle - 360 * flRevolutions);
	}
	else flNormalizedAngle = flAngle;
	return flNormalizedAngle;*/
	return remainderf(flAngle, 360.0f);
}

void Math::VectorAngles(const Vector& forward, QAngle& angles, Vector* up)
{
	Vector  left;
	float   len, up_z, pitch, yaw, roll;

	// get 2d length.
	len = forward.Length2D();

	if (up && len > 0.001f) {
		pitch = RAD2DEG(std::atan2(-forward.z, len));
		yaw = RAD2DEG(std::atan2(forward.y, forward.x));

		// get left direction vector using cross product.
		left = (*up).Cross(forward).Normalized();

		// calculate up_z.
		up_z = (left.y * forward.x) - (left.x * forward.y);

		// calculate roll.
		roll = RAD2DEG(std::atan2(left.z, up_z));
	}

	else {
		if (len > 0.f) {
			// calculate pitch and yaw.
			pitch = RAD2DEG(std::atan2(-forward.z, len));
			yaw = RAD2DEG(std::atan2(forward.y, forward.x));
			roll = 0.f;
		}

		else {
			pitch = float((forward.z > 0) ? 270 : 90);
			yaw = 0.f;
			roll = 0.f;
		}
	}

	// set out angles.
	angles = { pitch, yaw, roll };
}

void Math::VectorAngles(const Vector& vecForward, Vector& vecAngles)
{
	Vector vecView;
	if (vecForward.y == 0.f && vecForward.x == 0.f)
	{
		vecView.x = 0.f;
		vecView.y = 0.f;
	}
	else
	{
		vecView.y = atan2(vecForward.y, vecForward.x) * 180.f / M_PI;

		if (vecView.y < 0.f)
			vecView.y += 360;

		vecView.z = sqrt(vecForward.x * vecForward.x + vecForward.y * vecForward.y);

		vecView.x = atan2(vecForward.z, vecView.z) * 180.f / M_PI;
	}

	vecAngles.x = -vecView.x;
	vecAngles.y = vecView.y;
	vecAngles.z = 0.f;
}

void Math::VectorToAngles(const Vector& forward, const Vector& up, QAngle& angles)
{
	auto left = CrossProduct(up, forward);
	left.Normalize();

	auto xyDist = forward.Length2D();

	if (xyDist > 0.001f)
	{
		angles.x = RAD2DEG(atan2f(-forward.z, xyDist));
		angles.y = RAD2DEG(atan2f(forward.y, forward.x));

		auto up_z = (left.y * forward.x) - (left.x * forward.y);

		angles.z = RAD2DEG(atan2f(left.z, up_z));
	}
	else
	{
		angles.x = RAD2DEG(atan2f(-forward.z, xyDist));
		angles.y = RAD2DEG(atan2f(-left.x, left.y));
		angles.z = 0;
	}
}

void Math::AngleVectors(const QAngle& angles, Vector* forward, Vector* right, Vector* up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);
	SinCos(DEG2RAD(angles.z), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void Math::TransformAABB(matrix3x4_t& transform, const Vector& vecMinsIn, const Vector& vecMaxsIn, Vector& vecMinsOut, Vector& vecMaxsOut)
{
	Vector localCenter;
	VectorAdd(vecMinsIn, vecMaxsIn, localCenter);
	localCenter *= 0.5f;

	Vector localExtents;
	VectorSubtract(vecMaxsIn, localCenter, localExtents);

	Vector worldCenter;
	VectorTransform(localCenter, transform, worldCenter);

	Vector worldExtents;
	worldExtents.x = localExtents.Dot(transform[0]);
	worldExtents.y = localExtents.Dot(transform[1]);
	worldExtents.z = localExtents.Dot(transform[2]);

	VectorSubtract(worldCenter, worldExtents, vecMinsOut);
	VectorAdd(worldCenter, worldExtents, vecMaxsOut);
}

void Math::VectorMin(const Vector& a, const Vector& b, Vector& result)
{
	result.x = min(a.x, b.x);
	result.y = min(a.y, b.y);
	result.z = min(a.z, b.z);
}

void Math::VectorMax(const Vector& a, const Vector& b, Vector& result)
{
	result.x = max(a.x, b.x);
	result.y = max(a.y, b.y);
	result.z = max(a.z, b.z);
}

bool Math::IntersectionBoundingBox(const Vector& src, const Vector& dir, const Vector& min, const Vector& max, Vector* hit_point) {
	/*
	Fast Ray-Box Intersection
	by Andrew Woo
	from "Graphics Gems", Academic Press, 1990
	*/

	constexpr auto NUMDIM = 3;
	constexpr auto RIGHT = 0;
	constexpr auto LEFT = 1;
	constexpr auto MIDDLE = 2;

	bool inside = true;
	char quadrant[NUMDIM];
	int i;

	// Rind candidate planes; this loop can be avoided if
	// rays cast all from the eye(assume perpsective view)
	Vector candidatePlane;
	for (i = 0; i < NUMDIM; i++) {
		if (src[i] < min[i]) {
			quadrant[i] = LEFT;
			candidatePlane[i] = min[i];
			inside = false;
		}
		else if (src[i] > max[i]) {
			quadrant[i] = RIGHT;
			candidatePlane[i] = max[i];
			inside = false;
		}
		else {
			quadrant[i] = MIDDLE;
		}
	}

	// Ray origin inside bounding box
	if (inside) {
		if (hit_point)
			* hit_point = src;
		return true;
	}

	// Calculate T distances to candidate planes
	Vector maxT;
	for (i = 0; i < NUMDIM; i++) {
		if (quadrant[i] != MIDDLE && dir[i] != 0.f)
			maxT[i] = (candidatePlane[i] - src[i]) / dir[i];
		else
			maxT[i] = -1.f;
	}

	// Get largest of the maxT's for final choice of intersection
	int whichPlane = 0;
	for (i = 1; i < NUMDIM; i++) {
		if (maxT[whichPlane] < maxT[i])
			whichPlane = i;
	}

	// Check final candidate actually inside box
	if (maxT[whichPlane] < 0.f)
		return false;

	for (i = 0; i < NUMDIM; i++) {
		if (whichPlane != i) {
			float temp = src[i] + maxT[whichPlane] * dir[i];
			if (temp < min[i] || temp > max[i]) {
				return false;
			}
			else if (hit_point) {
				(*hit_point)[i] = temp;
			}
		}
		else if (hit_point) {
			(*hit_point)[i] = candidatePlane[i];
		}
	}

	// ray hits box
	return true;
}

bool Math::IntersectSegmentSphere(const Vector& vecRayOrigin, const Vector& vecRayDelta, const Vector& vecSphereCenter, float flRadius) {
	// Solve using the ray equation + the sphere equation
	// P = o + dt
	// (x - xc)^2 + (y - yc)^2 + (z - zc)^2 = r^2
	// (ox + dx * t - xc)^2 + (oy + dy * t - yc)^2 + (oz + dz * t - zc)^2 = r^2
	// (ox - xc)^2 + 2 * (ox-xc) * dx * t + dx^2 * t^2 +
	//		(oy - yc)^2 + 2 * (oy-yc) * dy * t + dy^2 * t^2 +
	//		(oz - zc)^2 + 2 * (oz-zc) * dz * t + dz^2 * t^2 = r^2
	// (dx^2 + dy^2 + dz^2) * t^2 + 2 * ((ox-xc)dx + (oy-yc)dy + (oz-zc)dz) t +
	//		(ox-xc)^2 + (oy-yc)^2 + (oz-zc)^2 - r^2 = 0
	// or, t = (-b +/- sqrt( b^2 - 4ac)) / 2a
	// a = DotProduct( vecRayDelta, vecRayDelta );
	// b = 2 * DotProduct( vecRayOrigin - vecCenter, vecRayDelta )
	// c = DotProduct(vecRayOrigin - vecCenter, vecRayOrigin - vecCenter) - flRadius * flRadius;

	Vector vecSphereToRay = vecRayOrigin - vecSphereCenter;

	float a = vecRayDelta.Dot(vecRayDelta);

	// This would occur in the case of a zero-length ray
	if (a == 0.0f)
		return vecSphereToRay.LengthSquared() <= flRadius * flRadius;

	float b = 2.f * vecSphereToRay.Dot(vecRayDelta);
	float c = vecSphereToRay.Dot(vecSphereToRay) - flRadius * flRadius;
	float flDiscrim = b * b - 4.f * a * c;
	return flDiscrim >= 0.0f;
}

bool Math::IntersectSegmentCapsule(const Vector& start, const Vector& end, const Vector& min, const Vector& max, float radius) {
	Vector d = max - min, m = start - min, n = end - start;
	float md = m.Dot(d);
	float nd = n.Dot(d);
	float dd = d.Dot(d);

	if (md < 0.0f && md + nd < 0.0f) {
		return IntersectSegmentSphere(start, n, min, radius);
	}
	if (md > dd && md + nd > dd) {
		return IntersectSegmentSphere(start, n, max, radius);
	}

	float t = 0.0f;
	float nn = n.Dot(n);
	float mn = m.Dot(n);
	float a = dd * nn - nd * nd;
	float k = m.Dot(m) - radius * radius;
	float c = dd * k - md * md;
	if (std::fabsf(a) < FLT_EPSILON) {
		if (c > 0.0f)
			return 0;
		if (md < 0.0f)
			IntersectSegmentSphere(start, n, min, radius);
		else if (md > dd)
			IntersectSegmentSphere(start, n, max, radius);
		else
			t = 0.0f;
		return true;
	}
	float b = dd * mn - nd * md;
	float discr = b * b - a * c;
	if (discr < 0.0f)
		return false;

	t = (-b - sqrt(discr)) / a;
	float t0 = t;
	if (md + t * nd < 0.0f) {
		return IntersectSegmentSphere(start, n, min, radius);
	}
	else if (md + t * nd > dd) {

		return IntersectSegmentSphere(start, n, max, radius);
	}
	t = t0;
	return t >= 0.0f && t <= 1.0f;
}

void Math::VectorMAInline(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	dest.x = start.x + direction.x * scale;
	dest.y = start.y + direction.y * scale;
	dest.z = start.z + direction.z * scale;
}

inline void CrossProduct(const Vector& a, const Vector& b, Vector& result)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	Assert(&a != &result);
	Assert(&b != &result);
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
}

void Math::VectorVectors(const Vector& forward, Vector& right, Vector& up)
{
	Vector tmp;

	if (fabs(forward[0]) < 1e-6 && fabs(forward[1]) < 1e-6)
	{
		// pitch 90 degrees up/down from identity
		right[0] = 0;
		right[1] = -1;
		right[2] = 0;
		up[0] = -forward[2];
		up[1] = 0;
		up[2] = 0;
	}
	else
	{
		tmp[0] = 0; tmp[1] = 0; tmp[2] = 1.0;
		CrossProduct(forward, tmp, right);
		right.Normalize();
		CrossProduct(right, forward, up);
		up.Normalize();
	}
}

void Math::VectorMatrix(const Vector& forward, matrix3x4_t& matrix)
{
	Vector right, up;
	VectorVectors(forward, right, up);

	MatrixSetColumn(forward, 0, matrix);
	MatrixSetColumn(right * -1.f, 1, matrix);
	MatrixSetColumn(up, 2, matrix);
}

void Math::VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	VectorMAInline(start, scale, direction, dest);
}

void Math::AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
{
	float sr, sp, sy, cr, cp, cy;
	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);
	SinCos(DEG2RAD(angles.z), &sr, &cr);

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
	if (right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}
	if (up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void Math::VectorAngles(const Vector& forward, Vector& up, QAngle& angles)
{
	Vector left = CrossProduct(up, forward);
	left.Normalize();

	float forwardDist = forward.Length2D();

	if (forwardDist > 0.001f)
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI_F;
		angles.y = atan2f(forward.y, forward.x) * 180 / M_PI_F;

		float upZ = (left.y * forward.x) - (left.x * forward.y);
		angles.z = atan2f(left.z, upZ) * 180 / M_PI_F;
	}
	else
	{
		angles.x = atan2f(-forward.z, forwardDist) * 180 / M_PI_F;
		angles.y = atan2f(-left.x, left.y) * 180 / M_PI_F;
		angles.z = 0;
	}
}

float Math::GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	Vector ang, aim;

	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSquared()));
}

float Math::GetFov(const Vector& viewAngle, const Vector& aimAngle)
{
	return RAD2DEG(acos(viewAngle.Dot(aimAngle) / viewAngle.LengthSquared()));
}

//inline float fast_asin(float x)
//{
//	const auto negate = float(x < 0);
//	x = abs(x);
//	auto ret = -0.0187293;
//	ret *= x;
//	ret += 0.0742610;
//	ret *= x;
//	ret -= 0.2121144;
//	ret *= x;
//	ret += 1.5707288;
//	ret = 3.14159265358979 * 0.5 - sqrt(1.0 - x) * ret;
//	return float(ret - 2 * negate * ret);
//}
//
//inline float fast_atan2(const float y, const float x)
//{
//	const auto xabs = fabs(x);
//	const auto yabs = fabs(y);
//	double t3 = xabs;
//	double t1 = yabs;
//	auto t0 = max(t3, t1);
//	t1 = min(t3, t1);
//	t3 = 1 / t0;
//	t3 = t1 * t3;
//
//	const auto t4 = t3 * t3;
//	t0 = -0.013480470;
//	t0 = t0 * t4 + 0.057477314;
//	t0 = t0 * t4 - 0.121239071;
//	t0 = t0 * t4 + 0.195635925;
//	t0 = t0 * t4 - 0.332994597;
//	t0 = t0 * t4 + 0.999995630;
//	t3 = t0 * t3;
//
//	t3 = (yabs > xabs) ? 1.570796327 - t3 : t3;
//	t3 = (x < 0) ? 3.141592654 - t3 : t3;
//	t3 = (y < 0) ? -t3 : t3;
//
//	return float(t3);
//}
//
//inline float fast_atan(const float x)
//{
//	return fast_atan2(x, float(1));
//}

QAngle Math::CalcAngle(Vector src, Vector dst)
{
	Vector forward = dst - src;
	/*
	float len = delta.Length();

	if (delta.z == 0.0f && len == 0.0f)
		return QAngle(0, 0, 0);

	if (delta.y == 0.0f && delta.x == 0.0f)
		return QAngle(0, 0, 0);

	QAngle angles;
	angles.x = (asinf(delta.z / delta.Length()) * M_RADPI);
	angles.y = (atanf(delta.y / delta.x) * M_RADPI);
	angles.z = 0.0f;
	if (delta.x >= 0.0f) { angles.y += 180.0f; }

	angles.Clamp();

	return angles;*/
	/*static const auto ang_zero = QAngle(0.0f, 0.0f, 0.0f);

	const auto delta = src - dst;
	if (delta.Length() <= 0.0f)
		return ang_zero;

	if (delta.z == 0.0f && delta.Length() == 0.0f)
		return ang_zero;

	if (delta.y == 0.0f && delta.x == 0.0f)
		return ang_zero;

	angles.x = (asinf(delta.z / delta.Length()) * M_RADPI);
	angles.y = (atanf(delta.y / delta.x) * M_RADPI);
	angles.z = 0.0f;
	if (delta.x >= 0.0f) { angles.y += 180.0f; }
	return angles;*/

	float   len, pitch, yaw, roll;

	// get 2d length.
	len = forward.Length2D();

	if (len > 0.f) {
		// calculate pitch and yaw.
		pitch = RAD2DEG(std::atan2(-forward.z, len));
		yaw = RAD2DEG(std::atan2(forward.y, forward.x));
		roll = 0.f;
	}

	else {
		pitch = (forward.z > 0.f) ? -90.f : 90.f;
		yaw = 0.f;
		roll = 0.f;
	}

	// set out angles.
	return { pitch, yaw, roll };
}

typedef __declspec(align(16)) union
{
	float f[4];
	__m128 v;
} m128;

__forceinline __m128 sqrt_ps(const __m128 squared)
{
	return _mm_sqrt_ps(squared);
}

//#define mSMALL_NUM   0.00000001 // anything that avoids division overflow
//// dot product (3D) which allows vector operations in arguments
//#define mdot(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
//#define mnorm(v)    sqrt(mdot(v,v))  // norm = length of  vector
//#define md(u,v)     nmorm(u-v)        // distance = norm of difference
//#define mabs(x)     ((x) >= 0 ? (x) : -(x))   //  absolute value

float segment_to_segment(Vector s1, Vector s2, Vector k1, Vector k2)
{
	static auto constexpr epsilon = 0.00000011920929f;

	const auto u = s2 - s1;
	const auto v = k2 - k1;
	const auto w = s1 - k1;

	const auto a = u.Dot(u);
	const auto b = u.Dot(v);
	const auto c = v.Dot(v);
	const auto d = u.Dot(w);
	const auto e = v.Dot(w);
	const auto D = a * c - b * b;
	float sn, sd = D;
	float tn, td = D;

	if (D < epsilon) {
		sn = 0.0f;
		sd = 1.0f;
		tn = e;
		td = c;
	}
	else {
		sn = b * e - c * d;
		tn = a * e - b * d;

		if (sn < 0.0f) {
			sn = 0.0f;
			tn = e;
			td = c;
		}
		else if (sn > sd) {
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	if (tn < 0.0f) {
		tn = 0.0f;

		if (-d < 0.0f)
			sn = 0.0f;
		else if (-d > a)
			sn = sd;
		else {
			sn = -d;
			sd = a;
		}
	}
	else if (tn > td) {
		tn = td;

		if (-d + b < 0.0f)
			sn = 0.f;
		else if (-d + b > a)
			sn = sd;
		else {
			sn = -d + b;
			sd = a;
		}
	}

	const float sc = abs(sn) < epsilon ? 0.0f : sn / sd;
	const float tc = abs(tn) < epsilon ? 0.0f : tn / td;

	const auto dp = w + u * sc - v * tc;
	return dp.Length();
}

inline float anglemod(float a)
{
	a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	return a;
}

float Math::ApproachAngle(float target, float value, float speed)
{
	target = anglemod(target);
	value = anglemod(value);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

bool Math::IntersectBB(Vector& start, Vector& delta, Vector& min, Vector& max) {
	float d1, d2, f;
	auto start_solid = true;
	auto t1 = -1.0, t2 = 1.0;

	const float _start[3] = { start.x, start.y, start.z };
	const float _delta[3] = { delta.x, delta.y, delta.z };
	const float mins[3] = { min.x, min.y, min.z };
	const float maxs[3] = { max.x, max.y, max.z };

	for (auto i = 0; i < 6; ++i) {
		if (i >= 3) {
			const auto j = (i - 3);

			d1 = _start[j] - maxs[j];
			d2 = d1 + _delta[j];
		}
		else {
			d1 = -_start[i] + mins[i];
			d2 = d1 - _delta[i];
		}

		if (d1 > 0 && d2 > 0) {
			start_solid = false;
			return false;
		}

		if (d1 <= 0 && d2 <= 0)
			continue;

		if (d1 > 0)
			start_solid = false;

		if (d1 > d2) {
			f = d1;
			if (f < 0)
				f = 0;

			f /= d1 - d2;
			if (f > t1)
				t1 = f;
		}
		else {
			f = d1 / (d1 - d2);
			if (f < t2)
				t2 = f;
		}
	}

	return start_solid || (t1 < t2 && t1 >= 0.0f);
}

bool Math::Intersect(Vector start, Vector end, Vector a, Vector b, float radius)
{
	const auto dist = segment_to_segment(start, end, a, b);
	return (dist < radius);
}

//int Math::ClipRayToHitbox(const Ray_t& ray, mstudiobbox_t* pbox, matrix3x4_t& matrix, CGameTrace& tr)
//{
//	return ClipRayToHitbox(ray,pbox, matrix, tr);
//}

//int __stdcall ClipRayToCapsuleWrapper(Ray_t* start, mstudiobbox_t* hitbox, matrix3x4_t* bones, trace_t* trace)
//{
//	__asm
//	{
//		push    ebp
//		mov     ebp, esp
//		push    dword ptr[ebp + 14h]
//		push    dword ptr[ebp + 10h]
//		mov     edx, [ebp + 0Ch]
//		mov     ecx, [ebp + 8]
//		call    ds : clp_fn
//		add     esp, 8
//		mov     esp, ebp
//		pop     ebp
//		retn    10h
//	}
//}

int Math::ClipRayToHitbox(const Ray_t& ray, mstudiobbox_t* pbox, matrix3x4_t& matrix, trace_t& tr)
{
	static auto clp_fn = Memory::Scan("client.dll", "55 8B EC 83 E4 F8 F3 0F 10 42");
	return reinterpret_cast<int(__fastcall*)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, trace_t&)>(clp_fn)(ray, pbox, matrix, tr);
}

void Math::CalcAngle(const Vector& vecSource, const Vector& vecDestination, QAngle& qAngles)
{
	QAngle delta = QAngle((vecSource.x - vecDestination.x), (vecSource.y - vecDestination.y), (vecSource.z - vecDestination.z));
	double hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);
	qAngles.x = (float)(atan(delta.z / hyp) * (180.0 / M_PI));
	qAngles.y = (float)(atan(delta.y / delta.x) * (180.0 / M_PI));
	qAngles.z = 0.f;
	if (delta.x >= 0.f)
		qAngles.y += 180.f;
}

float Math::VectorDistance(Vector v1, Vector v2)
{
	return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

void Math::VectorTransform(Vector& in1, matrix3x4a_t& in2, Vector& out)
{
	out.x = in1.Dot(in2.m_flMatVal[0]) + in2.m_flMatVal[0][3];
	out.y = in1.Dot(in2.m_flMatVal[1]) + in2.m_flMatVal[1][3];
	out.z = in1.Dot(in2.m_flMatVal[2]) + in2.m_flMatVal[2][3];
}

void Math::VectorITransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	out.x = (in1.x - in2[0][3]) * in2[0][0] + (in1.y - in2[1][3]) * in2[1][0] + (in1.z - in2[2][3]) * in2[2][0];
	out.y = (in1.x - in2[0][3]) * in2[0][1] + (in1.y - in2[1][3]) * in2[1][1] + (in1.z - in2[2][3]) * in2[2][1];
	out.z = (in1.x - in2[0][3]) * in2[0][2] + (in1.y - in2[1][3]) * in2[1][2] + (in1.z - in2[2][3]) * in2[2][2];
}

Vector Math::vector_rotate(const Vector& in1, const matrix3x4_t& in2)
{
	return Vector(in1.Dot(in2[0]), in1.Dot(in2[1]), in1.Dot(in2[2]));
}

Vertex_t Math::RotateVertex(const Vector& p, const Vertex_t& v, float angle) {
	// convert theta angle to sine and cosine representations.
	float c = std::cos(DEG2RAD(angle));
	float s = std::sin(DEG2RAD(angle));

	return Vertex_t(Vector2D(p.x + (v.m_Position.x - p.x) * c - (v.m_Position.y - p.y) * s,p.y + (v.m_Position.x - p.x) * s + (v.m_Position.y - p.y) * c));
}

__forceinline __m128 cos_52s_ps(const __m128 x)
{
	const auto c1 = _mm_set1_ps(0.9999932946f);
	const auto c2 = _mm_set1_ps(-0.4999124376f);
	const auto c3 = _mm_set1_ps(0.0414877472f);
	const auto c4 = _mm_set1_ps(-0.0012712095f);
	const auto x2 = _mm_mul_ps(x, x);
	return _mm_add_ps(c1, _mm_mul_ps(x2, _mm_add_ps(c2, _mm_mul_ps(x2, _mm_add_ps(c3, _mm_mul_ps(c4, x2))))));
}

__forceinline void sincos_ps(__m128 angle, __m128* sin, __m128* cos) {
	const auto anglesign = _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(signmask, angle));
	angle = _mm_andnot_ps(signmask, angle);
	angle = _mm_sub_ps(angle, _mm_mul_ps(_mm_cvtepi32_ps(_mm_cvttps_epi32(_mm_mul_ps(angle, _mm_set1_ps(invtwopi)))), _mm_set1_ps(twopi)));

	auto cosangle = angle;
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(pi), angle))));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(pi)), signmask));
	cosangle = _mm_xor_ps(cosangle, _mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(threehalfpi)), _mm_xor_ps(cosangle, _mm_sub_ps(_mm_set1_ps(twopi), angle))));

	auto result = cos_52s_ps(cosangle);
	result = _mm_xor_ps(result, _mm_and_ps(_mm_and_ps(_mm_cmpge_ps(angle, _mm_set1_ps(halfpi)), _mm_cmplt_ps(angle, _mm_set1_ps(threehalfpi))), signmask));
	*cos = result;

	const auto sinmultiplier = _mm_mul_ps(anglesign, _mm_or_ps(_mm_set1_ps(1.f), _mm_and_ps(_mm_cmpgt_ps(angle, _mm_set1_ps(pi)), signmask)));
	*sin = _mm_mul_ps(sinmultiplier, sqrt_ps(_mm_sub_ps(_mm_set1_ps(1.f), _mm_mul_ps(result, result))));
}

void Math::AngleMatrix(const QAngle& angles, matrix3x4_t& matrix)
{
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("AngleMatrix", "Mathlib");
#endif
	Assert(s_bMathlibInitialized);

	float sr, sp, sy, cr, cp, cy;

#ifdef _X360
	fltx4 radians, scale, sine, cosine;
	radians = LoadUnaligned3SIMD(angles.Base());
	scale = ReplicateX4(M_PI_F / 180.f);
	radians = MulSIMD(radians, scale);
	SinCos3SIMD(sine, cosine, radians);

	sp = SubFloat(sine, 0);	sy = SubFloat(sine, 1);	sr = SubFloat(sine, 2);
	cp = SubFloat(cosine, 0);	cy = SubFloat(cosine, 1);	cr = SubFloat(cosine, 2);
#else
	SinCos(DEG2RAD(angles.y), &sy, &cy);
	SinCos(DEG2RAD(angles.x), &sp, &cp);
	SinCos(DEG2RAD(angles.z), &sr, &cr);
#endif

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	// NOTE: Do not optimize this to reduce multiplies! optimizer bug will screw this up.
	matrix[0][1] = sr * sp * cy + cr * -sy;
	matrix[1][1] = sr * sp * sy + cr * cy;
	matrix[2][1] = sr * cp;
	matrix[0][2] = (cr * sp * cy + -sr * -sy);
	matrix[1][2] = (cr * sp * sy + -sr * cy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}

matrix3x4_t Math::AngleMatrix(const QAngle angles)
{
	matrix3x4_t result{};

	m128 angle, sin, cos;
	angle.f[0] = DEG2RAD(angles.x);
	angle.f[1] = DEG2RAD(angles.y);
	angle.f[2] = DEG2RAD(angles.z);
	sincos_ps(angle.v, &sin.v, &cos.v);

	result[0][0] = cos.f[0] * cos.f[1];
	result[1][0] = cos.f[0] * sin.f[1];
	result[2][0] = -sin.f[0];

	const auto crcy = cos.f[2] * cos.f[1];
	const auto crsy = cos.f[2] * sin.f[1];
	const auto srcy = sin.f[2] * cos.f[1];
	const auto srsy = sin.f[2] * sin.f[1];

	result[0][1] = sin.f[0] * srcy - crsy;
	result[1][1] = sin.f[0] * srsy + crcy;
	result[2][1] = sin.f[2] * cos.f[0];

	result[0][2] = sin.f[0] * crcy + srsy;
	result[1][2] = sin.f[0] * crsy - srcy;
	result[2][2] = cos.f[2] * cos.f[0];

	return result;
}

Vector Math::VectorRotate(const Vector& in1, const QAngle& in2)
{
	const auto matrix = AngleMatrix(in2);
	return vector_rotate(in1, matrix);
}

void Math::VectorIRotate(const Vector& in1, const matrix3x4_t& in2, Vector& out)
{
	out.x = in1.x * in2[0][0] + in1.y * in2[1][0] + in1.z * in2[2][0];
	out.y = in1.x * in2[0][1] + in1.y * in2[1][1] + in1.z * in2[2][1];
	out.z = in1.x * in2[0][2] + in1.y * in2[1][2] + in1.z * in2[2][2];
}

void Math::VectorTransform(Vector& in1, matrix3x4_t& in2, Vector& out)
{
	/*out.x = in1.Dot(in2.m_flMatVal[0]) + in2.m_flMatVal[0][3];
	out.y = in1.Dot(in2.m_flMatVal[1]) + in2.m_flMatVal[1][3];
	out.z = in1.Dot(in2.m_flMatVal[2]) + in2.m_flMatVal[2][3];*/
	out = Vector(in1.Dot(Vector(in2[0][0], in2[0][1], in2[0][2])) + in2[0][3],
	   in1.Dot(Vector(in2[1][0], in2[1][1], in2[1][2])) + in2[1][3],
	   in1.Dot(Vector(in2[2][0], in2[2][1], in2[2][2])) + in2[2][3]);
}

void Math::MatrixPosition(const matrix3x4_t& matrix, Vector& position)
{
	position.x = matrix[0][3];
	position.y = matrix[1][3];
	position.z = matrix[2][3];
}

void Math::VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

void Math::VectorAdd(const Vector& a, const Vector& b, Vector& c)
{
	CHECK_VALID(a);
	CHECK_VALID(b);
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

void Math::QAngleTransform(Vector& in1, matrix3x4a_t& in2, QAngle& out)
{
	out.x = in1.Dot(in2.m_flMatVal[0]) + in2.m_flMatVal[0][3];
	out.y = in1.Dot(in2.m_flMatVal[1]) + in2.m_flMatVal[1][3];
	out.z = in1.Dot(in2.m_flMatVal[2]) + in2.m_flMatVal[2][3];
}

float Math::GetDelta(float hspeed, float maxspeed, float airaccelerate)
{
	auto term = (30.0 - (airaccelerate * maxspeed / 66.0)) / hspeed;

	if (term < 1.0f && term > -1.0f) {
		return acos(term);
	}

	return 0.f;
}

float Math::NormalizeFloat(float value)
{
	/*while (value > 180)
		value -= 360.f;

	while (value < -180)
		value += 360.f;
	return value;*/
	return remainderf(value, 360.0f);
}

float Math::NormalizeFloatInPlace(float f)
{
	if (std::isnan(f) || std::isinf(f))
		f = 0;

	if (f > 9999999)
		f = 0;

	if (f < -9999999)
		f = 0;

	return remainderf(f, 360.0f);
}

__forceinline void Math::matrix_set_origin(Vector pos, matrix3x4_t& matrix)
{
	matrix.m_flMatVal[0][3] = pos.x;
	matrix.m_flMatVal[1][3] = pos.y;
	matrix.m_flMatVal[2][3] = pos.z;
}

__forceinline Vector Math::matrix_get_origin(matrix3x4_t src)
{
	return Vector{ src.m_flMatVal[0][3], src.m_flMatVal[1][3], src.m_flMatVal[2][3] };
}

__forceinline float Math::RemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return fsel(val - B, D, C);
	float cVal = (val - A) / (B - A);
	cVal = clamp<float>(cVal, 0.0f, 1.0f);

	return C + (D - C) * cVal;
}

inline float Math::RandFloat(float M, float N)
{
	return (float)(M + (rand() / (RAND_MAX / (N - M))));
}

void Math::MatrixGetColumn(const matrix3x4_t& in, int column, Vector& out)
{
	out.x = in[0][column];
	out.y = in[1][column];
	out.z = in[2][column];
}

void Math::MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out)
{
	out[0][column] = in.x;
	out[1][column] = in.y;
	out[2][column] = in.z;
}

void Math::AngleMatrix(const QAngle& angles, const Vector& position, matrix3x4_t& matrix)
{
	AngleMatrix(angles, matrix);
	MatrixSetColumn(position, 3, matrix);
}

void Math::MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out)
{
	//Assert(s_bMathlibInitialized);
	memcpy(out.Base(), in.Base(), sizeof(float) * 3 * 4);
}

float Math::AngleDiff(float destAngle, float srcAngle)
{
	float delta = fmodf(destAngle - srcAngle, 360.0f);

	if (destAngle > srcAngle)
	{
		if (delta >= 180.f)
			delta -= 360.f;
	}
	else
	{
		if (delta <= -180.f)
			delta += 360.f;
	}

	return delta;
}

float Math::angle_diff(float src_angle, float dest_angle)
{
	float delta = 0;

	for (; src_angle > 180.0; src_angle = src_angle - 360.0)
		;
	for (; src_angle < -180.0; src_angle = src_angle + 360.0)
		;
	for (; dest_angle > 180.0; dest_angle = dest_angle - 360.0)
		;
	for (; dest_angle < -180.0; dest_angle = dest_angle + 360.0)
		;
	for (delta = src_angle - dest_angle; delta > 180.0; delta = delta - 360.0)
		;
	for (; delta < -180.0; delta = delta + 360.0)
		;
	return delta;
}

float Math::DirDiff(const Vector &firstDirection, const Vector& secondDirection)
{
	const auto a = Math::normalize_angle(RAD2DEG(atan2f(firstDirection.y, firstDirection.x)));
	const auto b = Math::normalize_angle(RAD2DEG(atan2f(secondDirection.y, secondDirection.x)));

	float c = fmodf(a - b, 360.0f);

	if (a > b)
	{
		if (c >= 180.f)
			c -= 360.f;
	}
	else
	{
		if (c <= -180.f)
			c += 360.f;
	}

	return c;
}

void Math::ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out)
{
	//Assert(s_bMathlibInitialized);
	if (&in1 == &out)
	{
		matrix3x4_t in1b;
		MatrixCopy(in1, in1b);
		ConcatTransforms(in1b, in2, out);
		return;
	}
	if (&in2 == &out)
	{
		matrix3x4_t in2b;
		MatrixCopy(in2, in2b);
		ConcatTransforms(in1, in2b, out);
		return;
	}

	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
		in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
		in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
		in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
		in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
		in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
		in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
		in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
		in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
		in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
		in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
		in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
		in1[2][2] * in2[2][3] + in1[2][3];
}

//-----------------------------------------------------------------------------
// Purpose: converts engine euler angles into a matrix
// Input  : vec3_t angles - PITCH, YAW, ROLL
// Output : *matrix - left-handed column matrix
//			the basis vectors for the rotations will be in the columns as follows:
//			matrix[].x is forward
//			matrix[].y is left
//			matrix[].z is up
//-----------------------------------------------------------------------------
void Math::AngleMatrix(RadianEuler const& angles, const Vector& position, matrix3x4_t& matrix)
{
	AngleMatrix(angles, matrix);
	MatrixSetColumn(position, 3, matrix);
}

void Math::AngleMatrix(const RadianEuler& angles, matrix3x4_t& matrix)
{
	QAngle quakeEuler(RAD2DEG(angles.y), RAD2DEG(angles.z), RAD2DEG(angles.x));

	AngleMatrix(quakeEuler, matrix);
}

void Math::MatrixAngles(const matrix3x4_t& matrix, QAngle& angles)
{
	MatrixAngles(matrix, &angles.x);
}

void Math::MatrixAngles(const matrix3x4_t& matrix, float* angles)
{
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("MatrixAngles", "Mathlib");
#endif
	//Assert(s_bMathlibInitialized);
	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	// enough here to get angles?
	if (xyDist > 0.001f)
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// (roll)	z = ATAN( left.z, up.z );
		angles[2] = RAD2DEG(atan2f(left[2], up[2]));
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		angles[1] = RAD2DEG(atan2f(-left[0], left[1]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles[2] = 0;
	}
}

void Math::MatrixAngles(const matrix3x4_t& matrix, Vector* angles, Vector* forward)
{
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("MatrixAngles", "Mathlib");
#endif
	//Assert(s_bMathlibInitialized);
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward->x = matrix[0][0];
	forward->y = matrix[1][0];
	forward->z = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward->x * forward->x + forward->y * forward->y);

	// enough here to get angles?
	if (xyDist > 0.001f)
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		angles->y = RAD2DEG(atan2f(forward->y, forward->x));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles->x = RAD2DEG(atan2f(-forward->z, xyDist));

		// (roll)	z = ATAN( left.z, up.z );
		angles->z = RAD2DEG(atan2f(left[2], up[2]));
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		angles->y = RAD2DEG(atan2f(-left[0], left[1]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles->x = RAD2DEG(atan2f(-forward->z, xyDist));

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles->z = 0;
	}
}