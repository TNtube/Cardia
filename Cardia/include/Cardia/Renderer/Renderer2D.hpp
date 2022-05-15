#pragma once

#include "Camera.hpp"
#include "Texture.hpp"

#include <glm/glm.hpp>


namespace Cardia
{
	class Renderer2D
	{
	public:
		static void init();
		static void quit();

		static void beginScene(OrthographicCamera camera);
		static void beginScene(Camera& camera, glm::mat4 transform);
		static void beginScene(Camera& camera);
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