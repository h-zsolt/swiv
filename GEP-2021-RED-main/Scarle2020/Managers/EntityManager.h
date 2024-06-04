#pragma once

#include "pch.h"
#include "ECSDefinitions.h"

namespace ECS
{
	class EntityManager
	{
	public:
		EntityManager();

		Entity createEntity();
		void destroyEntity(Entity entity);
		void setSignature(Entity entity, Signature signature);
		Signature getSignature(Entity entity);
		bool entityValid(const Entity entity) const;
		void destroyAllEntities()
		{
			while (!m_entities_in_use.empty())
				destroyEntity(m_entities_in_use.back());
		}

	private:
		std::deque<Entity> m_available_entities			 = {};
		std::array<Signature, MAX_ENTITIES> m_signatures = {};
		std::vector<Entity> m_entities_in_use			 = {};
		uint32_t m_total_living_entities				 = 0;
	};
}