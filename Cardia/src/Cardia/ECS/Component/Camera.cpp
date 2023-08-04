#include "cdpch.hpp"
#include "Cardia/ECS/Component/Camera.hpp"

namespace Cardia::Component
{
	Json::Value Camera::Serialize() const
	{
		Json::Value root;

		auto& camera = root["Camera"];

		camera["CameraData"] = CameraData.Serialize();
		camera["Primary"] = Primary;

		return root;
	}

	std::optional<Camera> Camera::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Camera"))
			return std::nullopt;

		const auto& cameraNode = root["Camera"];

		if (!cameraNode.isMember("CameraData") ||
			!cameraNode.isMember("Primary"))
				return std::nullopt;

		Camera temp;

		if (const auto camera = SceneCamera::Deserialize(cameraNode["CameraData"]))
			temp.CameraData = camera.value();
		else
			return std::nullopt;

		temp.Primary = cameraNode["Primary"].asBool();

		return temp;
	}
}