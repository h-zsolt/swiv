#pragma once
#include "pch.h"
#include "ECS.h"

class InRangeTriggerSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs);

private:

};

