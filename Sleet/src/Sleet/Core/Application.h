#pragma once


#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/VulkanSwapchain.h"
#include "Sleet/Core/GameObject.h"

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
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapchain();
		void recordCommandBuffer(int imageIndex);
		void renderGameObjects(VkCommandBuffer commandBuffer);

		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
		VulkanDevice device{ window };
		Scope<VulkanSwapchain> swapchain;
		Scope<VulkanPipeline> pipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<GameObject> gameObjects;
	};

}