#pragma once

#include "System.h"

namespace ECS
{
	class ECS;
}

class PhysicsSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs, float delta_time);
};
