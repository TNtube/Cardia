#include "utpch.hpp"
#include "Buffer.hpp"
#include "Renderer.hpp"
#include "Utopia/Log.hpp"
#include "Platform/OpenGL/OpenGLBuffer.hpp"


namespace Utopia
{

	VertexBuffer *VertexBuffer::create(float *vertices, uint32_t size)
	{
		RendererAPI renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RendererAPI::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLVertexBuffer(vertices, size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	IndexBuffer *IndexBuffer::create(uint32_t *indices, uint32_t count)
	{
		RendererAPI renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RendererAPI::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RendererAPI::OpenGL:
				return new OpenGLIndexBuffer(indices, count);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				utCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}