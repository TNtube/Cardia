#include "cdpch.hpp"
#include "Cardia/ECS/SceneCamera.hpp"
#include <glm/ext/matrix_clip_space.hpp>


namespace Cardia
{
	SceneCamera::SceneCamera()
	{
		RecomputeProjection();
	}

	void SceneCamera::RecomputeProjection()
	{
		if(m_ProjectionType == ProjectionType::Orthographic) {
			float left = m_AspectRatio * m_OrthoSize * -0.5f;
			float right = m_AspectRatio * m_OrthoSize * 0.5f;
			float down = m_OrthoSize * -0.5f;
			float up = m_OrthoSize * 0.5f;
			m_Projection = glm::ortho(left, right, down, up, m_OrthoNear, m_OrthoFar);
		}
		else if (m_ProjectionType == ProjectionType::Perspective) {
			m_Projection = glm::perspective(m_PersFOV, m_AspectRatio, m_PersNear, m_PersFar);
		}

	}
}