#pragma once
#include <glm/glm.hpp>

namespace Cardia
{
	class SceneCamera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0, Orthographic = 1
		};

		SceneCamera();

		virtual ~SceneCamera() = default;

		inline ProjectionType getProjectionType() const
		{ return m_ProjectionType; }

		inline void setProjectionType(ProjectionType type)
		{
			m_ProjectionType = type;
			RecomputeProjection();
		}

	private:
		void RecomputeProjection();

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		glm::mat4 m_Projection;

		float m_PersFOV = glm::radians(60.0f);
		float m_PersNear = 0.01f, m_PersFar = 1000.0f;

		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}