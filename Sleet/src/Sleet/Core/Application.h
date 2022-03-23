#pragma once


#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/VulkanSwapchain.h"

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
		VulkanDevice device{ window };
		VulkanSwapchain swapchain{ device, window.getExtent() };
		Scope<VulkanPipeline> pipeline;

		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};

}