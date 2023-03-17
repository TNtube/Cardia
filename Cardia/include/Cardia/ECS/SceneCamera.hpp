#pragma once

#include "Cardia/Renderer/Camera.hpp"

#include <glm/glm.hpp>


namespace Cardia
{
	class SceneCamera : public Camera
	{
	public:
		enum class ProjectionType
		{
			Perspective = 0, Orthographic = 1
		};

		SceneCamera();

		~SceneCamera() override = default;
		void SetPerspective(float verticalFOV, float nearClip, float farClip);
		glm::vec3 GetPerspective() const;
		void SetOrthographic(float size, float nearClip, float farClip);
		glm::vec3 GetOrthographic() const;
		void UpdateView(const glm::mat4& transform);

		inline ProjectionType GetProjectionType() const { return m_ProjectionType; }
		inline void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecomputeProjection(); }

		inline void SetViewportSize(float width, float height) { m_AspectRatio = width / height; RecomputeProjection(); }

	private:
		void RecomputeProjection();

		ProjectionType m_ProjectionType = ProjectionType::Orthographic;

		float m_PersFOV = glm::radians(60.0f);
		float m_PersNear = 0.01f, m_PersFar = 1000.0f;

		float m_OrthoSize = 10.0f;
		float m_OrthoNear = -1.0f, m_OrthoFar = 1.0f;

		float m_AspectRatio = 0.0f;
	};
}