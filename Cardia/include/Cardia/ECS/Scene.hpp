#pragma once

#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Renderer/Texture.hpp"

#include <entt/entt.hpp>
#include <filesystem>

#include "Cardia/Renderer/Buffer.hpp"


namespace Cardia
{
	class Entity;
	class Scene
	{
	public:
		explicit Scene(std::string name = "Default Scene");
		explicit Scene(std::filesystem::path path);
		virtual ~Scene() = default;
		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityFromId(UUID uuid);

		void DestroyEntity(entt::entity entity);

		static std::unique_ptr<Scene> Copy(Scene& src);
		void OnRuntimeUpdate();
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnUpdateEditor(Camera& editorCamera, const glm::mat4& editorCameraTransform);
		void OnViewportResize(float width, float height);
		Entity GetEntityByUUID(const UUID& uuid);
		inline const char* GetName() const { return m_Name.c_str(); }
		inline entt::registry& GetRegistry() { return m_Registry; }
		const std::filesystem::path& GetPath() const { return m_Path; }
		std::filesystem::path& GetPath() { return m_Path; }
		void clear();

	private:
		std::filesystem::path m_Path;
		std::shared_ptr<Shader> m_BasicShader {};
		std::unique_ptr<UniformBuffer> m_UBO {};
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
	};
}
