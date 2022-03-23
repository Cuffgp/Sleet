#include "slpch.h"

#include "Sleet/Core/Application.h"

namespace Sleet {

	Application::Application()
	{
		createPipelineLayout();
		createPipeline();
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
		}
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
		VulkanPipeline::defaultPipelineConfigInfo(
			pipelineConfig,
			swapchain.width(),
			swapchain.height());
		pipelineConfig.renderPass = swapchain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		pipeline = CreateScope<VulkanPipeline>(
			device,
			"assets/shaders/simple_shader.vert.spv",
			"assets/shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void Application::createCommandBuffers()
	{

	}

	void Application::drawFrame()
	{

	}
}