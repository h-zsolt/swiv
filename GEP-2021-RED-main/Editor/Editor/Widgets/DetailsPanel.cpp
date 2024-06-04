#include <imgui.h>
#include <ECS.h>
#include <Components/TransformComponent.h>
#include <Components/SpriteComponent.h>
#include <Components/SpriteAnimComponent.h>
#include <Data/EnemySpawnerData.h>
#include <Data/EnvironmentTag.h>

#include "DetailsPanel.h"
#include "../EditorRenderer.h"
#include "../Assets/Tilemap.h"

void DetailsPanel::detailsPanel(ECS::ECS& ecs, EditorRenderer& renderer)
{
	ImGui::Begin("Details", nullptr, ImGuiWindowFlags_NoCollapse);

	if (m_selection_context)
	{
		drawComponents(m_selection_context->second, ecs, renderer);

		ImGui::NewLine();

		if (ImGui::Button("Add component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		// Add component popup list.
		if (ImGui::BeginPopup("AddComponent"))
		{
			// Sprite component.
			if (ImGui::MenuItem("Sprite component"))
			{
				if (!ecs.hasComponent<SpriteComponent>(m_selection_context->second))
				{
					ecs.addComponent<SpriteComponent>(m_selection_context->second, {});
				}
				ImGui::CloseCurrentPopup();
			}

			// Sprite anim component.
			if (ImGui::MenuItem("Sprite anim component"))
			{
				if (!ecs.hasComponent<SpriteAnimComponent>(m_selection_context->second))
				{
					ecs.addComponent<SpriteAnimComponent>(m_selection_context->second, {});
				}
				ImGui::CloseCurrentPopup();
			}

			// Enemy spawner component.
			if (ImGui::MenuItem("Enemy spawner component"))
			{
				if (!ecs.hasComponent<EnemySpawnerData>(m_selection_context->second))
				{
					ecs.addComponent<EnemySpawnerData>(m_selection_context->second, {});
				}
				ImGui::CloseCurrentPopup();
			}

			// Environment tag component.
			if (ImGui::MenuItem("Environment tag"))
			{
				if (!ecs.hasComponent<EnvironmentTag>(m_selection_context->second))
				{
					ecs.addComponent<EnvironmentTag>(m_selection_context->second, {});
				}
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
	else if(m_selected_tilemap)
	{
		drawTilemap(ecs);
	}

	ImGui::End();
}

void DetailsPanel::registerOnImportTextureEvent(const std::function<int()>& funct)
{
	m_on_import_texture = funct;
}

void DetailsPanel::setSelectionContext(std::pair<std::string, ECS::Entity>* selection_context)
{
	m_selection_context = selection_context;
}

void DetailsPanel::setSelectedTilemap(Tilemap* context)
{
	m_selected_tilemap = context;
}

void DetailsPanel::drawComponents(ECS::Entity entity, ECS::ECS& ecs, EditorRenderer& renderer)
{
	// Transform component.
	if (ecs.hasComponent<TransformComponent>(entity))
	{
		drawTransformComponent(ecs.getComponent<TransformComponent>(entity));
	}

	// Sprite component.
	if (ecs.hasComponent<SpriteComponent>(entity))
	{
		if (drawSpriteComponent(ecs.getComponent<SpriteComponent>(entity), renderer))
		{
			ecs.removeComponent<SpriteComponent>(entity);
		}
	}

	// Sprite anim component.
	if (ecs.hasComponent<SpriteAnimComponent>(entity))
	{
		if (drawSpriteAnimComponent(ecs.getComponent<SpriteAnimComponent>(entity), renderer))
		{
			ecs.removeComponent<SpriteAnimComponent>(entity);
		}
	}

	// Enemy spawner component.
	if (ecs.hasComponent<EnemySpawnerData>(entity))
	{
		if (drawEnemySpawnerComponent(ecs.getComponent<EnemySpawnerData>(entity)))
		{
			ecs.removeComponent<EnemySpawnerData>(entity);
		}
	}

	// Environment tag.
	if (ecs.hasComponent<EnvironmentTag>(entity))
	{
		if (drawEnvironmentTag(ecs.getComponent<EnvironmentTag>(entity)))
		{
			ecs.removeComponent<EnvironmentTag>(entity);
		}
	}
}

void DetailsPanel::drawTilemap(ECS::ECS& ecs)
{
	if (ImGui::TreeNodeEx("Tilemap", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		float pos[2] = { m_selected_tilemap->getPosition().x, m_selected_tilemap->getPosition().y };
		ImGui::DragFloat2("Position", pos);
		m_selected_tilemap->setPosition(pos[0], pos[1]);

		if (ImGui::TreeNodeEx("Tiles", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
		{
			auto& entities = m_selected_tilemap->getEntities();

			auto tile_size = m_selected_tilemap->getTileSize();
			tile_size = tile_size == 0 ? 1 : tile_size;
			std::string label = "Tile ";
			int i = 0;
			int columns = 1;
			ImGui::NewLine();
			for (auto& pair : entities)
			{
				auto& entity = pair.second;
				auto& sprite = ecs.getComponent<SpriteComponent>(entity);

				int source_tile_index[2];
				source_tile_index[0] = static_cast<int>(sprite.m_source_rect.left) / tile_size;
				source_tile_index[1] = static_cast<int>(sprite.m_source_rect.top) / tile_size;

				ImGui::SetNextItemWidth(40.F);

				if (columns == m_selected_tilemap->getWidth())
				{
					ImGui::DragInt2((label + std::to_string(i++)).c_str(), source_tile_index, M_SOURCE_TILE_SLIDER_SENSITIVITY);
					ImGui::NewLine();
					columns = 1;
				}
				else
				{
					ImGui::DragInt2((label + std::to_string(i++)).c_str(), source_tile_index, M_SOURCE_TILE_SLIDER_SENSITIVITY);
					ImGui::SameLine();
					++columns;
				}

				auto x = source_tile_index[0] * tile_size;
				auto y = source_tile_index[1] * tile_size;
				sprite.m_source_rect.left = static_cast<LONG>(x);
				sprite.m_source_rect.top = static_cast<LONG>(y);
				sprite.m_source_rect.right = static_cast<LONG>(x + tile_size);
				sprite.m_source_rect.bottom = static_cast<LONG>(y + tile_size);
				sprite.m_origin = 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>((sprite.m_source_rect.right - sprite.m_source_rect.left)),
					static_cast<float>((sprite.m_source_rect.bottom - sprite.m_source_rect.top)));
			}

			ImGui::TreePop();
		}

		ImGui::TreePop();
	}
}

void DetailsPanel::drawTransformComponent(TransformComponent& transform)
{
	if (ImGui::TreeNodeEx("Transform component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		float pos[2] = { transform.m_position.x, transform.m_position.y };
		ImGui::DragFloat2("Position", pos);
		transform.m_position = { pos[0], pos[1] };

		float scl[2] = { transform.m_scale.x, transform.m_scale.y };
		ImGui::DragFloat2("Scale", scl, M_SCALE_SLIDER_SENSITIVITY);
		transform.m_scale = { scl[0], scl[1] };

		float rot_deg = DirectX::XMConvertToDegrees(transform.m_rotation_radians);
		ImGui::DragFloat("Rotation(Deg)", &rot_deg);
		transform.m_rotation_radians = DirectX::XMConvertToRadians(rot_deg);

		ImGui::TreePop();
	}
}

bool DetailsPanel::drawSpriteComponent(SpriteComponent& sprite, EditorRenderer& renderer)
{
	if (ImGui::TreeNodeEx("Sprite component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool deleted = componentActionsPopup();

		if (deleted)
		{
			ImGui::TreePop();
			return deleted;
		}

		float rel_pos[2] = { sprite.m_relative_position.x, sprite.m_relative_position.y };
		ImGui::DragFloat2("Relative Position", rel_pos);
		sprite.m_relative_position = { rel_pos[0], rel_pos[1] };

		float rel_scl[2] = { sprite.m_relative_scale.x, sprite.m_relative_scale.y };
		ImGui::DragFloat2("Relative Scale", rel_scl, M_SCALE_SLIDER_SENSITIVITY);
		sprite.m_relative_scale = { rel_scl[0], rel_scl[1] };

		float rot_deg = DirectX::XMConvertToDegrees(sprite.m_relative_rotation_radians);
		ImGui::DragFloat("Relative Rotation(Deg)", &rot_deg);
		sprite.m_relative_rotation_radians = DirectX::XMConvertToRadians(rot_deg);

		float col[4] = { sprite.m_colour.x, sprite.m_colour.y, sprite.m_colour.z, sprite.m_colour.z };
		ImGui::DragFloat4("Color", col, M_COLOR_SLIDER_SENSITIVITY);
		sprite.m_colour = { col[0], col[1], col[2], col[3] };

		ImGui::DragFloat("Z Order", &sprite.m_z_order, 1.f, 0.f, 1.f);

		if (ImGui::Button("..."))
		{
			if (m_on_import_texture)
			{
				auto id = m_on_import_texture();
				if (id >= 0)
				{
					renderer.setupSpriteComponent(static_cast<size_t>(id), sprite);
				}
			}
		}
		ImGui::SameLine();
		ImGui::Text("Source texture");

		editSourceRectMenu("Source rect", sprite.m_source_rect);
		sprite.m_origin = 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>((sprite.m_source_rect.right - sprite.m_source_rect.left)),
			static_cast<float>((sprite.m_source_rect.bottom - sprite.m_source_rect.top)));

		ImGui::TreePop();
		return deleted;
	}
	return false;
}

bool DetailsPanel::drawSpriteAnimComponent(SpriteAnimComponent& sprite_anim, EditorRenderer& renderer)
{
	if (ImGui::TreeNodeEx("Sprite anim component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool deleted = componentActionsPopup();

		if (deleted)
		{
			ImGui::TreePop();
			return deleted;
		}

		float rel_pos[2] = { sprite_anim.m_relative_position.x, sprite_anim.m_relative_position.y };
		ImGui::DragFloat2("Relative Position", rel_pos);
		sprite_anim.m_relative_position = { rel_pos[0], rel_pos[1] };

		float rel_scl[2] = { sprite_anim.m_relative_scale.x, sprite_anim.m_relative_scale.y };
		ImGui::DragFloat2("Relative Scale", rel_scl, M_SCALE_SLIDER_SENSITIVITY);
		sprite_anim.m_relative_scale = { rel_scl[0], rel_scl[1] };

		float rot_deg = DirectX::XMConvertToDegrees(sprite_anim.m_relative_rotation_radians);
		ImGui::DragFloat("Relative Rotation(Deg)", &rot_deg);
		sprite_anim.m_relative_rotation_radians = DirectX::XMConvertToRadians(rot_deg);

		float col[4] = { sprite_anim.m_colour.x, sprite_anim.m_colour.y, sprite_anim.m_colour.z, sprite_anim.m_colour.z };
		ImGui::DragFloat4("Color", col, M_COLOR_SLIDER_SENSITIVITY);
		sprite_anim.m_colour = { col[0], col[1], col[2], col[3] };

		ImGui::DragFloat("Z Order", &sprite_anim.m_z_order, 1.f, 0.f, 1.f);

		int tex_id = -1;
		if (ImGui::Button("..."))
		{
			if (m_on_import_texture)
			{
				tex_id = m_on_import_texture();
				sprite_anim.m_texture_id = tex_id;
			}
		}
		ImGui::SameLine();
		ImGui::Text("Source texture");

		// Anim frames.
		if (ImGui::TreeNodeEx("Frames", ImGuiTreeNodeFlags_Framed))
		{
			for (size_t i = 0; i < sprite_anim.m_frames.size(); i++)
			{
				// Draw frame rect options.
				std::string label = "Frame " + std::to_string(i);
				editSourceRectMenu(label.c_str(), sprite_anim.m_frames[i]);
			}

			// Add new frame button.
			if (ImGui::Button("Add frame"))
			{
				auto& new_frame = sprite_anim.m_frames.emplace_back();

				if (tex_id >= 0)
				{
					auto desc = renderer.getTexture2DDesc(static_cast<size_t>(tex_id));
					new_frame.top = 0;
					new_frame.bottom = desc.Height;
					new_frame.left = 0;
					new_frame.right = desc.Width;
				}
				else
				{
					new_frame.top = 0;
					new_frame.bottom = 0;
					new_frame.left = 0;
					new_frame.right = 0;
				}
			}

			ImGui::TreePop();
		}

		// Calculate origin of sprite anim as the centre of the first frame.
		if (sprite_anim.m_frames.size() > 0)
		{
			const auto& frame_rect = sprite_anim.m_frames[0];
			sprite_anim.m_origin = 0.5f * DirectX::SimpleMath::Vector2(static_cast<float>((frame_rect.right - frame_rect.left)),
				static_cast<float>((frame_rect.bottom - frame_rect.top)));
		}

		ImGui::DragFloat("Playback rate(Seconds)", &sprite_anim.m_playback_rate_seconds, M_PLAYBACK_RATE_SLIDER_SENSITIVITY);
		if (sprite_anim.m_playback_rate_seconds < M_MIN_PLAYBACK_RATE) 
			sprite_anim.m_playback_rate_seconds = M_MIN_PLAYBACK_RATE;

		ImGui::Checkbox("Looping", &sprite_anim.m_looping);
		ImGui::Checkbox("Completed", &sprite_anim.m_completed);

		ImGui::TreePop();
		return deleted;
	}
	return false;
}

bool DetailsPanel::drawEnemySpawnerComponent(EnemySpawnerData& enemy_spawner)
{
	if (ImGui::TreeNodeEx("Enemy spawner component", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool deleted = componentActionsPopup();

		if (deleted)
		{
			ImGui::TreePop();
			return deleted;
		}

		float rel_pos[2] = { enemy_spawner.relative_position.x, enemy_spawner.relative_position.y };
		ImGui::DragFloat2("Relative Position", rel_pos);
		enemy_spawner.relative_position = { rel_pos[0], rel_pos[1] };

		int type = static_cast<int>(enemy_spawner.type);
		ImGui::ListBox("Enemy type", &type, s_enemy_type_display_names, IM_ARRAYSIZE(s_enemy_type_display_names), M_NUM_ENEMY_TYPES_DISPLAYED_AT_ONCE);
		enemy_spawner.type = static_cast<EnemyType>(type);

		ImGui::DragInt("Spawn count", &enemy_spawner.spawn_count, M_ENEMY_SPAWN_COUNT_SENSITIVITY);
		if (enemy_spawner.spawn_count < M_MIN_ENEMY_SPAWN_COUNT) enemy_spawner.spawn_count = M_MIN_ENEMY_SPAWN_COUNT;

		ImGui::DragFloat("Spawn interval", &enemy_spawner.spawn_interval, M_ENEMY_SPAWN_INTERVAL_SENSITIVITY);
		if (enemy_spawner.spawn_interval < M_MIN_ENEMY_SPAWN_INTERVAL) enemy_spawner.spawn_interval = M_MIN_ENEMY_SPAWN_INTERVAL;

		ImGui::TreePop();
		return deleted;
	}
	return false;
}

bool DetailsPanel::drawEnvironmentTag(EnvironmentTag& environment_tag)
{
	if (ImGui::TreeNodeEx("Environment tag", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_DefaultOpen))
	{
		bool deleted = componentActionsPopup();

		if (deleted)
		{
			ImGui::TreePop();
			return deleted;
		}

		// This component has no data.

		ImGui::TreePop();
		return deleted;
	}
	return false;
}

void DetailsPanel::editSourceRectMenu(const std::string& label, RECT& source_rect)
{
	ImGui::Text(label.c_str());
	int top = source_rect.top;
	ImGui::DragInt((label + " top").c_str(), &top, M_SOURCE_RECT_SLIDER_SENSITIVITY);
	source_rect.top = static_cast<LONG>(top);

	int bottom = source_rect.bottom;
	ImGui::DragInt((label + " bottom").c_str(), &bottom, M_SOURCE_RECT_SLIDER_SENSITIVITY);
	source_rect.bottom = static_cast<LONG>(bottom);

	int right = source_rect.right;
	ImGui::DragInt((label + " right").c_str(), &right, M_SOURCE_RECT_SLIDER_SENSITIVITY);
	source_rect.right = static_cast<LONG>(right);

	int left = source_rect.left;
	ImGui::DragInt((label + " left").c_str(), &left, M_SOURCE_RECT_SLIDER_SENSITIVITY);
	source_rect.left = static_cast<LONG>(left);
}

bool DetailsPanel::componentActionsPopup()
{
	bool deleted = false;

	if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
	{
		ImGui::OpenPopup("CompActions");
	}

	if (ImGui::BeginPopup("CompActions"))
	{
		if (ImGui::MenuItem("Delete component"))
		{
			deleted = true;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	return deleted;
}
