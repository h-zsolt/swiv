#include "pch.h"
#include "HUDImageRendererSystem.h"
#include "ECS.h"
#include <Components/TransformComponent.h>
#include <Components/HUDImageComponent.h>

void HUDImageRendererSystem::init(ECS::ECS& ecs)
{
	// Register signature for system.
	ECS::Signature renderer_sig;
	renderer_sig.set(ecs.getComponentBitflag<TransformComponent>());
	renderer_sig.set(ecs.getComponentBitflag<HUDImageComponent>());
	ecs.setSystemSignature<HUDImageRendererSystem>(renderer_sig);
}

void HUDImageRendererSystem::render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures, size_t available_textures)
{
#ifdef _DEBUG
	if (textures.size() == 0)
	{
		std::cout << "Warning: No textures are loaded." << std::endl;
	}
#endif

	for (const auto& entity : m_entities)
	{
		auto& image = ecs.getComponent<HUDImageComponent>(entity);
		if (image.m_texture_id < 0)
		{
			std::cout << "Error: Invalid texture id with HUDImageComponent::m_texture_id: " << image.m_texture_id << std::endl;
			continue;
		}
		auto texture_id = static_cast<size_t>(image.m_texture_id);
		if (texture_id >= available_textures)
		{
			std::cout << "Error: Invalid texture id with HUDImageComponent::m_texture_id: " << image.m_texture_id << std::endl;
			continue;
		}
		const auto& transform = ecs.getComponent<TransformComponent>(entity);

		image.m_z_order = std::clamp(image.m_z_order, 0.0f, 1.0f);

		sprite_batch->Draw(textures[texture_id].Get(),
			transform.m_position + image.m_relative_position,
			nullptr,
			image.m_colour,
			transform.m_rotation_radians + image.m_relative_rotation_radians,
			image.m_origin,
			transform.m_scale * image.m_relative_scale,
			DirectX::SpriteEffects::SpriteEffects_None,
			image.m_z_order
		);
	}
}
