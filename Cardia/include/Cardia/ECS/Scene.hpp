#pragma once

#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Renderer/Texture.hpp"
#include "Cardia/Math/Matrix4.hpp"

#include <entt/entt.hpp>
#include <filesystem>



namespace Cardia
{
	class Entity;
	class Scene
	{
	public:
		Scene(Renderer& renderer, std::string name = "Default Scene");
		Scene(Renderer& renderer, std::filesystem::path path);
		Scene(const Scene& other);
		Scene& operator=(const Scene& other);
		Scene(Scene&& other) noexcept;
		Scene& operator=(Scene&& other) noexcept;
		virtual ~Scene();

		Entity CreateEntity(const std::string& name = "", entt::entity parent = entt::null);
		Entity CreateEntityFromId(UUID uuid, entt::entity parent = entt::null);

		void DestroyEntity(entt::entity entity);

		static std::unique_ptr<Scene> Copy(Scene& src);
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnRuntimeRender(VkCommandBuffer commandBuffer);
		void OnRender(VkCommandBuffer commandBuffer, Camera& camera, const Matrix4f& cameraTransform);
		void OnViewportResize(float width, float height);
		Entity GetEntityByUUID(const UUID& uuid);
		inline const char* GetName() const { return m_Name.c_str(); }
		inline entt::registry& GetRegistry() { return m_Registry; }
		const std::filesystem::path& GetPath() const { return m_Path; }
		std::filesystem::path& GetPath() { return m_Path; }
		void Clear();

		Json::Value Serialize() const;
		static std::optional<Scene> Deserialize(const Json::Value& root);

	private:
		void CopyRegistry(const Scene& other);
		Renderer& m_Renderer;
		std::filesystem::path m_Path;
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
	};
}
