#pragma once
#include "Device.hpp"

namespace Cardia
{
	struct PipelineConfigInfo {};
	
	class Pipeline
	{
	public:
		Pipeline(Device& device, const std::string& vertPath, const std::string& fragPath, const PipelineConfigInfo& configInfo);
		virtual ~Pipeline() {}
		Pipeline(const Pipeline&) = delete;
		void operator=(const Pipeline&) = delete;

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
