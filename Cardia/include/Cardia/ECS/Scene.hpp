#pragma once

#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/UUID.hpp"

#include <entt/entt.hpp>
#include <filesystem>


namespace Cardia
{
	class Entity;
	namespace Panel { class SceneHierarchy; }
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
		void OnUpdateEditor(Camera& editorCamera);
		void OnViewportResize(float width, float height);
		Entity GetEntityByUUID(const UUID& uuid);
		inline const char* GetName() const { return m_Name.c_str(); }
		inline entt::registry& GetRegistry() { return m_Registry; }
		void clear();
		Entity GetCurrentEntity();
		inline void SetCurrentEntity(UUID entityUUID) { m_CurrentEntity = entityUUID; }

		std::filesystem::path path;
	private:
		UUID m_CurrentEntity;
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
	};
}
