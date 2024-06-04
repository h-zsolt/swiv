#pragma once
#include "pch.h"

struct RotationData
{
	using Vector2 = DirectX::SimpleMath::Vector2;
	
	Vector2 forward = { 0, -1 };
	float speed = 0;
};

