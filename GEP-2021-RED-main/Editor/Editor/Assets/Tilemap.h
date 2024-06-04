#pragma once

#include <pch.h>
#include <ECSDefinitions.h>

#include "TilemapCreationInfo.h"

namespace ECS { class ECS; }

class Tilemap
{
public:
	Tilemap(const TilemapCreationInfo& creation_info, const std::string& name, ECS::ECS* ecs);

	void setPosition(float x, float y);

	int getWidth() const;
	int getHeight() const;
	int getSourceTextureId() const;
	int getSourceSpritesheetWidth() const;
	int getSourceSpritesheetHeight() const;
	const DirectX::SimpleMath::Vector2& getPosition() const;
	int getTileSize() const;
	const std::string& getName() const;
	const std::vector<std::pair<std::string, ECS::Entity>>& getEntities() const;

	void push_back_entity(const std::pair<std::string, ECS::Entity>& entity);
	bool contains(const std::pair<std::string, ECS::Entity>& entity) const;

private:
	TilemapCreationInfo m_info;
	std::string m_name;
	ECS::ECS* m_ecs;
	std::vector<std::pair<std::string, ECS::Entity>> m_entities;
};