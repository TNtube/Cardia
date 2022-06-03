#pragma once

#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/Camera.hpp"

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
		Entity createEntity(const std::string& name = "");

		void destroyEntity(entt::entity entity);

		void onUpdate(DeltaTime deltaTime, const glm::vec2& viewport);
		void onUpdateEditor(DeltaTime deltaTime, Camera& editorCamera);
		inline const char* getName() const { return m_Name.c_str(); }
		inline const entt::registry& getRegistry() const { return m_Registry; }
		void clear();

		std::filesystem::path path;
	private:
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
		friend class Panel::SceneHierarchy;
	};
}
