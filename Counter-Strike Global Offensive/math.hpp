#pragma once
#include "sdk.hpp"

class matrix3x4a_t;
class RadianEuler;
class C_BasePlayer;
struct mstudiobbox_t;
struct Ray_t;
class Vertex_t;
class CGameTrace;

namespace Math
{
	extern inline void SinCos(float radians, float* sine, float* cosine);
	extern float PickRandomAngle(float numangles, ...);
	extern float GuwopNormalize(float flAngle);
	extern void VectorAngles(const Vector& forward, QAngle& angles, Vector* up = nullptr);
	extern void VectorAngles(const Vector& vecForward, Vector& vecAngles);
	extern void VectorToAngles(const Vector& forward, const Vector& up, QAngle& angles);
	extern void AngleVectors(const QAngle& angles, Vector* forward, Vector* right = nullptr, Vector* up = nullptr);
	extern void TransformAABB(matrix3x4_t& transform, const Vector& vecMinsIn, const Vector& vecMaxsIn, Vector& vecMinsOut, Vector& vecMaxsOut);
	extern void VectorMin(const Vector& a, const Vector& b, Vector& result);
	extern void VectorMax(const Vector& a, const Vector& b, Vector& result);
	extern bool IntersectionBoundingBox(const Vector& src, const Vector& dir, const Vector& min, const Vector& max, Vector* hit_point = nullptr);
	extern bool IntersectSegmentSphere(const Vector& vecRayOrigin, const Vector& vecRayDelta, const Vector& vecSphereCenter, float flRadius);
	extern bool IntersectSegmentCapsule(const Vector& start, const Vector& end, const Vector& min, const Vector& max, float radius);
	extern void VectorMAInline(const Vector& start, float scale, const Vector& direction, Vector& dest);
	void VectorVectors(const Vector& forward, Vector& right, Vector& up);
	extern void VectorMatrix(const Vector& forward, matrix3x4_t& matrix);
	extern void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest);
	extern void AngleVectors(const Vector& angles, Vector* forward, Vector* right = nullptr, Vector* up = nullptr);
	extern void VectorAngles(const Vector& forward, Vector& up, QAngle& angles);
	extern QAngle CalcAngle(Vector v1, Vector v2);
	extern float ApproachAngle(float flTarget, float flValue, float flSpeed);
	extern bool IntersectBB(Vector& start, Vector& end, Vector& min, Vector& max);
	extern bool Intersect(Vector start, Vector end, Vector a, Vector b, float radius);
	extern int ClipRayToHitbox(const Ray_t& ray, mstudiobbox_t* pbox, matrix3x4_t& matrix, CGameTrace& tr);
	extern void CalcAngle(const Vector& vecSource, const Vector& vecDestination, QAngle& qAngles);
	extern float GetFov(const QAngle& viewAngle, const QAngle& aimAngle);
	extern float GetFov(const Vector& viewAngle, const Vector& aimAngle);
	extern float VectorDistance(Vector v1, Vector v2);
	extern void VectorTransform(Vector& in1, matrix3x4a_t& in2, Vector& out);
	extern void VectorITransform(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	extern Vector vector_rotate(const Vector& in1, const matrix3x4_t& in2);
	extern Vertex_t RotateVertex(const Vector& p, const Vertex_t& v, float angle);
	extern matrix3x4_t AngleMatrix(const QAngle angles);
	extern Vector VectorRotate(const Vector& in1, const QAngle& in2);
	extern void VectorIRotate(const Vector& in1, const matrix3x4_t& in2, Vector& out);
	extern void VectorTransform(Vector& in1, matrix3x4_t& in2, Vector& out);
	extern void MatrixPosition(const matrix3x4_t& matrix, Vector& position);
	extern void VectorSubtract(const Vector& a, const Vector& b, Vector& c);
	extern void VectorAdd(const Vector& a, const Vector& b, Vector& c);
	extern void QAngleTransform(Vector& in1, matrix3x4a_t& in2, QAngle& out);
	extern float GetDelta(float hspeed, float maxspeed, float airaccelerate);
	extern float NormalizeFloat(float value);
	extern float NormalizeFloatInPlace(float f);
	extern void matrix_set_origin(Vector pos, matrix3x4_t& matrix);
	extern Vector matrix_get_origin(matrix3x4_t src);
	extern float RemapValClamped(float val, float A, float B, float C, float D);
	extern void MatrixGetColumn(const matrix3x4_t& in, int column, Vector& out);
	extern void MatrixSetColumn(const Vector& in, int column, matrix3x4_t& out);
	extern void AngleMatrix(RadianEuler const& angles, const Vector& position, matrix3x4_t& matrix);
	extern void AngleMatrix(const RadianEuler& angles, matrix3x4_t& matrix);
	extern void AngleMatrix(const QAngle& angles, const Vector& position, matrix3x4_t& matrix);
	extern void MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out);
	extern float AngleDiff(float destAngle, float srcAngle);
	extern float angle_diff(float src_angle, float dest_angle);
	extern float DirDiff(const Vector& firstDirection, const Vector& secondDirection);
	extern void ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);
	extern void AngleMatrix(const QAngle& angles, matrix3x4_t& matrix);
	extern void MatrixAngles(const matrix3x4_t& matrix, QAngle& angles);
	extern void MatrixAngles(const matrix3x4_t& matrix, float* angles);
	extern void MatrixAngles(const matrix3x4_t& matrix, Vector* angles, Vector* forward);
	extern inline float RandFloat(float M, float N);

	inline void MatrixAngles(const matrix3x4_t& matrix, QAngle& angles, Vector& position)
	{
		MatrixAngles(matrix, angles);
		MatrixPosition(matrix, position);
	}

	// sperg cried about the previous method, 
	//here's not only a faster one but inaccurate as well to trigger more people
	inline float FASTSQRT(float x)
	{
		unsigned int i = *(unsigned int*)& x;

		i += 127 << 23;
		// approximation of square root
		i >>= 1;
		return *(float*)& i;
	}

	inline float Approach(float target, float value, float speed)
	{
		float delta = target - value;

#if defined(_X360) || defined( _PS3 ) // use conditional move for speed on 360

		return fsel(delta - speed,	// delta >= speed ?
			value + speed,	// if delta == speed, then value + speed == value + delta == target  
			fsel((-speed) - delta, // delta <= -speed
				value - speed,
				target)
		);  // delta < speed && delta > -speed

#else

		if (delta > speed)
			value += speed;
		else if (delta < -speed)
			value -= speed;
		else
			value = target;

		return value;

#endif
	}

	inline Vector Approach(Vector target, Vector value, float speed)
	{
		Vector diff = (target - value);
		float delta = diff.Length();

		if (delta > speed)
			value += diff.Normalized() * speed;
		else if (delta < -speed)
			value -= diff.Normalized() * speed;
		else
			value = target;

		return value;
	}

	inline float Bias(float x, float biasAmt)
	{
		// WARNING: not thread safe
		static float lastAmt = -1;
		static float lastExponent = 0;
		if (lastAmt != biasAmt)
		{
			lastExponent = log(biasAmt) * -1.4427f; // (-1.4427 = 1 / log(0.5))
		}
		return pow(x, lastExponent);
	}

	inline float NormalizePitch(float pitch)
	{
		while (pitch > 89.f)
			pitch -= 180.f;
		while (pitch < -89.f)
			pitch += 180.f;

		return pitch;
	}

	__forceinline float normalize_angle(float angle)
	{
		//return remainderf(angle, 360.0f);
		float rtn = angle;

		for (; rtn > 180.0f; rtn = rtn - 360.0f)
			;
		for (; rtn < -180.0f; rtn = rtn + 360.0f)
			;

		return rtn;
	}

	__forceinline static float interpolate(const float from, const float to, const float percent)
	{
		if (from == to)
			return from;

		return to * percent + from * (1.f - percent);
	}

	__forceinline static Vector interpolate(Vector from, Vector to, const float percent)
	{
		if (from == to)
			return from;

		return to * percent + from * (1.f - percent);
	}

	__forceinline static QAngle interpolate(QAngle from, QAngle to, const float percent)
	{
		if (from == to)
			return from;

		return to * percent + from * (1.f - percent);
	}

	template < typename t >
	FORCEINLINE t Lerp(const t& t1, const t& t2, float progress)
	{
		return t1 + (t2 - t1) * progress;
	}

	// hermite basis function for smooth interpolation
// Similar to Gain() above, but very cheap to call
// value should be between 0 & 1 inclusive
	inline float SimpleSpline(float value)
	{
		float valueSquared = value * value;

		// Nice little ease-in, ease-out spline-like curve
		return (3 * valueSquared - 2 * valueSquared * value);
	}

	template< class T, class Y, class X >
	inline T clamp(T const& val, Y const& minVal, X const& maxVal)
	{
		if (val < minVal)
			return minVal;
		else if (val > maxVal)
			return maxVal;
		else
			return val;
	}

	// remaps a value in [startInterval, startInterval+rangeInterval] from linear to
// spline using SimpleSpline
	inline float SimpleSplineRemapValClamped(float val, float A, float B, float C, float D)
	{
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		cVal = clamp(cVal, 0.0f, 1.0f);
		return C + (D - C) * SimpleSpline(cVal);
	}

	FORCEINLINE float ClampCycle(float flCycleIn)
	{
		flCycleIn -= int(flCycleIn);

		if (flCycleIn < 0)
		{
			flCycleIn += 1;
		}
		else if (flCycleIn > 1)
		{
			flCycleIn -= 1;
		}

		return flCycleIn;
	}

	template < typename T >
	T interpolation(
		T p1,
		T p2,
		T d1,
		T d2,
		float t) {
		float tSqr = t * t;
		float tCube = t * tSqr;

		float b1 = 2.0f * tCube - 3.0f * tSqr + 1.0f;
		float b2 = 1.0f - b1; // -2*tCube+3*tSqr;
		float b3 = tCube - 2 * tSqr + t;
		float b4 = tCube - tSqr;

		T output;
		output = p1 * b1;
		output += p2 * b2;
		output += d1 * b3;
		output += d2 * b4;

		return output;
	}

	template < typename T >
	T interpolate(T p0, T p1, T p2, float t) {
		return interpolation(p1, p2, p1 - p0, p2 - p1, t);
	}
}