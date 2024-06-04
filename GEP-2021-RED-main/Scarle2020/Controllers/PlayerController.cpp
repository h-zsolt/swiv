#include "pch.h"
#include "PlayerController.h"
#include <Data/ButtonData.h>
#include <Data/BulletData.h>

void PlayerController::init(ECS::ECS& ecs)
{
    // Create Signiture
    ECS::Signature psSig;
    psSig.set(ecs.getComponentBitflag<ButtonData>());
    psSig.set(ecs.getComponentBitflag<BulletData>());
    ecs.setSystemSignature<PlayerController>(psSig);
}

void PlayerController::update(ECS::ECS& ecs, float& /*delta_time*/)
{
    for (auto& entity : m_entities)
    {
        auto& button_data = ecs.getComponent<ButtonData>(entity);
        auto& bullet_data = ecs.getComponent<BulletData>(entity);
        
        bullet_data.is_fireing = button_data.fire_button_down;
    }
}
