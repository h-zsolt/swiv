#include "pch.h"
#include "InRangeTrigger.h"
#include <Data/BulletData.h>
#include <Data/TargetData.h>
#include "Tags.h"

void InRangeTriggerSystem::init(ECS::ECS& ecs)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<InRangeTriggerTag>());
    psSig.set(ecs.getComponentBitflag<BulletData>());
    ecs.setSystemSignature<InRangeTriggerSystem>(psSig);
}

void InRangeTriggerSystem::update(ECS::ECS& ecs)
{
    for (auto& entity : m_entities)
    {
        auto& target_data = ecs.getComponent<TargetData>(entity);
        auto& bullet_data = ecs.getComponent<BulletData>(entity);

        if (target_data.has_target)
        {
            bullet_data.is_fireing = true;
        }
        else
        {
            bullet_data.is_fireing = false;
        }
    }
}
