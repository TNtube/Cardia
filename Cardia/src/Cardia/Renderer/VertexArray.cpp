#include "cdpch.hpp"
#include "Cardia/Renderer/VertexArray.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLVertexArray.hpp"


namespace Cardia
{

	std::unique_ptr<VertexArray> VertexArray::create()
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLVertexArray>();
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}