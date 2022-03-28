#include "slpch.h"
#include "Sleet/Core/Application.h"
#include "Sleet/Core/KeyboardMovementController.h"
#include "Sleet/Systems/SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

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
				renderer.beginSwapchainRenderPass(commandBuffer);
				simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
				renderer.endSwapchainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(device.device());
	}

	void Application::loadGameObjects()
	{
		Ref<VulkanModel> model = VulkanModel::createModelFromFile(device, "assets/models/smooth_vase.obj");

		auto gameObject = GameObject::createGameObject();
		gameObject.model = model;
		gameObject.transform.translation = { 0.f, 0.f, 2.5f };
		gameObject.transform.scale = { 5.f, 5.f, 5.f };

		gameObjects.push_back(std::move(gameObject));
	}

}