#pragma once

#include "Sleet/Core/Base.h"
#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanDevice.h"
#include "Sleet/Vulkan/VulkanPipeline.h"
#include "Sleet/Vulkan/VulkanRenderer.h"
#include "Sleet/Vulkan/VulkanDescriptor.h"
#include "Sleet/Core/Camera.h"
#include "Sleet/Core/Input.h"
#include "Sleet/Scene/Scene.h"
#include "Sleet/Scene/Entity.h"
#include "Sleet/Scene/Component.h"
#include "Sleet/Events/Event.h"
#include "Sleet/Events/ApplicationEvent.h"

namespace Sleet {

	class Application
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void Run();
		void OnEvent(Event& e);
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		void loadScene();

		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
		VulkanDevice device{ window };
		VulkanRenderer renderer{ window, device };

		Scene scene;

		Ref<VulkanTexture> texture1{};
		Ref<VulkanTexture> texture2{};
		Scope<DescriptorPool> globalPool{};
		Scope<DescriptorPool> modelPool{};
	};

}