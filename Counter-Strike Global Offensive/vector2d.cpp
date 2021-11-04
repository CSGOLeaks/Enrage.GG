#include "vector2d.hpp"

Vector2D::Vector2D()
{
	this->Set();
}

Vector2D::Vector2D( float x, float y )
{
	this->Set( x, y );
}

Vector2D::Vector2D( const Vector2D& v )
{
	this->Set( v.x, v.y );
}

Vector2D::Vector2D( const float* v )
{
	this->Set( v[0], v[1] );
}

void Vector2D::Set( float x /*= 0.0f*/, float y /*= 0.0f */)
{
	this->x = x;
	this->y = y;
}

float Vector2D::Dot( const Vector2D& v ) const
{
	return ( this->x * v.x +
			 this->y * v.y );
}

float Vector2D::LengthSquared() const
{
	return ( this->Dot( *this ) );
}

float Vector2D::Length() const
{
	return ( sqrt( this->LengthSquared() ) );
}

bool Vector2D::IsZero(float tolerance /*= 0.01f */)
{
	return (this->x > -tolerance && this->x < tolerance &&
		this->y > -tolerance && this->y < tolerance);
}

float Vector2D::operator [] ( const std::uint32_t index )
{
	return ( ( ( float* )this )[index] );
}

const float Vector2D::operator [] ( const std::uint32_t index ) const
{
	return ( ( ( const float* )this )[index] );
}

bool Vector2D::operator == (const Vector2D& src)
{
	return (src.x == x && src.y == y);
}

bool Vector2D::operator != (const Vector2D& src)
{
	return (src.x != x || src.y != y);
}

bool Vector2D::operator<(const Vector2D& v) const
{
	return (this->x < v.x || this->y < v.y);
}

bool Vector2D::operator>(const Vector2D& v) const
{
	return (this->x > v.x || this->y > v.y);
}


Vector2D& Vector2D::operator = ( const Vector2D& v )
{
	this->Set( v.x, v.y );

	return ( *this );
}

Vector2D& Vector2D::operator = ( const float* v )
{
	this->Set( v[0], v[1] );

	return ( *this );
}

Vector2D& Vector2D::operator += ( const Vector2D& v )
{
	this->x += v.x;
	this->y += v.y;

	return ( *this );
}

Vector2D& Vector2D::operator -= ( const Vector2D& v )
{
	this->x -= v.x;
	this->y -= v.y;

	return ( *this );
}

Vector2D& Vector2D::operator *= ( const Vector2D& v )
{
	this->x *= v.x;
	this->y *= v.y;

	return ( *this );
}

Vector2D& Vector2D::operator /= ( const Vector2D& v )
{
	this->x /= v.x;
	this->y /= v.y;

	return ( *this );
}
		  
Vector2D& Vector2D::operator += ( float fl )
{
	this->x += fl;
	this->y += fl;

	return ( *this );
}

Vector2D& Vector2D::operator -= ( float fl )
{
	this->x -= fl;
	this->y -= fl;

	return ( *this );
}

Vector2D& Vector2D::operator *= ( float fl )
{
	this->x *= fl;
	this->y *= fl;

	return ( *this );
}

Vector2D& Vector2D::operator /= ( float fl )
{
	this->x /= fl;
	this->y /= fl;

	return ( *this );
}

Vector2D Vector2D::operator + ( const Vector2D& v ) const
{
	return { this->x + v.x,
			 this->y + v.y };
}

Vector2D Vector2D::operator - ( const Vector2D& v ) const
{
	return { this->x - v.x,
			 this->y - v.y };
}

Vector2D Vector2D::operator * ( const Vector2D& v ) const
{
	return { this->x * v.x,
			 this->y * v.y };
}

Vector2D Vector2D::operator / ( const Vector2D& v ) const
{
	return { this->x / v.x,
			 this->y / v.y };
}

Vector2D Vector2D::operator + ( float fl ) const
{
	return { this->x + fl,
			 this->y + fl };
}

Vector2D Vector2D::operator - ( float fl ) const
{
	return { this->x - fl,
			 this->y - fl };
}

Vector2D Vector2D::operator * ( float fl ) const
{
	return { this->x * fl,
			 this->y * fl };
}

Vector2D Vector2D::operator / ( float fl ) const
{
	return { this->x / fl,
			 this->y / fl };
}

Vector2D Vector2D::Zero(0.0f, 0.0f);