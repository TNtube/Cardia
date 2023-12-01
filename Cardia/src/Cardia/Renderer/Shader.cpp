#include <utility>
#include <Cardia/Renderer/Renderer.hpp>

#include "cdpch.hpp"
#include "Cardia/Renderer/Shader.hpp"


namespace Cardia
{
	std::vector<char> ReadFile(const std::filesystem::path& filepath)
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

	ShaderModule::ShaderModule(const Device& device, ShaderType type, std::string filePath)
		: m_Device(device.GetDevice()), m_Type(type), m_FilePath(std::move(filePath))
	{
		Init();
	}

	ShaderModule::ShaderModule(ShaderModule&& other) noexcept : m_Device(other.m_Device)
	{
		m_FilePath = other.m_FilePath;
		m_Type = other.m_Type;
		m_ShaderModule = other.m_ShaderModule;
		m_ShaderInfo = other.m_ShaderInfo;

		other.m_ShaderModule = nullptr;
	}

	ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
	{
		m_Device = other.m_Device;
		m_FilePath = other.m_FilePath;
		m_Type = other.m_Type;
		m_ShaderModule = other.m_ShaderModule;
		m_ShaderInfo = other.m_ShaderInfo;

		other.m_ShaderModule = nullptr;

		return *this;
	}

	ShaderModule::~ShaderModule()
	{
		Cleanup();
	}

	void ShaderModule::Reload()
	{
		Cleanup();
		Init();
	}

	void ShaderModule::Init()
	{
		std::vector<char> contents(ReadFile(m_FilePath));

		CreateShaderModule(contents, &m_ShaderModule);


		m_ShaderInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_ShaderInfo.stage = static_cast<VkShaderStageFlagBits>(m_Type);
		m_ShaderInfo.module = m_ShaderModule;
		m_ShaderInfo.pName = "main";
		m_ShaderInfo.flags = 0;
		m_ShaderInfo.pNext = nullptr;
		m_ShaderInfo.pSpecializationInfo = nullptr;
	}

	void ShaderModule::Cleanup() const
	{
		if (m_ShaderModule)
		{
			vkDestroyShaderModule(m_Device, m_ShaderModule, nullptr);
		}
	}

	void ShaderModule::CreateShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule) const
	{
		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module");
		}
	}

	void Shader::AddShader(ShaderType type, const std::string& path)
	{
		m_ShaderModules[type] = ShaderModule(m_Device, type, path);
	}

	VkShaderModule Shader::GetShaderModule(ShaderType type) const
	{
		return m_ShaderModules.at(type).GetShaderModule();
	}

	VkPipelineShaderStageCreateInfo Shader::GetShaderStage(ShaderType type) const
	{
		return m_ShaderModules.at(type).GetShaderInfo();
	}

	std::vector<VkShaderModule> Shader::GetAllShaderModules() const
	{
		std::vector<VkShaderModule> modules;

		auto it = m_ShaderModules.begin();
		while (it != m_ShaderModules.end())
		{
			modules.push_back(it->second.GetShaderModule());
			++it;
		}

		return modules;
	}

	std::vector<VkPipelineShaderStageCreateInfo> Shader::GetAllShaderStages() const
	{
		std::vector<VkPipelineShaderStageCreateInfo> infos;

		auto it = m_ShaderModules.begin();
		while (it != m_ShaderModules.end())
		{
			infos.push_back(it->second.GetShaderInfo());
			++it;
		}

		return infos;
	}
}