#include "pch.h"
#include "HUDTextRendererSystem.h"
#include "ECS.h"
#include <Components/TransformComponent.h>
#include <Components/HUDTextComponent.h>

void HUDTextRendererSystem::init(ECS::ECS& ecs)
{
	// Register signature for system.
	ECS::Signature renderer_sig;
	renderer_sig.set(ecs.getComponentBitflag<TransformComponent>());
	renderer_sig.set(ecs.getComponentBitflag<HUDTextComponent>());
	ecs.setSystemSignature<HUDTextRendererSystem>(renderer_sig);
}

void HUDTextRendererSystem::render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, const std::vector<std::unique_ptr<DirectX::SpriteFont>>& fonts, size_t available_fonts)
{
#ifdef _DEBUG
	if (fonts.size() == 0)
	{
		std::cout << "Warning: No fonts are loaded." << std::endl;
	}
#endif

	for (const auto& entity : m_entities)
	{
		auto& text = ecs.getComponent<HUDTextComponent>(entity);

		if (!text.m_render)
		{
			continue;
		}

		if (text.m_font_id >= available_fonts)
		{
			std::cout << "Error: Invalid font id with HUDTextComponent::m_font_id: " << text.m_font_id << std::endl;
			continue;
		}
		const auto& transform = ecs.getComponent<TransformComponent>(entity);

		text.m_z_order = std::clamp(text.m_z_order, 0.0f, 1.0f);

		fonts[text.m_font_id]->DrawString(
			sprite_batch,
			text.m_text.c_str(),
			transform.m_position + text.m_relative_position,
			text.m_color,
			transform.m_rotation_radians + text.m_relative_rotation_radians,
			text.m_origin,
			transform.m_scale * text.m_relative_scale,
			DirectX::SpriteEffects::SpriteEffects_None,
			text.m_z_order
		);
	}
}