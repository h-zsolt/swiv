#include "pch.h"
#include "HealthSystem.h"

// Components
#include <Data/HealthData.h>

void HealthSystem::init(ECS::ECS& ecs)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<HealthData>());
	ecs.setSystemSignature<HealthSystem>(sig);
}

void HealthSystem::update(ECS::ECS& ecs)
{
	for(auto& entity : m_entities)
	{
		auto& health_data = ecs.getComponent<HealthData>(entity);

		// needs to be updated if player is going to have health
		if(health_data.m_current_health <= 0)
		{
			m_entities_to_destroy.push_back(entity);
		}
	}

	for (auto& entity : m_entities_to_destroy)
	{
		ecs.destroyEntity(entity);
	}
	m_entities_to_destroy.clear();
}

