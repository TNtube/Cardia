#pragma once
#include "RenderCommand.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

namespace Cardia
{

	class Renderer
	{
	public:
		static void beginScene(OrthographicCamera& camera);
		static void endScene();
		static void submit(const std::unique_ptr<VertexArray>& vertexArray,
				   const std::unique_ptr<Shader>& shader, const glm::mat4& transform = glm::mat4(1.0f));
		inline static RenderAPI::API getAPI() { return RenderAPI::getAPI(); };

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;
	};
}