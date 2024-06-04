#include "pch.h"
#include "LifetimeSystem.h"
#include <Data/LifetimeData.h>

void LifetimeSystem::init(ECS::ECS& ecs)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<LifetimeData>());
    ecs.setSystemSignature<LifetimeSystem>(psSig);
}

void LifetimeSystem::update(ECS::ECS& ecs, float& delta_time)
{
    std::vector<ECS::Entity> remove_list;

    for (auto& entity : m_entities)
    {
        auto& lifetime_data = ecs.getComponent<LifetimeData>(entity);

        if (lifetime_data.time > 0)
        {
            lifetime_data.time -= delta_time;
            continue;
        }

        if (lifetime_data.onDeathCallback != nullptr)
        {
            lifetime_data.onDeathCallback();
        }

        remove_list.push_back(entity);
    }

    while (!remove_list.empty())
    {
        ecs.destroyEntity(remove_list.back());
        remove_list.pop_back();
    }

}
