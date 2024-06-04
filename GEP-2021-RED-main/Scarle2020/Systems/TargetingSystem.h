#pragma once

#include "pch.h"
#include "ECS.h"

class TargetingSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs, std::vector<ECS::Entity>& players);
};

