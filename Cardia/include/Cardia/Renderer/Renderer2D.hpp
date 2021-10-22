#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Camera.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"

namespace Cardia
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Quit();

		static void beginScene(OrthographicCamera camera);
		static void endScene();

		static void startBash();
		static void nextBatch();
		static void render();

		static void drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture, const glm::vec4& color);
	};
}