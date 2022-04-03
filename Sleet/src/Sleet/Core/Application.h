#pragma once

#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/VulkanRenderer.h"
#include "Sleet/Vulkan/VulkanDescriptor.h"
#include "Sleet/Core/GameObject.h"
#include "Sleet/Core/Camera.h"
#include "Sleet/Core/Input.h"
#include "Sleet/Scene/Scene.h"
#include "Sleet/Scene/Entity.h"
#include "Sleet/Scene/Component.h"

namespace Sleet {

	class Application
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		void loadGameObjects();
		void loadScene();

		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
		VulkanDevice device{ window };
		VulkanRenderer renderer{ window, device };

		Scene scene;

		Ref<VulkanTexture> texture{};
		Scope<DescriptorPool> globalPool{};
		std::vector<GameObject> gameObjects;
	};

}