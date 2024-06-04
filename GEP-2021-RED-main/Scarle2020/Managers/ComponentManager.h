#pragma once

#include "pch.h"
#include "ECSDefinitions.h"
#include <Components/ComponentArray.h>

namespace ECS
{
	class ComponentManager
	{
	public:
		template<typename T>
		void registerComponent()
		{
			const char* typeName = typeid(T).name();

			assert(m_next_component_type <= MAX_COMPONENTS && "Max components have been registered.");
			assert(m_component_bitflags.find(typeName) == m_component_bitflags.end() && "Registering component type more than once.");

			m_component_bitflags.insert({ typeName, m_next_component_type });

			m_component_arrays.insert({ typeName, std::make_unique<ComponentArray<T>>() });

			++m_next_component_type;
		}

		template<typename T>
		ComponentBitflag getComponentBitflag()
		{
			const char* typeName = typeid(T).name();

			assert(m_component_bitflags.find(typeName) != m_component_bitflags.end() && "Component not registered before use.");

			return m_component_bitflags[typeName];
		}

		template<typename T>
		void addComponent(Entity entity, const T& component)
		{
			getComponentArray<T>()->insertData(entity, component);
		}

		template<typename T> 
		void removeComponent(Entity entity)
		{
			getComponentArray<T>()->removeData(entity);
		}

		template<typename T>
		T& getComponent(Entity entity)
		{
			return getComponentArray<T>()->getData(entity);
		}

		template<typename T>
		const T& getComponent(Entity entity) const
		{
			return getComponentArray<T>()->getData(entity);
		}

		template<typename T>
		bool componentExists(Entity entity)
		{
			return getComponentArray<T>()->valid(entity);
		}

		void entityDestroyed(Entity entity)
		{
			for (const auto& pair : m_component_arrays)
			{
				const auto& component = pair.second;
				component->entityDestroyed(entity);
			}
		}

		void clearMaps()
		{
			for (const auto& pair : m_component_arrays)
			{
				const auto& component = pair.second;
				component->clearMaps();
			}
		}

	private:
		template<typename T>
		ComponentArray<T>* getComponentArray()
		{
			const char* typeName = typeid(T).name();

			assert(m_component_bitflags.find(typeName) != m_component_bitflags.end() && "Component not registered before use.");

			return dynamic_cast<ComponentArray<T>*>(m_component_arrays[typeName].get()); // Changed from return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
		}

	private:
		std::unordered_map<const char*, ComponentBitflag> m_component_bitflags;
		std::unordered_map<const char*, std::unique_ptr<IComponentArray>> m_component_arrays; // This was shared_ptr
		ComponentBitflag m_next_component_type = 0;
	};
}