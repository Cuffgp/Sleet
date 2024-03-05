#pragma once

#include "Sleet/Vulkan/Vulkan.h"

#include "glfw/include/GLFW/glfw3.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include "glfw/include/GLFW/glfw3native.h"

namespace Sleet {

	using WindowResizeFunction = std::function<void(uint32_t width, uint32_t height)>;

	class Window
	{
	public:
		Window(uint32_t width, uint32_t height, std::string title);
		~Window();

		void SetResizeCallback(const WindowResizeFunction& function) { m_Data.ResizeCallback = function; }

		bool ShouldClose() { return glfwWindowShouldClose(m_Window); }
		void PollEvents() { glfwPollEvents(); }

		GLFWwindow* GetWindowPointer() { return m_Window; }
		HWND GetRawWindowPointer() { return glfwGetWin32Window(m_Window); }
		uint32_t Width() { return m_Data.Width; }
		uint32_t Height() { return m_Data.Height; }
		float AspectRatio() { return (float)m_Data.Width / (float)m_Data.Height; }
	private:
		struct WindowData
		{
			uint32_t Width;
			uint32_t Height;
			std::string Title;

			WindowResizeFunction ResizeCallback = nullptr;
		};

		WindowData m_Data;
		GLFWwindow* m_Window;

	};

}