#include "pch.h"
#include "FixedShootingPointSystem.h"
#include "Components/FixedShootingPointComponent.h"
#include "Components/BoundComponent.h"
#include "Components/TransformComponent.h"
#include "Components/FiringModesComponent.h"
#include "Data/BulletData.h"
#include "Tags.h"

void FixedShootingPointSystem::init(ECS::ECS& ecs)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<FixedShootingPointComponent>());
	ecs.setSystemSignature<FixedShootingPointSystem>(sig);
}

void FixedShootingPointSystem::update(ECS::ECS& ecs)
{
	for (auto& entity : m_entities)
	{
		auto& shooting_points = ecs.getComponent<FixedShootingPointComponent>(entity);
		unsigned firing_code = 0;
		bool start_fire = false; //bools are used so it doesn't stop at the first one, checks all
		bool stop_fire = false; //workaround of removing the tags at the end would be harder to understand, for me at least
		
		//check if the parent has fire control tags
		if (ecs.hasComponent<StartFixedPointShootingTag>(entity))
		{
			firing_code = ecs.getComponent<StartFixedPointShootingTag>(entity).m_mode;
			start_fire = true;
			ecs.removeComponent<StartFixedPointShootingTag>(entity);
		}
		if (ecs.hasComponent<StopFixedPointShootingTag>(entity))
		{
			stop_fire = true;
			ecs.removeComponent<StopFixedPointShootingTag>(entity);
		}

		if (shooting_points.m_spawned)
		{
			//loop through the spawned entities
			for (int i = 0; i < shooting_points.m_spawn_count; i++)
			{
				//Reposition
				auto& parent_pos = ecs.getComponent<TransformComponent>(entity);
				auto& child_pos = ecs.getComponent<TransformComponent>(shooting_points.m_spawned_IDs[i]);
				auto& distance = shooting_points.m_relative_positions[i];
				child_pos.m_position = parent_pos.m_position + distance;
				
				//FireControl
				if (stop_fire) //currently stops all, but it would be easy to implement a code based fire control like below
				{
					ecs.addComponent(shooting_points.m_spawned_IDs[i], HoldFireTag());
				}

				if (start_fire && firing_code == shooting_points.m_fire_on_code[i])
				{
					ecs.addComponent(shooting_points.m_spawned_IDs[i], StartFireTag());
				}
			}
		}
		else
		{
			if (ecs.hasComponent<TransformComponent>(entity) && ecs.hasComponent<BulletData>(entity) && ecs.hasComponent<FiringModesComponent>(entity)) //due to old design still tightly coupled to FiringModes
			{
				//extract into a function when finished "spawnFixedPoints(&entity, &shooting_component)"
				shooting_points.m_spawned = true;
				if (!ecs.hasComponent<BoundComponent>(entity))
				{
					ecs.addComponent(entity, BoundComponent{});
				}
				//Spawn the shooting points
				for (int i = 0; i < shooting_points.m_spawn_count; i++)
				{
					auto& parent_bullet = ecs.getComponent<BulletData>(entity);
					auto& parent_firemodes = ecs.getComponent<FiringModesComponent>(entity);
					ECS::Entity new_shooting_point = ecs.createEntity();
					shooting_points.m_spawned_IDs[i] = new_shooting_point;

					TransformComponent point_pos;
					//point_pos.m_position = {}; don't think there's a need to set it, linking the component to entity should be enough, as repositition happens one tick away anyways
					//a proper fix would b to check if they aren't spawned, then run the current if(spawned) stuff without a check, guaranteed to have spawned then
					ecs.addComponent(new_shooting_point, point_pos);

					BulletData shooting_point_bullet;
					//inherit angles
					if (parent_firemodes.m_bullet_angles[shooting_points.m_inherit_only[i]] == 0) //this still leaves a bug
					{
						shooting_point_bullet.bullet_angle = parent_bullet.bullet_angle; //this should check the previous fire mode until it runs out of options, same for the checks below
					}
					else
					{
						shooting_point_bullet.bullet_angle = parent_firemodes.m_bullet_angles[shooting_points.m_inherit_only[i]];
					}
					//inherit bullet count
					if (parent_firemodes.m_bullet_counts[shooting_points.m_inherit_only[i]] == 0)
					{
						shooting_point_bullet.bullet_count = parent_bullet.bullet_count;
					}
					else
					{
						shooting_point_bullet.bullet_count = parent_firemodes.m_bullet_counts[shooting_points.m_inherit_only[i]];
					}
					//inherit bullet speed
					if (parent_firemodes.m_bullet_speeds[shooting_points.m_inherit_only[i]] == 0)
					{
						shooting_point_bullet.bullet_speed = parent_bullet.bullet_speed;
					}
					else
					{
						shooting_point_bullet.bullet_speed= parent_firemodes.m_bullet_speeds[shooting_points.m_inherit_only[i]];
					}
					//inherit rate of fire
					if (parent_firemodes.m_rates_of_fire[shooting_points.m_inherit_only[i]] == 0)
					{
						shooting_point_bullet.rate_of_fire = parent_bullet.rate_of_fire;
					}
					else
					{
						shooting_point_bullet.rate_of_fire = parent_firemodes.m_rates_of_fire[shooting_points.m_inherit_only[i]];
					}
					shooting_point_bullet.bullet_damage = parent_bullet.bullet_damage; //think this is obsolete in current build, not bothering
					shooting_point_bullet.bullet_spawn_offset = parent_bullet.bullet_spawn_offset; //this might not need to be inherited at all
					shooting_point_bullet.bullet_type = parent_bullet.bullet_type; //FireMode does not currently support changing this
					shooting_point_bullet.is_fireing = false; //always false, require the tags to start and stop firing
					shooting_point_bullet.tag_ignore = parent_bullet.tag_ignore; //FireMode does not currently support changing this
					ecs.addComponent(entity, shooting_point_bullet);

					if (shooting_points.m_rotate_towards_tag)
					{
						ecs.addComponent(entity, RotateTowardTag());
					}

					ecs.getComponent<BoundComponent>(entity).m_entities.push_back(new_shooting_point);
				}
			}
			else
			{
				//Oops, components missing error or w/e
			}
		}
	}
}
