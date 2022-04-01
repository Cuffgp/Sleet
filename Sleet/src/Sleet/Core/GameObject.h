#pragma once

#include "Sleet/Core/Base.h"
#include "Sleet/Vulkan/VulkanModel.h"
#include "Sleet/Vulkan/VulkanTexture.h"

#include "glm/gtc/matrix_transform.hpp"

namespace Sleet {

	struct TransformComponent 
	{
		glm::vec3 translation{};  // (position offset)
		glm::vec3 scale{ 1.f, 1.f, 1.f};
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();

	};

	class GameObject
	{
	public:
		using id_t = unsigned int;

		static GameObject createGameObject()
		{
			static id_t currentId = 0;
			return GameObject(currentId++);
		}

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		id_t getId() const { return id; }

		// Game object components
		TransformComponent transform{};
		glm::vec3 color{};
		Ref<VulkanModel> model{};
		Ref<VulkanTexture> texture{};

	private:
		GameObject(id_t objId) : id(objId) {}

		id_t id;

	};


}