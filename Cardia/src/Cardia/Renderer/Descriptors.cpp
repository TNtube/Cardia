#include "cdpch.hpp"
#include "Cardia/Renderer/Descriptors.hpp"

#include <cassert>
#include <ranges>
#include <stdexcept>

namespace Cardia
{
	DescriptorSetLayout::Builder& DescriptorSetLayout::Builder::AddBinding(
		uint32_t binding,
		VkDescriptorType descriptorType,
		VkShaderStageFlags stageFlags,
		uint32_t count)
	{
		assert(!bindings.contains(binding) && "Binding already in use");
		VkDescriptorSetLayoutBinding layoutBinding{};
		layoutBinding.binding = binding;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.descriptorCount = count;
		layoutBinding.stageFlags = stageFlags;
		bindings[binding] = layoutBinding;
		return *this;
	}

	std::unique_ptr<DescriptorSetLayout> DescriptorSetLayout::Builder::Build() const {
		return std::make_unique<DescriptorSetLayout>(m_Device, bindings);
	}


	DescriptorSetLayout::DescriptorSetLayout(
		Device& device, const std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>& bindings)
	: m_Device{device}, m_Bindings{bindings}
	{
		std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings{};
		setLayoutBindings.reserve(bindings.size());
		for (auto val : bindings | std::views::values) {
			setLayoutBindings.push_back(val);
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
		descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
		descriptorSetLayoutInfo.pBindings = setLayoutBindings.data();

		if (vkCreateDescriptorSetLayout(
			m_Device.GetDevice(),
			&descriptorSetLayoutInfo,
			nullptr,
			&m_DescriptorSetLayout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout() {
		vkDestroyDescriptorSetLayout(m_Device.GetDevice(), m_DescriptorSetLayout, nullptr);
	}


	DescriptorPool::Builder& DescriptorPool::Builder::AddPoolSize(
		VkDescriptorType descriptorType, uint32_t count)
	{
		m_PoolSizes.push_back({descriptorType, count});
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetPoolFlags(
		VkDescriptorPoolCreateFlags flags)
	{
		m_PoolFlags = flags;
		return *this;
	}

	DescriptorPool::Builder& DescriptorPool::Builder::SetMaxSets(uint32_t count) {
		m_MaxSets = count;
		return *this;
	}

	DescriptorPool DescriptorPool::Builder::Build() const {
		return {m_Device, m_MaxSets, m_PoolFlags, m_PoolSizes};
	}


	DescriptorPool::DescriptorPool(
		Device& device,
		uint32_t maxSets,
		VkDescriptorPoolCreateFlags poolFlags,
		const std::vector<VkDescriptorPoolSize>& poolSizes) : m_Device{device}
	{
		VkDescriptorPoolCreateInfo descriptorPoolInfo{};
		descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		descriptorPoolInfo.pPoolSizes = poolSizes.data();
		descriptorPoolInfo.maxSets = maxSets;
		descriptorPoolInfo.flags = poolFlags;

		if (vkCreateDescriptorPool(m_Device.GetDevice(), &descriptorPoolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	DescriptorPool::~DescriptorPool()
	{
		vkDestroyDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, nullptr);
	}

	DescriptorPool::DescriptorPool(DescriptorPool&& other) noexcept
		: m_Device(other.m_Device)
	{
		std::swap(m_DescriptorPool, other.m_DescriptorPool);
	}

	DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other) noexcept
	{
		std::swap(m_DescriptorPool, other.m_DescriptorPool);
		return *this;
	}

	VkResult DescriptorPool::AllocateDescriptor(
		const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;


		// Might want to create a "DescriptorPoolManager" class that handles this case, and builds
		// a new pool whenever an old pool fills up. But this is beyond our current scope
		return vkAllocateDescriptorSets(m_Device.GetDevice(), &allocInfo, &descriptor);
	}

	void DescriptorPool::FreeDescriptors(std::vector<DescriptorSet>& descriptors) const
	{
		std::vector<VkDescriptorSet> vkDescriptors;

		std::ranges::transform(
			descriptors,
			std::back_inserter(vkDescriptors),
			[](DescriptorSet& descriptor)
			{
				const auto out = descriptor.m_DescriptorSet;
				descriptor.m_DescriptorSet = VK_NULL_HANDLE;
				return out;
			});
		FreeDescriptors(vkDescriptors);
	}

	void DescriptorPool::FreeDescriptors(const std::vector<VkDescriptorSet>& descriptors) const
	{
		vkFreeDescriptorSets(
			m_Device.GetDevice(),
			m_DescriptorPool,
			static_cast<uint32_t>(descriptors.size()),
			descriptors.data());
	}

	void DescriptorPool::ResetPool() const
	{
		vkResetDescriptorPool(m_Device.GetDevice(), m_DescriptorPool, 0);
	}

	void DescriptorAllocator::ResetPools()
	{
		for (auto& p : m_UsedPools)
		{
			p.ResetPool();
		}

		m_FreePools = std::move(m_UsedPools);
		m_UsedPools.clear();
		m_CurrentPool = nullptr;
	}

	std::optional<DescriptorSet> DescriptorAllocator::Allocate(const DescriptorSetLayout& layout)
	{
		if (!m_CurrentPool)
		{
			m_CurrentPool = &GrabPool();
		}

		DescriptorSet descriptorSet(*m_CurrentPool);
		auto result = m_CurrentPool->AllocateDescriptor(layout.GetDescriptorSetLayout(), descriptorSet.GetDescriptor());

		switch (result) {
		case VK_SUCCESS:
			//all good, return
			return { std::move(descriptorSet) };
		case VK_ERROR_FRAGMENTED_POOL:
		case VK_ERROR_OUT_OF_POOL_MEMORY:
			//reallocate pool
			break;
		default:
			//unrecoverable error
			return {};
		}

		m_CurrentPool = &GrabPool();
		descriptorSet = DescriptorSet(*m_CurrentPool);
		result = m_CurrentPool->AllocateDescriptor(layout.GetDescriptorSetLayout(), descriptorSet.GetDescriptor());

		if (result != VK_SUCCESS)
			return {};

		return { std::move(descriptorSet) };
	}

	DescriptorPool DescriptorAllocator::CreatePool(uint32_t count, VkDescriptorPoolCreateFlags flags) const
	{
		auto poolBuilder = DescriptorPool::Builder(m_Device);
		for (auto [type, factor] : m_PoolSizes)
		{
			poolBuilder.AddPoolSize(type, static_cast<uint32_t>(factor * static_cast<float>(count)));
		}

		poolBuilder.SetMaxSets(count);
		poolBuilder.SetPoolFlags(flags);

		return poolBuilder.Build();
	}

	DescriptorPool& DescriptorAllocator::GrabPool()
	{
		if (!m_FreePools.empty())
		{
			DescriptorPool& pool = m_FreePools.back();
			m_FreePools.pop_back();
			return pool;
		}
		else {
			m_UsedPools.push_back(CreatePool(1000, VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT));
			return m_UsedPools.back();
		}
	}


	DescriptorSet::Writer::Writer(DescriptorAllocator& allocator, DescriptorSetLayout& setLayout)
		: m_Allocator{allocator}, m_SetLayout{setLayout} {}
	
	DescriptorSet::Writer& DescriptorSet::Writer::WriteBuffer(
		uint32_t binding, const VkDescriptorBufferInfo *bufferInfo)
	{
		assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

		const auto& bindingDescription = m_SetLayout.m_Bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pBufferInfo = bufferInfo;
		write.descriptorCount = 1;

		m_Writes.push_back(write);
		return *this;
	}

	DescriptorSet::Writer& DescriptorSet::Writer::WriteImage(
		uint32_t binding, VkDescriptorImageInfo *imageInfo) {
		assert(m_SetLayout.m_Bindings.count(binding) == 1 && "Layout does not contain specified binding");

		auto& bindingDescription = m_SetLayout.m_Bindings[binding];

		assert(
			bindingDescription.descriptorCount == 1 &&
			"Binding single descriptor info, but binding expects multiple");

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.descriptorType = bindingDescription.descriptorType;
		write.dstBinding = binding;
		write.pImageInfo = imageInfo;
		write.descriptorCount = 1;

		m_Writes.push_back(write);
		return *this;
	}

	std::optional<DescriptorSet> DescriptorSet::Writer::Build() {
		auto out = m_Allocator.Allocate(m_SetLayout);
		if (!out.has_value()) {
			return {};
		}
		Overwrite(out.value());
		return out;
	}

	void DescriptorSet::Writer::Overwrite(const DescriptorSet& set) {
		for (auto& write : m_Writes) {
			write.dstSet = set.m_DescriptorSet;
		}
		vkUpdateDescriptorSets(m_Allocator.m_Device.GetDevice(), m_Writes.size(), m_Writes.data(), 0, nullptr);
	}

	DescriptorSet::~DescriptorSet()
	{
		m_Pool.FreeDescriptors({m_DescriptorSet});
	}

	DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept : m_Pool(other.m_Pool)
	{
		m_DescriptorSet = other.m_DescriptorSet;
		other.m_DescriptorSet = VK_NULL_HANDLE;
	}

	DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
	{
		m_DescriptorSet = other.m_DescriptorSet;
		other.m_DescriptorSet = VK_NULL_HANDLE;
		return *this;
	}
}
