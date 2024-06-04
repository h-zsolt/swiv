#include "pch.h"
#include "MovementSystem.h"
#include <Data/MoveData.h>
#include <Components/TransformComponent.h>


void MovementSystem::init(ECS::ECS& ecs)
{
    // Create Signiture
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<TransformComponent>());
    psSig.set(ecs.getComponentBitflag<MoveData>());
    ecs.setSystemSignature<MovementSystem>(psSig);
}

void MovementSystem::update(ECS::ECS& ecs, float& delta_time)
{
    for (auto& entity : m_entities)
    {
        auto& transform = ecs.getComponent<TransformComponent>(entity);
        auto& move_data = ecs.getComponent<MoveData>(entity);

        transform.m_position += move_data.direction * move_data.speed * delta_time;
    }
}
