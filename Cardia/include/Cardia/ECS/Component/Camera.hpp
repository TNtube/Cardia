#pragma once

#include "Cardia/ECS/SceneCamera.hpp"
#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia::Component
{
	struct Camera
	{
		SceneCamera CameraData;
		bool Primary = true;
		Camera() = default;

		inline void Reset() {
			CameraData = SceneCamera();
		}

		constexpr static auto properties = std::make_tuple(
			property(&Camera::CameraData, "CameraData"),
			property(&Camera::Primary, "Primary")
		);
	};
}