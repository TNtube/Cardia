#pragma once

#include <vulkan/vulkan_core.h>

#include "RenderAPI.hpp"
#include "Camera.hpp"
#include "Shader.hpp"


namespace Cardia
{
	class Window;

	class Renderer
	{
	public:
		static void beginScene(Camera& camera);
		static void endScene();
		static void submit(const VertexArray* vertexArray,
				   Shader* shader, const glm::mat4& transform = glm::mat4(1.0f));
		inline static RenderAPI::API& getAPI() { return RenderAPI::getAPI(); }


	private:
		
		Renderer() = default;

		
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;
	};
}