#pragma once

#include <pch.h>

class EditorRenderer;

struct TilemapCreationInfo;

class CreateTilemapMenu
{
public:

	void createTilemapMenu(const EditorRenderer& renderer);

	void registerOnCreateEvent(const std::function<void(const TilemapCreationInfo& creation_info)>& funct);
	void registerOnLoadSpritesheetEvent(const std::function<int()>& funct);

	bool isShown() const;
	void setShown(bool new_shown);

private:
	bool m_show = false;

	static constexpr float M_SLIDER_SENSITIVITY{ 0.01f };

	static constexpr int M_MIN_DIMS{ 0 };
	static constexpr int M_MIN_TILE_SIZE{ 0 };

	static constexpr float M_MIN_Z_ORDER{ 0.F };
	static constexpr float M_MAX_Z_ORDER{ 1.F };

	float m_position[2]{ 0.f, 0.f };
	int m_tilemap_dims[2]{ M_MIN_DIMS, M_MIN_DIMS };
	int m_tilemap_tile_size{ M_MIN_TILE_SIZE };
	int m_tilemap_source_spritesheet_id{ -1 };
	float m_z_order{ 0.F };

	std::function<void(const TilemapCreationInfo& creation_info)> m_on_create_callback;
	std::function<int()> m_on_set_source_spritesheet_callback;
};