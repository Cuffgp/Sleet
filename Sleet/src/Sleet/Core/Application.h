#pragma once

#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/VulkanRenderer.h"
#include "Sleet/Vulkan/VulkanDescriptor.h"
#include "Sleet/Core/GameObject.h"
#include "Sleet/Core/Camera.h"

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

		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
		VulkanDevice device{ window };
		VulkanRenderer renderer{ window, device };

		Ref<VulkanTexture> texture{};
		Scope<DescriptorPool> globalPool{};
		std::vector<GameObject> gameObjects;
	};

}