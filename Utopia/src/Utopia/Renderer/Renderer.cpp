#include "utpch.hpp"
#include "Renderer.hpp"

namespace Utopia
{
	RendererAPI Renderer::m_RenderAPI = RendererAPI::OpenGL;

	RendererAPI Renderer::getAPI()
	{
		return m_RenderAPI;
	}
}