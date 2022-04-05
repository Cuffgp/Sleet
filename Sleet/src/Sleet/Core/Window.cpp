#include "slpch.h"
#include <stdexcept>

#include "Sleet/Core/Window.h"

namespace Sleet {

	void glfwErrorCallback(int error, const char* description)
	{
		//SL_ERROR("GLFW Error {}: {}", error, description);
	}


	Window::Window(int w, int h, std::string name) :
		width(w), height(h), windowName(name)
	{
		initWindow();
	}

	Window::~Window()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void Window::initWindow()
	{
		SL_ASSERT(glfwInit());

		glfwSetErrorCallback(glfwErrorCallback);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizedCallback);
	}

	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			SL_ERROR("Failed to create window surface.");
		}
	}

	void Window::framebufferResizedCallback(GLFWwindow* window, int width, int height)
	{
		auto sleetWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		sleetWindow->framebufferResized = true;
		sleetWindow->width = width;
		sleetWindow->height = height;
	}

}