#include "cdpch.hpp"
#include "Cardia/ECS/Component/Transform.hpp"

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/Math/Quaternion.hpp"


namespace Cardia::Component
{
	Matrix4f Transform::GetLocalTransform() const
	{
		constexpr auto identity = Matrix4f::Identity();
		return identity.Translate(m_Position)
			 * Quaternion(m_Rotation).ToMatrix()
			 * identity.Scale(m_Scale);
	}

	Matrix4f Transform::GetWorldTransform() const
	{
		constexpr auto identity = Matrix4f::Identity();
		return identity.Translate(m_WorldPosition)
			 * m_WorldRotation.ToMatrix()
			 * identity.Scale(m_WorldScale);
	}

	Vector3f Transform::Forward() const
	{
		Vector3f forward;

		forward.x = cos(m_Rotation.x) * sin(m_Rotation.y);
		forward.y = -sin(m_Rotation.x);
		forward.z = cos(m_Rotation.x) * cos(m_Rotation.y);

		return forward;
	}

	Vector3f Transform::Up() const
	{
		return Forward().Cross(Right());
	}

	Vector3f Transform::Right() const
	{
		Vector3f right;
		right.x =  cos(m_Rotation.y);
		right.y =  0;
		right.z = -sin(m_Rotation.y);
		return right;
	}

	void Transform::RecomputeWorld(Entity entity)
	{
		if (!m_Dirty)
			return;

		m_Dirty = false;

		auto parent = entity.GetParent();

		if (parent.IsValid())
		{
			const auto& parentTransform = parent.GetComponent<Transform>();
			m_WorldPosition = parentTransform.m_WorldPosition + parentTransform.m_WorldRotation * (parentTransform.m_WorldScale * m_Position);
			m_WorldRotation = parentTransform.m_WorldRotation * Quatf(m_Rotation);
			m_WorldScale = parentTransform.m_WorldScale * m_Scale;
		} else
		{
			m_WorldPosition = m_Position;
			m_WorldRotation = Quaternion(m_Rotation);
			m_WorldScale = m_Scale;
		}

		for (auto child : entity.GetChildren())
		{
			auto& childTransform = child.GetComponent<Transform>();
			childTransform.m_Dirty = true;
			childTransform.RecomputeWorld(child);
		}
	}

	Json::Value Transform::Serialize() const
	{
		Json::Value root;

		auto& transform = root["Transform"];

		transform["Position"] = m_Position.Serialize();
		transform["Rotation"] = m_Rotation.Serialize();
		transform["Scale"] = m_Scale.Serialize();

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
			temp.m_Position = pos.value();
		else
			return std::nullopt;

		if (const auto rot = Vector3f::Deserialize(transform["Rotation"]))
			temp.m_Rotation = rot.value();
		else
			return std::nullopt;

		if (const auto scale = Vector3f::Deserialize(transform["Scale"]))
			temp.m_Scale = scale.value();
		else
			return std::nullopt;

		return temp;
	}
}