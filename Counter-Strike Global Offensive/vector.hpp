#pragma once
#include <cmath>
#include "core.hpp"

class Vector
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	FORCEINLINE Vector()
	{
		this->Set();
	}

	FORCEINLINE void clear()
	{
		this->Set(0, 0, 0);
	}

	FORCEINLINE Vector(float x, float y, float z)
	{
		this->Set(x, y, z);
	}

	FORCEINLINE Vector(const Vector& v)
	{
		this->Set(v.x, v.y, v.z);
	}

	FORCEINLINE Vector(const float* v)
	{
		this->Set(v[0], v[1], v[2]);
	}

	FORCEINLINE void  Set(float x = 0.0f, float y = 0.0f, float z = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	FORCEINLINE bool  IsZero(float tolerance = 0.01f) const
	{
		return (this->x > -tolerance && this->x < tolerance&&
			this->y > -tolerance && this->y < tolerance&&
			this->z > -tolerance && this->z < tolerance);
	}

	FORCEINLINE float  Normalize()
	{
		/*auto length = this->Length();

		(*this) /= length;

		return length;*/
		float len = Length();

		(*this) /= (len + std::numeric_limits< float >::epsilon());

		return len;
	}

	FORCEINLINE float  Dot(const Vector& v) const
	{
		return (x * v.x +
			y * v.y +
			z * v.z);
	}

	FORCEINLINE float  LengthSquared() const
	{
		return ((x * x) + (y * y) + (z * z));
	}

	FORCEINLINE float  Length() const
	{
		return (std::sqrt(LengthSquared()));
	}

	FORCEINLINE float  Length2D() const
	{
		return std::sqrt((x * x + y * y));
	}

	FORCEINLINE float  DistanceSquared(const Vector& v) const
	{
		return ((*this - v).LengthSquared());
	}

	FORCEINLINE float  Distance(const Vector& v) const
	{
		return ((*this - v).Length());
	}

	FORCEINLINE Vector  Normalized() const
	{
		/*Vector copy( *this );

		if (IsValid())
			copy.Normalize();
		else
			copy.clear();

		return copy;*/
		/*
		if (l > 0)
		{
			x /= l;
			y /= l;
			z /= l;
		}*/
		Vector vec(*this);

		float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

		// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
		float iradius = 1.f / (radius + FLT_EPSILON);

		vec.x *= iradius;
		vec.y *= iradius;
		vec.z *= iradius;

		return vec;
	}

	FORCEINLINE Vector  NormalizeInPlace()
	{
		Vector& vec = *this;

		float radius = sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);

		// FLT_EPSILON is added to the radius to eliminate the possibility of divide by zero.
		float iradius = 1.f / (radius + FLT_EPSILON);

		vec.x *= iradius;
		vec.y *= iradius;
		vec.z *= iradius;

		return vec;
	}

	FORCEINLINE Vector Cross(const Vector& v)
	{
		return { this->y * v.z - this->z * v.y,
				 this->z * v.x - this->x * v.z,
				 this->x * v.y - this->y * v.x };
	}

	FORCEINLINE bool  IsValid()
	{
		return (isfinite(x) && isfinite(y) && isfinite(z));
	}

	FORCEINLINE void  ToVectors(Vector& right, Vector& up) { // VectorVectors
		Vector tmp;
		if (x == 0.f && y == 0.f) {
			// pitch 90 degrees up/down from identity
			right[0] = 0.f;
			right[1] = -1.f;
			right[2] = 0.f;
			up[0] = -z;
			up[1] = 0.f;
			up[2] = 0.f;
		}
		else {
			tmp[0] = 0.f;
			tmp[1] = 0.f;
			tmp[2] = 1.0f;
			right = Cross(tmp);
			up = right.Cross(*this);

			right.Normalize();
			up.Normalize();
		}
	}

	//FORCEINLINE QAngle ToEulerAngles(Vector* pseudoup /*= nullptr*/)
	//{
	//	auto pitch = 0.0f;
	//	auto yaw = 0.0f;
	//	auto roll = 0.0f;

	//	auto length = this->ToVector2D().Length();

	//	if (pseudoup) {
	//		auto left = pseudoup->Cross(*this);

	//		left.Normalize();

	//		pitch = ToDegrees(std::atan2f(-this->z, length));

	//		if (pitch < 0.0f)
	//			pitch += 360.0f;

	//		if (length > 0.001f) {
	//			yaw = ToDegrees(std::atan2f(this->y, this->x));

	//			if (yaw < 0.0f)
	//				yaw += 360.0f;

	//			auto up_z = (this->x * left.y) - (this->y * left.x);

	//			roll = ToDegrees(std::atan2f(left.z, up_z));

	//			if (roll < 0.0f)
	//				roll += 360.0f;
	//		}
	//		else {
	//			yaw = ToDegrees(std::atan2f(-left.x, left.y));

	//			if (yaw < 0.0f)
	//				yaw += 360.0f;
	//		}
	//	}
	//	else {
	//		if (this->x == 0.0f && this->y == 0.0f) {
	//			if (this->z > 0.0f)
	//				pitch = 270.0f;
	//			else
	//				pitch = 90.0f;
	//		}
	//		else {
	//			pitch = ToDegrees(std::atan2f(-this->z, length));

	//			if (pitch < 0.0f)
	//				pitch += 360.0f;

	//			yaw = ToDegrees(std::atan2f(this->y, this->x));

	//			if (yaw < 0.0f)
	//				yaw += 360.0f;
	//		}
	//	}

	//	return { pitch, yaw, roll };
	//}

	FORCEINLINE float  operator [] (const std::uint32_t index) const
	{
		return (((float*)this)[index]);
	}

	FORCEINLINE float& operator [] (const std::uint32_t index)
	{
		return (((float*)this)[index]);
	}

	FORCEINLINE Vector& operator = (const Vector& v)
	{
		this->Set(v.x, v.y, v.z);

		return (*this);
	}

	FORCEINLINE Vector& operator = (const float* v)
	{
		this->Set(v[0], v[1], v[2]);

		return (*this);
	}

	FORCEINLINE bool  operator == (const Vector& src)
	{
		return (src.x == x && src.y == y && src.z == z);
	}

	FORCEINLINE bool  operator != (const Vector& src)
	{
		return (src.x != x || src.y != y || src.z != z);
	}

	FORCEINLINE bool  operator<(const Vector& src)
	{
		return (src.x > x || src.y > y || src.z > z);
	}

	FORCEINLINE bool  operator>(const Vector& src)
	{
		return (src.x < x || src.y < y || src.z < z);
	}

	FORCEINLINE Vector& operator += (const Vector& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;

		return (*this);
	}

	FORCEINLINE Vector& operator -= (const Vector& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;

		return (*this);
	}

	FORCEINLINE Vector& operator *= (const Vector& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;

		return (*this);
	}

	FORCEINLINE Vector& operator /= (const Vector& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;

		return (*this);
	}

	FORCEINLINE Vector& operator += (float fl)
	{
		this->x += fl;
		this->y += fl;
		this->z += fl;

		return (*this);
	}

	FORCEINLINE Vector& operator -= (float fl)
	{
		this->x -= fl;
		this->y -= fl;
		this->z -= fl;

		return (*this);
	}

	FORCEINLINE Vector& operator *= (float fl)
	{
		this->x *= fl;
		this->y *= fl;
		this->z *= fl;

		return (*this);
	}

	FORCEINLINE Vector& operator /= (float fl)
	{
		this->x /= fl;
		this->y /= fl;
		this->z /= fl;

		return (*this);
	}

	FORCEINLINE Vector  operator + (const Vector& v) const
	{
		return { this->x + v.x,
				 this->y + v.y,
				 this->z + v.z };
	}

	FORCEINLINE Vector  operator - (const Vector& v) const
	{
		return { this->x - v.x,
				 this->y - v.y,
				 this->z - v.z };
	}

	FORCEINLINE Vector  operator * (const Vector& v) const
	{
		return { this->x * v.x,
				 this->y * v.y,
				 this->z * v.z };
	}

	FORCEINLINE Vector  operator * (int v) const
	{
		return { this->x * v,
			this->y * v,
			this->z * v };
	}

	FORCEINLINE Vector  operator / (const Vector& v) const
	{
		return { this->x / v.x,
				 this->y / v.y,
				 this->z / v.z };
	}

	FORCEINLINE Vector  operator + (float fl) const
	{
		return { this->x + fl,
				 this->y + fl,
				 this->z + fl };
	}

	FORCEINLINE Vector  operator - (float fl) const
	{
		return { this->x - fl,
				 this->y - fl,
				 this->z - fl };
	}

	FORCEINLINE Vector  operator * (float fl) const
	{
		return { this->x * fl,
				 this->y * fl,
				 this->z * fl };
	}

	FORCEINLINE Vector  operator / (float fl) const
	{
		return { this->x / fl,
				 this->y / fl,
				 this->z / fl };
	}

public:
	static Vector Zero;
	Vector2D ToVector2D();
	Vector4D ToVector4D(float w = 0.0f);
};

//using QAngle = Vector;