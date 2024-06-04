#pragma once

#include "pch.h"
#include "ECSDefinitions.h"

struct HealthBarComponent
{
	float m_max_health = 10.0f; //TODO:data duplication issue with Health Data, think this belongs to Health data, but for ease of implementation it's here atm
	float m_current_health = 10.0f; //keep track of current health as float for knowing when to update and save on conversion later

	float m_feedback_clock = 0.0F;
	float m_feedback_drain_rate = 25.0f;

	bool m_created = false; //TODO:system uses this to initialize atm, slow
	bool m_following = true; //true -> update it's position to owning entity + transform

	DirectX::SimpleMath::Vector2 m_position = { 0,0 };
	DirectX::SimpleMath::Vector2 m_size = { 25,3 };
	
	std::array<ECS::Entity, 3> m_bars;
	std::array<DirectX::SimpleMath::Color, 3> m_bar_colours = { { { 0.2f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.5f, 0.25f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f } } };
	std::array<float, 3>  m_z_orders = { 0.3f, 0.2f, 0.1f };
};
