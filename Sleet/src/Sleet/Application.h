#pragma once


#include "Window.h"

namespace Sleet {

	class Application
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		void run();

	private:
		Window window{ WIDTH, HEIGHT, "Hello Vulkan" };
	};

}