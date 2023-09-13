#pragma once

#include "Camera.hpp"
#include "Texture.hpp"
#include "Cardia/DataStructure/Mesh.hpp"

#include <Cardia/ECS/Components.hpp>


namespace Cardia
{
	class Renderer2D
	{
	public:
		static void init();
		static void quit();
		static void beginScene(Camera& camera, const Matrix4f& transform);
		static void endScene();

		struct Stats {
			int drawCalls;
			int triangleCount;
		};

		static Stats& getStats();

		static void drawRect(const Vector3f& position, const Vector2f& size, const Vector4f& color);
		static void drawRect(const Vector3f& position, const Vector2f& size, float rotation, const Vector4f& color);
		static void drawRect(const Vector3f& position, const Vector2f& size, const Texture* texture, float tilingFactor = 1.0f);
		static void drawRect(const Vector3f& position, const Vector2f& size, float rotation, const Texture* texture, float tilingFactor = 1.0f);
		static void drawRect(const Vector3f& position, const Vector2f& size, const Texture* texture, const Vector4f& color, float tilingFactor = 1.0f);
		static void drawRect(const Vector3f& position, const Vector2f& size, float rotation, const Texture* texture, const Vector4f& color, float tilingFactor = 1.0f);
		static void drawRect(const Matrix4f& transform, const Vector4f& color);
		static void drawRect(const Matrix4f& transform, const Texture* texture, const Vector4f& color, float tilingFactor = 1.0f, int32_t zIndex = 0, float entityID = -1);

		static void addLight(const Component::Transform& transform, const Component::Light& lightComponent);
	};
}