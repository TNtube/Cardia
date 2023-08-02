#pragma once

#include "Cardia/Renderer/Texture.hpp"
#include "SceneCamera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include "Cardia/Renderer/MeshRenderer.hpp"

#include <utility>

#include "Cardia/Math/Matrix4.hpp"
#include "Cardia/Math/Quaternion.hpp"
#include "Cardia/Math/Vector3.hpp"


namespace Cardia::Component
{
	namespace LightType {
		enum LightType : int32_t {
			DirectionalLight = 0,
			PointLight,
			SpotLight
		};
	}

	struct Relationship
	{
		Relationship() = default;

		std::size_t ChildCount = 0;
		entt::entity Parent = entt::null;
		entt::entity FirstChild = entt::null;
		entt::entity PreviousSibling = entt::null;
		entt::entity NextSibling = entt::null;
		
		Json::Value Serialize() const;
		static std::optional<Relationship> Deserialize(const Json::Value& root);
	};

	struct Label
	{
		Label() = default;
		explicit Label(std::string Label)
			: Name(std::move(Label)) {}

		std::string Name;
		Vector4f Color { 1.0f };

		inline void Reset() {
			Name = "Default";
			Color = Vector4f(1.0f);
		}

		Json::Value Serialize() const;
		static std::optional<Label> Deserialize(const Json::Value& root);
	};

	struct ID
	{
		ID() : Uuid(UUID()) {}
		explicit ID(UUID uuid)
			: Uuid(uuid) {}

		UUID Uuid;

		Json::Value Serialize() const;
		static std::optional<ID> Deserialize(const Json::Value& root);
	};

	struct Transform
	{
		Transform() = default;
		Transform(Vector3f pos, Vector3f rot, Vector3f sca) : Position(pos), Rotation(rot), Scale(sca) {}

		Vector3f Position { 0.0f };
		Vector3f Rotation { 0.0f };
		Vector3f Scale { 1.0f };
		inline Matrix4f GetTransform() const {
			return Matrix4f::Identity().Translate(Position)
				 * Quaternion(Rotation).ToMatrix()
				 * Matrix4f::Identity().Scale(Scale);
		}

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

	struct SpriteRenderer
	{
		SpriteRenderer() = default;
		explicit SpriteRenderer(const Vector4f& color)
			: Color(color) {}

		Vector4f Color { 1.0f };
		std::shared_ptr<Texture2D> Texture = nullptr;
		float TillingFactor = 1.0f;
		int32_t ZIndex = 0;

		inline void Reset() {
			Texture = nullptr;
			TillingFactor = 1.0f;
			Color = Vector4f(1.0f);
		}

		Json::Value Serialize() const;
		static std::optional<SpriteRenderer> Deserialize(const Json::Value& root);
	};

	struct MeshRendererC
	{
		MeshRendererC() : Renderer(std::make_shared<MeshRenderer>()) {}

		std::shared_ptr<MeshRenderer> Renderer = nullptr;

		inline void Reset() {
			Renderer = nullptr;
		}

		Json::Value Serialize() const;
		static std::optional<MeshRendererC> Deserialize(const Json::Value& root);
	};

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

	struct Light
	{
		Light() = default;
		int32_t LightType {};
		Vector3f Color {};
		float Range = 2;
		float Angle = 35;
		float Smoothness = 1;

		inline void Reset() {
			Color = Vector3f(0);
			Range = 2;
			Angle = 35;
			Smoothness = 1;
		}

		Json::Value Serialize() const;
		static std::optional<Light> Deserialize(const Json::Value& root);
	};

	struct Script
	{
		Script() = default;
		explicit Script(std::string scriptPath) : m_Path(std::move(scriptPath)) {
			ReloadFile();
		}
		inline std::string GetPath() const {return m_Path; }
		inline void SetPath(const std::string& newPath) { m_Path = newPath; ReloadFile(); }
		ScriptClass Class;

		inline void Reset()
		{
			m_Path.clear();
			Class = ScriptClass();
		}

		Json::Value Serialize() const;
		static std::optional<Script> Deserialize(const Json::Value& root);

	private:
		void ReloadFile();
		std::string m_Path;
	};
}

namespace Cardia {


	template<typename... Components>
	struct ComponentGroup
	{

	};

	using AllComponents = ComponentGroup<
		Component::Relationship, Component::Label, Component::ID,
		Component::Transform, Component::SpriteRenderer, Component::MeshRendererC,
		Component::Camera, Component::Light, Component::Script
	>;

	using SerializableComponents = ComponentGroup<
		Component::Label, Component::ID, Component::Transform,
		Component::SpriteRenderer, Component::MeshRendererC, Component::Camera,
		Component::Light, Component::Script
	>;
}
