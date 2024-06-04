#pragma once

#include "pch.h"
#include "ECS.h"
#include "IRenderer.h"
#include <Systems/InputSystem.h>

#include <Components/StagedBehaviourComponent.h>
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/HealthBarComponent.h>
#include <Components/ADSpawnerComponent.h>
#include <Data/RotationData.h>
#include <Data/BulletData.h>
#include <Data/TargetData.h>
#include <Data/Box2DColliderData.h>
#include <Data/TagData.h>
#include <Data/HealthData.h>
#include "Tags.h"
#include <Data/EnemySpawnerData.h>
#include <Data/EnvironmentTag.h>
#include <PowerUpData.h>
#include <Components/FiringModesComponent.h>


namespace EntityFactory
{
	static ECS::Entity spawnPlayer(ECS::ECS& ecs, IRenderer& renderer, size_t& sprite_id, const DirectX::SimpleMath::Vector2& sprite_scale, InputSystem& input, InputType player)
	{
		ECS::Entity entity = ecs.createEntity();

		TransformComponent transform;
		transform.m_position = {960.f, 540.f};

		MoveData move_data;
		move_data.speed = 500;

		BulletData bullet_data;
		bullet_data.bullet_speed        = 1000;
		bullet_data.rate_of_fire        = .25f;
		bullet_data.bullet_spawn_offset = 10.f;
		bullet_data.bullet_damage		= 1.0f;
		bullet_data.bullet_type         = TagID::P_BULLET;
		Bits::addToBitFlag(bullet_data.tag_ignore, TagID::PLAYER);

		SpriteComponent sprite;
		renderer.setupSpriteComponent(sprite_id, sprite);
		sprite.m_relative_scale = sprite_scale;
		if (player == InputType::PLAYER2) sprite.m_z_order = 0.1F;

		auto sprite_extent = renderer.calculateSpriteExtent(transform, sprite);
		Box2DColliderData collider{ transform.m_position, sprite_extent };
		
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::P_BULLET);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::PLAYER);

		if (player == InputType::PLAYER1)
		{
			Bits::addToBitFlag(collider.m_ignore_tags, TagID::ENVIRONMENT);
		}

		TagData tag{TagID::PLAYER};

		// return entity after adding components
		ecs.addComponent(entity, InputData(input.getBindings(player)));
		ecs.addComponent(entity, ButtonData());
		ecs.addComponent(entity, transform);
		ecs.addComponent(entity, move_data);
		ecs.addComponent(entity, bullet_data);
		ecs.addComponent(entity, sprite);
		ecs.addComponent(entity, collider);
		ecs.addComponent(entity, tag);


		return entity;
	}

	static std::tuple<ECS::Entity, TransformComponent> createBaseEntity(ECS::ECS& ecs, IRenderer& renderer, EnemySpawnerData& spawn_data, TransformComponent& parent, size_t& sprite_id)
	{
		ECS::Entity entity = ecs.createEntity();

		TransformComponent transform = parent;
		transform.m_position += spawn_data.relative_position;
		transform.m_rotation_radians = parent.m_rotation_radians + 3.14f;

		SpriteComponent sprite;
		renderer.setupSpriteComponent(sprite_id, sprite);

		auto sprite_extent = renderer.calculateSpriteExtent(transform, sprite);
		Box2DColliderData collider{ transform.m_position, sprite_extent };
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::E_BULLET);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::ENVIRONMENT);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::ENEMY);

		TagData tag{ TagID::ENEMY };
		
		ecs.addComponent(entity, transform);
		ecs.addComponent(entity, sprite);
		ecs.addComponent(entity, collider);
		ecs.addComponent(entity, tag);

		return std::tuple<ECS::Entity, TransformComponent> { entity, transform};
	}

	static void addTurretComponents(ECS::ECS& ecs, ECS::Entity& entity)
	{
		RotationData rotation_data;
		rotation_data.speed = 10.f;
		ecs.addComponent(entity, rotation_data);

		HealthData health_component;
		health_component.m_current_health = 3;
		health_component.m_max_health	  = 3;
		ecs.addComponent(entity, health_component);

		HealthBarComponent health_bar;
		health_bar.m_following = true;
		ecs.addComponent(entity, health_bar);

		BulletData bullet_data;
		bullet_data.is_fireing			= true;
		bullet_data.bullet_speed		= 500;
		bullet_data.rate_of_fire		= .5f;
		bullet_data.bullet_spawn_offset = 10.f;
		bullet_data.bullet_count		= 1;
		bullet_data.bullet_type			= TagID::E_BULLET;
		Bits::addToBitFlag(bullet_data.tag_ignore, TagID::P_BULLET);
		ecs.addComponent(entity, bullet_data);

		TargetData target_data;
		target_data.targeting_range = 1000;
		ecs.addComponent(entity, target_data);

		// add behaviours
		ecs.addComponent(entity, InRangeTriggerTag());
		ecs.addComponent(entity, RotateTowardTag());
	}

	static void addHeavyMissileComponents(ECS::ECS& ecs, ECS::Entity& entity)
	{
		RotationData rotation_data;
		rotation_data.speed = 2.f;
		ecs.addComponent(entity, rotation_data);

		MoveData move_data;
		move_data.speed = 50;
		ecs.addComponent(entity, move_data);

		BulletData bullet_data;
		bullet_data.is_fireing = true;
		bullet_data.bullet_speed = 300;
		bullet_data.rate_of_fire = 1.5f;
		bullet_data.bullet_spawn_offset = 10.f;
		bullet_data.bullet_count = 1;
		bullet_data.bullet_type = TagID::E_BULLET;
		bullet_data.cooldown_timer = 1.0F; //Don't shoot right away
		Bits::addToBitFlag(bullet_data.tag_ignore, TagID::P_BULLET);
		ecs.addComponent(entity, bullet_data);

		HealthData health_component;
		health_component.m_current_health = 5;
		health_component.m_max_health = 5;
		ecs.addComponent(entity, health_component);

		HealthBarComponent health_bar;
		health_bar.m_following = true;
		ecs.addComponent(entity, health_bar);

		TargetData target_data;
		target_data.targeting_range = 1000;
		ecs.addComponent(entity, target_data);



		// add behaviours
		/*StagedBehaviourComponent behaviour;
		behaviour.m_tags[0] = StagedBehaviour::MOVE_TOWARD_TAG;
		behaviour.m_tag_duration_timers[0] = 0.1F;
		behaviour.m_next_tag_timers[0] = 0.0F;
		behaviour.m_tags[1] = StagedBehaviour::ROTATE_TOWARD_TAG;
		behaviour.m_tag_duration_timers[1] = 0.1F;
		behaviour.m_next_tag_timers[1] = 1.0F;
		ecs.addComponent(entity, behaviour);
		This was used to test behaviour system on normal enemies*/
		ecs.addComponent(entity, MoveTowardTag());
		ecs.addComponent(entity, RotateTowardTag());

	}

	static void addMissileComponents(ECS::ECS& ecs, ECS::Entity& entity)
	{
		RotationData rotation_data;
		rotation_data.speed = 10.f;
		ecs.addComponent(entity, rotation_data);

		MoveData move_data;
		move_data.speed = 200;
		ecs.addComponent(entity, move_data);

		HealthData health_component;
		health_component.m_current_health = 1;
		health_component.m_max_health	  = 1;
		ecs.addComponent(entity, health_component);

		TargetData target_data;
		target_data.targeting_range = 1000;
		ecs.addComponent(entity, target_data);

		

		// add behaviours
		/*StagedBehaviourComponent behaviour;
		behaviour.m_tags[0] = StagedBehaviour::MOVE_TOWARD_TAG;
		behaviour.m_tag_duration_timers[0] = 0.1F;
		behaviour.m_next_tag_timers[0] = 0.0F;
		behaviour.m_tags[1] = StagedBehaviour::ROTATE_TOWARD_TAG;
		behaviour.m_tag_duration_timers[1] = 0.1F;
		behaviour.m_next_tag_timers[1] = 1.0F;
		ecs.addComponent(entity, behaviour);
		This was used to test behaviour system on normal enemies*/
		ecs.addComponent(entity, MoveTowardTag());
		ecs.addComponent(entity, RotateTowardTag());

	}

	static void addBossComponents(ECS::ECS& ecs, ECS::Entity entity, IRenderer& renderer)
	{
		HealthData health_component;
		health_component.m_current_health = 101;
		health_component.m_max_health = 101;
		ecs.addComponent(entity, health_component);

		ecs.addComponent(entity, WinConditionTag());

		HealthBarComponent health_bar;
		health_bar.m_following = false;
		health_bar.m_position = { 895, 75 }; //magic number, just a guess for now, need to figure out how to fit to resolution
		health_bar.m_size = { 250, 30 }; //10x normal size
		ecs.addComponent(entity, health_bar);

		BulletData bullet_data;
		bullet_data.is_fireing = false; //spreadshot pattern
		bullet_data.bullet_speed = 275;
		bullet_data.rate_of_fire = .35f;
		bullet_data.bullet_spawn_offset = 10.f;
		bullet_data.bullet_count = 5;
		bullet_data.bullet_angle = 1.2F;
		bullet_data.bullet_type = TagID::E_BULLET;
		Bits::addToBitFlag(bullet_data.tag_ignore, TagID::P_BULLET);
		ecs.addComponent(entity, bullet_data);

		FiringModesComponent fire_modes;
		fire_modes.m_bullet_speeds[0] = bullet_data.bullet_speed; //save current bullet pattern as 
		fire_modes.m_bullet_counts[0] = bullet_data.bullet_count;
		fire_modes.m_rates_of_fire[0] = bullet_data.rate_of_fire;
		fire_modes.m_bullet_angles[0] = bullet_data.bullet_angle;
		fire_modes.m_fire_instantly[0] = false;
		fire_modes.m_bullet_speeds[1] = 600; //machinegun pattern
		fire_modes.m_bullet_counts[1] = 1;
		fire_modes.m_rates_of_fire[1] = 0.1F;
		fire_modes.m_bullet_angles[1] = 0;
		fire_modes.m_fire_instantly[1] = false;
		fire_modes.m_bullet_speeds[2] = 400; //shotgun pattern
		fire_modes.m_bullet_counts[2] = 9;
		fire_modes.m_rates_of_fire[2] = 0.1F;
		fire_modes.m_bullet_angles[2] = 0.2F;
		fire_modes.m_fire_instantly[2] = false;
		fire_modes.m_bullet_speeds[3] = 300; //spread 2 pattern
		fire_modes.m_bullet_counts[3] = 7;
		fire_modes.m_rates_of_fire[3] = 0.3F;
		fire_modes.m_bullet_angles[3] = 1.0F;
		fire_modes.m_fire_instantly[3] = false;
		fire_modes.m_bullet_speeds[4] = 600; //machinegun 2 pattern
		fire_modes.m_bullet_counts[4] = 3;
		fire_modes.m_rates_of_fire[4] = 0.3F;
		fire_modes.m_bullet_angles[4] = 0.314F;
		fire_modes.m_fire_instantly[4] = false;
		ecs.addComponent(entity, fire_modes);
		
		enum GunType
		{
			SPREADSHOT = 0,
			MACHINEGUN,
			SHOTGUN,
			STRONG_SPREADSHOT,
			STRONG_MACHINEGUN
		};

		TargetData target_data;
		target_data.targeting_range = 1000;
		ecs.addComponent(entity, target_data);

		ADSpawnerComponent spawn_component;
		spawn_component.m_relative_positions[0] = { -350, 120 };
		spawn_component.m_spawn_code[0] = 0;
		spawn_component.m_spawn_counts[0] = 2;
		spawn_component.m_spawn_intervals[0] = 1.0F;
		spawn_component.m_types[0] = EnemyType::ENEMY2;
		spawn_component.m_spawn_points++;
		spawn_component.m_relative_positions[1] = { 350, 120 };
		spawn_component.m_spawn_code[1] = 0;
		spawn_component.m_spawn_counts[1] = 2;
		spawn_component.m_spawn_intervals[1] = 1.0F;
		spawn_component.m_types[1] = EnemyType::ENEMY2;
		spawn_component.m_spawn_points++;
		spawn_component.m_relative_positions[2] = { -350, 120 };
		spawn_component.m_spawn_code[2] = 1;
		spawn_component.m_spawn_counts[2] = 1;
		spawn_component.m_spawn_intervals[2] = 1.0F;
		spawn_component.m_types[2] = EnemyType::ENEMY3;
		spawn_component.m_spawn_points++;
		spawn_component.m_relative_positions[3] = { 350, 120 };
		spawn_component.m_spawn_code[3] = 1;
		spawn_component.m_spawn_counts[3] = 1;
		spawn_component.m_spawn_intervals[3] = 1.0F;
		spawn_component.m_types[3] = EnemyType::ENEMY3;
		spawn_component.m_spawn_points++;
		ecs.addComponent(entity, spawn_component);

		enum ADSpawners
		{
			WEAK_MISSILE = 0,
			HEAVY_MISSILE
		};

		StagedBehaviourComponent behaviours;
		auto stage_helper = StagedBehaviour::MAX_TAGS_PER_STAGE;
		behaviours.m_next_timer = 1.0F; //Wait 1 second before firing, gives players time to ready up
		//Stage 1
		behaviours.m_tags[0] = StagedBehaviour::TEMPORARY_FIRE_TAGS; //Behaviour 1 End (spreadshot)
		behaviours.m_tag_duration_timers[0] = 1.1F;
		behaviours.m_next_tag_timers[0] = 3.5F;
		behaviours.m_tags[1] = StagedBehaviour::CHANGE_FIRING_MODE; //Behaviour 2 Begin (machinegun)
		behaviours.m_tag_helper[1] = MACHINEGUN; 
		behaviours.m_next_tag_timers[1] = 0;
		behaviours.m_tags[2] = StagedBehaviour::ROTATE_TOWARD_TAG;
		behaviours.m_tag_duration_timers[2] = 1.0F;
		behaviours.m_next_tag_timers[2] = 0;
		behaviours.m_tags[3] = StagedBehaviour::TEMPORARY_FIRE_TAGS; //Behaviour 2 End (machinegun)
		behaviours.m_tag_duration_timers[3] = 1.0F;
		behaviours.m_next_tag_timers[3] = 2.5F;
		behaviours.m_tags[4] = StagedBehaviour::SPAWN_ADS_TAG; //Behaviour 3 Begin and End (spawn weak ads)
		behaviours.m_tag_helper[4] = WEAK_MISSILE; 
		behaviours.m_next_tag_timers[4] = 2.5F;
		behaviours.m_tags[5] = StagedBehaviour::RESET_ROTATION; //Behaviour 1 Begin (spreadshot)
		behaviours.m_tag_duration_timers[5] = 0;
		behaviours.m_next_tag_timers[5] = 0;
		behaviours.m_tags[6] = StagedBehaviour::CHANGE_FIRING_MODE;
		behaviours.m_tag_helper[6] = SPREADSHOT;
		behaviours.m_next_tag_timers[6] = 0;
		//Stage 2
		behaviours.m_health_triggers[0] = 50;
		behaviours.m_tags[stage_helper * 1 + 0] = StagedBehaviour::SPAWN_ADS_TAG; //Behaviour 1 Begin and End
		behaviours.m_tag_helper[stage_helper * 1 + 0] = HEAVY_MISSILE;
		behaviours.m_next_tag_timers[stage_helper * 1 + 0] = 3.0F;
		behaviours.m_tags[stage_helper * 1 + 1] = StagedBehaviour::CHANGE_FIRING_MODE; //Behaviour 2 Begin
		behaviours.m_tag_helper[stage_helper * 1 + 1] = SHOTGUN;
		behaviours.m_next_tag_timers[stage_helper * 1 + 1] = 0;
		behaviours.m_tags[stage_helper * 1 + 2] = StagedBehaviour::ROTATE_TOWARD_TAG;
		behaviours.m_tag_duration_timers[stage_helper * 1 + 2] = 1.0F;
		behaviours.m_next_tag_timers[stage_helper * 1 + 2] = 1.0F;
		behaviours.m_tags[stage_helper * 1 + 3] = StagedBehaviour::TEMPORARY_FIRE_TAGS; //Behaviour 2 End
		behaviours.m_tag_duration_timers[stage_helper * 1 + 3] = 0.5F;
		behaviours.m_next_tag_timers[stage_helper * 1 + 3] = 0.5F;
		behaviours.m_tags[stage_helper * 1 + 4] = StagedBehaviour::RESET_ROTATION; //Behaviour 3 Begin
		behaviours.m_tag_duration_timers[stage_helper * 1 + 4] = 0;
		behaviours.m_next_tag_timers[stage_helper * 1 + 4] = 2.0F;
		behaviours.m_tags[stage_helper * 1 + 5] = StagedBehaviour::CHANGE_FIRING_MODE; 
		behaviours.m_tag_helper[stage_helper * 1 + 5] = STRONG_SPREADSHOT;
		behaviours.m_next_tag_timers[stage_helper * 1 + 5] = 0;
		behaviours.m_tags[stage_helper * 1 + 6] = StagedBehaviour::TEMPORARY_FIRE_TAGS; //Behaviour 3 End
		behaviours.m_tag_duration_timers[stage_helper * 1 + 6] = 1.6F;
		behaviours.m_next_tag_timers[stage_helper * 1 + 6] = 3.5F;
		behaviours.m_tags[stage_helper * 1 + 7] = StagedBehaviour::CHANGE_FIRING_MODE; //Behaviour 4 Begin 
		behaviours.m_tag_helper[stage_helper * 1 + 7] = STRONG_MACHINEGUN;
		behaviours.m_next_tag_timers[stage_helper * 1 + 7] = 0;
		behaviours.m_tags[stage_helper * 1 + 8] = StagedBehaviour::ROTATE_TOWARD_TAG;
		behaviours.m_tag_duration_timers[stage_helper * 1 + 8] = 1.6F;
		behaviours.m_next_tag_timers[stage_helper * 1 + 8] = 0;
		behaviours.m_tags[stage_helper * 1 + 9] = StagedBehaviour::TEMPORARY_FIRE_TAGS; //Behaviour 4 End
		behaviours.m_tag_duration_timers[stage_helper * 1 + 9] = 1.6F;
		behaviours.m_next_tag_timers[stage_helper * 1 + 9] = 3.5F;
		ecs.addComponent(entity, behaviours);
	}

	static ECS::Entity spawnEnemy(ECS::ECS& ecs, IRenderer& renderer, EnemySpawnerData& spawn_data, TransformComponent& parent, size_t& sprite_id)
	{
		auto [entity, transform] = createBaseEntity(ecs, renderer, spawn_data, parent, sprite_id);

		switch (spawn_data.type)
		{
			case EnemyType::ENEMY1:
				addTurretComponents(ecs, entity);
				break;
			case EnemyType::ENEMY2:
				addMissileComponents(ecs, entity);
				break;
			case EnemyType::ENEMY3:
				addHeavyMissileComponents(ecs, entity);
				break;
			case EnemyType::ENEMY6:
				addBossComponents(ecs, entity, renderer);
				break;
			default :
				break;
		}

		// return entity after ading components
		return entity;
	}

	static void spawnEnvironment(ECS::ECS& ecs, IRenderer& renderer, const ECS::Entity entity,const TransformComponent& transform, const SpriteComponent& sprite)
	{
		auto sprite_extents = renderer.calculateSpriteExtent(transform, sprite);
		Box2DColliderData collider{ transform.m_position, sprite_extents };
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::ENEMY);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::P_BULLET);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::E_BULLET);

		TagData tag{ TagID::ENVIRONMENT };

		ecs.addComponent<Box2DColliderData>(entity, collider);
		ecs.addComponent<TagData>(entity, tag);
	}

	static void spawnPowerUp(ECS::ECS& ecs, IRenderer& renderer, TransformComponent& parent, size_t& sprite_id)
	{
		ECS::Entity entity = ecs.createEntity();

		TransformComponent transform = TransformComponent();
		transform.m_position = parent.m_position;

		SpriteComponent sprite;
		renderer.setupSpriteComponent(sprite_id, sprite);
		sprite.m_z_order = 0.1F;

		auto sprite_extent = renderer.calculateSpriteExtent(transform, sprite);
		Box2DColliderData collider{ transform.m_position, sprite_extent };
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::E_BULLET);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::ENVIRONMENT);
		Bits::addToBitFlag(collider.m_ignore_tags, TagID::ENEMY);

		TagData tag{ TagID::POWER_UP };

		auto pUp_data = PowerUpData();
		pUp_data.type = PowerUpType::FIRE_RATE;
		ecs.addComponent(entity, pUp_data);

		ecs.addComponent(entity, transform);
		ecs.addComponent(entity, sprite);
		ecs.addComponent(entity, collider);
		ecs.addComponent(entity, tag);
	}
};
