#pragma once

#include "pch.h"

struct HealthData
{
	int m_current_health;

	// currently unused, could be moved to a different component
	int m_max_health;
	int m_regen_amount;
	float m_regen_timer;
	float m_regen_delay;
};
