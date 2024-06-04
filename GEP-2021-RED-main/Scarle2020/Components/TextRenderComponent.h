#pragma once

#include "pch.h"

struct TextRenderComponent
{
	DirectX::SimpleMath::Vector2 m_relative_position;
	float m_relative_rotation_radians = 0.f;
	DirectX::SimpleMath::Vector2 m_relative_scale = { 1.f, 1.f };
	std::string m_text = "Text";
	size_t m_font_id = 0;
	DirectX::SimpleMath::Vector2 m_origin;
	DirectX::SimpleMath::Color m_color = { 1.f, 1.f, 1.f, 1.f };
	// Drawn from highest to lowest meaning 0.0 will be on top.
	float m_z_order = 0.f;
};