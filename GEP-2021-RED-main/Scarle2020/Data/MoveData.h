#pragma once
#include "pch.h"

struct MoveData
{
	using Vector2 = DirectX::SimpleMath::Vector2;

	Vector2 direction;
	float   speed;
};
