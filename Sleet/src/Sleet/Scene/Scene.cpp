#include "slpch.h"

#include "Sleet/Scene/Scene.h"
#include "Sleet/Scene/Entity.h"
#include "Sleet/Scene/Component.h"

namespace Sleet {

	Scene::Scene()
	{}

	Scene::~Scene()
	{}

	Entity Scene::createEntity(std::string tag)
	{
		Entity entity(registry.create(), this);
		entity.add_component<TagComponent>(tag);
		return entity;
	}


}