#pragma once

#include <json/value.h>
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"

namespace Cardia::Serialization
{

	class SceneSerializer
	{
	public:
		explicit SceneSerializer(Scene& scene)
			: m_Scene(scene) {}

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

	private:
		Scene& m_Scene;
	};



	struct SceneArchiveOutput {
		explicit SceneArchiveOutput(entt::registry& registry) : m_Registry(registry) {}

		void operator()(entt::entity) {}
		void operator() (std::underlying_type_t<entt::entity>) {}

		template<typename T>
		void operator()(entt::entity entity, const T& component) {
			cdCoreAssert(false, "Component serialization should be implemented");
		}

		void operator()(entt::entity entity, const Component::Transform& component);
		void operator()(entt::entity entity, const Component::Name& component);
		void operator()(entt::entity entity, const Component::MeshRendererC& component);
		void operator()(entt::entity entity, const Component::SpriteRenderer& component);
		void operator()(entt::entity entity, const Component::Camera& component);
		void operator()(entt::entity entity, const Component::Light& component);
		void operator()(entt::entity entity, const Component::Script& component);


		friend std::ostream & operator<<(std::ostream& stream, const SceneArchiveOutput& archiveOutput);

		std::string ToString();

	private:
		friend class SceneArchiveInput;
		Json::Value m_Root;
		entt::registry& m_Registry;
	};

	struct SceneArchiveInput {
		explicit SceneArchiveInput(SceneArchiveOutput& archiveOutput)
			: m_Root(archiveOutput.m_Root) {}

		void operator()(entt::entity&) {}
		void operator() (std::underlying_type_t<entt::entity>&) {}

		template<typename T>
		void operator()(entt::entity& entity, T& component) {
			cdCoreAssert(false, "Component serialization should be implemented");
		}

		void operator()(entt::entity& entity, const Component::Transform& component);
		void operator()(entt::entity& entity, const Component::Name& component);
		void operator()(entt::entity& entity, const Component::MeshRendererC& component);
		void operator()(entt::entity& entity, const Component::SpriteRenderer& component);
		void operator()(entt::entity& entity, const Component::Camera& component);
		void operator()(entt::entity& entity, const Component::Light& component);
		void operator()(entt::entity& entity, const Component::Script& component);


		friend std::ostream & operator>>(std::ostream& stream, const SceneArchiveInput& archiveInput);

		std::unique_ptr<Scene> ToScene();

	private:
		Json::Value m_Root;
		entt::registry m_Registry;
	};
}