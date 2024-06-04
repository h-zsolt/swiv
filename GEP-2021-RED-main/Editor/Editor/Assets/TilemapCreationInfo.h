#pragma once

#include <pch.h>

struct TilemapCreationInfo
{
	DirectX::SimpleMath::Vector2 position;
	DirectX::SimpleMath::Vector2 dimensions;
	int tile_size = 1;
	int spritesheet_id = -1;
	int source_spritesheet_width  = 0;
	int source_spritesheet_height = 0;
	float z_order = 0.F;
};