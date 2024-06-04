#pragma once

#include <pch.h>
#include <ECSDefinitions.h>

class Tilemap;

class SceneOutliner
{
public:
	SceneOutliner(std::unordered_map<std::string, ECS::Entity>* scene_context, std::unordered_map< std::string, Tilemap>* tilemaps);

	void sceneOutliner();

	void registerOnEntitySelectedEvent(const std::function<void(const std::string&)>& funct);
	void registerOnCreateEntityEvent(const std::function<void()>& funct);
	void registerOnDestroyEntityEvent(const std::function<void(const std::string&)>& funct);
	void registerOnDuplicateEntityEvent(const std::function<void(const std::string&)>& funct);
	void registerOnTilemapSelectedEvent(const std::function<void(const std::string&)>& funct);

	void setSelectionContext(ECS::Entity context);
	void setTilemapSelectionContext(Tilemap* context);

	Tilemap* getTilemapSelectionContext() const;

private:
	void drawEntityNode(const std::pair<std::string, ECS::Entity>& context, std::vector<std::string>& entities_to_destroy,
		std::vector<std::string>& entities_to_duplicate);
	void drawTilemapNode(const std::pair<std::string, Tilemap>& context);

private:
	std::unordered_map<std::string, ECS::Entity>* m_scene_context;
	std::unordered_map<std::string, Tilemap>* m_scene_tilemaps;
	ECS::Entity m_selection_context;
	Tilemap* m_tilemap_selection_context = nullptr;

	std::function<void(const std::string&)> m_on_entity_selected_callback;
	std::function<void()> m_on_create_entity_callback;
	std::function<void(const std::string&)> m_on_destroy_entity_callback;
	std::function<void(const std::string&)> m_on_duplicate_entity_callback;
	std::function<void(const std::string&)> m_on_tilemap_selected_callback;
};