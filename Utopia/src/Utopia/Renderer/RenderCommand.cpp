#include "utpch.hpp"
#include "RenderCommand.hpp"
#include "Renderer.hpp"
#include "Utopia/Log.hpp"
#include "Platform/OpenGL/OpenGLRenderCommand.hpp"


namespace Utopia
{

	RenderCommand *RenderCommand::create()
	{
		RendererAPI renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RendererAPI::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLRenderCommand();
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}
