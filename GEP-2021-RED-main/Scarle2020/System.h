#pragma once

#include <pch.h>
#include "ECSDefinitions.h"

namespace ECS
{
	class System
	{
	public:
		virtual ~System() = default;

		virtual void clearEntities() { m_entities.clear(); }
		const std::set<Entity>& getEntities() const { return m_entities; }
		std::set<Entity>& getEntities() { return m_entities; }

	protected:
		// List of unique entities being tracked by the system.
		std::set<Entity> m_entities;
	};
}
