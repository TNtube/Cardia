#include "Platform/OpenGL/OpenGLShader.hpp"
#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{
	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	void Renderer::beginScene(OrthographicCamera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const std::unique_ptr<VertexArray>& vertexArray,
			      const std::unique_ptr<Shader>& shader, const glm::mat4& transform)
	{
		shader->bind();
		dynamic_cast<Cardia::OpenGLShader&>(*shader).setUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		dynamic_cast<Cardia::OpenGLShader&>(*shader).setUniformMat4("u_Model", transform);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}