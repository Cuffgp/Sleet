#include "slpch.h"

#include "Sleet/Core/Input.h"
#include "Sleet/Core/Application.h"

#include "Sleet/Renderer/OrthographicCamera.h"

namespace Sleet {

	OrthographicCamera::OrthographicCamera()
	{
		m_View = glm::mat4{ 1.0f };
		m_Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
	}

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
	{
		m_View = glm::mat4{ 1.0f };
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	OrthographicCamera::~OrthographicCamera()
	{

	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
	}

	void OrthographicCamera::OnWindowResize()
	{

	}

	void OrthographicCamera::OnUpdate(float ts)
	{
		if (Input::IsKeyDown(Key::A))
		{
			m_Position.x -= cos(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
			m_Position.y -= sin(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
		}
		else if (Input::IsKeyDown(Key::D))
		{
			m_Position.x += cos(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
			m_Position.y += sin(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
		}

		if (Input::IsKeyDown(Key::W))
		{
			m_Position.x += -sin(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
			m_Position.y += cos(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
		}
		else if (Input::IsKeyDown(Key::S))
		{
			m_Position.x -= -sin(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
			m_Position.y -= cos(glm::radians(m_Rotation)) * m_TranslationSpeed * ts;
		}

		RecalculateViewMatrix();

		m_AspectRatio = Sleet::Application::Get().GetWindow().AspectRatio();

		if (Input::IsKeyDown(Key::Q))
		{
			m_ZoomLevel -= 0.1f * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
			SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		}
		else if (Input::IsKeyDown(Key::E))
		{
			m_ZoomLevel += 0.1f * 0.25f;
			m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
			SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		}

		/*
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
		*/
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position);

		m_View = glm::inverse(transform);
	}
}