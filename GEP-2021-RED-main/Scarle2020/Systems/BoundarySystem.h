#pragma once

#include "pch.h"
#include "ECS.h"

struct Box2DColliderData;
struct BoundaryEvent;
class EventBus;

class BoundarySystem : public ECS::System
{
private: // TypeDefs //
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Collider = Box2DColliderData;

public: // Public Functions //
	void init(ECS::ECS& ecs, EventBus* event_bus);
	void update(ECS::ECS& ecs, const Vector2& bounds, const float& dt, float camera_y_pos);

private: // Private Functions //
	void nextPos(ECS::ECS& ecs, ECS::Entity entity, Vector2& next, const float& dt);
	int boundaryCheck(ECS::ECS& ecs, ECS::Entity entity, const Vector2& bounds, Vector2 next);

private: //
	EventBus* m_event_bus				 = nullptr;
	const float M_BOUNDARY_WIDTH_DIVISOR = 3.7F;
	float diff							 = 0.F;
};
