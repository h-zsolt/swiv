#pragma once

#include "ECS.h"
#include <Data/TagData.h>
#include "EventBus.h"

struct CollisionEvent;
struct BoundaryEvent;
struct EnvironmentCollisionEvent;

class EventBus;

class CollisionResolutionSystem : public ECS::System
{
private: // TypeDefs //
	using Vector2 = DirectX::SimpleMath::Vector2;

public: // Constructor & Destructor //
	CollisionResolutionSystem()  = default;
	~CollisionResolutionSystem() = default;

public: // Public Functions //
	void init(ECS::ECS& ecs, EventBus* event_bus);
	void update();

private: // Private Functions //
	// Events //
	void onCollisionEvent(CollisionEvent* _event);
	void onBoundaryCollision(BoundaryEvent* _event);

	// Collision Sorting Functions //
	void playerCollisionSort(CollisionEvent* e);
	void enemyCollisionSort(CollisionEvent* e);
	void bulletCollisionSort(CollisionEvent* e);

	// Player Collision Resolutions //
	void playerBulletCollision(const ECS::Entity player, const ECS::Entity enemy_bullet);
	void playerEnemyCollision(const ECS::Entity player, const ECS::Entity enemy);
	void playerEnvironmentCollision(const ECS::Entity player, const ECS::Entity envi, 
									const Vector2& axis, const float overlap, const float dt);
	void playerPowerUpCollision(const ECS::Entity player, const ECS::Entity power_up);


	// Enemy Collision Resolutions //
	void enemyBulletCollision(const ECS::Entity enemy, const ECS::Entity player_bullet);

	// Bullet Collision Resolutions //
	void bulletEnvironmentCollision(const ECS::Entity bullet);

	// Helper Functions //
	void delayedDestroyEntity(const ECS::Entity entity);
	void destroyEntities();
	std::tuple<TagID, ECS::Entity, ECS::Entity> sortEntities(const TagID compare, const ECS::Entity e_a, const ECS::Entity e_b);
	std::tuple<TagID, ECS::Entity, ECS::Entity> sortEntities(const TagID compare_a, const TagID compare_b, const ECS::Entity e_a, const ECS::Entity e_b);

private: // Variables //
	ECS::ECS* m_ecs		  = nullptr;
	EventBus* m_event_bus = nullptr;

	std::vector<ECS::Entity> m_entities_to_destroy = {};
};
