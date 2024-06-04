#include "pch.h"
#include "SpriteAnimRendererSystem.h"
#include <Components/SpriteAnimComponent.h>
#include <Components/TransformComponent.h>
#include "ECS.h"

void SpriteAnimRendererSystem::init(ECS::ECS& ecs)
{
	// Register signature for system.
	ECS::Signature renderer_sig;
	renderer_sig.set(ecs.getComponentBitflag<TransformComponent>());
	renderer_sig.set(ecs.getComponentBitflag<SpriteAnimComponent>());
	ecs.setSystemSignature<SpriteAnimRendererSystem>(renderer_sig);
}

void SpriteAnimRendererSystem::tick(float dt, ECS::ECS& ecs)
{
	m_elapsed_time += dt;

	// Update animations.
	for (const auto& entity : m_entities)
	{
		auto& anim_comp = ecs.getComponent<SpriteAnimComponent>(entity);

		if (anim_comp.m_completed || anim_comp.m_frames.empty())
		{
			continue;
		}

		if ((m_elapsed_time - anim_comp.m_last_frame_increment_time) > anim_comp.m_playback_rate_seconds)
		{
			if (anim_comp.m_looping)
			{
				anim_comp.m_current_frame = (anim_comp.m_current_frame + 1) % anim_comp.m_frames.size();
			}
			else if (anim_comp.m_current_frame != anim_comp.m_frames.size() - 1)
			{
				anim_comp.m_current_frame++;
			}
			else
			{
				anim_comp.m_completed = true;
			}
			anim_comp.m_last_frame_increment_time = m_elapsed_time;
		}
	}
}

void SpriteAnimRendererSystem::render(ECS::ECS& ecs, DirectX::SpriteBatch* sprite_batch, 
	const std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>>& textures, size_t available_textures)
{
#ifdef _DEBUG
	if (textures.size() == 0)
	{
		std::cout << "Warning: No textures are loaded." << std::endl;
	}
#endif

	for (const auto& entity : m_entities)
	{
		auto& anim_comp = ecs.getComponent<SpriteAnimComponent>(entity);
		if (anim_comp.m_frames.empty())
		{
			std::cout << "Warning: Attempting to render sprite anim which has no frames." << std::endl;
			continue;
		}

		if (anim_comp.m_texture_id < 0)
		{
			std::cout << "Error: Invalid texture id with SpriteAnimComponent::m_texture_id: " << anim_comp.m_texture_id << std::endl;
			continue;
		}
		auto texture_id = static_cast<size_t>(anim_comp.m_texture_id);
		if (texture_id >= available_textures)
		{
			std::cout << "Error: Invalid texture id with SpriteAnimComponent::m_texture_id: " << anim_comp.m_texture_id << std::endl;
			continue;
		}

		const auto& transform = ecs.getComponent<TransformComponent>(entity);

		anim_comp.m_z_order = std::clamp(anim_comp.m_z_order, 0.0f, 1.0f);

		sprite_batch->Draw(textures[texture_id].Get(),
			transform.m_position + anim_comp.m_relative_position,
			&anim_comp.m_frames[anim_comp.m_current_frame],
			anim_comp.m_colour,
			transform.m_rotation_radians + anim_comp.m_relative_rotation_radians,
			anim_comp.m_origin,
			transform.m_scale * anim_comp.m_relative_scale,
			DirectX::SpriteEffects::SpriteEffects_None,
			anim_comp.m_z_order
		);
	}
}
