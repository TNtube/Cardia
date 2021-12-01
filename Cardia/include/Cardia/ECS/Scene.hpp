#pragma once

#include "Cardia/Core/Time.hpp"
#include "Cardia/Renderer/EditorCamera.hpp"

#include <entt/entt.hpp>


namespace Cardia
{
	class Entity;
	namespace Panel { class SceneHierarchy; }
	class Scene
	{
	public:
		explicit Scene(std::string name);
		Entity createEntity(const std::string& name = "");

		void destroyEntity(entt::entity entity);

		void onUpdate(DeltaTime deltaTime);
		void onUpdateEditor(DeltaTime deltaTime, EditorCamera& editorCamera);
		inline const char* getName() const { return m_Name.c_str(); }

	private:
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
		friend class Panel::SceneHierarchy;
	};
}