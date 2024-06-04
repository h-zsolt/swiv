#pragma once

#include "pch.h"
#include "BehaviourDefinitions.h"
#include "Data/EnemySpawnerData.h"

struct ADSpawnerComponent
{
	ADSpawnerComponent()
	{
		for (int i = 0; i < StagedBehaviour::MAX_AD_ENEMY_SPAWNERS; i++)
		{
			m_relative_positions[i] = { 0,0 };
			m_types[i] = EnemyType::ENEMY1;
			m_spawn_counts[i] = 0;
			m_spawn_intervals[i] = .0f;
			m_spawn_code[i] = 0;
		}
	}
	unsigned m_spawn_points = 0;
	//bool m_fixed_positions[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] - easy to implement in system, but serves no purpose for current project
	DirectX::SimpleMath::Vector2 m_relative_positions[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] = {};
	EnemyType m_types[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] = {};
	int m_spawn_counts[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] = {};
	float m_spawn_intervals[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] = {};
	int m_spawn_code[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] = {};
	//float m_initial_delay[StagedBehaviour::MAX_AD_ENEMY_SPAWNERS] - can be replaced by activating the spawn at the "remove tag" event of staged behaviour
};