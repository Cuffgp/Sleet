#pragma once

#include "Sleet/Vulkan/VulkanModel.h"
#include "Sleet/Vulkan/VulkanTexture.h"
#include "Sleet/Core/Camera.h"
//#include "Sleet/Core/KeyboardMovementController.h"

#include <string>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

namespace Sleet {

	struct TagComponent
	{
		std::string tag;
	};

	struct TransformComponent
	{
		glm::vec3 translation{};  // (position offset)
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct ModelComponent
	{
		Ref<VulkanModel> model;

		ModelComponent() = default;
		ModelComponent(const ModelComponent & other) = default;
		ModelComponent(const Ref<VulkanModel> & model)
			: model(model) {}

		operator Ref<VulkanModel>() { return model; }
	};

	struct TextureComponent
	{
		Ref<VulkanTexture> texture;

		TextureComponent() = default;
		TextureComponent(const TextureComponent& other) = default;
		TextureComponent(const Ref<VulkanTexture> & texture)
			: texture(texture) {}

		operator Ref<VulkanTexture>() { return texture; }
	};

	struct CameraComponent
	{

	};

	struct KeyboardMovementComponent
	{

	};
}