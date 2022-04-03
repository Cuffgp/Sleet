#pragma once

#include "Sleet/Scene/Scene.h"

#include "entt/entt.hpp"

namespace Sleet {

	class Entity
	{
	public:
		Entity();
		Entity(entt::entity entityHandle, Scene* scene);

		inline entt::entity getHandle() { return entityHandle; }

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			SL_ASSERT(!hasComponent<T>(), "Entity already has component");
			return scene->registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& getComponent()
		{
			SL_ASSERT(hasComponent<T>(), "Entity does not have a component");
			return scene->registry.get<T>(entityHandle);
		}

		template<typename T>
		void removeComponent()
		{
			SL_ASSERT(hasComponent<T>(), "Entity does not have a component");
			return scene->registry.remove<T>(entityHandle);
		}

		template<typename T>
		bool hasComponent()
		{
			return scene->registry.all_of<T>(entityHandle);
		}

	private:
		entt::entity entityHandle = entt::entity(0);
		Scene* scene = nullptr;
	};

}