#pragma once

#include "pch.h"
#include <Managers/ComponentManager.h>
#include <Managers/EntityManager.h>
#include <Managers/SystemManager.h>

namespace ECS
{
	class ECS
	{
	public:
		void init();

		Entity createEntity();
		void destroyEntity(Entity entity);
		void finalDestroyEntity(Entity entity);
		bool entityValid(const Entity entity) const
		{
			return m_entity_manager->entityValid(entity);
		}

		void destroyAllEntities()
		{
			m_entity_manager->destroyAllEntities();
			m_component_manager->clearMaps();
			m_system_manager->clearEntities();
		}

		// Component methods.
		template<typename T>
		void registerComponent()
		{
			m_component_manager->registerComponent<T>();
		}

		template<typename T>
		void addComponent(Entity entity, const T& component)
		{
			m_component_manager->addComponent<T>(entity, component);

			// Make this entity have the component, represented in the entity's signature.
			auto signature = m_entity_manager->getSignature(entity);
			signature.set(m_component_manager->getComponentBitflag<T>(), true);
			m_entity_manager->setSignature(entity, signature);

			m_system_manager->entitySignatureChanged(entity, signature);
		}

		template<typename T>
		void removeComponent(Entity entity)
		{
			m_component_manager->removeComponent<T>(entity);

			// Remove the component from the entity. Update the entity's signature.
			auto signature = m_entity_manager->getSignature(entity);
			signature.set(m_component_manager->getComponentBitflag<T>(), false);
			m_entity_manager->setSignature(entity, signature);

			m_system_manager->entitySignatureChanged(entity, signature);
		}

		template<typename T>
		T& getComponent(Entity entity)
		{
			return m_component_manager->getComponent<T>(entity);
		}

		template<typename T>
		bool hasComponent(Entity entity)
		{
			return m_component_manager->componentExists<T>(entity);
		}

		template<typename T>
		ComponentBitflag getComponentBitflag()
		{
			return m_component_manager->getComponentBitflag<T>();
		}

		// System methods.
		template<typename T>
		T* registerSystem()
		{
			return m_system_manager->registerSystem<T>();
		}

		template<typename T>
		void setSystemSignature(Signature signature)
		{
			m_system_manager->setSignature<T>(signature);
		}

	private:
		std::unique_ptr<ComponentManager> m_component_manager;
		std::unique_ptr<EntityManager> m_entity_manager;
		std::unique_ptr<SystemManager> m_system_manager;
	};
}