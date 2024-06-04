#pragma once

#include "pch.h"
#include "TagData.h"
#include "ECSDefinitions.h"

//  vertex order
//  v1 ------ v2
//  |          |
//  |          |
//  |          |
//  v4 ------ v3

struct Box2DColliderData
{
	using Vector2 = DirectX::SimpleMath::Vector2;
	Box2DColliderData() = default;
	Box2DColliderData(const Vector2& origin, const Vector2& half_size) : m_previous_pos(origin), m_half_size(half_size)
	{
		// makes the colliders slightly smaller than sprite extents passed in
		Vector2 hs	  = { m_half_size.x - 0.5F, m_half_size.y - 0.5F };
		m_vertices[0] = { m_previous_pos - hs };
		m_vertices[1] = { m_previous_pos.x + hs.x, m_previous_pos.y - hs.y };
		m_vertices[2] = { m_previous_pos + hs };
		m_vertices[3] = { m_previous_pos.x - hs.x, m_previous_pos.y + hs.y };
	}

	std::array<Vector2, 4> m_vertices = {};
	Vector2 m_previous_pos			  = { 0.F, 0.F };
	Vector2 m_half_size				  = { 0.F, 0.F };
	TagID m_ignore_tags				  = TagID::NULL_TAG;     // used as a bitflag requires use of bit shifting
	ECS::Entity m_entities_collided	  = ECS::INVALID_ENTITY; // used as a bitflag requires use of bit shifting
	bool m_collidable				  = true;
};
