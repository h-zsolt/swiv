#include "pch.h"
#include "RotationSystem.h"
#include <Data/RotationData.h>
#include <Components/TransformComponent.h>

void RotationSystem::init(ECS::ECS& ecs)
{
    // Create Signiture
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<TransformComponent>());
    psSig.set(ecs.getComponentBitflag<RotationData>());
    ecs.setSystemSignature<RotationSystem>(psSig);
}

void RotationSystem::update(ECS::ECS& ecs, float& delta_time)
{
    for (auto& entity : m_entities)
    {
        auto& transform = ecs.getComponent<TransformComponent>(entity);
        auto& rotation_data = ecs.getComponent<RotationData>(entity);

        using Vector2 = DirectX::SimpleMath::Vector2;

        // convert radians to direction
        auto current_rotation = transform.m_rotation_radians;
        Vector2 current_direction = { sin(current_rotation), -cos(current_rotation) };

        // lerp direction based on turn speed
        auto new_direction = DirectX::SimpleMath::Vector2::Lerp(current_direction, rotation_data.forward, rotation_data.speed * delta_time);

    	transform.m_rotation_radians = atan2f(new_direction.x, -new_direction.y);
    }
}
