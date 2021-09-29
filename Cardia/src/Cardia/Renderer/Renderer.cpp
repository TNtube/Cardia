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

	void Renderer::submit(const std::unique_ptr<VertexArray>& vertexArray, const std::unique_ptr<Shader>& shader)
	{
		shader->bind();
		shader->setUniformMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);

		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}