#pragma once
#include <glm/glm.hpp>
#include "Cardia/Renderer/Camera.hpp"

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

		inline ProjectionType getProjectionType() const { return m_ProjectionType; }
		inline void setProjectionType(ProjectionType type) { m_ProjectionType = type; RecomputeProjection(); }

		inline float getPerspectiveFov() const { return m_PersFOV; }
		inline void setPerspectiveFov(float fov) { m_PersFOV = fov; RecomputeProjection(); }
		inline float getPerspectiveNear() const { return m_PersNear; }
		inline void setPerspectiveNear(float perspectiveNear) { m_PersNear = perspectiveNear; RecomputeProjection(); }
		inline float getPerspectiveFar() const { return m_PersFar; }
		inline void setPerspectiveFar(float perspectiveFar) { m_PersFar = perspectiveFar; RecomputeProjection(); }

		inline float getOrthographicSize() const { return m_OrthoSize; }
		inline void setOrthographicSize(float size) { m_OrthoSize = size; RecomputeProjection(); }
		inline float getOrthographicNear() const { return m_OrthoNear; }
		inline void setOrthographicNear(float orthoNear) { m_OrthoNear = orthoNear; RecomputeProjection(); }
		inline float getOrthographicFar() const { return m_OrthoFar; }
		inline void setOrthographicFar(float orthoFar) { m_OrthoFar = orthoFar; RecomputeProjection(); }

		inline void setViewportSize(float width, float height) { m_AspectRatio = width / height; RecomputeProjection(); }

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