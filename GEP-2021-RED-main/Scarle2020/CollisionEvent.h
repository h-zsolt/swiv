#pragma once

#include "ECS.h"
#include "IEvent.h"

struct CollisionEvent : public IEvent
{
	using Vector2 = DirectX::SimpleMath::Vector2;
	explicit CollisionEvent(ECS::Entity entity_a, ECS::Entity entity_b, Vector2 axis, float overlap, float dt) : m_entity_a(entity_a),
		m_entity_b(entity_b), m_axis_projection(axis), m_overlap(overlap), m_dt(dt) {}


	ECS::Entity m_entity_a = 0;
	ECS::Entity m_entity_b = 0;
	Vector2 m_axis_projection = {0.F, 0.F};
	float m_overlap = 0.F;
	float m_dt = 0.F;
};
