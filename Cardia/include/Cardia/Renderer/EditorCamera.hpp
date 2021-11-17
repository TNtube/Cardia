#pragma once
#include "Camera.hpp"


namespace Cardia
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera(float fov, float nearClip, float farClip, float aspectRatio)
			: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
		{

		}

		inline glm::mat4 getViewProjection() { return m_Projection * m_ViewMatrix; }

		~EditorCamera() override = default;

	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;
		glm::mat4 m_ViewMatrix {};

	};
}