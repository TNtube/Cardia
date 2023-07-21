#pragma once

#include <json/value.h>
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia::Serialization
{

	class SceneSerializer
	{
	public:
		explicit SceneSerializer(Scene& scene)
			: m_Scene(scene) {}

		bool Serialize(const std::filesystem::path& path);
		bool Serialize(Json::Value& root);
		bool Deserialize(Renderer& renderer, const std::filesystem::path& path);
		bool Deserialize(Renderer& renderer, const Json::Value& root);

	private:
		Scene& m_Scene;
	};



	struct SceneArchiveOutput {
		SceneArchiveOutput() = default;
		void operator() (std::underlying_type_t<entt::entity>) {}
		template<typename T>
		void operator()(entt::entity entity, const T& component) {
			cdCoreAssert(false, "Component serialization should be implemented");
		}

		void operator()(entt::entity entity, const Component::ID& component);
		void operator()(entt::entity entity, const Component::Transform& component);
		void operator()(entt::entity entity, const Component::Name& component);
		void operator()(entt::entity entity, const Component::MeshRendererC& component);
		void operator()(entt::entity entity, const Component::SpriteRenderer& component);
		void operator()(entt::entity entity, const Component::Camera& component);
		void operator()(entt::entity entity, const Component::Light& component);
		void operator()(entt::entity entity, const Component::Script& component);

		void Finalize();

		std::string ToString();
		friend std::ostream & operator<<(std::ostream& stream, const SceneArchiveOutput& archiveOutput);

	private:
		friend SceneSerializer;
		Json::Value m_Root;
	};
}