#include "cdpch.hpp"
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "Cardia/Log.hpp"
#include "Platform/OpenGL/OpenGLBuffer.hpp"


namespace Cardia
{

	VertexBuffer *VertexBuffer::create(float *vertices, uint32_t size)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
				case RenderAPI::API::OpenGL:
				return new OpenGLVertexBuffer(vertices, size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	IndexBuffer *IndexBuffer::create(uint32_t *indices, uint32_t count)
	{
		RenderAPI::API renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return new OpenGLIndexBuffer(indices, count);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}