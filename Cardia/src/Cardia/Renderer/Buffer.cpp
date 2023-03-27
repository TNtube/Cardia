#include "cdpch.hpp"
#include "Cardia/Renderer/Buffer.hpp"
#include "Cardia/Renderer/Renderer.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLBuffer.hpp"


namespace Cardia
{

	std::unique_ptr<VertexBuffer> VertexBuffer::create(Vertex* vertices, uint32_t size)
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

	Buffer::Buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
	{
		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		const auto& device = Renderer::Instance().GetDevice();

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
			throw std::runtime_error("Vulkan : failed to create vertex buffer !");

		VkMemoryRequirements memoryRequirements {};
		vkGetBufferMemoryRequirements(device, m_Buffer, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = Renderer::Instance().FindMemoryType(memoryRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocateInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
			throw std::runtime_error("Vulkan : Failed to allocate vertex buffer memory !");

		vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);
	}

	void Buffer::UploadData(size_t size, void* data) const
	{
		const auto& device = Renderer::Instance().GetDevice();
		void* memory;
		vkMapMemory(device, m_BufferMemory, 0, size, 0, &memory);
		memcpy(data, data, size);
		vkUnmapMemory(device, m_BufferMemory);
	}

	Buffer::~Buffer()
	{
		const auto& device = Renderer::Instance().GetDevice();
		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
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

	std::unique_ptr<StorageBuffer> StorageBuffer::create(uint32_t size)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLStorageBuffer>(size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	std::unique_ptr<StorageBuffer> StorageBuffer::create(void *data, uint32_t size)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLStorageBuffer>(data, size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	std::unique_ptr<UniformBuffer> UniformBuffer::create(uint32_t size)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLUniformBuffer>(size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}

	std::unique_ptr<UniformBuffer> UniformBuffer::create(void *data, uint32_t size)
	{
		RenderAPI::API& renderer = Renderer::getAPI();
		switch (renderer)
		{
			case RenderAPI::API::None:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
			case RenderAPI::API::OpenGL:
				return std::make_unique<OpenGLUniformBuffer>(data, size);
			default:
				Log::coreError("{0} is not supported for the moment !", renderer);
				cdCoreAssert(false, "Invalid API provided");
				return nullptr;
		}
	}
}
