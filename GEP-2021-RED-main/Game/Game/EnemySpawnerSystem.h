#pragma once
#include "pch.h"
#include "ECS.h"
#include "Data/EnemySpawnerData.h"

struct EnemySpawnerData;
struct TransformComponent;
class IRenderer;

class EnemySpawnerSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs, IRenderer& renderer, ID3D11Device1* device);
	void update(ECS::ECS& ecs, IRenderer& renderer, float& delta_time);

	void deleteAllEnemies(ECS::ECS& ecs);

private:
	std::map<EnemyType, size_t> sprite_id_map;
	std::vector<ECS::Entity> enemy_list;
};

