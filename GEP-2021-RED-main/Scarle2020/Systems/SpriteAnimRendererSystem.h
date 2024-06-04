#pragma once

#include "System.h"

// Forward declarations.
namespace ECS
{
	class ECS;
}

class SpriteRenderSystem;

// Class.
class SpriteAnimRendererSystem : public ECS::System
{
public:
	/** Registers the system signature with the ecs. */
	void init(ECS::ECS& ecs);

	/** Increments the tracked elapsed time and updates animation frames. */
	void tick(float dt, ECS::ECS& ecs);

	/** Renders all sprite components in the sprite batch. This expects the sprite batch used to have already had Begin() called. */
	void render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, 
		const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures, size_t available_textures);

private:
	float m_elapsed_time = 0.f;
};
