#include "slpch.h"

#include "Sleet/Core/KeyboardMovementController.h"
#include "Sleet/Core/Input.h"

namespace Sleet {

	void KeyboardMovementController::moveInPlaneXZ(float dt, TransformComponent& transform) 
	{
		glm::vec3 rotate{ 0 };

		if (Input::isKeyPressed(keys.lookRight) == GLFW_PRESS) rotate.y += 1.f;
		if (Input::isKeyPressed(keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
		if (Input::isKeyPressed(keys.lookUp) == GLFW_PRESS) rotate.x += 1.f;
		if (Input::isKeyPressed(keys.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

		if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) 
		{
			transform.rotation += lookSpeed * dt * glm::normalize(rotate);
		}

		// limit pitch values between about +/- 85ish degrees
		transform.rotation.x = glm::clamp(transform.rotation.x, -1.5f, 1.5f);
		transform.rotation.y = glm::mod(transform.rotation.y, glm::two_pi<float>());

		float yaw = transform.rotation.y;
		const glm::vec3 forwardDir{ sin(yaw), 0.f, cos(yaw) };
		const glm::vec3 rightDir{ forwardDir.z, 0.f, -forwardDir.x };
		const glm::vec3 upDir{ 0.f, -1.f, 0.f };

		glm::vec3 moveDir{ 0.f };

		if (Input::isKeyPressed(keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
		if (Input::isKeyPressed(keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
		if (Input::isKeyPressed(keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
		if (Input::isKeyPressed(keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
		if (Input::isKeyPressed(keys.moveUp) == GLFW_PRESS) moveDir += upDir;
		if (Input::isKeyPressed(keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

		if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) 
		{
			transform.translation += moveSpeed * dt * glm::normalize(moveDir);
		}
	}


}