#pragma once
#include "Descriptors.hpp"
#include "Device.hpp"
#include "RenderPass.hpp"
#include "Shader.hpp"

namespace Cardia
{

	class Pipeline
	{
	public:
		explicit Pipeline(const Device &device, const VkPipeline& pipeline, const VkPipelineLayout& layout);
		virtual ~Pipeline();

		void Bind(VkCommandBuffer commandBuffer) const;

		const VkPipeline& GetPipeline() const { return m_Pipeline; }
		const VkPipelineLayout& GetLayout() const { return m_Layout; }

	private:
		const Device& m_Device;
		VkPipeline m_Pipeline {};
		VkPipelineLayout m_Layout {};
	};

	class PipelineBuilder
	{
	public:
		explicit PipelineBuilder(const Device& device) : m_Device(device) {}

		void SetShader(Shader* pShader);
		void SetInputAssembly(VkPrimitiveTopology topology, bool primitiveRestartEnable);
		void SetViewport(uint32_t width, uint32_t height);
		void SetRasterizer(VkPolygonMode polygonMode, VkCullModeFlagBits cullMode);
		void SetMultisampling();
		void SetDepthStencil(bool depthTest, bool depthWrite, VkCompareOp compareOp);
		void SetColorBlend(bool blendEnable, VkBlendOp colorBlendOp, VkBlendOp alphaBlendOp, bool logicOpEnable, VkLogicOp logicOp);
		void SetDescriptorSetLayout(uint32_t layoutsCount, const VkDescriptorSetLayout* pLayouts, uint32_t pushConstCount, const VkPushConstantRange* pPushConstants);

		std::unique_ptr<Pipeline> BuildGraphics(const RenderPass& renderPass);

	private:
		const Device& m_Device;

		Shader* m_pShader{};
		VkPipelineInputAssemblyStateCreateInfo m_InputAssembly{};
		VkViewport m_Viewport{};
		VkRect2D m_Scissor{};
		VkPipelineRasterizationStateCreateInfo m_Rasterizer{};
		VkPipelineMultisampleStateCreateInfo m_Multisampling{};
		VkPipelineDepthStencilStateCreateInfo m_DepthStencil{};
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachment{};
		VkPipelineColorBlendStateCreateInfo m_ColorBlending{};
		VkPipelineLayoutCreateInfo m_PipelineLayoutInfo{};
	};
}
