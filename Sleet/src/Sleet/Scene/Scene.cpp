#include "slpch.h"

#include "Sleet/Scene/Scene.h"
#include "Sleet/Scene/Entity.h"

namespace Sleet {

	Scene::Scene()
	{}

	Scene::~Scene()
	{}

	Entity Scene::createEntity(std::string tag)
	{
		Entity entity(registry.create(), this);
		entity.addComponent<TagComponent>(tag);
		return entity;
	}


}