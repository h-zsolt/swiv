#include "pch.h"
#include "BulletSpawnerSystem.h"
#include <Data/MoveData.h>
#include <Components/SpriteComponent.h>
#include "IRenderer.h"
#include <Data/LifetimeData.h>
#include <Data/Box2DColliderData.h>
#include <Data/TagData.h>
#include <Components/BulletParentComponent.h>
#include <Components/BulletDamageComponent.h>

void BulletSpawnerSystem::init(ECS::ECS &ecs, size_t &sprite_id)
{
	// Create Signiture
	ECS::Signature psSig;
	psSig.set(ecs.getComponentBitflag<TransformComponent>());
	psSig.set(ecs.getComponentBitflag<BulletData>());
	ecs.setSystemSignature<BulletSpawnerSystem>(psSig);

	// Keep the sprite id stored
	m_sprite_id = sprite_id;
}

void BulletSpawnerSystem::update(ECS::ECS &ecs, IRenderer &renderer, float &delta_time)
{
    for (auto& entity : m_entities)
    {
        auto& bullet_data = ecs.getComponent<BulletData>(entity);
        //bullet_data.cooldown_timer -= delta_time;

		if (bullet_data.cooldown_timer > 0.F)
		{
			bullet_data.cooldown_timer -= delta_time;
			continue;
		}

		if (!bullet_data.is_fireing)
		{
            continue;
		}

        auto &transform = ecs.getComponent<TransformComponent>(entity);
        if (bullet_data.bullet_count % 2) //if odd number, shoot one without changing the rotations
        {
            spawnBullet(ecs, bullet_data, transform, renderer, entity);
        }
        for (unsigned i = 1; i <= bullet_data.bullet_count / 2; i++) //starts by spawning the furthest out bullets, going towards the inside (when the pattern is "flower-like", from the outer petals to the inner ones)
        {
            auto pos_transform = transform;
            pos_transform.m_rotation_radians += bullet_data.bullet_angle * (float)i / (bullet_data.bullet_count / 2);
            auto neg_transform = transform;
            neg_transform.m_rotation_radians -= bullet_data.bullet_angle * (float)i / (bullet_data.bullet_count / 2);
            spawnBullet(ecs, bullet_data, pos_transform, renderer, entity);
            spawnBullet(ecs, bullet_data, neg_transform, renderer, entity);
        }
        bullet_data.cooldown_timer = bullet_data.rate_of_fire;
    }
}

void BulletSpawnerSystem::spawnBullet(ECS::ECS& ecs, BulletData& bullet_data, TransformComponent& origin,
                                      IRenderer& renderer, ECS::Entity parent)
{
	auto bullet = ecs.createEntity();

    BulletParentComponent bullet_parent;
    bullet_parent.parent = parent;
    ecs.addComponent(bullet, bullet_parent);

    BulletDamageComponent damage;
    damage.m_damage = bullet_data.bullet_damage;
    ecs.addComponent(bullet, damage);

    MoveData move_data;
    move_data.direction.y = -cos(origin.m_rotation_radians);
    move_data.direction.x =  sin(origin.m_rotation_radians);
    move_data.speed = bullet_data.bullet_speed;
    ecs.addComponent(bullet, move_data);

    TransformComponent transform;
    transform.m_position = origin.m_position + ( move_data.direction * bullet_data.bullet_spawn_offset);
    transform.m_rotation_radians = origin.m_rotation_radians;
    ecs.addComponent(bullet, transform);

    /// Need to add the renderer include for this to work
    SpriteComponent sprite;
    renderer.setupSpriteComponent(m_sprite_id, sprite);
    sprite.m_z_order = 0.2;
    ecs.addComponent(bullet, sprite);

    LifetimeData lifetime_data;
    lifetime_data.time = 3.f;
    ecs.addComponent(bullet, lifetime_data);

    // Temporary setup until bullet size is finalized
    auto extents = renderer.calculateSpriteExtent(transform, sprite);
    Box2DColliderData collider{transform.m_position, extents};
    collider.m_ignore_tags = bullet_data.tag_ignore;
    ecs.addComponent(bullet, collider);

    TagData tag{ bullet_data.bullet_type };
    ecs.addComponent(bullet, tag);
}
