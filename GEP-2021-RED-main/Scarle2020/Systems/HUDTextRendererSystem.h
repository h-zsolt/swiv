#pragma once

#include "System.h"

namespace ECS
{
	class ECS;
}

class HUDTextRendererSystem : public ECS::System
{
public:
	/** Registers system signature with the ecs. */
	void init(ECS::ECS& ecs);

	/** Renders all text inside the sprite batch. This expects the sprite batch used to have already had Begin() called. */
	void render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, const std::vector<std::unique_ptr<DirectX::SpriteFont>>& fonts, size_t available_fonts);
};