#include "cdpch.hpp"
#include "Cardia/Renderer/Framebuffer.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLFramebuffer.hpp"


namespace Cardia
{

	std::unique_ptr<Framebuffer> Framebuffer::create(FramebufferSpec spec)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLFramebuffer>(spec);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}