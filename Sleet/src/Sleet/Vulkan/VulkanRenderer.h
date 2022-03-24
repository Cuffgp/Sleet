#pragma once

#include "Sleet/Core/Base.h"
#include "Sleet/Core/Window.h"
#include "Sleet/Core/Log.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanSwapchain.h"

namespace Sleet {

	class VulkanRenderer
	{
	public:
		VulkanRenderer(Window& window, VulkanDevice& device);
		~VulkanRenderer();

		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;

		VkRenderPass getSwapchainRenderPass() const { return swapchain->getRenderPass(); }
		bool isFrameInProgress() const { return isFrameStarted; }
		VkCommandBuffer getCurrentCommandBuffer() 
		{
			SL_ASSERT(isFrameStarted, "Cannot get command buffer while frame is not in progress.");
			return commandBuffers[currentFrameIndex];
		}

		int getFrameIndex() const 
		{
			SL_ASSERT(isFrameStarted, "Cannot get frame index when frame not in progress");
			return currentFrameIndex;
		}

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapchainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapchainRenderPass(VkCommandBuffer commandBuffer);

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapchain();

		Window& window;
		VulkanDevice& device;
		Scope<VulkanSwapchain> swapchain;
		std::vector<VkCommandBuffer> commandBuffers;

		uint32_t currentImageIndex = 0;
		int currentFrameIndex = 0;
		bool isFrameStarted = false;
	};

}