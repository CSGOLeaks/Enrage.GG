#pragma once

#include "core.hpp"

class Vector;

class QAngle
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	FORCEINLINE void clear()
	{
		x = 0.f;
		y = 0.f;
		z = 0.f;
	}

	FORCEINLINE QAngle()
	{
		this->Set();
	}

	FORCEINLINE QAngle(float x, float y, float z)
	{
		this->Set(x, y, z);
	}

	FORCEINLINE QAngle(const QAngle& v)
	{
		this->Set(v.x, v.y, v.z);
	}

	FORCEINLINE QAngle(const float* v)
	{
		this->Set(v[0], v[1], v[2]);
	}

	FORCEINLINE void Set(float x = 0.0f, float y = 0.0f, float z = 0.0f)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	FORCEINLINE void Normalize()
	{
		AngleNormalize(this->x);
		AngleNormalize(this->y);
		this->z = 0.f;
	}

	FORCEINLINE void Clamp()
	{
		Normalize();

		//LimitValue( this->x, -89.0f, 89.0f );
		//LimitValue( this->y, -180.0f, 180.0f );
		//LimitValue( this->z, -50.0f, 50.0f );

		this->x = remainderf(this->x, 360.0f); LimitValue(this->x, -89.0f, 89.0f);
		this->y = remainderf(this->y, 360.0f); LimitValue(this->y, -180.0f, 180.0f);
		//this->z = remainderf(this->z, 360.0f); LimitValue(this->z, -45.0f, 45.0f);

		this->z = 0.f;
	}

	FORCEINLINE bool IsZero(float tolerance = 0.01f)
	{
		return (this->x > -tolerance && this->x < tolerance&&
			this->y > -tolerance && this->y < tolerance&&
			this->z > -tolerance && this->z < tolerance);
	}

	FORCEINLINE QAngle Normalized()
	{
		QAngle copy(*this);
		copy.Normalize();
		return copy;
	}

	FORCEINLINE QAngle Clamped()
	{
		QAngle copy(*this);
		copy.Clamp();
		return copy;
	}



	FORCEINLINE float operator [] (const std::uint32_t index)
	{
		return (((float*)this)[index]);
	}

	FORCEINLINE const float operator [] (const std::uint32_t index) const
	{
		return (((const float*)this)[index]);
	}

	FORCEINLINE QAngle& operator = (const QAngle& v)
	{
		this->Set(v.x, v.y, v.z);

		return (*this);
	}

	FORCEINLINE QAngle& operator = (const float* v)
	{
		this->Set(v[0], v[1], v[2]);

		return (*this);
	}

	FORCEINLINE bool operator==(const QAngle& v)
	{
		return (this->x == v.x && this->y == v.y && this->z == v.z);
	}

	FORCEINLINE bool operator!=(const QAngle& v)
	{
		return !(this->x == v.x && this->y == v.y && this->z == v.z);
	}

	FORCEINLINE QAngle& operator += (const QAngle& v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;

		return (*this);
	}

	FORCEINLINE QAngle& operator -= (const QAngle& v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;

		return (*this);
	}

	FORCEINLINE QAngle& operator *= (const QAngle& v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;

		return (*this);
	}

	FORCEINLINE QAngle& operator /= (const QAngle& v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;

		return (*this);
	}

	FORCEINLINE QAngle& operator += (float fl)
	{
		this->x += fl;
		this->y += fl;
		this->z += fl;

		return (*this);
	}

	FORCEINLINE QAngle& operator -= (float fl)
	{
		this->x -= fl;
		this->y -= fl;
		this->z -= fl;

		return (*this);
	}

	FORCEINLINE QAngle& operator *= (float fl)
	{
		this->x *= fl;
		this->y *= fl;
		this->z *= fl;

		return (*this);
	}

	FORCEINLINE QAngle& operator /= (float fl)
	{
		this->x /= fl;
		this->y /= fl;
		this->z /= fl;

		return (*this);
	}

	FORCEINLINE QAngle operator + (const QAngle& v) const
	{
		return { this->x + v.x,
				 this->y + v.y,
				 this->z + v.z };
	}

	FORCEINLINE QAngle operator - (const QAngle& v) const
	{
		return { this->x - v.x,
				 this->y - v.y,
				 this->z - v.z };
	}

	FORCEINLINE QAngle operator * (const QAngle& v) const
	{
		return { this->x * v.x,
				 this->y * v.y,
				 this->z * v.z };
	}

	FORCEINLINE QAngle operator / (const QAngle& v) const
	{
		return { this->x / v.x,
				 this->y / v.y,
				 this->z / v.z };
	}

	FORCEINLINE QAngle operator + (float fl) const
	{
		return { this->x + fl,
				 this->y + fl,
				 this->z + fl };
	}

	FORCEINLINE QAngle operator - (float fl) const
	{
		return { this->x - fl,
				 this->y - fl,
				 this->z - fl };
	}

	FORCEINLINE QAngle operator * (float fl) const
	{
		return { this->x * fl,
				 this->y * fl,
				 this->z * fl };
	}

	FORCEINLINE QAngle operator / (float fl) const
	{
		return { this->x / fl,
				 this->y / fl,
				 this->z / fl };
	}

	Vector ToVectors(Vector* side = nullptr, Vector* up = nullptr);
	Vector NiceCode();
	Vector ToVectorsTranspose(Vector* side = nullptr, Vector* up = nullptr);

public:
	static QAngle Zero;
};