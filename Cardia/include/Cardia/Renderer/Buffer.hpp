#pragma once

#include <volk.h>

#include "Device.hpp"
#include "Cardia/Core/Core.hpp"


namespace Cardia
{
	class Buffer
	{
	public:
		Buffer(const Device& device, VkDeviceSize size, uint32_t instanceCount, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize minOffsetAlignment = 1);
		Buffer(Buffer && other) noexcept;
		Buffer& operator=(Buffer&& other) noexcept;
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		void UploadData(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);
		void UploadDataAtIndex(const void* data, uint32_t index);
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
		const VkBuffer& GetBuffer() const { return m_Buffer; }
		
		virtual ~Buffer();
	private:
		const Device& m_Device;
		VkBuffer m_Buffer {};
		VkDeviceMemory m_BufferMemory {};
		void* m_MappedMemory = nullptr;
		VkDeviceSize m_InstanceSize;
		VkDeviceSize m_AlignmentSize;
	};
}
