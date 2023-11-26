#include "cdpch.hpp"
#include "Cardia/ECS/SceneCamera.hpp"

namespace Cardia
{
	SceneCamera::SceneCamera()
	{
		RecomputeProjection();
	}
	
	void SceneCamera::SetPerspective(const PerspectiveData data)
	{
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveData = data;
		RecomputeProjection();
	}

	void SceneCamera::SetOrthographic(OrthographicData data)
	{
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthographicData = data;
		RecomputeProjection();
	}

	PerspectiveData SceneCamera::GetPerspective() const
	{
		return m_PerspectiveData;
	}

	OrthographicData SceneCamera::GetOrthographic() const
	{
		return m_OrthographicData;
	}

	void SceneCamera::RecomputeProjection()
	{
		if(m_ProjectionType == ProjectionType::Orthographic) {
			const float left = m_AspectRatio * m_OrthographicData.Size * -0.5f;
			const float right = m_AspectRatio * m_OrthographicData.Size * 0.5f;
			const float down = m_OrthographicData.Size * -0.5f;
			const float up = m_OrthographicData.Size * 0.5f;
			m_ProjectionMatrix = Matrix4f::Orthographic(left, right, down, up, m_OrthographicData.NearClip, m_OrthographicData.FarClip);
		}
		else if (m_ProjectionType == ProjectionType::Perspective) {
			m_ProjectionMatrix = Matrix4f::Perspective(m_PerspectiveData.VerticalFOV, m_AspectRatio, m_PerspectiveData.NearClip, m_PerspectiveData.FarClip);
		}

	}
}
