#include "pch.h"
#include "EnemySpawnerSystem.h"
#include "Data/EnemySpawnerData.h"
#include "EntityFactory.h"
#include "IRenderer.h"

void EnemySpawnerSystem::init(ECS::ECS& ecs, IRenderer& renderer, ID3D11Device1* device)
{
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<EnemySpawnerData>());
    ecs.setSystemSignature<EnemySpawnerSystem>(psSig);

    sprite_id_map[EnemyType::ENEMY1] = renderer.loadTexture(device, "Assets/enemyBlack1.dds");
    sprite_id_map[EnemyType::ENEMY2] = renderer.loadTexture(device, "Assets/enemyBlack5.dds");
    sprite_id_map[EnemyType::ENEMY3] = renderer.loadTexture(device, "Assets/enemyBlack1.dds");
    sprite_id_map[EnemyType::ENEMY4] = renderer.loadTexture(device, "Assets/enemyBlack1.dds");
    sprite_id_map[EnemyType::ENEMY5] = renderer.loadTexture(device, "Assets/enemyBlack1.dds");
    sprite_id_map[EnemyType::ENEMY6] = renderer.loadTexture(device, "Assets/enemyRed4.dds");
}

void EnemySpawnerSystem::update(ECS::ECS& ecs, IRenderer& renderer, float& delta_time)
{
    std::vector<ECS::Entity> expended_spawners;

    for (auto& entity : m_entities)
    {
        auto& transform = ecs.getComponent<TransformComponent>(entity);
        // NOTE :: CAMERA POSITION IS TOP OF SCREEN
        auto distance = abs(-renderer.getGameCameraPosition2D().y - transform.m_position.y);

        // check the sawner dist to camera
        if (distance < 10 || renderer.getGameCameraPosition2D().y > -transform.m_position.y)
        {   //if enemy spawns on screen through spawners or for bossfight the distance isn't enough so edited this slightly, sorry
            // get data and update the timer
            auto& spawner_data = ecs.getComponent<EnemySpawnerData>(entity);
            spawner_data.spawn_timer -= delta_time;

            if (spawner_data.spawn_timer > 0)
            {
                continue;
            }

            auto enemy = EntityFactory::spawnEnemy(ecs, renderer, spawner_data, transform, sprite_id_map[spawner_data.type]);
            enemy_list.push_back(enemy);
            spawner_data.spawn_timer = spawner_data.spawn_interval;

            // reduce count and add to list if it needs to be removed 
            if (--spawner_data.spawn_count == 0)
            {
                expended_spawners.push_back(entity);
                continue;
            }
        }
    }

    // delete the spawners
    while (!expended_spawners.empty())
    {
        ecs.destroyEntity(expended_spawners.back());
        expended_spawners.pop_back();
    }
    expended_spawners.clear();
}

void EnemySpawnerSystem::deleteAllEnemies(ECS::ECS& ecs)
{
    for (auto& enemy : enemy_list)
    {
        ecs.destroyEntity(enemy);
    }
}


