#pragma once

#include <GLFW/glfw3.h>

namespace Sleet {

	class Input
	{
	public:
		static void init(GLFWwindow* windowHandle);

		static bool isKeyPressed(int keyCode);
		static bool isMousePressed(int mouseCode);

	private:
		static GLFWwindow* window;
	};


}