#include "pch.h"
#include "RotateTowardSystem.h"
#include <Data/RotationData.h>
#include <Data/TargetData.h>
#include "Tags.h"

void RotateTowardSystem::init(ECS::ECS& ecs)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<RotateTowardTag>());
    psSig.set(ecs.getComponentBitflag<RotationData>());
    psSig.set(ecs.getComponentBitflag<TargetData>());
    ecs.setSystemSignature<RotateTowardSystem>(psSig);
}

void RotateTowardSystem::update(ECS::ECS& ecs)
{
    for (auto& entity : m_entities)
    {
        auto& rotation_data = ecs.getComponent<RotationData>(entity);
        auto& target_data = ecs.getComponent<TargetData>(entity);

        if (target_data.target_distance > target_data.targeting_range)
        {
            continue;
        }
        rotation_data.forward = target_data.target_direction;
    }
}
