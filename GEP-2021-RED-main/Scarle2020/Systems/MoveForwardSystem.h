#pragma once

#include "pch.h"
#include "ECS.h"

class MoveForwardSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);
};
