#include "pch.h"
#include "SpriteRendererSystem.h"
#include "ECS.h"
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>

void SpriteRendererSystem::init(ECS::ECS& ecs)
{
	// Register signature for system.
	ECS::Signature renderer_sig;
	renderer_sig.set(ecs.getComponentBitflag<TransformComponent>());
	renderer_sig.set(ecs.getComponentBitflag<SpriteComponent>());
	ecs.setSystemSignature<SpriteRendererSystem>(renderer_sig);
}

void SpriteRendererSystem::render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures, size_t available_textures)
{
#ifdef _DEBUG
	if (textures.size() == 0)
	{
		std::cout << "Warning: No textures are loaded." << std::endl;
	}
#endif

	for (const auto& entity : m_entities)
	{
		auto& sprite = ecs.getComponent<SpriteComponent>(entity);
		if (!sprite.m_render)
		{
			continue;
		}

		if (sprite.m_texture_id < 0)
		{
			std::cout << "Error: Invalid texture id with SpriteComponent::m_texture_id: " << sprite.m_texture_id << std::endl;
			continue;
		}
		auto texture_id = static_cast<size_t>(sprite.m_texture_id);
		if (texture_id >= available_textures)
		{
			std::cout << "Error: Invalid texture id with SpriteComponent::m_texture_id: " << sprite.m_texture_id << std::endl;
			continue;
		}
		const auto& transform = ecs.getComponent<TransformComponent>(entity);

		sprite.m_z_order = std::clamp(sprite.m_z_order, 0.0f, 1.0f);

		sprite_batch->Draw(textures[texture_id].Get(),
			transform.m_position + sprite.m_relative_position,
			&sprite.m_source_rect,
			sprite.m_colour,
			transform.m_rotation_radians + sprite.m_relative_rotation_radians,
			sprite.m_origin,
			transform.m_scale * sprite.m_relative_scale,
			DirectX::SpriteEffects::SpriteEffects_None,
			sprite.m_z_order
		);
	}
}
