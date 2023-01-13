#pragma once

#include <glm/glm.hpp>

#include "Cardia/Core/Core.hpp"


namespace Cardia {
	class Camera
	{
	public:
		Camera() = default;
		explicit Camera(const glm::mat4& projection)
			: m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		virtual const glm::mat4& getProjectionMatrix() { return m_ProjectionMatrix; }
		virtual const glm::mat4& getViewMatrix() { return m_ViewMatrix; }
		
		virtual const glm::mat4& getViewProjectionMatrix()
		{
			m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			return m_ViewProjectionMatrix;
		}
	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix {};
		glm::mat4 m_ViewProjectionMatrix {};
	};
}
