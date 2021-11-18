#pragma once
#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"
#include "Camera.hpp"
#include <glm/gtc/quaternion.hpp>


namespace Cardia
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera(float fov, float nearClip, float farClip, float aspectRatio);
		EditorCamera() = default;

		~EditorCamera() override = default;

		void onUpdate(DeltaTime deltaTime);
		void onEvent(Event& e);

		inline float getDistance() const { return m_Distance; }
		inline void setDistance(float distance) { m_Distance = distance; }

		inline void setViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; updateProjection(); }

		const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
		glm::mat4 getViewProjection() const { return m_Projection * m_ViewMatrix; }

		glm::vec3 getUpDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getForwardDirection() const;
		const glm::vec3& getPosition() const { return m_Position; }
		glm::quat getOrientation() const;

		float getPitch() const { return m_Pitch; }
		float getYaw() const { return m_Yaw; }

	private:
		void updateProjection();
		void updateView();

		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		glm::vec2 panSpeed() const;
		float rotationSpeed() const;
		float zoomSpeed() const;

	private:
		float m_FOV = 45.0f, m_AspectRatio = 1.778f, m_NearClip = 0.1f, m_FarClip = 1000.0f;

		glm::mat4 m_ViewMatrix {};
		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

		glm::vec2 m_InitialMousePosition = { 0.0f, 0.0f };

		float m_Distance = 10.0f;
		float m_Pitch = 0.0f, m_Yaw = 0.0f;

		float m_ViewportWidth = 1280, m_ViewportHeight = 720;

	};
}