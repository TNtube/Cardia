#pragma once

#include <glm/glm.hpp>

#include "Cardia/Core/Core.hpp"
#include "Cardia/Math/Matrix4.hpp"


namespace Cardia {
	class Camera
	{
	public:
		Camera() = default;
		explicit Camera(const Matrix4f& projection)
			: m_ProjectionMatrix(projection) {}

		virtual ~Camera() = default;

		virtual const Matrix4f& GetProjectionMatrix() { return m_ProjectionMatrix; }
	protected:
		Matrix4f m_ProjectionMatrix = Matrix4f::Identity();
		Matrix4f m_ViewProjectionMatrix {};
	};
}
