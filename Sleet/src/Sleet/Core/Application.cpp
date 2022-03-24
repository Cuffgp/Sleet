#include "slpch.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Systems/SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace Sleet {

	Application::Application()
	{
		loadGameObjects();
	}

	Application::~Application()
	{

	}

	void Application::run()
	{
		SimpleRenderSystem simpleRenderSystem{ device, renderer.getSwapchainRenderPass() };

		while (!window.shouldClose())
		{
			glfwPollEvents();
			
			if (auto commandBuffer = renderer.beginFrame())
			{
				renderer.beginSwapchainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
				renderer.endSwapchainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(device.device());
	}

	void Application::loadGameObjects()
	{
		std::vector<VulkanModel::Vertex> vertices{
			{{0.0f, -0.5f}, {1.0, 0.0, 0.0}},
			{{0.5f, 0.5f}, {0.0, 1.0, 0.0}},
			{{-0.5f, 0.5f}, {0.0, 0.0, 1.0}}
		};
		auto model = CreateRef<VulkanModel>(device, vertices);

		auto triangle = GameObject::createGameObject();
		triangle.model = model;
		triangle.color = { .1f, .8f, .1f };
		triangle.transform2d.translation.x = .2f;
		triangle.transform2d.scale = { 2.f, .5f };
		triangle.transform2d.rotation = .25f * glm::two_pi<float>();

		gameObjects.push_back(std::move(triangle));
	}

}