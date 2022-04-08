#pragma once

#include "Sleet/Events/Event.h"

#include <string>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Sleet {

	using EventCallbackFn = std::function<void(Event&)>;

	class Window
	{
	public:
		Window(int w, int h, std::string name);
		~Window();

		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void SetEventCallback(const EventCallbackFn& callback) { data.EventCallback = callback; }
		bool shouldClose() { return glfwWindowShouldClose(window); }
		VkExtent2D getExtent() { return { static_cast<uint32_t>(data.width), static_cast<uint32_t>(data.height) }; }
		bool wasWindowResized() { return data.framebufferResized; }
		void resetWindowResizedFlag() { data.framebufferResized = false; }
		GLFWwindow* getGLFWwindow() const { return window; }

		void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

	private:
		void initWindow();

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool VSync;
			bool framebufferResized = false;

			EventCallbackFn EventCallback;
		};

		WindowData data;
		GLFWwindow* window;
	};

}