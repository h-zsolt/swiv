#pragma once

#include "pch.h"
#include "IEvent.h"
#include "ECSDefinitions.h"

enum class GameEventType : uint8_t
{
	GameEventType_None = 0,
	GameEventType_1,
	GameEventType_2,
	GameEventType_3,
	GameEventType_4
};

struct GameEvent : public IEvent
{
	GameEventType m_event_type{ GameEventType::GameEventType_None };
	ECS::Entity m_player_id{ 957142 };
	ECS::Entity m_enemy_id{ 957142 };
};