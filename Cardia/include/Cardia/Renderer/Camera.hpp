#pragma once

#include <glm/glm.hpp>


namespace Cardia {
	class Camera
	{
	public:
		Camera() = default;
		explicit Camera(const glm::mat4& projection)
			: m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		virtual const glm::mat4& getProjectionMatrix() { return m_ProjectionMatrix; }
		virtual const glm::mat4& getViewMatrix() { return m_ViewMatrix; }
		virtual const glm::mat4& getViewProjectionMatrix() { return m_ViewProjectionMatrix; }

		virtual const glm::vec3& getPosition() { return m_Position; }
	protected:
		glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		glm::mat4 m_ViewMatrix {};
		glm::mat4 m_ViewProjectionMatrix {};

		glm::vec3 m_Position {};
	};

	
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		void setBounds(float left, float right, float bottom, float top);

		inline const glm::vec3& getPosition() const { return m_Position; }
		inline void setPosition(const glm::vec3& position) { m_Position = position; RecomputeViewMatrix(); }

		inline float getRotation() const { return m_Rotation; }
		inline void setRotation(float rotation) { m_Rotation = rotation; RecomputeViewMatrix(); }
		
	private:
		void RecomputeViewMatrix();

	private:
		float m_Rotation = 0.0f;
	};
}