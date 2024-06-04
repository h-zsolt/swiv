#include "pch.h"
#include "CollisionSystem.h"

#include "EventBus.h"
#include "CollisionEvent.h"

// Components
#include <Components/TransformComponent.h>
#include <Data/Box2DColliderData.h>
#include <Data/TagData.h>

void CollisionSystem::init(ECS::ECS& ecs, EventBus* event_bus)
{
	ECS::Signature sig;
	sig.set(ecs.getComponentBitflag<TagData>());
	sig.set(ecs.getComponentBitflag<Box2DColliderData>());
	ecs.setSystemSignature<CollisionSystem>(sig);

	m_event_bus = event_bus;
}

void CollisionSystem::update(ECS::ECS& ecs, const float dt)
{
	for (auto& ent_a : m_entities)
	{
		for (auto& ent_b : m_entities)
		{
			if (ent_a == ent_b) continue; // skip this loop if entities are the same

			auto& cc_a = ecs.getComponent<Box2DColliderData>(ent_a);
			auto& cc_b = ecs.getComponent<Box2DColliderData>(ent_b);
			const auto& a_tag = ecs.getComponent<TagData>(ent_a);
			const auto& b_tag = ecs.getComponent<TagData>(ent_b);
			
			// check to see if collider can be collided with
			if (!cc_a.m_collidable || !cc_b.m_collidable) continue;

			// skip rest of loop if entities have collided with each other
			if (bitFlagCheck(cc_a.m_entities_collided, ent_b) || bitFlagCheck(cc_b.m_entities_collided, ent_a)) continue;

			// checks if entities should collide with each other
			if (a_tag.tag == b_tag.tag || bitFlagCheck(cc_a.m_ignore_tags, b_tag.tag) 
									   || bitFlagCheck(cc_b.m_ignore_tags, a_tag.tag))

			{
				continue;
			}

			// if collision occurs add to collision vector
			if (broadPhase(cc_a, cc_b) && narrowPhase(cc_a, cc_b))
			{
				// setup collisions to be handled outside of loop
 				m_entities_collided.push_back(std::make_pair(ent_a, ent_b));
				m_axis.push_back(m_axis_projection);
				m_overlaps.push_back(m_overlap);

				Bits::addToBitFlag(cc_a.m_entities_collided, ent_b);
				Bits::addToBitFlag(cc_b.m_entities_collided, ent_a);
				break;
			}
		}
	}

	// call a collision event if there are collisions to handle
	if (!m_entities_collided.empty())
	{
		size_t i = 0;
		for (auto& c : m_entities_collided)
		{
			CollisionEvent event = CollisionEvent(c.first, c.second, m_axis[i], m_overlaps[i], dt);
			m_event_bus->envoke(&event);
			++i;
		}

		resetCollisionFlags(ecs);

		m_entities_collided.clear();
		m_axis.clear();
		m_overlaps.clear();
	}
}

bool CollisionSystem::broadPhase(const Collider& cc_a, const Collider& cc_b)
{
	// create min and max co-ordinates for the colliders
	float a_minx = INFINITY;
	float a_miny = INFINITY;
	float a_maxx = -INFINITY;
	float a_maxy = -INFINITY;

	float b_minx = INFINITY;
	float b_miny = INFINITY;
	float b_maxx = -INFINITY;
	float b_maxy = -INFINITY;

	// calculate the min and max co-ordinates
	for (const auto& v : cc_a.m_vertices)
	{
		a_minx = (v.x < a_minx) ? v.x : a_minx;
		a_miny = (v.y < a_miny) ? v.y : a_miny;
		a_maxx = (v.x > a_maxx) ? v.x : a_maxx;
		a_maxy = (v.y > a_maxy) ? v.y : a_maxy;
	}

	for (const auto& v : cc_b.m_vertices)
	{
		b_minx = (v.x < b_minx) ? v.x : b_minx;
		b_miny = (v.y < b_miny) ? v.y : b_miny;
		b_maxx = (v.x > b_maxx) ? v.x : b_maxx;
		b_maxy = (v.y > b_maxy) ? v.y : b_maxy;
	}

	// expand the bounding boxes
	a_miny -= m_aabb_buffer;
	a_minx -= m_aabb_buffer;
	a_maxx += m_aabb_buffer;
	a_maxy += m_aabb_buffer;

	b_minx -= m_aabb_buffer;
	b_miny -= m_aabb_buffer;
	b_maxx += m_aabb_buffer;
	b_maxy += m_aabb_buffer;

	// do simple AABB collision detection
	if (a_maxx < b_minx || a_minx > b_maxx) return false;
	if (a_maxy < b_miny || a_miny > b_maxy)	return false;

	return true;
}
bool CollisionSystem::narrowPhase(const Collider& cc_a, const Collider& cc_b)
{
	auto* vertices_a = &cc_a.m_vertices; // NOLINT(cppcoreguidelines-init-variables)
	auto* vertices_b = &cc_b.m_vertices; // NOLINT(cppcoreguidelines-init-variables)
	float overlap = INFINITY;
	float a_px = 0.F; // used to store axis of projection for use after 'for loop'
	float a_py = 0.F; // used to store axis of projection for use after 'for loop'

	for (auto shape = 0; shape < 2; shape++)
	{
		if (shape == 1) // flip pointers to other vertices
		{
			vertices_a = &cc_b.m_vertices;
			vertices_b = &cc_a.m_vertices;
		}

		// only need to project along half of box colliders axis
		auto axis = 2;

		for (size_t i = 0; i < axis; i++) // currently goes through every axis of proj
		{
			size_t j = i + 1;

			// calculate the axis of projection
			Vector2 axis_projection = { -(vertices_a->at(j).y - vertices_a->at(i).y),
									vertices_a->at(j).x - vertices_a->at(i).x };

			float min_a = INFINITY;
			float min_b = INFINITY;
			float max_a = -INFINITY;
			float max_b = -INFINITY;

			// get the min and max points of the first colliders vertices
			for (const auto& v : *vertices_a)
			{
				float dot = (v.x * axis_projection.x + v.y * axis_projection.y); // scalar/dot product
				min_a = (dot < min_a) ? dot : min_a;
				max_a = (dot > max_a) ? dot : max_a;
			}

			for (auto& v : *vertices_b)
			{
				float dot = (v.x * axis_projection.x + v.y * axis_projection.y); // scalar/dot product
				min_b = (dot < min_b) ? dot : min_b;
				max_b = (dot > max_b) ? dot : max_b;
			}

			// calculate the minimum overlap
			auto o = std::min(std::min(max_a, max_b) - std::max(min_a, min_b), overlap);
			if (o < overlap)
			{
				// store the minimum overlap and axis of least penetration
				overlap = o;
				a_px = axis_projection.x;
				a_py = axis_projection.y;
			}

			// if one of the axis of axis_projection does not have an overlap return false
			if (!(max_b >= min_a && max_a >= min_b))
			{
				return false;
			}
		}
	}

	// set variables to be used in collision event
	m_axis_projection.x = a_px;
	m_axis_projection.y = a_py;
	m_overlap = overlap;

	// if all axis have an overlap then return true
	return true;
}

void CollisionSystem::resetCollisionFlags(ECS::ECS& ecs)
{
	for (auto& entity : m_entities_collided)
	{
		if(ecs.entityValid(entity.first))
		{
			auto& flag = ecs.getComponent<Box2DColliderData>(entity.first).m_entities_collided;
			flag = ECS::INVALID_ENTITY;
		}

		if(ecs.entityValid(entity.second))
		{
			auto& flag = ecs.getComponent<Box2DColliderData>(entity.second).m_entities_collided;
			flag = ECS::INVALID_ENTITY;
		}
	}
}
