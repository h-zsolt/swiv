#include "pch.h"
#include "EnvironmentSpawnSystem.h"
#include <Components/TransformComponent.h>
#include <Data/EnvironmentTag.h>
#include "../../Game/Game/EntityFactory.h"

void EnvironmentSpawnSystem::init(ECS::ECS& ecs, IRenderer& /*renderer*/, ID3D11Device1* /*device*/)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<TransformComponent>());
	sig.set(ecs.getComponentBitflag<EnvironmentTag>());
	sig.set(ecs.getComponentBitflag<SpriteComponent>());
	ecs.setSystemSignature<EnvironmentSpawnSystem>(sig);

}

void EnvironmentSpawnSystem::spawnEnvironment(ECS::ECS& ecs, IRenderer& renderer)
{
	for (auto& entity : m_entities)
	{
		auto& transform = ecs.getComponent<TransformComponent>(entity);
		auto& sprite	= ecs.getComponent<SpriteComponent>(entity);

		EntityFactory::spawnEnvironment(ecs, renderer, entity, transform, sprite);
	}
}
