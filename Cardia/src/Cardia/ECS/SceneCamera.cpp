#include "cdpch.hpp"
#include "Cardia/ECS/SceneCamera.hpp"
#include <glm/ext/matrix_clip_space.hpp>

namespace Cardia
{
	SceneCamera::SceneCamera()
	{
		RecomputeProjection();
	}
	
	void SceneCamera::setPerspective(float verticalFOV, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PersFOV = verticalFOV;
		m_PersNear = nearClip;
		m_PersFar = farClip;
		RecomputeProjection();
	}

	void SceneCamera::setOrthographic(float size, float nearClip, float farClip)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoSize = size;
		m_OrthoNear = nearClip;
		m_OrthoFar = farClip;
		RecomputeProjection();
	}

	void SceneCamera::RecomputeProjection()
	{
		if(m_ProjectionType == ProjectionType::Orthographic) {
			const float left = m_AspectRatio * m_OrthoSize * -0.5f;
			const float right = m_AspectRatio * m_OrthoSize * 0.5f;
			const float down = m_OrthoSize * -0.5f;
			const float up = m_OrthoSize * 0.5f;
			m_ProjectionMatrix = glm::ortho(left, right, down, up, m_OrthoNear, m_OrthoFar);
		}
		else if (m_ProjectionType == ProjectionType::Perspective) {
			m_ProjectionMatrix = glm::perspective(m_PersFOV, m_AspectRatio, m_PersNear, m_PersFar);
		}

	}
}
