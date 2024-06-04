#include <imgui.h>

#include "SceneOutliner.h"
#include "../Assets/Tilemap.h"

SceneOutliner::SceneOutliner(std::unordered_map<std::string, ECS::Entity>* scene_context, std::unordered_map<std::string, Tilemap>* tilemaps)
	: m_scene_context(scene_context), m_scene_tilemaps(tilemaps), m_selection_context(9817239)
{
}

void SceneOutliner::sceneOutliner()
{
	assert(m_scene_context != nullptr && "Scene entities pointer is null in the scene outliner.");

	ImGui::Begin("Scene outliner", nullptr, ImGuiWindowFlags_NoCollapse);
	{
		std::vector<std::string> entities_to_destroy;
		std::vector<std::string> entities_to_duplicate;

		if (ImGui::Button("Add Entity"))
		{
			if (m_on_create_entity_callback)
			{
				m_on_create_entity_callback();
			}
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_tilemap_selection_context = nullptr;

			m_selection_context = 9817239;
			if (m_on_entity_selected_callback)
			{
				m_on_entity_selected_callback("");
			}
		}

		// Copy map key-values into vector and sort into ascending order based on value(entity, uint32).
		auto cmp = [](const std::pair<std::string, ECS::Entity>& a,
			const std::pair<std::string, ECS::Entity>& b) {
			return a.second < b.second;
		};

		std::vector<std::pair<std::string, ECS::Entity>> pairs;
		for (const auto& pair : *m_scene_context)
		{
			pairs.push_back(pair);
		}

		std::sort(pairs.begin(), pairs.end(), cmp);

		for (const auto& pair : pairs)
		{
			drawEntityNode(pair, entities_to_destroy, entities_to_duplicate);
		}

		if (!m_scene_tilemaps->empty())
		{
			ImGui::Separator();

			for (const auto& pair : *m_scene_tilemaps)
			{
				drawTilemapNode(pair);
			}
		}

		// Delayed duplicate all entities marked for duplication.
		for (const auto& name : entities_to_duplicate)
		{
			if (m_on_duplicate_entity_callback)
			{
				m_on_duplicate_entity_callback(name);
			}
		}

		// Delayed destroy all entities marked for destroy.
		for (const auto& name : entities_to_destroy)
		{
			if (m_on_destroy_entity_callback)
			{
				m_on_destroy_entity_callback(name);
			}
		}

		// Right click in empty space of scene outliner menu popup.
		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Add Entity"))
			{
				if (m_on_create_entity_callback)
				{
					m_on_create_entity_callback();
				}
			}

			ImGui::EndPopup();
		}
	}
	ImGui::End();
}

void SceneOutliner::registerOnEntitySelectedEvent(const std::function<void(const std::string&)>& funct)
{
	m_on_entity_selected_callback = funct;
}

void SceneOutliner::registerOnCreateEntityEvent(const std::function<void()>& funct)
{
	m_on_create_entity_callback = funct;
}

void SceneOutliner::registerOnDestroyEntityEvent(const std::function<void(const std::string&)>& funct)
{
	m_on_destroy_entity_callback = funct;
}

void SceneOutliner::registerOnDuplicateEntityEvent(const std::function<void(const std::string&)>& funct)
{
	m_on_duplicate_entity_callback = funct;
}

void SceneOutliner::registerOnTilemapSelectedEvent(const std::function<void(const std::string&)>& funct)
{
	m_on_tilemap_selected_callback = funct;
}

void SceneOutliner::setSelectionContext(ECS::Entity context)
{
	m_selection_context = context;
}

void SceneOutliner::setTilemapSelectionContext(Tilemap* context)
{
	m_tilemap_selection_context = context;
}

Tilemap* SceneOutliner::getTilemapSelectionContext() const
{
	return m_tilemap_selection_context;
}

void SceneOutliner::drawEntityNode(const std::pair<std::string, ECS::Entity>& context, std::vector<std::string>& entities_to_destroy, std::vector<std::string>& entities_to_duplicate)
{
	ImGuiTreeNodeFlags flags = ((m_selection_context == context.second) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	bool opened = ImGui::TreeNodeEx((const void*)((uint64_t)context.second), flags, context.first.c_str());
	if (ImGui::IsItemClicked())
	{
		m_selection_context = context.second;
		if (m_on_entity_selected_callback)
		{
			m_on_entity_selected_callback(context.first);
		}
	}

	// Right click on entity menu.
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Duplicate entity"))
		{
			entities_to_duplicate.push_back(context.first);
		}

		if (ImGui::MenuItem("Delete entity"))
		{
			entities_to_destroy.push_back(context.first);
		}

		ImGui::EndPopup();
	}

	if (opened)
	{
		ImGui::TreePop();
	}
}

void SceneOutliner::drawTilemapNode(const std::pair<std::string, Tilemap>& context)
{
	ImGuiTreeNodeFlags flags = ((m_tilemap_selection_context && (m_tilemap_selection_context->getName() == context.second.getName())) ? ImGuiTreeNodeFlags_Selected : 0) 
		| ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
	bool opened = ImGui::TreeNodeEx((const void*)(&context.second), flags, context.first.c_str());
	if (ImGui::IsItemClicked())
	{
		if (m_on_tilemap_selected_callback)
		{
			m_on_tilemap_selected_callback(context.first);
		}
	}

	if (opened)
	{
		ImGui::TreePop();
	}
}
