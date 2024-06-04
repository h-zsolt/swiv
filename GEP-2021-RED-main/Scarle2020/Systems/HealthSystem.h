#pragma once

#include "ECS.h"

struct HealthData;

class HealthSystem : public ECS::System
{
public: // Public functions //
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);
private:
	std::vector<ECS::Entity> m_entities_to_destroy;
};
