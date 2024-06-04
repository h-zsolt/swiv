#pragma once

#include "ECS.h"

enum struct PowerUpType
{
	NONE,
	BONUS_SCORE,
	FIRE_RATE
};

struct PowerUpData
{
	PowerUpType type = PowerUpType::NONE;
	ECS::Entity player_entity = 65535;
};

