#pragma once

#include "pch.h"

struct TransformComponent
{
	DirectX::SimpleMath::Vector2 m_position;
	DirectX::SimpleMath::Vector2 m_scale = { 1.f, 1.f };
	float m_rotation_radians = 0.f;
};