#include "cdpch.hpp"
#include "Cardia/Renderer/Buffer.hpp"
#include "Cardia/Core/Log.hpp"


namespace Cardia
{
	static VkDeviceSize GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) {
		if (minOffsetAlignment > 0) {
			return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
		}
		return instanceSize;
	}

	Buffer::Buffer(Device& device, VkDeviceSize size, uint32_t instanceCount, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize minOffsetAlignment)
		: m_Device(device), m_InstanceSize(size)
	{
		m_AlignmentSize = GetAlignment(size, minOffsetAlignment);
		const auto bufferSize = m_AlignmentSize * instanceCount;
		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		const auto& vkDevice = m_Device.GetDevice();

		if (vkCreateBuffer(vkDevice, &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS)
			throw std::runtime_error("Vulkan : failed to create vertex buffer !");

		VkMemoryRequirements memoryRequirements {};
		vkGetBufferMemoryRequirements(vkDevice, m_Buffer, &memoryRequirements);

		VkMemoryAllocateInfo allocateInfo {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.allocationSize = memoryRequirements.size;
		allocateInfo.memoryTypeIndex = m_Device.FindMemoryType(memoryRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(vkDevice, &allocateInfo, nullptr, &m_BufferMemory) != VK_SUCCESS)
			throw std::runtime_error("Vulkan : Failed to allocate vertex buffer memory !");

		vkBindBufferMemory(vkDevice, m_Buffer, m_BufferMemory, 0);
	}

	Buffer::Buffer(Buffer&& other) noexcept : m_Device(other.m_Device)
	{
		m_Buffer = other.m_Buffer;
		m_BufferMemory = other.m_BufferMemory;
		m_MappedMemory = other.m_MappedMemory;

		other.m_Buffer = VK_NULL_HANDLE;
		other.m_BufferMemory = VK_NULL_HANDLE;
		other.m_MappedMemory = nullptr;

		m_AlignmentSize = other.m_AlignmentSize;
		m_InstanceSize = other.m_InstanceSize;
	}

	Buffer& Buffer::operator=(Buffer&& other) noexcept
	{
		m_Buffer = other.m_Buffer;
		m_BufferMemory = other.m_BufferMemory;
		m_MappedMemory = other.m_MappedMemory;

		other.m_Buffer = VK_NULL_HANDLE;
		other.m_BufferMemory = VK_NULL_HANDLE;
		other.m_MappedMemory = nullptr;

		m_AlignmentSize = other.m_AlignmentSize;
		m_InstanceSize = other.m_InstanceSize;

		return *this;
	}

	void Buffer::UploadData(VkDeviceSize size, const void* data, VkDeviceSize offset)
	{
		vkMapMemory(m_Device.GetDevice(), m_BufferMemory, 0, VK_WHOLE_SIZE, 0, &m_MappedMemory);
		memcpy(m_MappedMemory, data, m_InstanceSize);
		vkUnmapMemory(m_Device.GetDevice(), m_BufferMemory);
	}

	void Buffer::UploadDataAtIndex(const void* data, uint32_t index)
	{
		UploadData(m_InstanceSize, data, index * m_InstanceSize);
	}

	VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset) const
	{
		return VkDescriptorBufferInfo{
			m_Buffer,
			offset,
			size,
		};
	}

	Buffer::~Buffer()
	{
		const auto& device = m_Device.GetDevice();
		// if (m_MappedMemory)
		vkDeviceWaitIdle(device);
		vkDestroyBuffer(device, m_Buffer, nullptr);
		vkFreeMemory(device, m_BufferMemory, nullptr);
	}

	std::unique_ptr<VertexBuffer> VertexBuffer::create(uint32_t size)
	{
		return nullptr;
	}

	std::unique_ptr<IndexBuffer> IndexBuffer::create(uint32_t *indices, uint32_t count)
	{
		return nullptr;
	}

	std::unique_ptr<IndexBuffer> IndexBuffer::create(uint32_t count)
	{
		return nullptr;
	}

	std::unique_ptr<StorageBuffer> StorageBuffer::create(uint32_t size)
	{
		return nullptr;
	}

	std::unique_ptr<StorageBuffer> StorageBuffer::create(void *data, uint32_t size)
	{
		return nullptr;
	}

	std::unique_ptr<UniformBuffer> UniformBuffer::create(uint32_t size)
	{
		return nullptr;
	}

	std::unique_ptr<UniformBuffer> UniformBuffer::create(void *data, uint32_t size)
	{
		return nullptr;
	}
}
