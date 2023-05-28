#pragma once

#include <vulkan/vulkan_core.h>

#include "Device.hpp"
#include "Cardia/Core/Core.hpp"


namespace Cardia
{
	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:	return 4;
			case ShaderDataType::Float2:	return 4 * 2;
			case ShaderDataType::Float3:	return 4 * 3;
			case ShaderDataType::Float4:	return 4 * 4;
			case ShaderDataType::Mat3:	return 4 * 3 * 3;
			case ShaderDataType::Mat4:	return 4 * 4 * 4;
			case ShaderDataType::Int:	return 4;
			case ShaderDataType::Int2:	return 4 * 2;
			case ShaderDataType::Int3:	return 4 * 3;
			case ShaderDataType::Int4:	return 4 * 4;
			case ShaderDataType::Bool:	return 1;
			default:
				cdCoreAssert(false, "Unknown ShaderDataType.");
				return 0;
		}
	}

	struct BufferElement
	{
		ShaderDataType type {};
		std::string name;
		uint32_t offset{};
		int size{};
		bool normalized{};

		BufferElement() = default;
		BufferElement(ShaderDataType _type, std::string name, bool normalized = false)
			: type(_type), name(std::move(name)), offset(0), size(ShaderDataTypeSize(type)), normalized(false) {}

		int getElementCount() const
		{
			switch (type)
			{
				case ShaderDataType::Float:	return 1;
				case ShaderDataType::Float2:	return 2;
				case ShaderDataType::Float3:	return 3;
				case ShaderDataType::Float4:	return 4;
				case ShaderDataType::Mat3:	return 3 * 3;
				case ShaderDataType::Mat4:	return 4 * 4;
				case ShaderDataType::Int:	return 1;
				case ShaderDataType::Int2:	return 2;
				case ShaderDataType::Int3:	return 3;
				case ShaderDataType::Int4:	return 4;
				case ShaderDataType::Bool:	return 1;
				default:
					cdCoreAssert(false, "Unknown ShaderDataType.");
					return 0;
			}
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements) {
			calculateOffsetAndStride();
		}
		inline const int getStride() const { return m_Stride; }
		inline const auto& getElement() const { return m_Elements; }
		auto begin() { return m_Elements.begin(); }
		auto end() { return m_Elements.end(); }
		auto begin() const { return m_Elements.begin(); }
		auto end() const { return m_Elements.end(); }

	private:
		void calculateOffsetAndStride()
		{
			m_Stride = 0;
			int offset = 0;
			for (auto& element : m_Elements)
			{
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}

		std::vector<BufferElement> m_Elements;
		int m_Stride{};
	};

	class Buffer
	{
	public:
		Buffer(Device& device, VkDeviceSize size, uint32_t instanceCount, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize minOffsetAlignment = 1);
		Buffer(Buffer && other) noexcept;
		Buffer& operator=(Buffer&& other) noexcept;

		void UploadData(VkDeviceSize size, const void* data, VkDeviceSize offset = 0);
		void UploadDataAtIndex(const void* data, uint32_t index);
		VkDescriptorBufferInfo DescriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
		const VkBuffer& GetBuffer() const { return m_Buffer; }
		
		virtual ~Buffer();
	private:
		Device& m_Device;
		VkBuffer m_Buffer {};
		VkDeviceMemory m_BufferMemory {};
		void* m_MappedMemory = nullptr;
		VkDeviceSize m_InstanceSize;
		VkDeviceSize m_AlignmentSize;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual void setLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& getLayout() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;

		static std::unique_ptr<VertexBuffer> create(uint32_t size);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual void bind() const = 0;
		virtual void unbind() const = 0;
		virtual int getCount() const = 0;

		virtual void setData(const void* data, uint32_t size) = 0;
		static std::unique_ptr<IndexBuffer> create(uint32_t count);
		static std::unique_ptr<IndexBuffer> create(uint32_t* indices, uint32_t count);
	};

	class StorageBuffer
	{
	public:
		virtual ~StorageBuffer() = default;
		virtual void bind(int index) const = 0;
		virtual void unbind() const = 0;
		virtual void setData(const void* data, uint32_t size) = 0;

		static std::unique_ptr<StorageBuffer> create(uint32_t size);
		static std::unique_ptr<StorageBuffer> create(void* data, uint32_t size);
	};
	
	class Shader;

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;
		virtual void bind(int index) const = 0;
		virtual void unbind() const = 0;
		virtual void setData(const void* data, uint32_t size) = 0;

		static std::unique_ptr<UniformBuffer> create(uint32_t size);
		static std::unique_ptr<UniformBuffer> create(void* data, uint32_t size);
	};

}
