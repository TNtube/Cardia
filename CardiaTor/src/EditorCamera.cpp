#include "EditorCamera.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Core/KeyCodes.hpp"


namespace Cardia
{
	EditorCamera::EditorCamera(float fov, float nearClip, float farClip)
	{
		m_Camera.SetPerspective(fov, nearClip, farClip);
		m_Transform.position = {0, 5, 10};
	}

	void EditorCamera::OnUpdate()
	{
		const glm::vec2& mouse{ Input::getMouseX(), Input::getMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.01f;
		m_InitialMousePosition = mouse;

		if (Input::isMouseButtonPressed(Mouse::Middle))
			mousePan(delta);
		else if (Input::isMouseButtonPressed(Mouse::Right))
		{
			mouseRotate(delta);
			if (Input::isKeyPressed(Key::A)) {
				m_Transform.position +=
					-m_Transform.Right()
					* m_MovementSpeed
					* Time::deltaTime().seconds();
			}
			if (Input::isKeyPressed(Key::D)) {
				m_Transform.position +=
					m_Transform.Right()
					* m_MovementSpeed
					* Time::deltaTime().seconds();
			}
			if (Input::isKeyPressed(Key::W)) {
				m_Transform.position +=
					-m_Transform.Forward()
					* m_MovementSpeed
					* Time::deltaTime().seconds();
			}
			if (Input::isKeyPressed(Key::S)) {
				m_Transform.position +=
					m_Transform.Forward()
					* m_MovementSpeed *
					Time::deltaTime().seconds();
			}
			if (Input::isKeyPressed(Key::Q)) {
				m_Transform.position +=
					-m_Transform.Up()
					* m_MovementSpeed *
					Time::deltaTime().seconds();
			}
			if (Input::isKeyPressed(Key::E)) {
				m_Transform.position +=
					m_Transform.Up()
					* m_MovementSpeed *
					Time::deltaTime().seconds();
			}

			if (Input::isKeyPressed(Key::LeftShift)) {
				m_MovementSpeed = glm::mix(m_MovementSpeed, m_MaxMovementSpeed, Time::deltaTime().seconds());
			}
			else if (m_MovementSpeed - m_BaseMovementSpeed >= 0.01f) {
				m_MovementSpeed = glm::mix(m_MovementSpeed, m_BaseMovementSpeed, Time::deltaTime().seconds() * 2);
			}
		}
	}

	void EditorCamera::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) -> bool {
			float delta = event.getOffSetY();
			m_Transform.position +=
				m_Transform.Forward()
				* delta
				* m_MovementSpeed
				* Time::deltaTime().seconds();
			return false;
		});
	}

	void EditorCamera::mousePan(const glm::vec2 &delta)
	{
		m_Transform.position +=
			-m_Transform.Right()
			* delta.x
			* m_MovementSpeed
			* Time::deltaTime().seconds();
		m_Transform.position +=
			m_Transform.Up()
			* delta.y
			* m_MovementSpeed
			* Time::deltaTime().seconds();
	}

	void EditorCamera::mouseRotate(const glm::vec2 &delta)
	{
		m_Transform.rotation += glm::vec3(-delta.y, -delta.x, 0) * rotationSpeed();
	}


	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}
}