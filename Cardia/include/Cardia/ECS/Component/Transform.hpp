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
		Transform(Vector3f pos, Vector3f rot, Vector3f sca)
			: m_Position(pos), m_Rotation(rot), m_Scale(sca),
			 m_WorldPosition(pos), m_WorldRotation(rot), m_WorldScale(sca) {}

		const Vector3f& GetPosition() const { return m_Position; }
		const Vector3f& GetRotation() const{ return m_Rotation; }
		const Vector3f& GetScale() const { return m_Scale; }

		void SetPosition(const Vector3f& pos) { m_Position = pos; m_Dirty = true; }
		void SetRotation(const Vector3f& rot) { m_Rotation = rot; m_Dirty = true; }
		void SetScale(const Vector3f& sca) { m_Scale = sca; m_Dirty = true; }

		Matrix4f GetLocalTransform() const;
		Matrix4f GetWorldTransform() const;

		Vector3f Forward() const;
		Vector3f Up() const;
		Vector3f Right() const;

		bool IsDirty() const { return m_Dirty; }

		void RecomputeWorld(Entity entity);

		inline void Reset() {
			m_Position = Vector3f(0);
			m_Rotation = Vector3f(0);
			m_Scale = Vector3f(1);
			m_Dirty = true;
		}

		Json::Value Serialize() const;
		static std::optional<Transform> Deserialize(const Json::Value& root);


	private:
		bool m_Dirty { true };

		Vector3f m_Position { 0.0f };
		Vector3f m_Rotation { 0.0f };
		Vector3f m_Scale { 1.0f };

		Vector3f m_WorldPosition { 0.0f };
		Quatf m_WorldRotation {};
		Vector3f m_WorldScale { 1.0f };
	};
}
