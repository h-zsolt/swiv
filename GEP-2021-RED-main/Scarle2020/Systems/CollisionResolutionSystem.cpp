#include "pch.h"
#include "CollisionResolutionSystem.h"

// Events
#include "CollisionEvent.h"
#include "BoundaryEvent.h"
#include "GameEvent.h"

// Components
#include <Audio/AudioManager.hpp>
#include <Data/MoveData.h>
#include <Data/Box2DColliderData.h>
#include <Components/TransformComponent.h>
#include <Components/BulletParentComponent.h>
#include <Components/BulletDamageComponent.h>
#include <Data/BulletData.h>
#include <Data/HealthData.h>
#include <PowerUpData.h>

void CollisionResolutionSystem::init(ECS::ECS& ecs, EventBus* event_bus)
{
	// Create callbacks to event functions
	m_event_bus  = event_bus;
	event_bus->subscribe(this, &CollisionResolutionSystem::onCollisionEvent);
	event_bus->subscribe(this, &CollisionResolutionSystem::onBoundaryCollision);

	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<TagData>());
	ecs.setSystemSignature<CollisionResolutionSystem>(sig);

	m_ecs = &ecs;
}

void CollisionResolutionSystem::update()
{
	destroyEntities();
}

// --------------------------------------------------------------------------------------------- //
//                                    Event Callback Functions                                   //
// --------------------------------------------------------------------------------------------- //

void CollisionResolutionSystem::onCollisionEvent(CollisionEvent* _event)
{

	auto& tc_a = m_ecs->getComponent<TagData>(_event->m_entity_a);
	auto& tc_b = m_ecs->getComponent<TagData>(_event->m_entity_b);
	auto tag = (tc_a.tag < tc_b.tag) ? tc_a.tag : tc_b.tag;

	switch (tag)
	{
		case TagID::PLAYER:
			// sort player collisions
			playerCollisionSort(_event);
			break;
		case TagID::ENEMY:
			// sort enemy collisions
			enemyCollisionSort(_event);
			break;
		case TagID::P_BULLET:
			bulletCollisionSort(_event);
			break;
		case TagID::E_BULLET:
			bulletCollisionSort(_event);
			break;
		default:
			std::cout << "unknown entity collision unhandled\n";
			break;
	}
}

void CollisionResolutionSystem::onBoundaryCollision(BoundaryEvent* _event)
{
	auto& mc = m_ecs->getComponent<MoveData>(_event->entity);

	switch (_event->boundary)
	{
	case 1:
	{
		// bottom boundary
		mc.direction.y = 0.F; 
		auto& transform = m_ecs->getComponent<TransformComponent>(_event->entity);
		auto& collider = m_ecs->getComponent<Box2DColliderData>(_event->entity);

		if (_event->diff >= 10.F)
		{
			std::cout << "stuck inside of boundary\n";

			GameEvent game_event;
			game_event.m_player_id = _event->entity;
			game_event.m_event_type = GameEventType::GameEventType_2;
			m_event_bus->envoke(&game_event);

			// push player outside of the boundary
			transform.m_position.y -= 10.F;
		}
		else
		{
			transform.m_position.y -= (collider.m_half_size.x * 1.53F) * _event->dt;
		}
		break;
	}
	case 2:
	{
		// bottom corners
		mc.direction.x = 0.F;
		mc.direction.y = 0.F;
		auto& transform = m_ecs->getComponent<TransformComponent>(_event->entity);
		auto& collider = m_ecs->getComponent<Box2DColliderData>(_event->entity);

		if (_event->diff >= 10.F)
		{
			std::cout << "stuck inside of boundary\n";

			GameEvent game_event;
			game_event.m_player_id = _event->entity;
			game_event.m_event_type = GameEventType::GameEventType_2;
			m_event_bus->envoke(&game_event);

			// push player outside of the boundary
			transform.m_position.y -= 10.F;
		}
		else
		{
			transform.m_position.y -= (collider.m_half_size.x * 1.53F) * _event->dt;
		}
		break;
	}
	case 3:
	{
		// left or right boundaries
		mc.direction.x = 0.F;
		break;
	}
	case 4:
	{
		// top boundary
		mc.direction.y = 0.F;
		break;
	}
	case 5:
	{
		// top corner boundaries
		mc.direction.x = 0.F;
		mc.direction.y = 0.F;
		break;
	}
	default:
		std::cout << "unhandled boundary collision\n";
		break;
	}
}

// --------------------------------------------------------------------------------------------- //
//                                 Collision Sorting Function                                    //
// --------------------------------------------------------------------------------------------- //

void CollisionResolutionSystem::playerCollisionSort(CollisionEvent* e)
{
	auto [tag, player, other] = sortEntities(TagID::PLAYER, e->m_entity_a, e->m_entity_b);

	switch (tag)
	{
	case TagID::E_BULLET:
		playerBulletCollision(player, other);
		break;
	case TagID::ENEMY:
		playerEnemyCollision(player, other);
		break;
	case TagID::ENVIRONMENT:
		playerEnvironmentCollision(player, other, e->m_axis_projection, e->m_overlap, e->m_dt);
		break;
	case TagID::POWER_UP:
		playerPowerUpCollision(player, other);
		break;
	default:
		std::cout << "unhandled player collision \n";
		break;
	}
}

void CollisionResolutionSystem::enemyCollisionSort(CollisionEvent* e)
{
	auto [tag, enemy, other] = sortEntities(TagID::ENEMY, e->m_entity_a, e->m_entity_b);

	switch (tag)
	{
	case TagID::P_BULLET:
		enemyBulletCollision(enemy, other);
		break;
	default:
		std::cout << "unhandled enemy collision \n";
		break;
	}
}

void CollisionResolutionSystem::bulletCollisionSort(CollisionEvent* e)
{
	auto [tag, bullet, other] = sortEntities(TagID::P_BULLET, TagID::E_BULLET, e->m_entity_a, e->m_entity_b);

	switch (tag)
	{
	case TagID::ENVIRONMENT:
		bulletEnvironmentCollision(bullet);
		break;
	default:
		std::cout << "unhandled bullet collision \n";
		break;
	}
}

// --------------------------------------------------------------------------------------------- //
//                            Player Collision Resolution Functions                              //
// --------------------------------------------------------------------------------------------- //

/* @brief player and enemy bullet collision resolution
*
* @param ECS::Entity -> player 
* @param ECS::Entity -> enemy_bullet
*/
void CollisionResolutionSystem::playerBulletCollision(const ECS::Entity player, const ECS::Entity enemy_bullet)
{
	// destroy enemy bullet
	delayedDestroyEntity(enemy_bullet);

	// game event calls to InGame for player invincibility
	GameEvent g_ev;
	g_ev.m_event_type = GameEventType::GameEventType_2;
	g_ev.m_player_id = player;
	m_event_bus->envoke(&g_ev);

	// play explosion
	const auto i = AudioManager::loadSoundEffect("Assets/ship_explosion.wav");
	auto* inst = AudioManager::createEffectInstance(i);

	if (AudioManager::playSound(inst, 0.5F))
	{
		printf("Sound effect %d played successfully\n", i);
	}
}

/* @brief player and enemy collision resolution
*
* @param ECS::Entity -> player
* @param ECS::Entity -> enemy
*/
void CollisionResolutionSystem::playerEnemyCollision(const ECS::Entity player, const ECS::Entity enemy)
{
	// destroy enemy
	delayedDestroyEntity(enemy);

	// game event calls to InGame for player invincibility
	GameEvent g_ev;
	g_ev.m_event_type = GameEventType::GameEventType_2;
	g_ev.m_player_id = player;
	m_event_bus->envoke(&g_ev);

	const auto i = AudioManager::loadSoundEffect("Assets/ship_explosion.wav");
	auto* inst = AudioManager::createEffectInstance(i);

	if (AudioManager::playSound(inst, 0.5F))
	{
		printf("Sound effect %d played successfully\n", i);
	}
}

/* @brief player and environment collision resolution
*
* 
* @param ECS::Entity -> player
* @param ECS::Entity -> environment
* @param Vector2 -> axis
* @param float -> overlap
* @param float -> delta_time
*/
void CollisionResolutionSystem::playerEnvironmentCollision
(const ECS::Entity player, const ECS::Entity envi, const Vector2& axis, const float overlap, const float dt)
{
	auto& trc_p = m_ecs->getComponent<TransformComponent>(player); // player transform
	auto& trc_e = m_ecs->getComponent<TransformComponent>(envi);   // environment transform

	auto& mc_p = m_ecs->getComponent<MoveData>(player); // player movement data

	// calculate the minimum translation vector
	auto normalized = std::sqrt((axis.x * axis.x) + (axis.y * axis.y));
	auto mtv_x = (axis.x / normalized) * overlap;
	auto mtv_y = (axis.y / normalized) * overlap;

	// displacement
	auto nx = mtv_x * dt * 0.25F;
	auto ny = mtv_y * dt * 0.25F;
	
	// difference between positions
	auto dx = trc_e.m_position.x - trc_p.m_position.x;
	auto dy = trc_e.m_position.y - trc_p.m_position.y;

	// calculate which side player is on and set displacement to appropriate value
	nx *= (dx > 0) ? -1 : 1;
	ny *= (dy > 0) ? -1 : 1;

	if ((ny < 0.F && mc_p.direction.y > 0.F) || (ny > 0.F && mc_p.direction.y < 0.F))
	{
		mc_p.direction.y = 0.F;
	}

	if ((nx < 0.F && mc_p.direction.x > 0.F) || (nx > 0.F && mc_p.direction.x < 0.F))
	{
		mc_p.direction.x = 0.F;
	}

	// apply displacement to player transform
	trc_p.m_position.x -= nx;
	trc_p.m_position.y += ny;
}

void CollisionResolutionSystem::playerPowerUpCollision(const ECS::Entity player, const ECS::Entity power_up)
{
	auto& pUp_data = m_ecs->getComponent<PowerUpData>(power_up);
	pUp_data.player_entity = player;
}

// --------------------------------------------------------------------------------------------- //
//                             Enemy Collision Resolution Functions                              //
// --------------------------------------------------------------------------------------------- //

/* @brief enemy and player bullet collision resolution
*
* @param ECS::Entity -> enemy
* @param ECS::Entity -> player_bullet
*/
void CollisionResolutionSystem::enemyBulletCollision(const ECS::Entity enemy, const ECS::Entity player_bullet)
{
	delayedDestroyEntity(player_bullet);

	auto bullet_parent = m_ecs->getComponent<BulletParentComponent>(player_bullet);
	
	GameEvent g_ev_shot;
	g_ev_shot.m_event_type = GameEventType::GameEventType_1;
	g_ev_shot.m_player_id = bullet_parent.parent;
	m_event_bus->envoke(&g_ev_shot);

	const auto i = AudioManager::loadSoundEffect("Assets/ship_explosion.wav");
	auto* inst = AudioManager::createEffectInstance(i);

	if (AudioManager::playSound(inst, 0.5F))
	{
		printf("Sound effect %d played successfully\n", i);
	}

	if (m_ecs->hasComponent<HealthData>(enemy))
	{
		auto& health_data = m_ecs->getComponent<HealthData>(enemy);
		const auto& bullet_damage = m_ecs->getComponent<BulletDamageComponent>(player_bullet).m_damage;
		health_data.m_current_health -= bullet_damage;

		if (health_data.m_current_health <= 0)
		{
			// Enemy has been destroyed
			GameEvent g_ev_destroyed;
			g_ev_destroyed.m_event_type = GameEventType::GameEventType_3;
			g_ev_destroyed.m_player_id = bullet_parent.parent;
			g_ev_destroyed.m_enemy_id = enemy;

			m_event_bus->envoke(&g_ev_destroyed);
		}
	}
}

// --------------------------------------------------------------------------------------------- //
//                            Bullet Collision Resolution Functions                              //
// --------------------------------------------------------------------------------------------- //

/* @brief enemy bullet/player bullet and environment collision resolution
*
* @param ECS::Entity -> bullet
*/
void CollisionResolutionSystem::bulletEnvironmentCollision(const ECS::Entity bullet)
{
	delayedDestroyEntity(bullet);
}

// --------------------------------------------------------------------------------------------- //
//                                       Helper Functions                                        //
// --------------------------------------------------------------------------------------------- //

void CollisionResolutionSystem::delayedDestroyEntity(const ECS::Entity entity)
{
	auto itr = std::find(m_entities_to_destroy.cbegin(), m_entities_to_destroy.cend(), entity);

	if (itr == m_entities_to_destroy.cend())
	{
		m_entities_to_destroy.push_back(entity);
	}
}

void CollisionResolutionSystem::destroyEntities()
{
	if (!m_entities_to_destroy.empty())
	{
		for (auto& e : m_entities_to_destroy)
		{
			m_ecs->destroyEntity(e);
		}

		m_entities_to_destroy.clear();
	}
}

std::tuple<TagID, ECS::Entity, ECS::Entity> CollisionResolutionSystem::sortEntities
(const TagID compare, const ECS::Entity e_a, const ECS::Entity e_b)
{
	auto tag = m_ecs->getComponent<TagData>(e_a).tag;

	auto f = (tag == compare) ? e_a : e_b;
	auto s = (tag != compare) ? e_a : e_b;

	return std::tuple<TagID, ECS::Entity, ECS::Entity>(m_ecs->getComponent<TagData>(s).tag, f, s);
}

std::tuple<TagID, ECS::Entity, ECS::Entity> CollisionResolutionSystem::sortEntities(const TagID compare_a, const TagID compare_b, const ECS::Entity e_a, const ECS::Entity e_b)
{
	auto tag = m_ecs->getComponent<TagData>(e_a).tag;

	auto f = (tag == compare_a || tag == compare_b) ? e_a : e_b;
	auto s = (tag != compare_a || tag == compare_b) ? e_a : e_b;

	return std::tuple<TagID, ECS::Entity, ECS::Entity>(m_ecs->getComponent<TagData>(s).tag, f, s);
}
