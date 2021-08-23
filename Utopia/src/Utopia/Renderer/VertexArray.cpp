#include "utpch.hpp"
#include "VertexArray.hpp"
#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLVertexArray.hpp"

namespace Utopia
{

	VertexArray *VertexArray::create()
	{
		RendererAPI renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RendererAPI::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLVertexArray();
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}