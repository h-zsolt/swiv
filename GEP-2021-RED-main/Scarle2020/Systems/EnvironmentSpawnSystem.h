#pragma once

#include "ECS.h"
#include <map>
#include <Data/EnvironmentTag.h>


class IRenderer;

class EnvironmentSpawnSystem : public ECS::System
{
public:
	void init(ECS::ECS& ecs, IRenderer& renderer, ID3D11Device1* device);
	void spawnEnvironment(ECS::ECS& ecs, IRenderer& renderer);
};

