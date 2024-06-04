#pragma once

#include "pch.h"
#include "ECS.h"
#include <Data/BulletData.h>
#include <Data/RotationData.h>
#include <Components/TransformComponent.h>
#include <Components/BulletParentComponent.h>

class IRenderer;

class BulletSpawnerSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs, size_t& sprite_id);
	void update(ECS::ECS& ecs, IRenderer& renderer, float& delta_time);

private:
	void spawnBullet(ECS::ECS& ecs, BulletData& bullet_data, TransformComponent& origin, IRenderer& renderer, ECS::Entity parent);
	
private:
	size_t m_sprite_id = 0;
};


