#include "qangle.hpp"
#include "vector.hpp"

Vector QAngle::ToVectors(Vector* side /*= nullptr*/, Vector* up /*= nullptr*/)
{
	auto rad_pitch = ToRadians(this->x);
	auto rad_yaw = ToRadians(this->y);
	float rad_roll;

	auto sin_pitch = sinf(rad_pitch);
	auto sin_yaw = sinf(rad_yaw);
	float sin_roll;

	auto cos_pitch = cosf(rad_pitch);
	auto cos_yaw = cosf(rad_yaw);
	float cos_roll;

	if (side || up) {
		rad_roll = ToRadians(this->z);
		sin_roll = sinf(rad_roll);
		cos_roll = cosf(rad_roll);
	}

	if (side) {
		side->x = -1.0f * sin_roll * sin_pitch * cos_yaw + -1.0f * cos_roll * -sin_yaw;
		side->y = -1.0f * sin_roll * sin_pitch * sin_yaw + -1.0f * cos_roll * cos_yaw;
		side->z = -1.0f * sin_roll * cos_pitch;
	}

	if (up) {
		up->x = cos_roll * sin_pitch * cos_yaw + -sin_roll * -sin_yaw;
		up->y = cos_roll * sin_pitch * sin_yaw + -sin_roll * cos_yaw;
		up->z = cos_roll * cos_pitch;
	}

	return { cos_pitch * cos_yaw, cos_pitch * sin_yaw, -sin_pitch };
}

Vector QAngle::NiceCode()
{
	return Vector(x, y, z);
}

Vector QAngle::ToVectorsTranspose(Vector* side /*= nullptr*/, Vector* up /*= nullptr*/)
{
	auto rad_pitch = ToRadians(this->x);
	auto rad_yaw = ToRadians(this->y);
	auto rad_roll = ToRadians(this->z);

	auto sin_pitch = sinf(rad_pitch);
	auto sin_yaw = sinf(rad_yaw);
	auto sin_roll = sinf(rad_roll);

	auto cos_pitch = cosf(rad_pitch);
	auto cos_yaw = cosf(rad_yaw);
	auto cos_roll = cosf(rad_roll);

	if (side)
	{
		side->x = cos_pitch * sin_yaw;
		side->y = sin_roll * sin_pitch * sin_yaw + cos_roll * cos_yaw;
		side->z = cos_roll * sin_pitch * sin_yaw + -sin_roll * cos_yaw;
	}

	if (up)
	{
		up->x = -sin_pitch;
		up->y = sin_roll * cos_pitch;
		up->z = cos_roll * cos_pitch;
	}

	return { cos_pitch * cos_yaw,
			 sin_roll * sin_pitch * cos_yaw + cos_roll * -sin_yaw,
			 cos_roll * sin_pitch * cos_yaw + -sin_roll * -sin_yaw };
}

QAngle QAngle::Zero(0.0f, 0.0f, 0.0f);