#pragma once

#include "Device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace Cardia {
	class DescriptorLayoutCache;

	class DescriptorSetLayout
	{
	public:
		class Builder {
		public:
			Builder(DescriptorLayoutCache& cache) : m_Cache(cache) {}

			Builder& AddBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			DescriptorSetLayout& Build() const;

		private:
			DescriptorLayoutCache& m_Cache;
			std::vector<VkDescriptorSetLayoutBinding> m_Bindings{};
		};

		DescriptorSetLayout(Device& device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		~DescriptorSetLayout();
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
		DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
	private:
		Device& m_Device;
		VkDescriptorSetLayout m_DescriptorSetLayout{};
		std::vector<VkDescriptorSetLayoutBinding> m_Bindings;

		friend class DescriptorSet;
	};

	class DescriptorLayoutCache
	{
	public:
		DescriptorLayoutCache(Device& device) : m_Device(device) {}

		DescriptorSetLayout& CreateLayout(const std::vector<VkDescriptorSetLayoutBinding>& bindings);

		struct DescriptorLayoutInfo {
			std::vector<VkDescriptorSetLayoutBinding> Bindings;

			bool operator==(const DescriptorLayoutInfo& other) const;

			size_t hash() const;
		};

		friend class DescriptorSetLayout;

	private:

		struct DescriptorLayoutHash
		{

			std::size_t operator()(const DescriptorLayoutInfo& k) const
			{
				return k.hash();
			}
		};

		Device& m_Device;
		std::unordered_map<DescriptorLayoutInfo, DescriptorSetLayout, DescriptorLayoutHash> m_LayoutCache;
	};



	class DescriptorPool
	{
	public:
		class Builder {
		public:
			Builder(Device& device) : m_Device{device} {}
			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(uint32_t count);
			DescriptorPool Build() const;

		private:
			Device& m_Device;
			std::vector<VkDescriptorPoolSize> m_PoolSizes{};
			uint32_t m_MaxSets = 1000;
			VkDescriptorPoolCreateFlags m_PoolFlags = 0;
		};

		DescriptorPool(
			Device& device,
			uint32_t maxSets,
			VkDescriptorPoolCreateFlags poolFlags,
			const std::vector<VkDescriptorPoolSize>& poolSizes);
		~DescriptorPool();
		DescriptorPool(const DescriptorPool&) = delete;
		DescriptorPool& operator=(const DescriptorPool&) = delete;
		DescriptorPool(DescriptorPool&& other) noexcept;
		DescriptorPool& operator=(DescriptorPool&& other) noexcept;

		const VkDescriptorPool& GetPool() const { return m_DescriptorPool; }

		VkResult AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void FreeDescriptors(std::vector<DescriptorSet>& descriptors) const;
		void FreeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const;

		void ResetPool() const;

	private:
		Device& m_Device;
		VkDescriptorPool m_DescriptorPool {VK_NULL_HANDLE};

		friend class DescriptorSet;
	};


	class DescriptorAllocator
	{
	public:
		DescriptorAllocator(Device& device) : m_Device(device) {}

		void ResetPools();
		std::optional<DescriptorSet> Allocate(const DescriptorSetLayout& layout);

	private:
		DescriptorPool CreatePool(uint32_t count, VkDescriptorPoolCreateFlags flags) const;
		DescriptorPool& GrabPool();

		Device& m_Device;

		DescriptorPool* m_CurrentPool {};
		std::vector<DescriptorPool> m_UsedPools;
		std::vector<DescriptorPool> m_FreePools;

		const std::vector<std::pair<VkDescriptorType, float>> m_PoolSizes
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.f },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.f },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.f },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.5f }
		};

		friend class DescriptorSet;
	};


	class DescriptorSet
	{
	public:
		class Writer
		{
		public:
			Writer(DescriptorAllocator& allocator, DescriptorSetLayout& setLayout);
			Writer& WriteBuffer(uint32_t binding, const VkDescriptorBufferInfo *bufferInfo);
			Writer& WriteImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

			std::optional<DescriptorSet> Build();
			void Overwrite(const DescriptorSet& set);
	
		private:
			DescriptorAllocator& m_Allocator;
			DescriptorSetLayout& m_SetLayout;
			std::vector<VkWriteDescriptorSet> m_Writes;
		};


		explicit DescriptorSet(DescriptorPool& descriptorPool) : m_Pool(descriptorPool) {}
		virtual ~DescriptorSet();
		DescriptorSet(DescriptorSet && other) noexcept;
		DescriptorSet& operator=(DescriptorSet&& other) noexcept;
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet& operator=(const DescriptorSet&) = delete;
		const VkDescriptorSet& GetDescriptor() const { return m_DescriptorSet; }
		VkDescriptorSet& GetDescriptor() { return m_DescriptorSet; }

	private:
		DescriptorPool& m_Pool;
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

		friend class DescriptorPool;
	};
}
