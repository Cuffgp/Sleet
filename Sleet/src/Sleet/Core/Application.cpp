#include "slpch.h"

#include "Sleet/Core/Application.h"

namespace Sleet {

	Application::Application()
	{
		loadModels();
		createPipelineLayout();
		recreateSwapchain();
		createCommandBuffers();
	}

	Application::~Application()
	{
		vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
	}

	void Application::run()
	{
		while (!window.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(device.device());
	}

	void Application::loadModels()
	{
		std::vector<VulkanModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0, 0.0, 0.0}},
			{{0.5f, 0.5f}, {0.0, 1.0, 0.0}},
			{{-0.5f, 0.5f}, {0.0, 0.0, 1.0}}
		};
		model = CreateScope<VulkanModel>(device, vertices);
	}

	void Application::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
			VK_SUCCESS) 
		{
			SL_ERROR("Failed to create pipeline layout!");
		}
	}

	void Application::createPipeline()
	{
		PipelineConfigInfo pipelineConfig{};
		VulkanPipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = swapchain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = CreateScope<VulkanPipeline>(
			device,
			"assets/shaders/simple_shader.vert.spv",
			"assets/shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void Application::recreateSwapchain()
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
			swapchain = CreateScope<VulkanSwapchain>(device, extent, std::move(swapchain));
			if (swapchain->imageCount() != commandBuffers.size()) 
			{
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline();
	}

	void Application::createCommandBuffers()
	{
		commandBuffers.resize(swapchain->imageCount());

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

	void Application::freeCommandBuffers()
	{
		vkFreeCommandBuffers(
			device.device(),
			device.getCommandPool(),
			static_cast<uint32_t>(commandBuffers.size()),
			commandBuffers.data());
		commandBuffers.clear();
	}

	void Application::recordCommandBuffer(int imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			SL_ERROR("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = swapchain->getRenderPass();
		renderPassInfo.framebuffer = swapchain->getFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapchain->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapchain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(swapchain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		VkRect2D scissor{ {0, 0}, swapchain->getSwapChainExtent() };
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		pipeline->bind(commandBuffers[imageIndex]);
		model->bind(commandBuffers[imageIndex]);
		model->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
		{
			SL_ERROR("Failed to record command buffer!");
		}
	}

	void Application::drawFrame()
	{
		uint32_t imageIndex;
		auto result = swapchain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapchain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
		{
			SL_ERROR("Failed to acquire swap chain image!");
		}

		recordCommandBuffer(imageIndex);
		result = swapchain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized())
		{
			window.resetWindowResizedFlag();
			recreateSwapchain();
			return;
		}

		if (result != VK_SUCCESS) 
		{
			SL_ERROR("Failed to present swap chain image!");
		}
	}
}