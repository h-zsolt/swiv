#pragma once

#include "pch.h"
#include "ECSDefinitions.h"
#include "System.h"

namespace ECS
{
	class SystemManager
	{
	public:
		template<typename T>
		T* registerSystem()
		{
			const char* typeName = typeid(T).name();

			assert(m_systems.find(typeName) == m_systems.end() && "Registering system more than once.");

			// Create a new system and insert it into the map of systems
			m_systems.insert({ typeName, std::make_unique<T>()});

			return dynamic_cast<T*>(m_systems[typeName].get());
		}

		template <typename T>
		void setSignature(const Signature& signature)
		{
			const char* typeName = typeid(T).name();

			assert(m_systems.find(typeName) != m_systems.end() && "System used before register.");

			m_signatures.insert({ typeName, signature });
		}

		void entityDestroyed(Entity entity)
		{
			for (const auto& pair : m_systems)
			{
				const auto& system = pair.second;

				system->getEntities().erase(entity);
			}
		}

		void entitySignatureChanged(Entity entity, Signature entitySignature)
		{
			for (const auto& pair : m_systems)
			{
				const auto& type = pair.first;
				const auto& system = pair.second;
				const auto& systemSignature = m_signatures[type];

				// Entity signature matches system signature. Add the entity to the system's tracked entities.
				if ((entitySignature & systemSignature) == systemSignature)
				{
					system->getEntities().insert(entity);
				}
				// Entity does not have the required components to be supported by this system. Remove entity from system.
				else
				{
					system->getEntities().erase(entity);
				}
			}
		}

		void clearEntities()
		{
			for (const auto& pair : m_systems)
			{
				const auto& system = pair.second;
				system->clearEntities();
			}
		}

	private:
		std::unordered_map<const char*, Signature> m_signatures = {};
		std::unordered_map<const char*, std::unique_ptr<System>> m_systems = {};
	};
}