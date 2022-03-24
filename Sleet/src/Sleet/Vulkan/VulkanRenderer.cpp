#include "slpch.h"

#include "Sleet/Vulkan/VulkanRenderer.h"

namespace Sleet {

	VulkanRenderer::VulkanRenderer(Window& window, VulkanDevice& device):
		window(window), device(device)
	{
		recreateSwapchain();
		createCommandBuffers();
	}

	VulkanRenderer::~VulkanRenderer()
	{
		freeCommandBuffers();
	}

	void VulkanRenderer::recreateSwapchain()
	{
		auto extent = window.getExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = window.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device.device());
		if (swapchain == nullptr)
		{
			swapchain = CreateScope<VulkanSwapchain>(device, extent);
		}
		else
		{
			Ref<VulkanSwapchain> oldSwapchain = std::move(swapchain);
			swapchain = CreateScope<VulkanSwapchain>(device, extent, oldSwapchain);

			if (!oldSwapchain->compareSwapchainFormats(*swapchain.get()))
			{
				SL_ERROR("Swapchain image or depth format has changed.");
			}

		}
	}

	void VulkanRenderer::createCommandBuffers()
	{
		commandBuffers.resize(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS)
		{
			SL_ERROR("Failed to allocate command buffers!");
		}
	}

	void VulkanRenderer::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			device.device(),
			device.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	VkCommandBuffer VulkanRenderer::beginFrame()
	{
		SL_ASSERT(!isFrameInProgress(), "Cannot begin frame while still in progress!")

		auto result = swapchain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			SL_ERROR("Failed to acquire swap chain image!");
		}

		isFrameStarted = true;
		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			SL_ERROR("Failed to begin recording command buffer!");
		}

		return commandBuffer;
	}

	void VulkanRenderer::endFrame()
	{
		SL_ASSERT(isFrameStarted, "Cannot call endFrame while frame is not in progress");
		auto commandBuffer = getCurrentCommandBuffer();

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			SL_ERROR("Failed to record command buffer!");
		}

		auto result = swapchain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
		{
			window.resetWindowResizedFlag();
			recreateSwapchain();
		}

		if (result != VK_SUCCESS)
		{
			SL_ERROR("Failed to present swap chain image!");
		}

		isFrameStarted = false;
		currentFrameIndex = (currentFrameIndex + 1) % VulkanSwapchain::MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::beginSwapchainRenderPass(VkCommandBuffer commandBuffer)
	{
		SL_ASSERT(isFrameStarted, "Can't call beginSwapchainRenderPass if frame is not in progress.");
		SL_ASSERT(commandBuffer == getCurrentCommandBuffer(), "Can't begin render pass on command buffer from different frame.");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->getRenderPass();
		renderPassInfo.framebuffer = swapchain->getFrameBuffer(currentImageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapchain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);


	}

	void VulkanRenderer::endSwapchainRenderPass(VkCommandBuffer commandBuffer)
	{
		SL_ASSERT(isFrameStarted, "Can't call endSwapchainRenderPass if frame is not in progress.");
		SL_ASSERT(commandBuffer == getCurrentCommandBuffer(), "Can't end render pass on command buffer from different frame.");

		vkCmdEndRenderPass(commandBuffer);
	}
}