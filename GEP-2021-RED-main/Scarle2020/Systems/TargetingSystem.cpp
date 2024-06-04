#include "pch.h"
#include "TargetingSystem.h"
#include <Data/TargetData.h>
#include <Components/TransformComponent.h>
#include "Tags.h"

void TargetingSystem::init(ECS::ECS& ecs)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<TargetData>());
    ecs.setSystemSignature<TargetingSystem>(psSig);
}

void TargetingSystem::update(ECS::ECS& ecs, std::vector<ECS::Entity>& possible_targets)
{
    for (auto& entity : m_entities)
    {
        auto& target_data = ecs.getComponent<TargetData>(entity);

        if (possible_targets.empty())
        {
            target_data.has_target = false;
            continue;
        }

        auto& transform = ecs.getComponent<TransformComponent>(entity);

        ECS::Entity closest_target = {};
        float target_distance = 99999.f;
        DirectX::SimpleMath::Vector2 target_position;

        for (auto& target : possible_targets)
        {
            if (!ecs.hasComponent<TransformComponent>(target))
            {
                continue;
            }

            auto& target_transform = ecs.getComponent<TransformComponent>(target);
            auto distance_between = DirectX::SimpleMath::Vector2::Distance(transform.m_position, target_transform.m_position);
            
            if (distance_between < target_distance)
            {
                target_distance = distance_between;
                closest_target = target;
                target_position = target_transform.m_position;
            }
        }

        target_data.has_target = target_distance < target_data.targeting_range;
        target_data.target = closest_target;
        target_data.target_distance = target_distance;
        target_data.target_direction = target_position - transform.m_position;
        target_data.target_direction.Normalize();
    }
}
