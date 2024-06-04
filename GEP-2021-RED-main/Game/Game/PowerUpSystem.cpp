#include "PowerUpSystem.h"
#include <PowerUpData.h>
#include <EventBus.h>
#include <GameEvent.h>
#include <Components/TransformComponent.h>
#include <Data/BulletData.h>
#include "EntityFactory.h"
#include <random>

void PowerUpSystem::init(ECS::ECS& ecs, IRenderer& renderer, ID3D11Device1* device, EventBus& event_bus)
{
	m_ecs = &ecs;
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<PowerUpData>());
	ecs.setSystemSignature<PowerUpSystem>(sig);

	m_event_bus = &event_bus;
	m_event_bus->subscribe(this, &PowerUpSystem::onGameEvent);

	m_renderer = &renderer;
	m_sprite_id = m_renderer->loadTexture(device, "Assets/PowerUp.dds");
	
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void PowerUpSystem::update(ECS::ECS& ecs)
{
	std::vector<ECS::Entity> expended_pUp;

	for (auto& entity : m_entities)
	{
		auto& pUp_data = ecs.getComponent<PowerUpData>(entity);

		if (pUp_data.player_entity != 65535)
		{
			switch (pUp_data.type)
			{
			case PowerUpType::FIRE_RATE:
				applyFireRatePowerUp(pUp_data.player_entity);
				break;
			case PowerUpType::BONUS_SCORE:
				break;
			default:
				std::cout << "PowerUp not handled by PowerUpSystem::update()\n";
				break;
			}

			expended_pUp.push_back(entity);
		}
	}

	while (!expended_pUp.empty())
	{
		ecs.destroyEntity(expended_pUp.back());
		expended_pUp.pop_back();
	}
}

void PowerUpSystem::onGameEvent(GameEvent* game_event)
{
	switch (game_event->m_event_type)
	{
	case GameEventType::GameEventType_2: // Player Hit
		/// could be used for shileds
		/// set invulnerability and then track number of hits
		break;

	case GameEventType::GameEventType_3: // Enemy Destroyed
		onEnemyDestroyed(game_event->m_enemy_id);
		break;

	default:
		/// IGNORE
		break;
	}
}

void PowerUpSystem::onEnemyDestroyed(ECS::Entity& enemy)
{
	// 1 in 6 chance a power up will spawn when enemy is killed
	//int rand_int = std::rand() / ((RAND_MAX + 1u) / 5);

	std::random_device device;
	std::mt19937 gen(device());
	std::uniform_int_distribution<int> dist(1, 6);
	int rand_int = dist(gen);

 	if (rand_int != 1) return;

	auto& transform = m_ecs->getComponent<TransformComponent>(enemy);
	EntityFactory::spawnPowerUp(*m_ecs, *m_renderer, transform, m_sprite_id );
}

void PowerUpSystem::applyFireRatePowerUp(ECS::Entity& player)
{
	auto& bullet_data = m_ecs->getComponent<BulletData>(player);
	bullet_data.rate_of_fire /= 2;
}

