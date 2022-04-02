#include "slpch.h"

#include "Sleet/Scene/Entity.h"

namespace Sleet {

	Entity::Entity(entt::entity entityHandle, Scene* scene)
		: entityHandle(entityHandle), scene(scene)
	{
	}

}