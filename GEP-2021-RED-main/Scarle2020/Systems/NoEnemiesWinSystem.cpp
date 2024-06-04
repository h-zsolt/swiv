#include "pch.h"
#include "NoEnemiesWinSystem.h"
#include "Components/TransformComponent.h"
#include "Data/EnemySpawnerData.h"
#include "Tags.h"
#include "GameEvent.h"
#include "EventBus.h"

#include "Data/HealthData.h"

void NoEnemiesWinSystem::init(ECS::ECS& ecs, EventBus* event_bus)
{
	m_event_bus = event_bus;
	ECS::Signature sig;
	//sig.set(ecs.getComponentBitflag<TransformComponent>());
	sig.set(ecs.getComponentBitflag<WinConditionTag>());
	ecs.setSystemSignature<NoEnemiesWinSystem>(sig);
}

void NoEnemiesWinSystem::update(ECS::ECS& ecs)
{
	//botched this on first go, workaround system in place
	bool is_there_any_left = false;
	for (auto& entity : m_entities)
	{
		/*if (ecs.hasComponent<EnemyTag>(entity))
		{
			is_there_any_left = true;
		}
		if (ecs.hasComponent<EnemySpawnerData>(entity) && ecs.getComponent<EnemySpawnerData>(entity).spawn_count>0)
		{
			is_there_any_left = true;
		}*/
		if (ecs.hasComponent<HealthData>(entity) && ecs.getComponent<HealthData>(entity).m_current_health<=1)
		{
			ecs.getComponent<HealthData>(entity).m_current_health -= 1;
			GameEvent game_event;
			game_event.m_event_type = GameEventType::GameEventType_4;
			m_event_bus->envoke(&game_event);
		}
	} 
	/*if (!is_there_any_left)
	{
		GameEvent game_event;
		game_event.m_event_type = GameEventType::GameEventType_4;
		m_event_bus->envoke(&game_event);
	}*/
}
