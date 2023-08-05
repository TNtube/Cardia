#pragma once

#include "Cardia/Core/Event.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/ECS/Components.hpp"


namespace Cardia
{
	class EditorCamera
	{
	public:
		EditorCamera(Radianf fov, float nearClip, float farClip);
		EditorCamera() = default;

		void OnUpdate();
		void OnEvent(Event& e);

		inline void SetViewportSize(float width, float height) { m_Camera.SetViewportSize(width, height); }

		Matrix4f GetTransformMatrix() const { return m_Transform.GetLocalTransform(); }

		Component::Transform& GetTransformComponent() { return m_Transform; }

		SceneCamera& GetCamera()
		{
			return m_Camera;
		}
	private:

		void mousePan(const Vector2f& delta);
		void mouseRotate(const Vector2f& delta);
		float rotationSpeed() const;

	private:
		SceneCamera m_Camera;
		Component::Transform m_Transform;

		Vector2f m_InitialMousePosition { 0.0f, 0.0f };
		float m_MovementSpeed = 10;
		float m_BaseMovementSpeed = 10;
		float m_MaxMovementSpeed = 100;

	};
}