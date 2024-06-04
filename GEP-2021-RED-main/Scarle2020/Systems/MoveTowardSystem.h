#pragma once

#include "pch.h"
#include "ECS.h"

class MoveTowardSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);
};

