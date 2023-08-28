#include <Cardia/DataStructure/Vertex.hpp>
#include "cdpch.hpp"
#include "Cardia/Renderer/Pipeline.hpp"

#include "Cardia/DataStructure/SubMesh.hpp"

namespace Cardia
{
	Pipeline::Pipeline(const Device &device, const VkPipeline& pipeline, const VkPipelineLayout& layout)
		: m_Device(device)
	{
		m_Pipeline = pipeline;
		m_Layout = layout;
	}

	Pipeline::~Pipeline()
	{
		vkDestroyPipeline(m_Device.GetDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_Layout, nullptr);
	}

	void Pipeline::Bind(VkCommandBuffer commandBuffer) const
	{
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
	}

	void PipelineBuilder::SetShader(Shader* pShader)
	{
		m_pShader = pShader;
	}

	void PipelineBuilder::SetInputAssembly(VkPrimitiveTopology topology, bool primitiveRestartEnable)
	{
		m_InputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		m_InputAssembly.topology = topology;
		m_InputAssembly.primitiveRestartEnable = primitiveRestartEnable;
	}

	void PipelineBuilder::SetViewport(uint32_t width, uint32_t height)
	{
		m_Viewport.x = 0.0f;
		m_Viewport.y = 0.0f;
		m_Viewport.width = static_cast<float>(width);
		m_Viewport.height = static_cast<float>(height);
		m_Viewport.minDepth = 0;
		m_Viewport.maxDepth = 0;

		m_Scissor.offset = {0, 0};
		m_Scissor.extent = {width, height};
	}

	void PipelineBuilder::SetRasterizer(VkPolygonMode polygonMode, VkCullModeFlagBits cullMode)
	{
		m_Rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_Rasterizer.depthClampEnable = VK_FALSE;
		m_Rasterizer.rasterizerDiscardEnable = VK_FALSE;
		m_Rasterizer.polygonMode = polygonMode;
		m_Rasterizer.lineWidth = 1.0f;
		m_Rasterizer.cullMode = cullMode;
		m_Rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		m_Rasterizer.depthBiasEnable = VK_FALSE;
		m_Rasterizer.depthBiasConstantFactor = 0.0f;
		m_Rasterizer.depthBiasClamp = 0.0f;
		m_Rasterizer.depthBiasSlopeFactor = 0.0f;
	}

	void PipelineBuilder::SetMultisampling()
	{
		m_Multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		m_Multisampling.sampleShadingEnable = VK_FALSE;
		m_Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	}

	void PipelineBuilder::SetDepthStencil(bool depthTest, bool depthWrite, VkCompareOp compareOp)
	{
		m_DepthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		m_DepthStencil.depthTestEnable = depthTest;
		m_DepthStencil.depthWriteEnable = depthWrite;
		m_DepthStencil.depthCompareOp = compareOp;
		m_DepthStencil.depthBoundsTestEnable = VK_FALSE;
		m_DepthStencil.stencilTestEnable = VK_FALSE;
	}

	void PipelineBuilder::SetColorBlend(bool blendEnable, VkBlendOp colorBlendOp, VkBlendOp alphaBlendOp,
	                                    bool logicOpEnable, VkLogicOp logicOp)
	{
		m_ColorBlendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
				VK_COLOR_COMPONENT_A_BIT;
		m_ColorBlendAttachment.blendEnable = blendEnable;
		m_ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		m_ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_ColorBlendAttachment.colorBlendOp = colorBlendOp;
		m_ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_ColorBlendAttachment.alphaBlendOp = alphaBlendOp;

		m_ColorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		m_ColorBlending.logicOpEnable = logicOpEnable;
		m_ColorBlending.logicOp = logicOp;
		m_ColorBlending.attachmentCount = 1;
		m_ColorBlending.pAttachments = &m_ColorBlendAttachment;
	}

	void PipelineBuilder::SetDescriptorSetLayout(uint32_t layoutsCount, const VkDescriptorSetLayout* pLayouts,
	                                             uint32_t pushConstCount, const VkPushConstantRange* pPushConstants)
	{
		m_PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		m_PipelineLayoutInfo.setLayoutCount = layoutsCount;
		m_PipelineLayoutInfo.pSetLayouts = pLayouts;
		m_PipelineLayoutInfo.pushConstantRangeCount = pushConstCount;
		m_PipelineLayoutInfo.pPushConstantRanges = pPushConstants;
	}

	std::unique_ptr<Pipeline> PipelineBuilder::BuildGraphics(const RenderPass& renderPass)
	{
		auto bindingDescriptions = Vertex::GetBindingDescriptions();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();


		VkPipelineViewportStateCreateInfo viewportInfo {};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &m_Viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &m_Scissor;

		// First we need to create the pipeline layout.
		VkPipelineLayout pipelineLayout;
		if (vkCreatePipelineLayout(m_Device.GetDevice(), &m_PipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan : Failed to create pipeline layout");
		}

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		auto shaderStages = m_pShader->GetAllShaderStages();

		VkGraphicsPipelineCreateInfo pipelineInfo {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &m_InputAssembly;
		pipelineInfo.pViewportState = &viewportInfo;
		pipelineInfo.pRasterizationState = &m_Rasterizer;
		pipelineInfo.pMultisampleState = &m_Multisampling;
		pipelineInfo.pColorBlendState = &m_ColorBlending;
		pipelineInfo.pDepthStencilState = &m_DepthStencil;
		pipelineInfo.pDynamicState = &dynamicState;

		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass.GetRenderPass();
		pipelineInfo.subpass = 0;

		pipelineInfo.basePipelineIndex = -1;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		VkPipeline vkPipeline;
		if (vkCreateGraphicsPipelines(m_Device.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &vkPipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to crate graphics pipeline !");
		}

		return std::make_unique<Pipeline>(m_Device, vkPipeline, pipelineLayout);
	}
}
