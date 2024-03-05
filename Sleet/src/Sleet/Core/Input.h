#pragma once

#include <glm/glm.hpp>

#include "Sleet/Core/KeyCodes.h"

namespace Sleet {

	class Input
	{
	public:

		static bool IsKeyDown(KeyCode keycode);
		static bool IsMouseButtonDown(MouseButton button);

		static glm::vec2 GetMousePosition();

		static void SetCursorMode(CursorMode mode);
	};
}