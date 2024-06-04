#pragma once

#include "pch.h"
#include "BehaviourDefinitions.h"
#include "ECSDefinitions.h"

struct FixedShootingPointComponent
{
	FixedShootingPointComponent()
	{
		for (int i = 0; i < StagedBehaviour::MAX_ADD_SHOOTING_POINTS; i++)
		{
			m_relative_positions[i] = { 0,0 };
			//m_inherit_all[i] = true;
			m_inherit_only[i] = 0;
			m_listen_to_commands[i] = true;
			m_rotate_towards_tag[i] = false;
			m_fire_on_code[i] = 0;
		}
	}
	bool m_spawned = false;
	unsigned m_spawn_count = 0;
	ECS::Entity m_spawned_IDs[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {};
	DirectX::SimpleMath::Vector2 m_relative_positions[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {};
	//bool m_inherit_all[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {}; //inherits all or only one, perhaps expand to include a pick-and-choose option?
	unsigned m_inherit_only[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {}; //design changed to always inherit only one
	bool m_listen_to_commands[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {};
	bool m_rotate_towards_tag[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {};
	//bool m_support_rotation[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {}; would be cool, but lots of sin and cos calculations; gon leave it for now
	//this would also work better if sprites for these would be implemented, but a program is never feature complete

	unsigned m_fire_on_code[StagedBehaviour::MAX_ADD_SHOOTING_POINTS] = {}; //design changed, tag now selects which fixed points shoot rather than telling them to switch firing mode
};
