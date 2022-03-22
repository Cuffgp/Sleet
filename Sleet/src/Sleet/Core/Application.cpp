#include "slpch.h"

#include "Sleet/Core/Application.h"

namespace Sleet {

	void Application::run()
	{
		while (!window.shouldClose())
		{
			glfwPollEvents();
		}
	}
}