#include "cdpch.hpp"
#include "Renderer.hpp"

namespace Cardia
{

	void Renderer::beginScene()
	{
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const std::unique_ptr<VertexArray> &vertexArray)
	{
		vertexArray->bind();
		RenderCommand::drawIndexed(vertexArray);
	}
}