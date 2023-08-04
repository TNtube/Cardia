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

		Json::Value Serialize() const;
		static std::optional<Camera> Deserialize(const Json::Value& root);
	};
}