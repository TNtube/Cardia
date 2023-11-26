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
		Vector3f forward(0.0f, 0.0f, 1.0f);
		return m_Rotation * forward;
	}

	Vector3f Transform::Up() const
	{
		return Forward().Cross(Right());
	}

	Vector3f Transform::Right() const
	{
		Vector3f right(1.0f, 0.0f, 0.0f);
		return m_Rotation * right;
	}

	void Transform::RotateAround(const Vector3f &point, const Vector3f &axis, float angleInDegrees)
	{
		Quatf q = Quatf::FromAxisAngle(axis, Radianf::FromDegree(angleInDegrees));
		m_Position = point + q * (m_Position - point);
		m_Rotation = q * m_Rotation;
		m_EulerRotation += q.ToEuler();

		m_Dirty = true;
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
			m_WorldRotation = parentTransform.m_WorldRotation * m_Rotation;
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
}