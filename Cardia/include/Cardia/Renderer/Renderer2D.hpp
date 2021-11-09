#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Camera.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "Texture.hpp"
#include "Cardia/ECS/SceneCamera.hpp"

namespace Cardia
{
	class Renderer2D
	{
	public:
		static void Init();
		static void Quit();

		static void beginScene(OrthographicCamera camera);
		static void beginScene(const Camera& camera, glm::mat4 transform);
		static void endScene();

		static void startBash();
		static void nextBatch();
		static void render();

		struct Stats {
			int drawCalls;
			int triangleCount;
		};

		static Stats& getStats();

		static void drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, float rotation, const glm::vec4& color);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture, float tilingFactor = 1.0f);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, float rotation, const Texture2D* texture, float tilingFactor = 1.0f);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture, const glm::vec4& color, float tilingFactor = 1.0f);
		static void drawRect(const glm::vec3& position, const glm::vec2& size, float rotation, const Texture2D* texture, const glm::vec4& color, float tilingFactor = 1.0f);
		static void drawRect(const glm::mat4& transform, const glm::vec4& color);
		static void drawRect(const glm::mat4& transform, const Texture2D* texture, const glm::vec4& color, float tilingFactor = 1.0f);
	};
}