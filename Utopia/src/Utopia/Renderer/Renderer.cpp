#include "utpch.hpp"
#include "Renderer.hpp"

namespace Utopia
{
	RendererAPI Renderer::s_RenderAPI = RendererAPI::OpenGL;

	RendererAPI Renderer::getAPI()
	{
		return s_RenderAPI;
	}

	Renderer::Renderer()
		: renderCommand(RenderCommand::create()) {}
}