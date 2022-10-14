#include "cdpch.hpp"
#include "Cardia/Renderer/Buffer.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLBuffer.hpp"


namespace Cardia
{

	std::unique_ptr<VertexBuffer> VertexBuffer::create(float *vertices, uint32_t size)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLVertexBuffer>(vertices, size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	std::unique_ptr<VertexBuffer> VertexBuffer::create(uint32_t size)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLVertexBuffer>(size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	std::unique_ptr<IndexBuffer> IndexBuffer::create(uint32_t *indices, uint32_t count)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLIndexBuffer>(indices, count);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	std::unique_ptr<IndexBuffer> IndexBuffer::create(uint32_t count)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
		case RenderAPI::API::None:
			Log::coreError("{0} is not supported for the moment !", renderer);
			cdCoreAssert(false, "Invalid API provided");
			return nullptr;
		case RenderAPI::API::OpenGL:
			return std::make_unique<OpenGLIndexBuffer>(count);
		default:
			Log::coreError("{0} is not supported for the moment !", renderer);
			cdCoreAssert(false, "Invalid API provided");
			return nullptr;
		}
	}
}
