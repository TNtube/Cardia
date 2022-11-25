#pragma once

#include "Cardia/Renderer/Texture.hpp"
#include "SceneCamera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <utility>


namespace Cardia::Component
{
	struct Name
	{
		Name() = default;
		Name(const Name&) = default;
		explicit Name(std::string name)
			: name(std::move(name)) {}

		std::string name;
	};

	struct ID
	{
		ID() : uuid(UUID()) {};
		ID(const ID&) = default;

		explicit ID(UUID uuid)
			: uuid(uuid) {}

		UUID uuid;
	};

	struct Transform
	{
		Transform() = default;
		Transform(const Transform&) = default;
		Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 sca) : position(pos), rotation(rot), scale(sca) {}

		glm::vec3 position { 0.0f };
		glm::vec3 rotation { 0.0f };
		glm::vec3 scale { 1.0f };
		inline glm::mat4 getTransform() const {
			return glm::translate(glm::mat4(1.0f), position)
			     * glm::toMat4(glm::quat(rotation))
			     * glm::scale(glm::mat4(1.0f), scale);
		}

		inline void reset() {
			position = glm::vec3(0);
			rotation = glm::vec3(0);
			scale = glm::vec3(1);
		}
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

		inline void reset() {
			texture = nullptr;
			tillingFactor = 1.0f;
			color = glm::vec4(1.0f);
		}
	};

	struct Camera
	{
		SceneCamera camera;
		bool primary = true;
		Camera() = default;
		Camera(const Camera&) = default;

		inline void reset()
		{
			camera = SceneCamera();
		}
	};


	struct PointLight2D
	{
		PointLight2D() : color(1), range(2.0f) {}
		PointLight2D(const PointLight2D&) = default;

		glm::vec3 color;
		float range;

		inline void reset() {
			color = glm::vec3(1);
			range = 2.0f;
		}
	};

	struct Script
	{
		Script() = default;
		Script(const Script&) { reloadFile(); }
		explicit Script(std::string scriptPath) : m_Path(std::move(scriptPath)) {
			reloadFile();
		}
		inline std::string getPath() const {return m_Path; }
		inline void setPath(const std::string& newPath) { m_Path = newPath; reloadFile(); }
		ScriptClass scriptClass;

		inline void reset()
		{
			m_Path.clear();
			scriptClass = ScriptClass();
		}

	private:
		void reloadFile();
		std::string m_Path;
	};
}
