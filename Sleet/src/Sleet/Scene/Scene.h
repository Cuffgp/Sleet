#pragma once

#include "entt/entt.hpp"
#include "Sleet/Scene/Component.h"

namespace Sleet {

	class Entity;

	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity createEntity(std::string tag);

		friend class Entity;
		entt::registry registry;
	private:


	};

}