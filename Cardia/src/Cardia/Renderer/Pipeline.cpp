#include "cdpch.hpp"
#include "Cardia/Renderer/Pipeline.hpp"

#include "Cardia/DataStructure/SubMesh.hpp"

namespace Cardia
{
	PipelineLayout::PipelineLayout(Device& device, const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges)
		: m_Device(device)
	{
		Init(descriptorSetLayouts, pushConstantRanges);
	}

	PipelineLayout::PipelineLayout(Device& device, const std::vector<DescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges)
		: m_Device(device)
	{
		std::vector<VkDescriptorSetLayout> vkDescriptorSetLayouts;
		std::ranges::transform(
			descriptorSetLayouts,
			std::back_inserter(vkDescriptorSetLayouts),
			[](const DescriptorSetLayout& descriptor)
			{
				return descriptor.GetDescriptorSetLayout();
			});
		Init(vkDescriptorSetLayouts, pushConstantRanges);
	}

	PipelineLayout::~PipelineLayout()
	{
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void PipelineLayout::Init(const std::vector<VkDescriptorSetLayout>& descriptorSetLayouts, const std::vector<VkPushConstantRange>& pushConstantRanges)
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan : Failed to create pipeline layout");
		}
	}

	Pipeline::Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath,
			   const PipelineConfigInfo& info) : m_Device(device)
	{
		CreateGraphicsPipeline(vertPath, fragPath, info);
	}

	Pipeline::~Pipeline()
	{
		vkDeviceWaitIdle(m_Device.GetDevice());
		vkDestroyShaderModule(m_Device.GetDevice(), m_VertShader, nullptr);
		vkDestroyShaderModule(m_Device.GetDevice(), m_FragShader, nullptr);
		vkDestroyPipeline(m_Device.GetDevice(), m_GraphicsPipeline, nullptr);
	}

	void Pipeline::Bind(VkCommandBuffer commandBuffer) const
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);
	}

	PipelineConfigInfo Pipeline::DefaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		PipelineConfigInfo info {};

		info.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		info.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

		info.viewport.x = 0.0f;
		info.viewport.y = 0.0f;
		info.viewport.width = static_cast<float>(width);
		info.viewport.height = static_cast<float>(height);
		info.viewport.minDepth = 0;
		info.viewport.maxDepth = 0;

		info.scissor.offset = {0, 0};
		info.scissor.extent = {width, height};

		info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		info.rasterizationInfo.depthClampEnable = VK_FALSE;
		info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		info.rasterizationInfo.lineWidth = 1.0f;
		info.rasterizationInfo.cullMode = VK_CULL_MODE_FRONT_BIT;
		info.rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		info.rasterizationInfo.depthBiasEnable = VK_FALSE;
		info.rasterizationInfo.depthBiasConstantFactor = 0.0f;
		info.rasterizationInfo.depthBiasClamp = 0.0f;
		info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;
 
		info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.depthStencilInfo.depthTestEnable = VK_TRUE;
		info.depthStencilInfo.depthWriteEnable = VK_TRUE;
		info.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		info.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
		info.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
		info.depthStencilInfo.stencilTestEnable = VK_FALSE;
		info.depthStencilInfo.front = {};  // Optional
		info.depthStencilInfo.back = {};   // Optional

		info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		info.multisampleInfo.sampleShadingEnable = VK_FALSE;
		info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		info.multisampleInfo.minSampleShading = 1.0f;           // Optional
		info.multisampleInfo.pSampleMask = nullptr;             // Optional
		info.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
		info.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional
 
		info.colorBlendAttachment.colorWriteMask =
		    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		    VK_COLOR_COMPONENT_A_BIT;
		info.colorBlendAttachment.blendEnable = VK_FALSE;
		info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
		info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
		info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
		info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional
 
		
		return info;
	}

	std::vector<char> Pipeline::ReadFile(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}
		const std::streamsize fileSize = file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	void Pipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath,
					      const PipelineConfigInfo& info)
	{
		const auto vertCode = ReadFile(vertPath);
		const auto fragCode = ReadFile(fragPath);

		CreateShaderModule(vertCode, &m_VertShader);
		CreateShaderModule(fragCode, &m_FragShader);

		VkPipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStages[0].module = m_VertShader;
		shaderStages[0].pName = "main";
		shaderStages[0].flags = 0;
		shaderStages[0].pNext = nullptr;
		shaderStages[0].pSpecializationInfo = nullptr;
		
		shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStages[1].module = m_FragShader;
		shaderStages[1].pName = "main";
		shaderStages[1].flags = 0;
		shaderStages[1].pNext = nullptr;
		shaderStages[1].pSpecializationInfo = nullptr;

		const auto bindingDescriptions = SubMesh::Vertex::GetBindingDescriptions();
		const auto attributeDescriptions = SubMesh::Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();


		VkPipelineViewportStateCreateInfo viewportInfo {};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &info.viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &info.scissor;

		VkPipelineColorBlendStateCreateInfo colorBlendInfo {};
		colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendInfo.logicOpEnable = VK_FALSE;
		colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
		colorBlendInfo.attachmentCount = 1;
		colorBlendInfo.pAttachments = &info.colorBlendAttachment;
		colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
		colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
		colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
		colorBlendInfo.blendConstants[3] = 0.0f;  // Optional
		
		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkGraphicsPipelineCreateInfo pipelineInfo {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &info.inputAssemblyInfo;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &info.rasterizationInfo;
		pipelineInfo.pMultisampleState = &info.multisampleInfo;
		pipelineInfo.pColorBlendState = &colorBlendInfo;
		pipelineInfo.pDepthStencilState = &info.depthStencilInfo;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = info.pipelineLayout;
		pipelineInfo.renderPass = info.renderPass;
		pipelineInfo.subpass = info.subpass;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_GraphicsPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to crate graphics pipeline !");
		}
	}

	void Pipeline::CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const
	{
		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device.GetDevice(), &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}
}
