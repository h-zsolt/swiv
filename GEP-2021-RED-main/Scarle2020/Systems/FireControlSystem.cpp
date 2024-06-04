#include "pch.h"
#include "FireControlSystem.h"
#include "Tags.h"
#include "Data/BulletData.h"

void FireControlSystem::init(ECS::ECS& ecs)
{
	ECS::Signature psSig;
	//psSig.set(ecs.getComponentBitflag<HoldFireTag>());
	//psSig.set(ecs.getComponentBitflag<StartFireTag>());
	psSig.set(ecs.getComponentBitflag<BulletData>());
	ecs.setSystemSignature<FireControlSystem>(psSig);
}

void FireControlSystem::update(ECS::ECS& ecs)
{
	for (auto& entity : m_entities)
	{
		if (ecs.hasComponent<HoldFireTag>(entity) && ecs.hasComponent<BulletData>(entity))
		{
			ecs.getComponent<BulletData>(entity).is_fireing = false;
		}
		//prioritises the StartFireTag over HoldFire, so if everything gets HoldFire and some get StartFire, some stuff will still fire
		//useful for FixedShootingPoint system, since 
		if (ecs.hasComponent<StartFireTag>(entity) && ecs.hasComponent<BulletData>(entity))
		{
			ecs.getComponent<BulletData>(entity).is_fireing = true;
		}
		//this is here so entities get added only once
		if (ecs.hasComponent<StartFireTag>(entity) || ecs.hasComponent<HoldFireTag>(entity))
		{
			m_entities_to_remove_from.push_back(entity);
		}
	}
	removeComponents(ecs);
}

void FireControlSystem::removeComponents(ECS::ECS& ecs)
{
	//this way of removal is quite CPU heavy because of the amount of if checks, needs to be optimized
	for (auto& entity : m_entities_to_remove_from)
	{
		if (ecs.hasComponent<StartFireTag>(entity))
		{
			ecs.removeComponent<StartFireTag>(entity);
		}

		if (ecs.hasComponent<HoldFireTag>(entity))
		{
			ecs.removeComponent<HoldFireTag>(entity);
		}
	}
}
