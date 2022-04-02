#pragma once

#include "Sleet/Scene/Scene.h"

#include "entt/entt.hpp"

namespace Sleet {

	class Entity
	{
	public:
		Entity();
		Entity(entt::entity entityHandle, Scene* scene);

		inline entt::entity get_handle() { return entityHandle; }

		template<typename T, typename... Args>
		T& add_component(Args&&... args)
		{
			return scene->registry.emplace<T>(entityHandle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& get_component()
		{
			return scene->registry.get<T>(entityHandle);
		}

		template<typename T>
		void remove_component()
		{
			return scene->registry.remove<T>(entityHandle);
		}

		template<typename T>
		bool has_component()
		{
			return scene->registry.all_of<T>(entityHandle);
		}

	private:
		entt::entity entityHandle = entt::entity(0);
		Scene* scene = nullptr;
	};

}