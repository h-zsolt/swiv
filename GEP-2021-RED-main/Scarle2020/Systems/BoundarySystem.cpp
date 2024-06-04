#include "pch.h"
#include "BoundarySystem.h"

// Events
#include "EventBus.h"
#include "BoundaryEvent.h"

// Components
#include <Data/MoveData.h>
#include <Components/TransformComponent.h>
#include <Data/Box2DColliderData.h>

void BoundarySystem::init(ECS::ECS& ecs, EventBus* event_bus)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<Box2DColliderData>());
	sig.set(ecs.getComponentBitflag<MoveData>());
	sig.set(ecs.getComponentBitflag<TransformComponent>());
	sig.set(ecs.getComponentBitflag<TagData>());
	ecs.setSystemSignature<BoundarySystem>(sig);

	m_event_bus = event_bus;
}

void BoundarySystem::update(ECS::ECS& ecs, const Vector2& bounds, const float& dt, float camera_y_pos)
{
	for (auto& entity : m_entities)
	{
		auto& tag = ecs.getComponent<TagData>(entity).tag;

		if (tag != TagID::PLAYER) { continue; }

		Vector2 next_pos;

		nextPos(ecs, entity, next_pos, dt);

		next_pos.y += camera_y_pos;

		const auto boundary = boundaryCheck(ecs, entity, bounds, next_pos);

		if (boundary > 0)
		{
			// boundary collision occurred
			BoundaryEvent event{ entity, boundary, dt, diff };
			m_event_bus->envoke(&event);
		}
	}
}

void BoundarySystem::nextPos(ECS::ECS& ecs, ECS::Entity entity, Vector2& next, const float& dt)
{
	const auto& tc = ecs.getComponent<TransformComponent>(entity);
	const auto& mc = ecs.getComponent<MoveData>(entity);


	next.x = (tc.m_position.x + mc.direction.x * (mc.speed * dt));
	next.y = (tc.m_position.y + mc.direction.y * (mc.speed * dt));
}

int BoundarySystem::boundaryCheck(ECS::ECS& ecs, ECS::Entity entity, const Vector2& bounds, Vector2 next)
{
	const auto& cc = ecs.getComponent<Collider>(entity);

	if (next.y + cc.m_half_size.y > bounds.y &&
		!((next.x - cc.m_half_size.x) < bounds.x / M_BOUNDARY_WIDTH_DIVISOR || (next.x + cc.m_half_size.x) > bounds.x))
	{
		// bottom boundary
		diff = fabs(next.y + cc.m_half_size.y - bounds.y);
		return 1;
	}
	if (next.y + cc.m_half_size.y > bounds.y &&
		((next.x - cc.m_half_size.x) < bounds.x / M_BOUNDARY_WIDTH_DIVISOR || (next.x + cc.m_half_size.x) > bounds.x))
	{
		// bottom corner boundaries
		diff = fabs(next.y + cc.m_half_size.y - bounds.y);
		return 2;
	}
	
	if (((next.x - cc.m_half_size.x) < bounds.x / M_BOUNDARY_WIDTH_DIVISOR || (next.x + cc.m_half_size.x) > bounds.x) &&
	    !((next.y - cc.m_half_size.y) < 0.F || (next.y + cc.m_half_size.y) > bounds.y))
	{
		// left or right boundaries
		return 3;
	}
	if ((next.y - cc.m_half_size.y) < 0.F &&
		!((next.x - cc.m_half_size.x) < bounds.x / M_BOUNDARY_WIDTH_DIVISOR || (next.x + cc.m_half_size.x) > bounds.x))
	{
		// top boundary
		return 4;
	}
	if (next.y - cc.m_half_size.y < 0.F &&
		((next.x - cc.m_half_size.x) < bounds.x / M_BOUNDARY_WIDTH_DIVISOR || (next.x + cc.m_half_size.x) > bounds.x))
	{
		// top corner boundaries
		return 5;
	}
	
	return 0;
}
