#include <imgui.h>

#include "CreateTilemapMenu.h"
#include "../EditorRenderer.h"
#include "../Assets/TilemapCreationInfo.h"

void CreateTilemapMenu::createTilemapMenu(const EditorRenderer& renderer)
{
	ImGui::Begin("Create tilemap", &m_show, ImGuiWindowFlags_NoCollapse);

	ImGui::DragFloat2("Position", m_position, M_SLIDER_SENSITIVITY);

	ImGui::DragInt2("Dimensions", m_tilemap_dims, M_SLIDER_SENSITIVITY, 1);
	if (m_tilemap_dims[0] < M_MIN_DIMS) m_tilemap_dims[0] = M_MIN_DIMS;
	if (m_tilemap_dims[1] < M_MIN_DIMS) m_tilemap_dims[1] = M_MIN_DIMS;

	ImGui::DragInt("Tile size", &m_tilemap_tile_size, M_SLIDER_SENSITIVITY, 1);
	if (m_tilemap_tile_size < M_MIN_TILE_SIZE) m_tilemap_tile_size = M_MIN_TILE_SIZE;

	ImGui::DragFloat("Z Order", &m_z_order, M_SLIDER_SENSITIVITY, M_MIN_Z_ORDER, M_MAX_Z_ORDER);

	if (ImGui::Button("..."))
	{
		if (m_on_set_source_spritesheet_callback)
		{
			m_tilemap_source_spritesheet_id = m_on_set_source_spritesheet_callback();
		}
	}
	ImGui::SameLine();
	ImGui::Text("Source sprite sheet");

	if (ImGui::Button("Create"))
	{
		if (m_on_create_callback)
		{
			TilemapCreationInfo info = {};
			info.position = { m_position[0], m_position[1] };
			info.dimensions = { static_cast<float>(m_tilemap_dims[0]), static_cast<float>(m_tilemap_dims[1]) };
			info.tile_size = m_tilemap_tile_size;
			info.spritesheet_id = m_tilemap_source_spritesheet_id;
			info.z_order = m_z_order;

			if (info.spritesheet_id >= 0)
			{
				const auto& desc = renderer.getTexture2DDesc(info.spritesheet_id);
				info.source_spritesheet_width = static_cast<int>(desc.Width);
				info.source_spritesheet_height = static_cast<int>(desc.Height);
			}

			m_on_create_callback(info);
		}

		m_show = false;
	}

	ImGui::End();
}

void CreateTilemapMenu::registerOnCreateEvent(const std::function<void(const TilemapCreationInfo& creation_info)>& funct)
{
	m_on_create_callback = funct;
}

void CreateTilemapMenu::registerOnLoadSpritesheetEvent(const std::function<int()>& funct)
{
	m_on_set_source_spritesheet_callback = funct;
}

bool CreateTilemapMenu::isShown() const
{
	return m_show;
}

void CreateTilemapMenu::setShown(bool new_shown)
{
	m_show = new_shown;
}
