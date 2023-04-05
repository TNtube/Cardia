#include "cdpch.hpp"
#include "Cardia/Renderer/Pipeline.hpp"

namespace Cardia
{
	Pipeline::Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath,
		const PipelineConfigInfo& configInfo) : m_Device(device)
	{
		CreateGraphicsPipeline(vertPath, fragPath, configInfo);
	}

	PipelineConfigInfo Pipeline::DefaultPipelineConfigInfo(uint32_t width, uint32_t height)
	{
		constexpr PipelineConfigInfo info {};
		return info;
	}

	std::vector<char> Pipeline::ReadFile(const std::filesystem::path& filepath)
	{
		std::ifstream t(filepath);
		return { std::istreambuf_iterator<char>(t), std::istreambuf_iterator<char>() };
	}

	void Pipeline::CreateGraphicsPipeline(const std::string& vertPath, const std::string& fragPath,
	                                      const PipelineConfigInfo& configInfo)
	{
		const auto vertCode = ReadFile(vertPath);
		const auto fragCode = ReadFile(fragPath);

		CreateShaderModule(vertCode, &m_VertShader);
		CreateShaderModule(fragCode, &m_FragShader);
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
