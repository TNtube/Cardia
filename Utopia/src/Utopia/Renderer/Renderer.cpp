#include "utpch.hpp"
#include "Renderer.hpp"

namespace Utopia
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