#include "pch.h"
#include "EntityManager.h"

ECS::EntityManager::EntityManager()
{
	for (Entity ent = 1; ent < MAX_ENTITIES; ++ent)
	{
		m_available_entities.push_back(ent);
	}
}

ECS::Entity ECS::EntityManager::createEntity()
{
	assert(m_total_living_entities < MAX_ENTITIES && "Too many entities in existence.");

	// Take entity from front of the entity queue.
	Entity id = m_available_entities.front();
	m_available_entities.pop_front();

	// Add to vector of in use entities
	m_entities_in_use.push_back(id);
	++m_total_living_entities;

	return id;
}

void ECS::EntityManager::destroyEntity(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range");

	// Invalidate entity's signature.
	m_signatures[entity].reset();

	// Put the destroyed entity to the back of the queue.
	m_available_entities.push_back(entity);

	// remove from currently in use entities
	auto itr = std::find(m_entities_in_use.cbegin(), m_entities_in_use.cend(), entity);
	if (itr != m_entities_in_use.cend())
	{
		m_entities_in_use.erase(itr);
	}

	if (m_total_living_entities > 0)
	{
		--m_total_living_entities;
	}
}

void ECS::EntityManager::setSignature(Entity entity, Signature signature)
{
	assert(entity < MAX_ENTITIES && "Entity out of range");

	// Put this entity's signature into the array.
	m_signatures[entity] = signature;
}

ECS::Signature ECS::EntityManager::getSignature(Entity entity)
{
	assert(entity < MAX_ENTITIES && "Entity out of range");

	// Get this entity's signature from the array.
	return m_signatures[entity];
}

bool ECS::EntityManager::entityValid(const Entity entity) const
{
	auto itr = std::find(m_available_entities.cbegin(), m_available_entities.cend(), entity);
	return itr == m_available_entities.cend();
}

