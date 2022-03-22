#pragma once


#include "Sleet/Core/Window.h"
#include "Sleet/Vulkan/VulkanPipeline.h"

namespace Sleet {

	class Application
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
		VulkanPipeline pipeline{ "assets/shaders/simple_shader.vert.spv",
		"assets/shaders/simple_shader.frag.spv" };
	};

}