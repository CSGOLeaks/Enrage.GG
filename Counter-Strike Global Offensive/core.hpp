#pragma once

#include "auto.hpp"

constexpr auto RadPi = 3.14159265358979323846;
constexpr auto DegPi = 180.0;

class Vector2D;
class Vector;
class Vector4D;

class QAngle;

struct matrix3x4_t;
class VMatrix;

template<typename T>
T Square( T value )
{
	return ( value * value );
}

template<typename T>
void LimitValue( T& value, const T& min, const T& max )
{
	if( value > max )
		value = max;
	else if( value < min )
		value = min;
}

template<typename T>
T ToRadians( T degrees )
{
	return ( degrees * ( static_cast<T>( RadPi ) / static_cast<T>( DegPi ) ) );
}

template<typename T>
T ToDegrees( T radians )
{
	return ( radians * static_cast<T>(57.295776f) );
}

void FORCEINLINE AngleNormalize( float& angle )
{
	angle = fmodf( angle, 360.0f );

	if( angle > 180.0f )
		angle -= 360.0f;
	else if( angle < -180.0f )
		angle += 360.0f;
}