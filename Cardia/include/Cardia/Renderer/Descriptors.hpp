#pragma once

#include "Device.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace Cardia {
	class DescriptorSetLayout {
	public:
		class Builder {
		public:
			Builder(Device& device) : m_Device{device} {}

			Builder& AddBinding(
				uint32_t binding,
				VkDescriptorType descriptorType,
				VkShaderStageFlags stageFlags,
				uint32_t count = 1);
			std::unique_ptr<DescriptorSetLayout> Build() const;

		private:
			Device& m_Device;
			std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
		};

		DescriptorSetLayout(
			Device& device, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings);
		~DescriptorSetLayout();
		DescriptorSetLayout(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
		DescriptorSetLayout(const DescriptorSetLayout&&) = delete;
		DescriptorSetLayout& operator=(const DescriptorSetLayout&&) = delete;

		VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }
	private:
		Device& m_Device;
		VkDescriptorSetLayout m_DescriptorSetLayout{};
		std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;

		friend class DescriptorSet;
	};

	class DescriptorPool {
	public:
		class Builder {
		public:
			Builder(Device& device) : m_Device{device} {}
			Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
			Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
			Builder& SetMaxSets(uint32_t count);
			std::unique_ptr<DescriptorPool> Build() const;

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
		DescriptorPool(const DescriptorPool&&) = delete;
		DescriptorPool& operator=(const DescriptorPool&&) = delete;

		const VkDescriptorPool& GetPool() const { return m_DescriptorPool; }

		bool AllocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

		void FreeDescriptors(std::vector<DescriptorSet>& descriptors) const;
		void FreeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const;

		void ResetPool() const;

	private:
		Device& m_Device;
		VkDescriptorPool m_DescriptorPool{};

		friend class DescriptorSet;
	};

	class DescriptorSet
	{
	public:
		class Writer
		{
		public:
			Writer(DescriptorSetLayout& setLayout, DescriptorPool& pool);

			Writer& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
			Writer& WriteImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

			std::optional<DescriptorSet> Build();
			void Overwrite(const DescriptorSet& set);
	
		private:
			DescriptorSetLayout& m_SetLayout;
			DescriptorPool& m_Pool;
			std::vector<VkWriteDescriptorSet> m_Writes;
		};

		virtual ~DescriptorSet();
		DescriptorSet(DescriptorSet && other) noexcept;
		DescriptorSet& operator=(DescriptorSet&& other) noexcept;
		DescriptorSet(const DescriptorSet&) = delete;
		DescriptorSet& operator=(const DescriptorSet&) = delete;
		const VkDescriptorSet& GetDescriptor() const { return m_DescriptorSet; }

	private:
		explicit DescriptorSet(DescriptorPool& descriptorPool) : m_Pool(descriptorPool) {}
		DescriptorPool& m_Pool;
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;

		friend class DescriptorPool;
	};
}
