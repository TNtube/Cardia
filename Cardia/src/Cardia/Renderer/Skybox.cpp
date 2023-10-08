#include "cdpch.hpp"

#include "Cardia/Renderer/Skybox.hpp"
#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{

	Skybox::Skybox(const Renderer& renderer, const AssetHandle& assetHandle)
		: m_Renderer(renderer)
	{
		m_SkyboxTexture = Texture::Builder(m_Renderer.GetDevice())
			.SetAssetHandle(assetHandle)
			.SetTextureMode(TextureMode::CubeMap)
			.Build();

		Shader skyboxShader(m_Renderer.GetDevice());

		skyboxShader.AddShader(ShaderType::Vertex, "resources/shaders/skybox.vert.spv");
		skyboxShader.AddShader(ShaderType::Fragment, "resources/shaders/skybox.frag.spv");

		auto mvpLayout = DescriptorSetLayout::Builder(m_Renderer.GetDescriptorLayoutCache())
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
			.Build();

		auto skyboxDescriptorLayout = DescriptorSetLayout::Builder(m_Renderer.GetDescriptorLayoutCache())
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1) // Skybox
			.Build();

		const std::array<VkDescriptorSetLayout, 2> sbDescLayouts = {
			mvpLayout->GetDescriptorSetLayout(),
			skyboxDescriptorLayout->GetDescriptorSetLayout()
		};

		auto& swapChain = m_Renderer.GetSwapChain();

		PipelineBuilder builder{ m_Renderer.GetDevice() };
		builder.SetShader(&skyboxShader);
		builder.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
		builder.SetViewport(swapChain.GetExtent().width, swapChain.GetExtent().height);
		builder.SetRasterizer(VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT);
		builder.SetMultisampling();
		builder.SetDepthStencil(true, false, VK_COMPARE_OP_LESS_OR_EQUAL);
		builder.SetColorBlend(true, VK_BLEND_OP_ADD, VK_BLEND_OP_ADD, false, VK_LOGIC_OP_COPY);
		builder.SetDescriptorSetLayout(static_cast<uint32_t>(sbDescLayouts.size()), sbDescLayouts.data(), 0, nullptr);

		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vector3f);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(1);
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = 0;

		m_SkyboxPipeline = builder.BuildGraphics(swapChain.GetRenderPass(), bindingDescriptions, attributeDescriptions);
		m_SkyboxTextureDescriptorSet = DescriptorSet::Writer(m_Renderer.GetDescriptorAllocator(), *skyboxDescriptorLayout)
			.WriteImage(0, m_SkyboxTexture->GetImageInfo())
			.Build();

		std::vector<Vector3f> vertices = {
			{-1, -1, -1},
			{1, -1, -1},
			{1, 1, -1},
			{-1, 1, -1},
			{-1, -1, 1},
			{1, -1, 1},
			{1, 1, 1},
			{-1, 1, 1}
		};

		std::vector<uint32_t> indices = {
			0, 1, 3, 3, 1, 2,
			1, 5, 2, 2, 5, 6,
			5, 4, 6, 6, 4, 7,
			4, 0, 7, 7, 0, 3,
			3, 2, 7, 7, 2, 6,
			4, 5, 0, 0, 5, 1
		};

		m_VertexCount = static_cast<uint32_t>(vertices.size());
		m_IndexCount = static_cast<uint32_t>(indices.size());
		m_VertexBuffer = std::make_unique<Buffer>(m_Renderer.GetDevice(),
		                                          sizeof(vertices[0]) * m_VertexCount,
		                                          1,
		                                          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		                                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_IndexBuffer = std::make_unique<Buffer>(m_Renderer.GetDevice(),
		                                         sizeof(indices[0]) * m_IndexCount,
		                                         1,
		                                         VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;
		m_VertexBuffer->UploadData(bufferSize, vertices.data());

		bufferSize = sizeof(indices[0]) * m_IndexCount;

		Buffer stagingBuffer(
			m_Renderer.GetDevice(),
			bufferSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.UploadData(bufferSize, indices.data());

		m_Renderer.GetDevice().CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer->GetBuffer(), bufferSize);
	}

	void Skybox::Draw(VkCommandBuffer commandBuffer) const
	{
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_SkyboxPipeline->GetLayout(),
			1, 1,
			&m_SkyboxTextureDescriptorSet->GetDescriptor(),
			0, nullptr);

		const VkBuffer buffers[] = {m_VertexBuffer->GetBuffer()};
		constexpr VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
	}
}