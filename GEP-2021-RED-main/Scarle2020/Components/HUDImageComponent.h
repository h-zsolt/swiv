#pragma once

#include "pch.h"

struct HUDImageComponent
{
	DirectX::SimpleMath::Vector2 m_relative_position;
	float m_relative_rotation_radians = 0.f;
	DirectX::SimpleMath::Vector2 m_relative_scale = { 1.f, 1.f };
	DirectX::SimpleMath::Vector2 m_origin;
	DirectX::SimpleMath::Color m_colour = { 1.f, 1.f, 1.f, 1.f };
	int m_texture_id = -1;
	// Drawn from highest to lowest meaning 0.0 will be on top.
	float m_z_order = 0.f;
};