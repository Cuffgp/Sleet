#pragma once

namespace Sleet {

	class Camera
	{
	public:
		Camera();
		Camera(glm::vec3 position, glm::vec3 direction);
		~Camera();

		glm::mat4 GetProjectionView() { return m_Projection * m_View; }
		glm::mat4 GetView() { return m_View; }
		glm::mat4 GetProjection() { return m_Projection; }
		glm::vec3 GetPosition() { return m_Position; }
		glm::vec3 GetDirection() { return m_ForwardDirection; }

		void OnWindowResize();
		void OnUpdate(float ts);
	private:
		glm::vec2 m_LastMousePosition;
		glm::vec3 m_Position;
		glm::vec3 m_ForwardDirection;

		glm::mat4 m_View;
		glm::mat4 m_Projection;
	};

}