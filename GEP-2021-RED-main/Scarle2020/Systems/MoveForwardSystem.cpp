#include "pch.h"
#include "MoveForwardSystem.h"
#include <Data/MoveData.h>
#include <Components/TransformComponent.h>
#include "Tags.h"

void MoveForwardSystem::init(ECS::ECS& ecs)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<MoveForwardTag>());
    psSig.set(ecs.getComponentBitflag<MoveData>());
    psSig.set(ecs.getComponentBitflag<TransformComponent>());
    ecs.setSystemSignature<MoveForwardSystem>(psSig);
}

void MoveForwardSystem::update(ECS::ECS& ecs)
{
    for (auto& entity : m_entities)
    {
        auto& transform = ecs.getComponent<TransformComponent>(entity);
        auto& move_data = ecs.getComponent<MoveData>(entity);

        move_data.direction = { sin(transform.m_rotation_radians), -cos(transform.m_rotation_radians) };
    }
}
