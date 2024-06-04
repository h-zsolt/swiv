#pragma once
#include "pch.h"
#include "ECS.h"

class EventBus;
class IRenderer;
struct GameEvent;

class PowerUpSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs, IRenderer& renderer, ID3D11Device1* device, EventBus& event_bus);
	void update(ECS::ECS& ecs);

private:
	void onGameEvent(GameEvent* game_event);
	void onEnemyDestroyed(ECS::Entity& enemy);

private:
	void applyFireRatePowerUp(ECS::Entity& player);

private:
	ECS::ECS* m_ecs       = nullptr;
	IRenderer* m_renderer = nullptr;
	EventBus* m_event_bus = nullptr;

	size_t m_sprite_id;
};

