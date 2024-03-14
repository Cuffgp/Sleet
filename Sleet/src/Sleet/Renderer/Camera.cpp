#include "slpch.h"

#include "Sleet/Renderer/Camera.h"
#include "Sleet/Core/Input.h"
#include "Sleet/Core/Application.h"

namespace Sleet {

	Camera::Camera(glm::vec3 position, glm::vec3 direction)
	{
		m_LastMousePosition = glm::vec2{ 0., 0. };
		m_Position = position;
		m_ForwardDirection = direction;

		// Create the view matrix
		m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3{ 0., 1., 0. });

		OnWindowResize();
	}

	Camera::~Camera()
	{

	}

	void Camera::OnWindowResize()
	{
		// Create the projection matrix
		constexpr float fov = glm::radians(45.0f);  // Field of view in degrees
		float aspectRatio = Application::Get().GetWindow().AspectRatio();  // Width / Height
		float nearPlane = 0.1f;
		float farPlane = 100.0f;
		m_Projection = glm::perspective(fov, aspectRatio, nearPlane, farPlane);
	}

	void Camera::OnUpdate(float ts)
	{
		glm::vec2 mousePos = Input::GetMousePosition();
		glm::vec2 delta = (mousePos - m_LastMousePosition) * 0.002f;
		m_LastMousePosition = mousePos;

		if (!Input::IsMouseButtonDown(MouseButton::Right))
		{
			Input::SetCursorMode(CursorMode::Normal);
			return;
		}

		Input::SetCursorMode(CursorMode::Locked);

		bool moved = false;

		constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
		glm::vec3 rightDirection = glm::cross(m_ForwardDirection, upDirection);

		float speed = 5.0f;

		// Movement
		if (Input::IsKeyDown(KeyCode::W))
		{
			m_Position += m_ForwardDirection * speed * ts;
			moved = true;
		}
		else if (Input::IsKeyDown(KeyCode::S))
		{
			m_Position -= m_ForwardDirection * speed * ts;
			moved = true;
		}
		if (Input::IsKeyDown(KeyCode::A))
		{
			m_Position -= rightDirection * speed * ts;
			moved = true;
		}
		else if (Input::IsKeyDown(KeyCode::D))
		{
			m_Position += rightDirection * speed * ts;
			moved = true;
		}
		if (Input::IsKeyDown(KeyCode::Q))
		{
			m_Position -= upDirection * speed * ts;
			moved = true;
		}
		else if (Input::IsKeyDown(KeyCode::E))
		{
			m_Position += upDirection * speed * ts;
			moved = true;
		}

		// Rotation
		if (delta.x != 0.0f || delta.y != 0.0f)
		{
			float pitchDelta = -delta.y * 0.6f;
			float yawDelta = delta.x * 0.6f;

			glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
				glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))));
			m_ForwardDirection = glm::rotate(q, m_ForwardDirection);

			moved = true;
		}

		if (moved)
		{
			m_View = glm::lookAt(m_Position, m_Position + m_ForwardDirection, glm::vec3{ 0., 1., 0. });
		}
	}
}