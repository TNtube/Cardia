#pragma once

#include "Cardia/Math/Vector3.hpp"
#include "Cardia/Math/Matrix4.hpp"
#include "Cardia/Math/Quaternion.hpp"

namespace Cardia
{
	class Entity;
}

namespace Cardia::Component
{
	struct Transform
	{
		Transform() = default;
		Transform(const Vector3f& pos, const Vector3f& rot, const Vector3f& sca)
			: m_Position(pos), m_Rotation(rot), m_Scale(sca), m_EulerRotation(rot),
			 m_WorldPosition(pos), m_WorldRotation(rot), m_WorldScale(sca) {}

		const Vector3f& GetPosition() const { return m_Position; }
		const Vector3f& GetRotation() const{ return m_EulerRotation; }
		const Vector3f& GetScale() const { return m_Scale; }

		void SetPosition(const Vector3f& pos) { m_Position = pos; m_Dirty = true; }
		void SetRotation(const Vector3f& rot) { m_Rotation = Quatf(rot); m_EulerRotation = rot; m_Dirty = true; }
		void SetScale(const Vector3f& sca) { m_Scale = sca; m_Dirty = true; }

		void Translate(const Vector3f& translation) { m_Position += translation; m_Dirty = true; }
		void Rotate(const Vector3f & rotation) { m_Rotation *= Quatf(rotation); m_EulerRotation += rotation; m_Dirty = true; }
		void RotateAround(const Vector3f &point, const Vector3f &axis, float angleInDegrees);

		Matrix4f GetLocalTransform() const;
		Matrix4f GetWorldTransform() const;

		Vector3f Forward() const;
		Vector3f Up() const;
		Vector3f Right() const;

		bool IsDirty() const { return m_Dirty; }

		void RecomputeWorld(Entity entity);

		inline void Reset() {
			m_Position = Vector3f(0);
			m_Rotation = Quatf::Identity();
			m_Scale = Vector3f(1);
			m_Dirty = true;
		}

	private:
		bool m_Dirty { true };

		Vector3f m_Position { 0.0f };
		Quatf m_Rotation = Quatf::Identity();
		Vector3f m_Scale { 1.0f };

		Vector3f m_EulerRotation { 0.0f };

		Vector3f m_WorldPosition { 0.0f };
		Quatf m_WorldRotation = Quatf::Identity();
		Vector3f m_WorldScale { 1.0f };

	public:
		constexpr static auto properties = std::make_tuple(
			property(&Transform::m_Position, "Position"),
			property(&Transform::m_Rotation, "Rotation"),
			property(&Transform::m_Scale, "Scale")
		);
	};
}
