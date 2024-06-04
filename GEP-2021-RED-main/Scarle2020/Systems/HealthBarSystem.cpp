#include "pch.h"
#include "HealthBarSystem.h"
#include "Components/HealthBarComponent.h"
#include "Components/TransformComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/BoundComponent.h"
#include "Data/HealthData.h"
#include "IRenderer.h"

void HealthBarSystem::init(ECS::ECS& ecs, IRenderer* renderer)
{
	m_renderer = renderer;
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<HealthBarComponent>());
	ecs.setSystemSignature<HealthBarSystem>(sig);
}

void HealthBarSystem::update(ECS::ECS& ecs, const float dt, const float camera_y)
{
	for (auto& entity : m_entities)
	{
		auto& health_bar = ecs.getComponent<HealthBarComponent>(entity);
		if (ecs.hasComponent<TransformComponent>(entity) && ecs.hasComponent<HealthData>(entity) && ecs.hasComponent<SpriteComponent>(entity))
		{
			const auto& entity_transform = ecs.getComponent<TransformComponent>(entity);
			const auto& entity_health = ecs.getComponent<HealthData>(entity);
			if (!health_bar.m_created)
			{
				//init the bars
				health_bar.m_current_health = entity_health.m_current_health;
				health_bar.m_max_health = entity_health.m_max_health;
				
				std::array<ECS::Entity, 3> bars = { ecs.createEntity(), ecs.createEntity(), ecs.createEntity() };
				TransformComponent bar_transform;
				if (health_bar.m_following)
				{
					const auto& sprite_size = ecs.getComponent<SpriteComponent>(entity).m_source_rect;
					bar_transform.m_position = health_bar.m_position + entity_transform.m_position;
					bar_transform.m_position.y -= sprite_size.bottom - sprite_size.top + GAP;
				}
				else
				{
					bar_transform.m_position = health_bar.m_position;
					bar_transform.m_position.y += camera_y;
				}
				std::array<SpriteComponent, 3> bar_sprites;
				for (int i = 0; i < 3; i++)
				{
					m_renderer->setupSpriteComponent(m_texture_ids[i], bar_sprites[i]);
					bar_sprites[i].m_colour = health_bar.m_bar_colours[i];
					bar_sprites[i].m_relative_scale = health_bar.m_size;
					bar_sprites[i].m_z_order = health_bar.m_z_orders[i];
					//bar_sprites[i].m_relative_position = health_bar.m_transform;
					ecs.addComponent<TransformComponent>(bars[i], bar_transform);
					ecs.addComponent<SpriteComponent>(bars[i], bar_sprites[i]);
					health_bar.m_bars[i] = bars[i];
					if (!ecs.hasComponent<BoundComponent>(entity))
					{
						ecs.addComponent(entity, BoundComponent{});
					}
					ecs.getComponent<BoundComponent>(entity).m_entities.push_back(bars[i]);
				}
				health_bar.m_created = true;
			}
			else
			{
				//update position
				if (health_bar.m_following)
				{
					for (auto bar : health_bar.m_bars)
					{
						const auto& sprite_size = ecs.getComponent<SpriteComponent>(entity).m_source_rect;
						ecs.getComponent<TransformComponent>(bar).m_position = health_bar.m_position + entity_transform.m_position;
						ecs.getComponent<TransformComponent>(bar).m_position.y -= sprite_size.bottom - sprite_size.top + GAP;
					}
				}
				else
				{
					for (auto bar : health_bar.m_bars)
					{
						ecs.getComponent<TransformComponent>(bar).m_position = health_bar.m_position;
						ecs.getComponent<TransformComponent>(bar).m_position.y -= camera_y;
					}
				}
				//update feedback
				auto& current_bar_scale = ecs.getComponent<SpriteComponent>(health_bar.m_bars[CURRENT]).m_relative_scale;
				auto& feedback_bar_scale = ecs.getComponent<SpriteComponent>(health_bar.m_bars[FEEDBACK]).m_relative_scale;
				if (health_bar.m_feedback_clock <= 0.0f)
				{
					//dt * health_bar.m_feedback_drain_rate //pixels lost per second
					if (feedback_bar_scale.x > current_bar_scale.x)
					{
						feedback_bar_scale.x -= dt * health_bar.m_feedback_drain_rate;
					}
				}
				else
				{
					health_bar.m_feedback_clock -= dt;
				}
				//update current health bar
				if (health_bar.m_current_health != entity_health.m_current_health)
				{
					health_bar.m_current_health = entity_health.m_current_health;
					const float BAR_FEEDBACK_DELAY = 0.8f;
					health_bar.m_feedback_clock = BAR_FEEDBACK_DELAY;
					const float HEALTH_RATIO = health_bar.m_current_health / health_bar.m_max_health;
					current_bar_scale.x = health_bar.m_size.x * HEALTH_RATIO;
				}
			}
		}
		else
		{
			//TODO: Log error, missing component(s)
		}
	}
}

void HealthBarSystem::setTextureIDs(size_t full, size_t feedback, size_t current)
{
	m_texture_ids[FULL] = full;
	m_texture_ids[FEEDBACK] = feedback;
	m_texture_ids[CURRENT] = current;
}
