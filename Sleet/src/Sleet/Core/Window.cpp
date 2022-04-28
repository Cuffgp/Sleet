#include "slpch.h"
#include <stdexcept>

#include "Sleet/Core/Window.h"
#include "Sleet/Events/ApplicationEvent.h"
#include "Sleet/Events/KeyEvent.h"
#include "Sleet/Events/MouseEvent.h"

namespace Sleet {

	void glfwErrorCallback(int error, const char* description)
	{
		//SL_ERROR("GLFW Error {}: {}", error, description);
	}


	Window::Window(int w, int h, std::string name)
	{
		data.width = w;
		data.height = h;
		data.title = name;
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

		window = glfwCreateWindow(data.width, data.height, data.title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, &data);


		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& windowData = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						windowData.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						windowData.EventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						windowData.EventCallback(event);
						break;
					}
				}
			});

		glfwSetCharCallback(window, [](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.EventCallback(event);
			});

		glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallback(event);
			});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallback(event);
						break;
					}
				}
			});

		glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallback(event);
			});

		glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)(glfwGetWindowUserPointer(window));

				WindowCloseEvent event;
				data.EventCallback(event);
			});

		glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
			{
				auto data = *(WindowData*)(glfwGetWindowUserPointer(window));

				data.framebufferResized = true;
				data.width = width;
				data.height = height;

				WindowResizeEvent event(width, height);
				data.EventCallback(event);
			});

		glfwSetWindowSizeCallback(window, nullptr);
		glfwSetCursorEnterCallback(window, nullptr);
		glfwSetWindowFocusCallback(window, nullptr);
		glfwSetDropCallback(window, nullptr);

	}

	void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
	{
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
		{
			SL_ERROR("Failed to create window surface.");
		}
	}

}