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
		Entity createEntity(const std::string& name = "");
		Entity createEntityFromId(UUID uuid);

		void destroyEntity(entt::entity entity);

		void onUpdate(DeltaTime deltaTime, const glm::vec2& viewport);
		void onUpdateEditor(DeltaTime deltaTime, Camera& editorCamera);
		inline const char* getName() const { return m_Name.c_str(); }
		inline entt::registry& getRegistry() { return m_Registry; }
		void clear();

		std::filesystem::path path;
	private:
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
		friend class Panel::SceneHierarchy;
	};
}
