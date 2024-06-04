#pragma once
#include "pch.h"
#include "ECS.h"

class FireControlSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);
private:
	void removeComponents(ECS::ECS& ecs);
	std::vector<ECS::Entity> m_entities_to_remove_from;
};