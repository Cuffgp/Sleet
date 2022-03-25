#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Core/GameObject.h"
#include "Sleet/Core/Camera.h"

namespace Sleet {

	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(VulkanDevice &device, VkRenderPass renderPass);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject> &gameObjects, const Camera &camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice &device;
		Scope<VulkanPipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};

}