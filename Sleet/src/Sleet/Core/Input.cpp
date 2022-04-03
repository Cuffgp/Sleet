#include "slpch.h"

#include "Sleet/Core/Input.h"

namespace Sleet {

	GLFWwindow* Input::window = nullptr;

	void Input::init(GLFWwindow* windowHandle)
	{
		window = windowHandle;
	}

	bool Input::isKeyPressed(int keyCode)
	{
		auto state = glfwGetKey(window, static_cast<int32_t>(keyCode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMousePressed(int mouseCode)
	{
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(mouseCode));
		return state == GLFW_PRESS;
	}

}