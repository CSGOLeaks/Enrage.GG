#include "vector.hpp"
#include "vector2d.hpp"
#include "vector4d.hpp"
#include "qangle.hpp"

#define CHECK_VALID( _v ) 0

 Vector2D   Vector::ToVector2D()
{
	return { this->x, this->y };
}

 Vector4D   Vector::ToVector4D(float w /*= 0.0f */)
{
	return { this->x, this->y, this->z, w };
}

Vector Vector::Zero(0.0f, 0.0f, 0.0f);