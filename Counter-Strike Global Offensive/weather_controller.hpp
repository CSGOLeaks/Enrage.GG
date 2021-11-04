#pragma once

#include "source.hpp"

// Types of precipitation
enum PrecipitationType_t : int
{
	PRECIPITATION_TYPE_RAIN = 0,
	PRECIPITATION_TYPE_SNOW,
	PRECIPITATION_TYPE_ASH,
	PRECIPITATION_TYPE_SNOWFALL,
	PRECIPITATION_TYPE_PARTICLERAIN,
	PRECIPITATION_TYPE_PARTICLEASH,
	PRECIPITATION_TYPE_PARTICLERAINSTORM,
	PRECIPITATION_TYPE_PARTICLESNOW,
	NUM_PRECIPITATION_TYPES
};

class c_weather_controller
{
public:
	virtual void find_precipitation();

	IClientEntity* m_precipitation = nullptr;
	C_BasePlayer* m_precipitation_ent = nullptr;
	int m_index = -1;
	int last_update_tick = 0;
};