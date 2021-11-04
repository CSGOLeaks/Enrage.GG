#pragma once

#include "core.hpp"

class Vector4D
{
public:
	Vector4D();
	Vector4D( float x, float y, float z, float w );
	Vector4D( const Vector4D& v );
	Vector4D( const float* v );

public:
	void Set( float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 0.0f );

	float Dot( const Vector4D& v ) const;

	float LengthSquared() const;
	float Length() const;

public:
	Vector4D& operator += ( const Vector4D& v );
	Vector4D& operator -= ( const Vector4D& v );
	Vector4D& operator *= ( const Vector4D& v );
	Vector4D& operator /= ( const Vector4D& v );

	Vector4D& operator += ( float fl );
	Vector4D& operator -= ( float fl );
	Vector4D& operator *= ( float fl );
	Vector4D& operator /= ( float fl );

	Vector4D operator + ( const Vector4D& v ) const;
	Vector4D operator - ( const Vector4D& v ) const;
	Vector4D operator * ( const Vector4D& v ) const;
	Vector4D operator / ( const Vector4D& v ) const;

	Vector4D operator + ( float fl ) const;
	Vector4D operator - ( float fl ) const;
	Vector4D operator * ( float fl ) const;
	Vector4D operator / ( float fl ) const;

public:
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	float w = 0.0f;
};