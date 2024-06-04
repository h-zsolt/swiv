#pragma once

#include "pch.h"
#include "TagData.h"

enum struct BulletType
{
	NONE,
	ENEMY,
	PLAYER
};

struct BulletData
{
	//using Vector2 = DirectX::SimpleMath::Vector2;

	bool  is_fireing = false;
	float rate_of_fire = 0.F;
	float cooldown_timer = 0.F;

	float bullet_speed = 0.F;
	float bullet_damage = 0.F;
	float bullet_spawn_offset = 0.F;

	unsigned bullet_count = 1;
	float bullet_angle = 1.F;

	TagID bullet_type{};
	TagID tag_ignore{};
};

