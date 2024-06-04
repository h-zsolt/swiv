#pragma once
#include "pch.h"
#include "BehaviourDefinitions.h"

struct FiringModesComponent
{
	FiringModesComponent()
	{
		for (int i = 0; i < StagedBehaviour::MAX_FIRING_MODES; i++)
		{
			m_fire_instantly[i] = false;
			m_rates_of_fire[i] = 0;
			m_bullet_speeds[i] = 0;
			m_bullet_damages[i] = 0;
			m_bullet_counts[i] = 0;
			m_bullet_angles[i] = 0;
		}
	}
	bool m_fire_instantly[StagedBehaviour::MAX_FIRING_MODES] = {};
	float m_rates_of_fire[StagedBehaviour::MAX_FIRING_MODES] = {};
	float m_bullet_speeds[StagedBehaviour::MAX_FIRING_MODES] = {};
	float m_bullet_damages[StagedBehaviour::MAX_FIRING_MODES] = {};
	unsigned m_bullet_counts[StagedBehaviour::MAX_FIRING_MODES] = {};
	float m_bullet_angles[StagedBehaviour::MAX_FIRING_MODES] = {};
};