#include "slpch.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Core/KeyboardMovementController.h"
#include "Sleet/Systems/SimpleRenderSystem.h"
#include "Sleet/Vulkan/VulkanBuffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace Sleet {

	struct GlobalUbo
	{
		glm::mat4 projectionView{ 1.f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
	};

	Application::Application()
	{
		globalPool = DescriptorPool::Builder(device)
			.setMaxSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadGameObjects();
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		std::vector<Scope<VulkanBuffer>> uboBuffers{ VulkanSwapchain::MAX_FRAMES_IN_FLIGHT };

		for (int i = 0; i < uboBuffers.size(); i++)
		{
			uboBuffers[i] = CreateScope<VulkanBuffer>(
				device,
				sizeof(GlobalUbo),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
				device.properties.limits.minUniformBufferOffsetAlignment);
			uboBuffers[i]->map();
		}

		auto globalSetLayout = DescriptorSetLayout::Builder(device)
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ 
			device, 
			renderer.getSwapchainRenderPass(), 
			globalSetLayout->getDescriptorSetLayout()};
		Camera camera{};

		auto viewerObject = GameObject::createGameObject();
		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!window.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);
			
			if (auto commandBuffer = renderer.beginFrame())
			{
				int frameIndex = renderer.getFrameIndex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera,
					globalDescriptorSets[frameIndex]
				};

				// Update
				GlobalUbo ubo{};
				ubo.projectionView = camera.getProjection() * camera.getView();
				uboBuffers[frameIndex]->writeToBuffer(&ubo);
				uboBuffers[frameIndex]->flush();

				// Render
				renderer.beginSwapchainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
				renderer.endSwapchainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(device.device());
	}

	void Application::loadGameObjects()
	{
		Ref<VulkanModel> model = VulkanModel::createModelFromFile(device, "assets/models/flat_vase.obj");

		auto gameObject = GameObject::createGameObject();
		gameObject.model = model;
		gameObject.transform.translation = { 0.f, 1.f, 2.5f };
		gameObject.transform.scale = { 5.f, 5.f, 5.f };

		gameObjects.push_back(std::move(gameObject));
	}

}