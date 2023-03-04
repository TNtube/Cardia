#pragma once

#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/UUID.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Renderer/Texture.hpp"

#include <entt/entt.hpp>
#include <filesystem>


namespace Cardia
{
	class Entity;
	class Scene
	{
	public:
		Scene(std::string name = "Default Scene");
		virtual ~Scene() = default;
		Entity CreateEntity(const std::string& name = "");
		Entity CreateEntityFromId(UUID uuid);

		void DestroyEntity(entt::entity entity);

		void OnRuntimeUpdate();
		void OnRuntimeStart();
		void OnRuntimeStop();
		void OnUpdateEditor(Camera& editorCamera, const glm::mat4& editorCameraTransform);
		void OnViewportResize(float width, float height);
		Entity GetEntityByUUID(const UUID& uuid);
		inline const char* GetName() const { return m_Name.c_str(); }
		inline entt::registry& GetRegistry() { return m_Registry; }
		void clear();

		std::filesystem::path path;
	private:
		Shader* m_BasicShader {};
		std::string m_Name;
		entt::registry m_Registry;
		std::unique_ptr<Texture2D> m_WhiteTexture;
		friend class Entity;
	};
}
