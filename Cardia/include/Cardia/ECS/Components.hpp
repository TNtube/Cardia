#pragma once
#include <glm/glm.hpp>
#include <utility>
#include "Cardia/Renderer/Texture.hpp"
#include "SceneCamera.hpp"


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
		glm::vec3 scale { 1.0f };;
	};

	struct SpriteRenderer
	{
		SpriteRenderer() = default;
		SpriteRenderer(const SpriteRenderer&) = default;
		explicit SpriteRenderer(const glm::vec4 color)
			: color(color) {}

		glm::vec4 color { 1.0f };
		Texture2D* texture = nullptr;
		float tillingFactor = 1.0f;
	};

	struct Camera
	{
		SceneCamera camera;
		Camera() = default;
		Camera(const Camera&) = default;

	private:
		glm::mat4 m_Projection;
	};
}
