#pragma once
#include "pch.h"
#include "ECS.h"

class ADSpawnerSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);
private:
	void removeTags(ECS::ECS& ecs);
	std::vector<ECS::Entity> m_entities_to_remove;
};