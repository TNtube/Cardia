#pragma once

#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Renderer/Texture.hpp"
#include "Cardia/Math/Matrix4.hpp"

#include <entt/entt.hpp>
#include <filesystem>
#include <Cardia/ECS/Component/Transform.hpp>


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

		Entity CreateEntity(const std::string& name = "Entity", entt::entity parent = entt::null);
		Entity CreateEntityFromId(UUID uuid, entt::entity parent = entt::null);

		void DestroyEntity(Entity entity);

		static std::unique_ptr<Scene> Copy(Scene& src);
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnRuntimeRender(VkCommandBuffer commandBuffer);
		void OnRender(VkCommandBuffer commandBuffer, Camera& camera, const Component::Transform& cameraTransform);
		void OnViewportResize(float width, float height);
		Entity GetEntityByUUID(const UUID& uuid);
		bool IsEntityValid(entt::entity entity) const;
		inline const char* GetName() const { return m_Name.c_str(); }
		inline entt::registry& GetRegistry() { return m_Registry; }
		const std::filesystem::path& GetPath() const { return m_Path; }
		std::filesystem::path& GetPath() { return m_Path; }
		void Clear();


	private:
		void PopulateDefaultEntity(Entity& entity, std::string name = "Entity", UUID uuid = UUID(), entt::entity parent = entt::null);
		void CopyRegistry(const Scene& other);
		Renderer& m_Renderer;
		std::filesystem::path m_Path;
		std::string m_Name;
		entt::registry m_Registry;

		Json::Value Serialize() const;
		static Scene Deserialize(const Json::Value& root);

		friend class Entity;
		friend Scene Serialization::FromJson<Scene>(const Json::Value& data);
		friend Json::Value Serialization::ToJson<Scene>(const Scene& object);
	};


	template<>
	inline Json::Value Serialization::ToJson(const Scene& object)
	{
		return object.Serialize();
	}


	template<>
	inline Scene Serialization::FromJson(const Json::Value& data)
	{
		return Scene::Deserialize(data);
	}
}
