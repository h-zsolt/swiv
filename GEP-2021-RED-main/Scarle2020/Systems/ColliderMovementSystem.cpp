#include "pch.h"
#include "ColliderMovementSystem.h"

// components
#include <Data/Box2DColliderData.h>
#include <Components/TransformComponent.h>

void ColliderMovementSystem::init(ECS::ECS& ecs)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<Box2DColliderData>());
	sig.set(ecs.getComponentBitflag<TransformComponent>());
	ecs.setSystemSignature<ColliderMovementSystem>(sig);
}

void ColliderMovementSystem::update(ECS::ECS& ecs, const float dt)
{
	for (auto& entity : m_entities)
	{
		auto& collider  = ecs.getComponent<Box2DColliderData>(entity);
		auto& transform = ecs.getComponent<TransformComponent>(entity);
		auto& tag       = ecs.getComponent<TagData>(entity);
		nextPos(collider.m_previous_pos, transform.m_position, collider.m_vertices, transform.m_rotation_radians);
	}
}

// Calculates the next position and rotation of all the vertices of the given collider
void ColliderMovementSystem::nextPos(Vector2& current_position, Vector2& next_pos, Vertices& vertices,
                                     float rotation_radians)
{
	// get copy of origin co-ordinate to translate vertices back to local space
	const float ox = current_position.x;
	const float oy = current_position.y;

	// cache rotation for better performance
	const float sr = sinf(rotation_radians);
	const float cr = cosf(rotation_radians);

	for (auto& vertex : vertices)
	{
		// translated co-ordinates to local space (0, 0)
		const auto px = vertex.x - ox;
		const auto py = vertex.y - oy;

		// calculate new rotated co-ordinates and translate back to world space position
		const auto nx = (cr * px) - (sr * py) + next_pos.x;
		const auto ny = (sr * px) + (cr * py) + next_pos.y;

		// apply new co-ordinates to vertex positions
		vertex.x = nx;
		vertex.y = ny;
	}

	current_position = next_pos; // next position must be copied to the current position
}
