﻿#pragma once
#include "Descriptors.hpp"
#include "Device.hpp"

namespace Cardia
{

	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class PipelineLayout
	{
	public:
		PipelineLayout(Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
		PipelineLayout(Device& device, const std::vector<DescriptorSetLayout>& descriptorSetLayouts);
		PipelineLayout(const PipelineLayout&) = delete;
		PipelineLayout& operator=(const PipelineLayout&) = delete;
		PipelineLayout(const PipelineLayout&&) = delete;
		PipelineLayout& operator=(const PipelineLayout&&) = delete;
		virtual ~PipelineLayout();

		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }

	private:
		void Init(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts);
		Device& m_Device;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	};

	class Pipeline
	{
	public:
		Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);
		virtual ~Pipeline();
		Pipeline(const Pipeline&) = delete;
		Pipeline& operator=(const Pipeline&) = delete;
		Pipeline(const Pipeline&&) = delete;
		Pipeline& operator=(const Pipeline&&) = delete;

		void Bind(VkCommandBuffer commandBuffer) const;

		static PipelineConfigInfo DefaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> ReadFile(const std::filesystem::path& filepath);

		void CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);

		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const;

	private:
		Device& m_Device;

		VkPipeline m_GraphicsPipeline {};
		VkShaderModule m_VertShader {};
		VkShaderModule m_FragShader {};
	};
}
