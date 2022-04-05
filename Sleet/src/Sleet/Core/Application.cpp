#include "slpch.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Core/KeyboardMovementController.h"
#include "Sleet/Systems/SimpleRenderSystem.h"
#include "Sleet/Systems/ImguiSystem.h"
#include "Sleet/Vulkan/VulkanBuffer.h"
#include "Sleet/Vulkan/VulkanTexture.h"

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
		Input::init(window.getGLFWwindow());

		globalPool = DescriptorPool::Builder(device)
			.setMaxSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
			.addPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VulkanSwapchain::MAX_FRAMES_IN_FLIGHT)
			.build();
		loadScene();
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
			.addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.addBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1)
			.build();

		std::vector<VkDescriptorSet> globalDescriptorSets(VulkanSwapchain::MAX_FRAMES_IN_FLIGHT);
		for (int i = 0; i < globalDescriptorSets.size(); i++)
		{
			auto bufferInfo = uboBuffers[i]->descriptorInfo();
			auto textureInfo = texture->descriptorInfo();
			DescriptorWriter(*globalSetLayout, *globalPool)
				.writeBuffer(0, &bufferInfo)
				.writeImage(1, &textureInfo)
				.build(globalDescriptorSets[i]);
		}

		SimpleRenderSystem simpleRenderSystem{ 
			device, 
			renderer.getSwapchainRenderPass(), 
			globalSetLayout->getDescriptorSetLayout()
		};

		ImguiSystem imguiSystem{
			window,
			device,
			renderer.getSwapchainRenderPass(),
			VulkanSwapchain::MAX_FRAMES_IN_FLIGHT
		};

		Entity viewerEntity = scene.createEntity("viewer");
		viewerEntity.addComponent<TransformComponent>();
		viewerEntity.addComponent<CameraComponent>();

		auto camera = viewerEntity.getComponent<CameraComponent>().camera;
		auto transform = viewerEntity.getComponent<TransformComponent>();

		KeyboardMovementController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!window.shouldClose())
		{
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;


			cameraController.moveInPlaneXZ(frameTime, transform);
			camera.setViewYXZ(transform.translation, transform.rotation);

			float aspect = renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 100.f);
			
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
				imguiSystem.beginFrame();
				renderer.beginSwapchainRenderPass(commandBuffer);

				simpleRenderSystem.renderSceneObjects(frameInfo, scene);
				imguiSystem.example();

				imguiSystem.render(commandBuffer);
				renderer.endSwapchainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(device.device());
	}

	void Application::loadScene()
	{
		Ref<VulkanModel> model = VulkanModel::createModelFromFile(device, "assets/models/flat_vase.obj");
		texture = CreateRef<VulkanTexture>(device, "assets/textures/wall.jpg");

		Entity gameObject1 = scene.createEntity("object");
		gameObject1.addComponent<TransformComponent>(glm::vec3(-0.75f, 1.f, 2.5f), glm::vec3(5.f, 5.f, 5.f));
		gameObject1.addComponent<ModelComponent>(model);
		gameObject1.addComponent<TextureComponent>(texture);

		Entity gameObject2 = scene.createEntity("object");
		gameObject2.addComponent<TransformComponent>(glm::vec3(0.75f, 1.f, 2.5f), glm::vec3(5.f, 5.f, 5.f));
		gameObject2.addComponent<ModelComponent>(model);
		gameObject2.addComponent<TextureComponent>(texture);
	}

}