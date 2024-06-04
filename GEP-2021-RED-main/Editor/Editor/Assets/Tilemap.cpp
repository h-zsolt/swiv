#include <ECS.h>
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>

#include "Tilemap.h"

Tilemap::Tilemap(const TilemapCreationInfo& creation_info, const std::string& name, ECS::ECS* ecs)
	: m_info(creation_info), m_name(name), m_ecs(ecs)
{
}

void Tilemap::setPosition(float x, float y)
{
	m_info.position.x = x;
	m_info.position.y = y;

	for (int y = 0, i = 0; y < static_cast<int>(m_info.dimensions.y); y++)
	{
		for (int x = 0; x < static_cast<int>(m_info.dimensions.x); x++, i++)
		{
			auto& entity = m_entities[static_cast<size_t>(i)].second;
			auto& transform = m_ecs->getComponent<TransformComponent>(entity);

			transform.m_position = m_info.position + DirectX::SimpleMath::Vector2{ static_cast<float>(x * m_info.tile_size), static_cast<float>(y * m_info.tile_size) };
		}
	}
}

int Tilemap::getWidth() const
{
	return static_cast<int>(m_info.dimensions.x);
}

int Tilemap::getHeight() const
{
	return static_cast<int>(m_info.dimensions.y);
}

int Tilemap::getSourceTextureId() const
{
	return m_info.spritesheet_id;
}

int Tilemap::getSourceSpritesheetWidth() const
{
	return m_info.source_spritesheet_width;
}

int Tilemap::getSourceSpritesheetHeight() const
{
	return m_info.source_spritesheet_height;
}

const DirectX::SimpleMath::Vector2& Tilemap::getPosition() const
{
	return m_info.position;
}

int Tilemap::getTileSize() const
{
	return m_info.tile_size;
}

const std::string& Tilemap::getName() const
{
	return m_name;
}

const std::vector<std::pair<std::string, ECS::Entity>>& Tilemap::getEntities() const
{
	return m_entities;
}

void Tilemap::push_back_entity(const std::pair<std::string, ECS::Entity>& entity)
{
	m_entities.push_back(entity);
}

bool Tilemap::contains(const std::pair<std::string, ECS::Entity>& entity) const
{
	return std::find(m_entities.begin(), m_entities.end(), entity) != m_entities.end();
}
