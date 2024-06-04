#pragma once

#include "State.h"

class ExampleState : public State
{
public:
	ExampleState() = default;
	~ExampleState() override = default;

	void start() override;
	void updateObjects(GameData* gd) override;
	void end() override;

private:
	std::vector<ECS::Entity> m_player_entity_list = {};

	const float M_CAMERA_SCROLL_SPEED = 25.f;
};	


