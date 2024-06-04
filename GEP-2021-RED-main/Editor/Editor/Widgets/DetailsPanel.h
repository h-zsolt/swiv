#pragma once

#include <pch.h>
#include <ECSDefinitions.h>

namespace ECS { class ECS; }

struct TransformComponent;
struct SpriteComponent;
struct SpriteAnimComponent;
struct EnemySpawnerData;
struct EnvironmentTag;

class EditorRenderer;
class Tilemap;

class DetailsPanel
{
public:
	void detailsPanel(ECS::ECS& ecs, EditorRenderer& renderer);

	void registerOnImportTextureEvent(const std::function<int()>& funct);

	void setSelectionContext(std::pair<std::string, ECS::Entity>* selection_context);
	void setSelectedTilemap(Tilemap* context);

private:
	void drawComponents(ECS::Entity entity, ECS::ECS& ecs, EditorRenderer& renderer);
	void drawTilemap(ECS::ECS& ecs);

	void drawTransformComponent(TransformComponent& transform);

	// These draw the component options in the details panel and return whether the component has been
	// deleted.
	bool drawSpriteComponent(SpriteComponent& sprite, EditorRenderer& renderer);
	bool drawSpriteAnimComponent(SpriteAnimComponent& sprite_anim, EditorRenderer& renderer);
	bool drawEnemySpawnerComponent(EnemySpawnerData& enemy_spawner);
	bool drawEnvironmentTag(EnvironmentTag& environment_tag);

	void editSourceRectMenu(const std::string& label, RECT& source_rect);
	// Returns true if delete component has been selected.
	bool componentActionsPopup();

private:
	const float M_SCALE_SLIDER_SENSITIVITY = 0.01f;
	const float M_COLOR_SLIDER_SENSITIVITY = 0.01f;
	const float M_PLAYBACK_RATE_SLIDER_SENSITIVITY = 0.01f;
	const float M_MIN_PLAYBACK_RATE = 0.0f;
	const float M_SOURCE_RECT_SLIDER_SENSITIVITY = 0.125f;
	const float M_ENEMY_SPAWN_COUNT_SENSITIVITY = 0.05f;
	const int M_MIN_ENEMY_SPAWN_COUNT = 0;
	const float M_ENEMY_SPAWN_INTERVAL_SENSITIVITY = 0.05f;
	const float M_MIN_ENEMY_SPAWN_INTERVAL = 0.f;
	const int M_NUM_ENEMY_TYPES_DISPLAYED_AT_ONCE = 3;
	const float M_SOURCE_TILE_SLIDER_SENSITIVITY = 0.01f;
	const float M_BOX2D_POSITION_SLIDER_SENSITIVITY = 0.01f;
	const float M_BOX2D_HALF_SIZE_SLIDER_SENSITIVITY = 0.01f;

	std::pair<std::string, ECS::Entity>* m_selection_context = nullptr;
	Tilemap* m_selected_tilemap = nullptr;
	std::function<int()> m_on_import_texture;
};