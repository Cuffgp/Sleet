#pragma once

#include "glm/glm.hpp"

namespace Sleet {

	class OrthographicCamera
	{
	public:
		OrthographicCamera();
		OrthographicCamera(float left, float right, float bottom, float top);
		~OrthographicCamera();

		void SetProjection(float left, float right, float bottom, float top);
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		glm::mat4 GetProjectionView() { return m_Projection * m_View; }
		glm::mat4 GetView() { return m_View; }
		glm::mat4 GetProjection() { return m_Projection; }
		glm::vec3 GetPosition() { return m_Position; }

		void OnWindowResize();
		void OnUpdate(float ts);
	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_View;
		glm::mat4 m_Projection;

		float m_AspectRatio = 1.0f;
		float m_ZoomLevel = 1.0f;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		float m_Rotation = 0.0f; //In degrees, in the anti-clockwise direction
		float m_TranslationSpeed = 5.0f;
		float m_RotationSpeed = 180.0f;

	};

}