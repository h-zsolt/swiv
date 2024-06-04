#pragma once
#include "pch.h"
#include "ECS.h"

enum
{
	FULL,
	FEEDBACK,
	CURRENT
};

class IRenderer;

class HealthBarSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs, IRenderer* renderer);
	void update(ECS::ECS& ecs, const float dt, const float camera_y);
	void setTextureIDs(size_t full, size_t feedback, size_t current);
private:
	std::array<size_t, 3> m_texture_ids = {0};
	const float GAP = -10.0F;
	IRenderer* m_renderer = nullptr;
};