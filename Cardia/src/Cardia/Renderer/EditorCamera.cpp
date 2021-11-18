#include "cdpch.hpp"
#include "Cardia/Renderer/EditorCamera.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Core/KeyCodes.hpp"

#include <glm/gtx/quaternion.hpp>

namespace Cardia
{
	EditorCamera::EditorCamera(float fov, float nearClip, float farClip, float aspectRatio)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		updateView();
	}

	void EditorCamera::onUpdate(DeltaTime deltaTime)
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
				mouseZoom(0.09f);
			}
			if (Input::isKeyPressed(Key::S)) {
				mouseZoom(-0.09f);
			}
		}

		updateView();
	}

	void EditorCamera::onEvent(Event &e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<MouseScrolledEvent>([this](MouseScrolledEvent& event) -> bool {
			float delta = event.getOffSetY() * 0.1f;
			mouseZoom(delta);
			updateView();
			return false;
		});
	}

	glm::vec3 EditorCamera::getUpDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getRightDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getForwardDirection() const
	{
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCamera::getOrientation() const
	{
		return {(glm::vec3(-m_Pitch, -m_Yaw, 0.0f))};
	}

	void EditorCamera::updateProjection()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
	}

	void EditorCamera::updateView()
	{
		m_Position = calculatePosition();

		glm::quat orientation = getOrientation();
		m_ViewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation));
	}

	void EditorCamera::mousePan(const glm::vec2 &delta)
	{
		auto speed = panSpeed();
		m_FocalPoint += -getRightDirection() * delta.x * speed.x * m_Distance;
		m_FocalPoint += getUpDirection() * delta.y * speed.y * m_Distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2 &delta)
	{
		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * rotationSpeed();
		m_Pitch += delta.y * rotationSpeed();
	}

	void EditorCamera::mouseZoom(float delta)
	{
		m_Distance -= delta * zoomSpeed();
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += getForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::calculatePosition() const
	{
		return m_FocalPoint - getForwardDirection() * m_Distance;
	}

	glm::vec2 EditorCamera::panSpeed() const
	{
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::rotationSpeed() const
	{
		return 0.8f;
	}

	float EditorCamera::zoomSpeed() const
	{
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}
}