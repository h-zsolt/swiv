#pragma once
#include "pch.h"
#include "ECS.h"

class EventBus;

class NoEnemiesWinSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs, EventBus* event_bus);
	void update(ECS::ECS& ecs);
private:
	EventBus* m_event_bus = nullptr;
};