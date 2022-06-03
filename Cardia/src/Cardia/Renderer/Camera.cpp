#include "cdpch.hpp"
#include <Cardia/Renderer/Camera.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Cardia {

	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: Camera(glm::ortho(left, right, bottom, top, -1.0f, 1.0f)), m_Rotation()
	{
		m_ViewMatrix = glm::mat4(1.0f);
		m_ViewProjectionMatrix = glm::mat4(m_ProjectionMatrix * m_ViewMatrix);
	}

	void OrthographicCamera::setBounds(float left, float right, float bottom, float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		RecomputeViewMatrix();
	}

	void OrthographicCamera::RecomputeViewMatrix()
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}
}