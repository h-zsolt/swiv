#pragma once

#include "pch.h"

enum struct EnemyType : int
{
    ENEMY1 = 0,
    ENEMY2,
    ENEMY3,
    ENEMY4,
    ENEMY5,
    ENEMY6,
    NUM_ENEMY_TYPES
};

static const char* s_enemy_type_display_names[static_cast<size_t>(EnemyType::NUM_ENEMY_TYPES)] = {
    "Enemy1", 
    "Enemy2",
    "Enemy3", 
    "Enemy4",
    "Enemy5",
    "Enemy6"
};

struct EnemySpawnerData
{
    DirectX::SimpleMath::Vector2 relative_position;
    EnemyType type = EnemyType::ENEMY1;
    int spawn_count = 0;
    float spawn_interval = 0.f;
    float spawn_timer = 0.f;
};