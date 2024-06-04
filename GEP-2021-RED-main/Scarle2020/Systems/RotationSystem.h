#pragma once

#include "pch.h"
#include "ECS.h"

class RotationSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs, float& delta_time);

private:
};

