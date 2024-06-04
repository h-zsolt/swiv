#pragma once

#include "IEvent.h"
#include "ECSDefinitions.h"

struct BoundaryEvent : public IEvent
{
	explicit BoundaryEvent(ECS::Entity _entity, int _boundary, float delta_time, float _diff) : entity(_entity), boundary(_boundary),
		dt(delta_time), diff(_diff) {}

	ECS::Entity entity = ECS::INVALID_ENTITY;
	int boundary	   = 0;
	float dt		   = 0.F;
	float diff		   = 0.F;
};
