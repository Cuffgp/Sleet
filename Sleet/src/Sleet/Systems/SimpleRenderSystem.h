#pragma once

#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/FrameInfo.h"
#include "Sleet/Core/GameObject.h"
#include "Sleet/Core/Camera.h"
#include "Sleet/Scene/Scene.h"
#include "Sleet/Scene/Entity.h"
#include "Sleet/Scene/Component.h"

namespace Sleet {

	class SimpleRenderSystem
	{
	public:
		SimpleRenderSystem(VulkanDevice &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
		~SimpleRenderSystem();

		SimpleRenderSystem(const SimpleRenderSystem&) = delete;
		SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

		void renderGameObjects(FrameInfo &frameInfo, std::vector<GameObject> &gameObjects);
		void renderSceneObjects(FrameInfo& frameInfo, Scene& scene);

	private:
		void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
		void createPipeline(VkRenderPass renderPass);

		VulkanDevice &device;
		Scope<VulkanPipeline> pipeline;
		VkPipelineLayout pipelineLayout;
	};

}