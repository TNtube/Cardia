#include "EditorCamera.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Core/KeyCodes.hpp"

#include <glm/gtx/quaternion.hpp>


namespace Cardia
{
	EditorCamera::EditorCamera(float fov, float nearClip, float farClip)
	{
		m_Camera.SetPerspective(fov, nearClip, farClip);
	}

	void EditorCamera::OnUpdate()
	{
		const glm::vec2& mouse{ Input::getMouseX(), Input::getMouseY() };
		glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.003f;
		m_InitialMousePosition = mouse;

		if (Input::isMouseButtonPressed(Mouse::Middle))
			mousePan(delta);
		else if (Input::isMouseButtonPressed(Mouse::Right))
		{
			mouseRotate(delta);
			if (Input::isKeyPressed(Key::A)) {
				mousePan(glm::vec2(0.02f, 0.0f));
			}
			if (Input::isKeyPressed(Key::D)) {
				mousePan(glm::vec2(-0.02f, 0.0f));
			}
			if (Input::isKeyPressed(Key::W)) {
				m_Transform.position.z += 0.09f;
			}
			if (Input::isKeyPressed(Key::S)) {
				m_Transform.position.z += -0.09f;
			}
		}
	}

	void EditorCamera::OnEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) -> bool {
			float delta = event.getOffSetY() * 0.1f;
			m_Transform.position.z += delta;
			return false;
		});
	}

	void EditorCamera::mousePan(const glm::vec2 &delta)
	{
		auto speed = 5.f;
		m_Transform.position += -m_Transform.Right() * delta.x * speed;
		m_Transform.position += m_Transform.Up() * delta.y * speed;
	}

	void EditorCamera::mouseRotate(const glm::vec2 &delta)
	{
		float yawSign = m_Transform.Up().y < 0 ? -1.0f : 1.0f;
		m_Transform.rotation.y += yawSign * delta.x * rotationSpeed();
		m_Transform.rotation.x += delta.y * rotationSpeed();
	}


	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}
}