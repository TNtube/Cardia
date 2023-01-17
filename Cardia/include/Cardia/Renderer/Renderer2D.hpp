#pragma once

#include "Camera.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <Cardia/ECS/Components.hpp>


namespace Cardia
{
	class Renderer2D
	{
	public:
		static void init();
		static void quit();
		static void beginScene(Camera& camera, const glm::mat4& transform);
		static void endScene();

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
		static void drawRect(const glm::mat4& transform, const Texture2D* texture, const glm::vec4& color, float tilingFactor = 1.0f, int32_t zIndex = 0, float entityID = -1);
		static void drawMesh(const glm::mat4& transform, Mesh* mesh, int32_t zIndex = 0, float entityID = -1);

		static void addLight(const Component::Transform& transform, const Component::Light& lightComponent);
	};
}