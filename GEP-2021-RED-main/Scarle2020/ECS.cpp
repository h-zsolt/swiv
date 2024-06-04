#include "pch.h"
#include "ECS.h"
#include "Components/BoundComponent.h"

void ECS::ECS::init()
{
	m_component_manager = std::make_unique<ComponentManager>();
	m_entity_manager = std::make_unique<EntityManager>();
	m_system_manager = std::make_unique<SystemManager>();
}

ECS::Entity ECS::ECS::createEntity()
{
	return m_entity_manager->createEntity();
}

void ECS::ECS::destroyEntity(Entity entity)
{
	if (hasComponent<BoundComponent>(entity))
	{
		for (const auto& bound_entity : getComponent<BoundComponent>(entity).m_entities)
		{
			finalDestroyEntity(bound_entity);
		}
	}
	finalDestroyEntity(entity);
}

void ECS::ECS::finalDestroyEntity(Entity entity)
{
	m_entity_manager->destroyEntity(entity);
	m_component_manager->entityDestroyed(entity);
	m_system_manager->entityDestroyed(entity);
}
