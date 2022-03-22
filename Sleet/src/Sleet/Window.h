#pragma once

#include "string"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Sleet {

	class Window
	{
	public:
		Window(int w, int h, std::string name);
		~Window();

		bool shouldClose() { return glfwWindowShouldClose(window); }
	private:
		void initWindow();

		const int width;
		const int height;
		std::string windowName;
		GLFWwindow* window;
	};

}