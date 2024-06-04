#pragma once

#include "ECS.h"

struct Box2DColliderData;
class EventBus;

class CollisionSystem : public ECS::System
{
private:
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Vertices = std::vector<Vector2>;
	using Collider = Box2DColliderData;

public: // Public Functions //
	void init(ECS::ECS& ecs, EventBus* event_bus);
	void update(ECS::ECS& ecs, const float dt);

private: // Collision Detection Functions //
	bool broadPhase(const Collider& cc_a, const Collider& cc_b);
	bool narrowPhase(const Collider& cc_a, const Collider& cc_b);

private: // Helper Functions //
	template<typename L, typename R>
	bool bitFlagCheck(const L lhs, const R rhs);
	void resetCollisionFlags(ECS::ECS& ecs);

private: // Variables //
	EventBus* m_event_bus = nullptr;

	// Delayed collision resolution
	std::vector<std::pair<ECS::Entity, ECS::Entity>> m_entities_collided{};
	std::vector<Vector2> m_axis{};
	std::vector<float> m_overlaps{};

	// collision resolution helpers
	Vector2 m_axis_projection = { 0.F, 0.F };
	float m_overlap			  = 0.F;
	float m_aabb_buffer		  = 5.F;
};

template<typename L, typename R>
inline bool CollisionSystem::bitFlagCheck(const L lhs, const R rhs)
{
	auto val_after = lhs | (1 << rhs);
	auto val = (lhs == val_after);
	return val;
}

