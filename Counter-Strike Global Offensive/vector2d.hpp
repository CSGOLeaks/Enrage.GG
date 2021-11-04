#pragma once

#include "core.hpp"

class Vector2D
{
public:
	Vector2D();
	Vector2D( float x, float y );
	Vector2D( const Vector2D& v );
	Vector2D( const float* v );

public:
	void Set( float x = 0.0f, float y = 0.0f );

	bool IsZero(float tolerance = 0.01f);

	float Dot( const Vector2D& v ) const;

	float LengthSquared() const;
	float Length() const;

public:
	float operator [] ( const std::uint32_t index );
	const float operator [] ( const std::uint32_t index ) const;

	bool operator < (const Vector2D& v) const;
	bool operator > (const Vector2D& v) const;

	bool operator ==(const Vector2D& src);
	bool operator != (const Vector2D& v);

	Vector2D& operator = ( const Vector2D& v );
	Vector2D& operator = ( const float* v );

	Vector2D& operator += ( const Vector2D& v );
	Vector2D& operator -= ( const Vector2D& v );
	Vector2D& operator *= ( const Vector2D& v );
	Vector2D& operator /= ( const Vector2D& v );

	Vector2D& operator += ( float fl );
	Vector2D& operator -= ( float fl );
	Vector2D& operator *= ( float fl );
	Vector2D& operator /= ( float fl );

	Vector2D operator + ( const Vector2D& v ) const;
	Vector2D operator - ( const Vector2D& v ) const;
	Vector2D operator * ( const Vector2D& v ) const;
	Vector2D operator / ( const Vector2D& v ) const;

	Vector2D operator + ( float fl ) const;
	Vector2D operator - ( float fl ) const;
	Vector2D operator * ( float fl ) const;
	Vector2D operator / ( float fl ) const;

	static Vector2D Zero;

public:
	float x = 0.0f;
	float y = 0.0f;
};