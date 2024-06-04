#pragma once

#include "pch.h"
#include "ECS.h"
#include <vector>

struct TargetData
{
	using Vector2 = DirectX::SimpleMath::Vector2;
	
	ECS::Entity target;
	bool  has_target;
	float targeting_range;

	float target_distance;
	Vector2 target_direction;
};

