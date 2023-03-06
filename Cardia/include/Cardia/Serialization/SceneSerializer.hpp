#pragma once

#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"

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
}