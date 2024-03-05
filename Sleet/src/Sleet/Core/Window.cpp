#include "slpch.h"

#include "Sleet/Core/Window.h"

namespace Sleet {

    void ErrorCallback(int error, const char* description)
    {
        SL_ERROR("GLFW: {}", description);
    }

	Window::Window(uint32_t width, uint32_t height, std::string title)
	{
        m_Data.Width = width;
        m_Data.Height = height;
        m_Data.Title = title;

        if (!glfwInit())
        {
            std::cout << "GLFW failed to init" << std::endl;
        }

        glfwSetErrorCallback(ErrorCallback);

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (!m_Window)
        {
            glfwTerminate();
            std::cout << "Failed to create window" << std::endl;
        }

        glfwSetWindowUserPointer(m_Window, &m_Data);

        glfwSetKeyCallback(m_Window, nullptr);
        glfwSetCharCallback(m_Window, nullptr);
        glfwSetCursorPosCallback(m_Window, nullptr);
        glfwSetMouseButtonCallback(m_Window, nullptr);
        glfwSetScrollCallback(m_Window, nullptr);
        glfwSetWindowCloseCallback(m_Window, nullptr);
        glfwSetFramebufferSizeCallback(m_Window, nullptr);

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                auto& data = *(WindowData*)(glfwGetWindowUserPointer(window));

                data.Width = width;
                data.Height = height;

                data.ResizeCallback(width, height);
            });
	}

	Window::~Window()
	{
        glfwTerminate();
	}

}