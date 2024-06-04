#pragma once

#include "ECS.h"

class ColliderMovementSystem : public ECS::System
{
private: // TypeDefs //
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Vertices = std::array<Vector2, 4>;

public: // Public Functions //
	void init(ECS::ECS& ecs);
	void update(ECS::ECS& ecs, float dt);

private: // Private Functions //
	void nextPos(Vector2& current_position, Vector2& next_pos, Vertices& vertices,  float rotation_radians);
};
