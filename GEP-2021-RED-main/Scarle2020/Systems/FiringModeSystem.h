#pragma once
#include "pch.h"
#include "BehaviourDefinitions.h"
#include "ECS.h"

class FiringModeSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);
private:
	void removeComponents(ECS::ECS& ecs);
	std::vector<ECS::Entity> m_entities_to_remove_from;
};