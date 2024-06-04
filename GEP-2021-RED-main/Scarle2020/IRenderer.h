#pragma once
#include "pch.h"

struct SpriteComponent;
struct TransformComponent;

class IRenderer
{
public:
	IRenderer() = default;
	virtual ~IRenderer() = default;

	virtual void setupSpriteComponent(size_t texture_id, SpriteComponent& sprite_component) = 0;
	virtual DirectX::SimpleMath::Vector2 calculateSpriteExtent(const TransformComponent& parent_transform, const SpriteComponent& sprite) = 0;

	virtual DirectX::SimpleMath::Vector2 getGameCameraPosition2D() const = 0;
	virtual size_t loadTexture(ID3D11Device* device, const std::string& filepath) = 0;
};

