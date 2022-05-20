#pragma once

#include "Cardia/Renderer/Texture.hpp"
#include "SceneCamera.hpp"

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

	struct Transform
	{
		Transform() = default;
		Transform(const Transform&) = default;

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
}
