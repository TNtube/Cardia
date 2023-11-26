#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "Cardia/Assets/Asset.hpp"

namespace Cardia
{
	enum class ShaderType : VkShaderStageFlags
	{
		Vertex = VK_SHADER_STAGE_VERTEX_BIT,
		Geometry = VK_SHADER_STAGE_GEOMETRY_BIT,
		Fragment = VK_SHADER_STAGE_FRAGMENT_BIT,
		Compute = VK_SHADER_STAGE_COMPUTE_BIT,
	};

	class ShaderModule
	{
	public:
		ShaderModule() = default;
		ShaderModule(const Device& device, ShaderType type, std::string  filePath);
		ShaderModule(const ShaderModule& other) = delete;
		ShaderModule& operator=(const ShaderModule& other) = delete;
		ShaderModule(ShaderModule&& other) noexcept;
		ShaderModule& operator=(ShaderModule&& other) noexcept;
		~ShaderModule();

		void Reload();

		VkShaderModule GetShaderModule() const { return m_ShaderModule; }

		VkPipelineShaderStageCreateInfo GetShaderInfo() const { return m_ShaderInfo; }

	private:
		void CreateShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) const;
		void Init();
		void Cleanup() const;

	private:
		VkDevice m_Device {};
		ShaderType m_Type {};
		std::string m_FilePath;

		VkShaderModule m_ShaderModule {};
		VkPipelineShaderStageCreateInfo m_ShaderInfo {};
	};

	class Shader final : public Asset
	{
	public:
		explicit Shader(const Device& device) : m_Device(device) {}
		void AddShader(ShaderType type, const std::string& path);

		void Reload() override;
		bool CheckForDirtyInDependencies() override { return false; }

		VkShaderModule GetShaderModule(ShaderType type) const;
		VkPipelineShaderStageCreateInfo GetShaderStage(ShaderType type) const;
		std::vector<VkShaderModule> GetAllShaderModules() const;
		std::vector<VkPipelineShaderStageCreateInfo> GetAllShaderStages() const;

	private:
		const Device& m_Device;
		std::unordered_map<ShaderType, ShaderModule> m_ShaderModules;
	};
}