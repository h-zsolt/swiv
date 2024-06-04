#include "pch.h"
#include "PhysicsSystem.h"
#include <Components/PhysicsComponents.h>
#include "ECS.h"

void PhysicsSystem::init(ECS::ECS& ecs)
{
	ECS::Signature psSig;
	psSig.set(ecs.getComponentBitflag<Gravity>());
	psSig.set(ecs.getComponentBitflag<RigidBody>());
	psSig.set(ecs.getComponentBitflag<Transform>());
	ecs.setSystemSignature<PhysicsSystem>(psSig);
}

void PhysicsSystem::update(ECS::ECS& ecs, float delta_time) // This should take a fixed time step as part of a fixed physics simulation.
{
	for (auto& entity : m_entities)
	{
		auto& gravity = ecs.getComponent<Gravity>(entity);
		auto& rigidBody = ecs.getComponent<RigidBody>(entity);
		auto& transform = ecs.getComponent<Transform>(entity);

		// Ignoring DirectX Simd operations for simplicity
		//DirectX::XMVECTOR acc = DirectX::XMLoadFloat2(&rigidBody.acceleration);
		//DirectX::XMVECTOR grav = DirectX::XMLoadFloat2(&gravity.force);
		//DirectX::XMVECTOR res = DirectX::XMVectorAdd(acc, grav);
		//DirectX::XMStoreFloat2(&rigidBody.acceleration, res);

		transform.position.x += rigidBody.velocity.x * delta_time;
		transform.position.y += rigidBody.velocity.y * delta_time;

		// Calculate acceleration for this step.
		rigidBody.acceleration.x += gravity.force.x * delta_time;
		rigidBody.acceleration.y += gravity.force.y * delta_time;
		// All other forces.

		rigidBody.velocity.x += rigidBody.acceleration.x;
		rigidBody.velocity.y += rigidBody.acceleration.y;

		// Zero out acceleration.
		rigidBody.acceleration.x = 0.f;
		rigidBody.acceleration.y = 0.f;
	}
}
