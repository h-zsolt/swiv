#include "pch.h"
#include "MoveTowardSystem.h"
#include <Data/MoveData.h>
#include <Data/TargetData.h>
#include "Tags.h"

void MoveTowardSystem::init(ECS::ECS& ecs)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<MoveTowardTag>());
    psSig.set(ecs.getComponentBitflag<MoveData>());
    psSig.set(ecs.getComponentBitflag<TargetData>());
    ecs.setSystemSignature<MoveTowardSystem>(psSig);
}

void MoveTowardSystem::update(ECS::ECS& ecs)
{
    for (auto& entity : m_entities)
    {
        auto& target_data = ecs.getComponent<TargetData>(entity);
        auto& move_data = ecs.getComponent<MoveData>(entity);

        if (target_data.target_distance > target_data.targeting_range)
        {
            continue;
        }

        move_data.direction = target_data.target_direction;
    }
}
