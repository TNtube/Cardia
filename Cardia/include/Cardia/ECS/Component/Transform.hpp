#pragma once

#include "Cardia/Math/Vector3.hpp"
#include "Cardia/Math/Matrix4.hpp"
#include "Cardia/Math/Quaternion.hpp"

namespace Cardia::Component
{
	struct Transform
	{
		Transform() = default;
		Transform(Vector3f pos, Vector3f rot, Vector3f sca) : Position(pos), Rotation(rot), Scale(sca) {}

		Vector3f Position { 0.0f };
		Vector3f Rotation { 0.0f };
		Vector3f Scale { 1.0f };
		Matrix4f GetLocalTransform() const;

		Vector3f Forward() const;
		Vector3f Up() const;
		Vector3f Right() const;

		inline void Reset() {
			Position = Vector3f(0);
			Rotation = Vector3f(0);
			Scale = Vector3f(1);
		}

		Json::Value Serialize() const;
		static std::optional<Transform> Deserialize(const Json::Value& root);
	};
}