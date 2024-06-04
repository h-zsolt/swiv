#include "pch.h"
#include "FiringModeSystem.h"
#include "Tags.h"
#include "Components/FiringModesComponent.h"
#include "Data/BulletData.h"

void FiringModeSystem::init(ECS::ECS& ecs)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<ChangeFireModeTag>());
	ecs.setSystemSignature<FiringModeSystem>(sig);
}

void FiringModeSystem::update(ECS::ECS& ecs)
{
	for (auto& entity : m_entities)
	{
		if (ecs.hasComponent<FiringModesComponent>(entity) && ecs.hasComponent<BulletData>(entity))
		{
			auto new_mode_id = ecs.getComponent<ChangeFireModeTag>(entity).m_mode;
			auto& firing_modes = ecs.getComponent<FiringModesComponent>(entity);
			auto& current_mode = ecs.getComponent<BulletData>(entity);
			if (new_mode_id < StagedBehaviour::MAX_FIRING_MODES) //confirm it's valid, so there's no OOB err
			{
				if (firing_modes.m_fire_instantly[new_mode_id])
					current_mode.cooldown_timer = 0;
				if (firing_modes.m_rates_of_fire[new_mode_id] != 0) //0 means leave unchanged as 0 doesn't make sense for most of these values
					current_mode.rate_of_fire = firing_modes.m_rates_of_fire[new_mode_id]; //the mode's values are set, could add a bool to see if the new mode is designed as a setter (=) or changer (+=)
				if (firing_modes.m_bullet_speeds[new_mode_id] != 0)
					current_mode.bullet_speed = firing_modes.m_bullet_speeds[new_mode_id];
				if (firing_modes.m_bullet_damages[new_mode_id] != 0)
					current_mode.bullet_damage = firing_modes.m_bullet_damages[new_mode_id];
				if (firing_modes.m_bullet_counts[new_mode_id] != 0)
					current_mode.bullet_count = firing_modes.m_bullet_counts[new_mode_id];
				if (firing_modes.m_bullet_angles[new_mode_id] != 0)
					current_mode.bullet_angle = firing_modes.m_bullet_angles[new_mode_id];
			}
			m_entities_to_remove_from.push_back(entity);
		}
	}
	removeComponents(ecs);
}

void FiringModeSystem::removeComponents(ECS::ECS& ecs)
{
	for (auto& entity : m_entities_to_remove_from)
	{
		ecs.removeComponent<ChangeFireModeTag>(entity);
	}
	m_entities_to_remove_from.clear();
}
