#pragma once

#include "Cardia/Renderer/Texture.hpp"
#include "SceneCamera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include "Cardia/Renderer/MeshRenderer.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <utility>


namespace Cardia::Component
{
	namespace LightType {
		enum LightType : int32_t {
			DirectionalLight = 0,
			PointLight,
			SpotLight
		};
	}

	struct Name
	{
		Name() = default;
		Name(const Name&) = default;
		explicit Name(std::string name)
			: name(std::move(name)) {}

		std::string name;

		static constexpr std::string ClassName() { return "Name"; };
	};

	struct ID
	{
		ID() : uuid(UUID()) {};
		ID(const ID&) = default;

		explicit ID(UUID uuid)
			: uuid(uuid) {}

		UUID uuid;

		static constexpr std::string ClassName() { return "ID"; };
	};

	struct Transform
	{
		Transform() = default;
		Transform(const Transform&) = default;
		Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca) : position(pos), rotation(rot), scale(sca) {}

		glm::vec3 position { 0.0f };
		glm::vec3 rotation { 0.0f };
		glm::vec3 scale { 1.0f };
		inline glm::mat4 GetTransform() const {
			return glm::translate(glm::mat4(1.0f), position)
				 * glm::toMat4(glm::quat(rotation))
				 * glm::scale(glm::mat4(1.0f), scale);
		}

		glm::vec3 Forward() const;
		glm::vec3 Up() const;
		glm::vec3 Right() const;

		inline void Reset() {
			position = glm::vec3(0);
			rotation = glm::vec3(0);
			scale = glm::vec3(1);
		}

		static constexpr std::string ClassName() { return "Transform"; }
	};

	struct SpriteRenderer
	{
		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		explicit SpriteRenderer(const glm::vec4 color)
			: color(color) {}

		glm::vec4 color { 1.0f };
		std::shared_ptr<Texture2D> texture = nullptr;
		float tillingFactor = 1.0f;
		int32_t zIndex = 0;

		inline void Reset() {
			texture = nullptr;
			tillingFactor = 1.0f;
			color = glm::vec4(1.0f);
		}

		static constexpr std::string ClassName() { return "SpriteRenderer"; }
	};

	struct MeshRendererC
	{
		MeshRendererC() : meshRenderer(std::make_shared<MeshRenderer>()) {}
		MeshRendererC(const MeshRendererC&) = default;


		std::shared_ptr<MeshRenderer> meshRenderer = nullptr;

		inline void Reset() {
			meshRenderer = nullptr;
		}

		static constexpr std::string ClassName() { return "MeshRendererC"; }
	};

	struct Camera
	{
		SceneCamera camera;
		bool primary = true;
		Camera() = default;
		Camera(const Camera&) = default;

		inline void Reset() {
			camera = SceneCamera();
		}

		static constexpr std::string ClassName() { return "Camera"; }
	};

	struct Light
	{
		Light() = default;
		Light(const Light&) = default;
		int32_t lightType {};
		glm::vec3 color {};
		float range = 2;
		float angle = 35;
		float smoothness = 1;

		inline void Reset() {
			color = glm::vec3(0);
			range = 2;
			angle = 35;
			smoothness = 1;
		}

		static constexpr std::string ClassName() { return "Light"; }
	};

	struct Script
	{
		Script() = default;
		Script(const Script&) { ReloadFile(); }
		explicit Script(std::string scriptPath) : m_Path(std::move(scriptPath)) {
			ReloadFile();
		}
		inline std::string GetPath() const {return m_Path; }
		inline void SetPath(const std::string& newPath) { m_Path = newPath; ReloadFile(); }
		ScriptClass scriptClass;

		inline void Reset()
		{
			m_Path.clear();
			scriptClass = ScriptClass();
		}

		static constexpr std::string ClassName() { return "Script"; }

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

	using AllComponents = ComponentGroup<Component::Transform, Component::MeshRendererC, Component::Name,
										 Component::SpriteRenderer, Component::Camera, Component::Script,
										 Component::Light, Component::ID>;
}
