#include "cdpch.hpp"
#include "Cardia/ECS/Component/Transform.hpp"


namespace Cardia::Component
{
	Matrix4f Transform::GetLocalTransform() const
	{
		return Matrix4f::Identity().Translate(Position)
			 * Quaternion(Rotation).ToMatrix()
			 * Matrix4f::Identity().Scale(Scale);
	}

	Vector3f Transform::Forward() const
	{
		Vector3f forward;

		forward.x = cos(Rotation.x) * sin(Rotation.y);
		forward.y = -sin(Rotation.x);
		forward.z = cos(Rotation.x) * cos(Rotation.y);

		return forward;
	}

	Vector3f Transform::Up() const
	{
		return Forward().Cross(Right());
	}

	Vector3f Transform::Right() const
	{
		Vector3f right;
		right.x =  cos(Rotation.y);
		right.y =  0;
		right.z = -sin(Rotation.y);
		return right;
	}

	Json::Value Transform::Serialize() const
	{
		Json::Value root;

		auto& transform = root["Transform"];

		transform["Position"] = Position.Serialize();
		transform["Rotation"] = Rotation.Serialize();
		transform["Scale"] = Scale.Serialize();

		return root;
	}

	std::optional<Transform> Transform::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Transform"))
			return std::nullopt;

		const auto& transform = root["Transform"];

		if (!transform.isMember("Position") ||
			!transform.isMember("Rotation") ||
			!transform.isMember("Scale"))
				return std::nullopt;

		Transform temp;

		if (const auto pos = Vector3f::Deserialize(transform["Position"]))
			temp.Position = pos.value();
		else
			return std::nullopt;

		if (const auto rot = Vector3f::Deserialize(transform["Rotation"]))
			temp.Rotation = rot.value();
		else
			return std::nullopt;

		if (const auto scale = Vector3f::Deserialize(transform["Scale"]))
			temp.Scale = scale.value();
		else
			return std::nullopt;

		return temp;
	}
}