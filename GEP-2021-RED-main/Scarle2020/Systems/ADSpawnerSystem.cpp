#include "pch.h"
#include "ADSpawnerSystem.h"
#include "Components/ADSpawnerComponent.h"
#include "Components/TransformComponent.h"
#include "Data/EnemySpawnerData.h"
#include "Tags.h"

void ADSpawnerSystem::init(ECS::ECS& ecs)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<SpawnADsTag>());
	ecs.setSystemSignature<ADSpawnerSystem>(sig);
}

void ADSpawnerSystem::update(ECS::ECS& ecs)
{
	for (auto& entity : m_entities)
	{
		if (ecs.hasComponent<ADSpawnerComponent>(entity) && ecs.hasComponent<TransformComponent>(entity))
		{
			auto& spawner_component = ecs.getComponent<ADSpawnerComponent>(entity);
			auto spawning_code = ecs.getComponent<SpawnADsTag>(entity).m_code;
			for (int i = 0; i < spawner_component.m_spawn_points; i++) //go through each registered spawn point
			{
				if (spawning_code == spawner_component.m_spawn_code[i]) //if this spawn point is among the ones that need to spawn
				{
					auto new_spawn_point = ecs.createEntity();
					//location of spawn point
					auto& parent_transform = ecs.getComponent<TransformComponent>(entity);
					TransformComponent spawn_transform;
					spawn_transform.m_position = parent_transform.m_position + spawner_component.m_relative_positions[i];
					ecs.addComponent(new_spawn_point, spawn_transform);
					//data of spawn point
					EnemySpawnerData spawn_data;
					spawn_data.spawn_count = spawner_component.m_spawn_counts[i];
					spawn_data.spawn_interval = spawner_component.m_spawn_intervals[i];
					spawn_data.type = spawner_component.m_types[i];
					//spawn_data.relative_position = { 0,0 };
					//spawn_data.spawn_timer = 0;
					ecs.addComponent(new_spawn_point, spawn_data);
				}
			}
		}
		m_entities_to_remove.push_back(entity);
	}
	removeTags(ecs);
}

void ADSpawnerSystem::removeTags(ECS::ECS& ecs)
{
	for (auto& entity : m_entities_to_remove)
	{
		ecs.removeComponent<SpawnADsTag>(entity);
	}
	m_entities_to_remove.clear();
}
