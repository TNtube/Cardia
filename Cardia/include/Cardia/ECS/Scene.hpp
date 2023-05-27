#pragma once

#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Renderer/Texture.hpp"

#include <entt/entt.hpp>
#include <filesystem>

#include "Cardia/Renderer/Buffer.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	class Entity;
	class Scene
	{
	public:
		Scene(Renderer& renderer, std::string name = "Default Scene");
		Scene(Renderer& renderer, std::filesystem::path path);
		virtual ~Scene();
		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityFromId(UUID uuid);

		void DestroyEntity(entt::entity entity);

		static std::unique_ptr<Scene> Copy(Scene& src);
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnRuntimeRender(VkCommandBuffer commandBuffer);
		void OnRender(VkCommandBuffer commandBuffer, Camera& camera, const glm::mat4& cameraTransform);
		void OnViewportResize(float width, float height);
		Entity GetEntityByUUID(const UUID& uuid);
		inline const char* GetName() const { return m_Name.c_str(); }
		inline entt::registry& GetRegistry() { return m_Registry; }
		const std::filesystem::path& GetPath() const { return m_Path; }
		std::filesystem::path& GetPath() { return m_Path; }
		void clear();

	private:
		Renderer& m_Renderer;
		std::filesystem::path m_Path;
		std::shared_ptr<Pipeline> m_Pipeline {};
		std::unique_ptr<Buffer> m_UBO {};
		VkPipelineLayout m_PipelineLayout; // TODO: Remove ?
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
	};
}
