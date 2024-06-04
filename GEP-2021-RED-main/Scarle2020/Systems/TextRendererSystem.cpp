#include "pch.h"
#include "TextRendererSystem.h"
#include "ECS.h"
#include <Components/TransformComponent.h>
#include <Components/TextRenderComponent.h>
#include "Helper.h"

void TextRendererSystem::init(ECS::ECS& ecs)
{
	// Register signature for system.
	ECS::Signature renderer_sig;
	renderer_sig.set(ecs.getComponentBitflag<TransformComponent>());
	renderer_sig.set(ecs.getComponentBitflag<TextRenderComponent>());
	ecs.setSystemSignature<TextRendererSystem>(renderer_sig);
}

void TextRendererSystem::render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, const std::vector<std::unique_ptr<DirectX::SpriteFont>>& fonts, size_t available_fonts)
{
#ifdef _DEBUG
	if (fonts.size() == 0)
	{
		std::cout << "Warning: No fonts are loaded." << std::endl;
	}
#endif

	for (const auto& entity : m_entities)
	{
		auto& text_render = ecs.getComponent<TextRenderComponent>(entity);
		if (text_render.m_font_id >= available_fonts)
		{
			std::cout << "Error: Invalid font id with TextRenderComponent::m_font_id: " << text_render.m_font_id << std::endl;
			continue;
		}
		const auto& transform = ecs.getComponent<TransformComponent>(entity);

		text_render.m_z_order = std::clamp(text_render.m_z_order, 0.0f, 1.0f);

		fonts[text_render.m_font_id]->DrawString(
			sprite_batch,
			text_render.m_text.c_str(),
			transform.m_position + text_render.m_relative_position,
			text_render.m_color,
			transform.m_rotation_radians + text_render.m_relative_rotation_radians,
			text_render.m_origin,
			transform.m_scale * text_render.m_relative_scale,
			DirectX::SpriteEffects::SpriteEffects_None,
			text_render.m_z_order
		);
	}
}
