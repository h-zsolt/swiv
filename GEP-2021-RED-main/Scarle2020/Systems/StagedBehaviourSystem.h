#pragma once
#include "pch.h"
#include "ECS.h"

class StagedBehaviourSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs, const float dt);
private:
	void addTag(ECS::ECS& ecs, ECS::Entity entity, unsigned target_behaviour, unsigned behaviour_helper);
	void removeTag(ECS::ECS& ecs, ECS::Entity entity, unsigned target_behaviour, unsigned behaviour_helper);
};