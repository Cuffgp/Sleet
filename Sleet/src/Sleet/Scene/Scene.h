#pragma once

#include "entt/entt.hpp"

namespace Sleet {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity(std::string tag);

		friend class Entity;
	private:
		entt::registry registry;


	};

}