#pragma once

#include "pch.h"
#include "ECSDefinitions.h"

namespace ECS
{
	// Component array interface
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void entityDestroyed(Entity entity) = 0;
		virtual void clearMaps()					= 0;
	};

	template<typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		~ComponentArray() final = default;
		void insertData(Entity entity, const T& component)
		{
			assert(m_entity_to_index.find(entity) == m_entity_to_index.end() && "Component added to same entity more than once.");

			size_t newIndex = m_num_components;
			m_entity_to_index[entity] = newIndex;
			m_index_to_entity[newIndex] = entity;
			m_components[newIndex] = component;
			++m_num_components;
		}

		void removeData(Entity entity)
		{
			assert(m_entity_to_index.find(entity) != m_entity_to_index.end() && "Removing non-existent component.");

			// Copy last element into "deleted" element. "Move"
			size_t index_of_removed_entity = m_entity_to_index[entity];
			size_t index_of_last_element = m_num_components - 1;
			m_components[index_of_removed_entity] = m_components[index_of_last_element];

			// Update map to point to "moved" element.
			Entity entity_of_last_element = m_index_to_entity[index_of_last_element];
			m_entity_to_index[entity_of_last_element] = index_of_removed_entity;
			m_index_to_entity[index_of_removed_entity] = entity_of_last_element;

			m_entity_to_index.erase(entity);
			m_index_to_entity.erase(index_of_last_element);

			--m_num_components;
		}

		T& getData(Entity entity)
		{
			assert(m_entity_to_index.find(entity) != m_entity_to_index.end() && "Retrieving non-existent component.");

			return m_components[m_entity_to_index[entity]];
		}

		void entityDestroyed(Entity entity) override
		{
			if (m_entity_to_index.find(entity) != m_entity_to_index.end())
			{
				removeData(entity);
			}
		}
		
		void clearMaps() override
		{
			m_entity_to_index.clear();
			m_index_to_entity.clear();
		}

		bool valid(Entity entity)
		{
			return m_entity_to_index.find(entity) != m_entity_to_index.end();
		}

	private:
		std::array<T, MAX_ENTITIES> m_components = {};

		std::unordered_map<Entity, size_t> m_entity_to_index = {};

		std::unordered_map<size_t, Entity> m_index_to_entity = {};

		size_t m_num_components = 0;
	};
}