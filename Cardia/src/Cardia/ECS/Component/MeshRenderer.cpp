#include "cdpch.hpp"
#include "Cardia/Asset/AssetsManager.hpp"
#include "Cardia/ECS/Component/MeshRenderer.hpp"

namespace Cardia::Component
{
	Json::Value MeshRendererC::Serialize() const
	{
		Json::Value root;

		auto& mesh = root["MeshRenderer"];
		mesh["Mesh"] = AssetsManager::GetPathFromAsset(Renderer->GetMesh()).string();

		return root;
	}

	std::optional<MeshRendererC> MeshRendererC::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("MeshRenderer"))
			return std::nullopt;

		const auto& mesh = root["MeshRenderer"];

		if (!mesh.isMember("Mesh"))
			return std::nullopt;

		MeshRendererC temp;

		if (const auto meshAssets = AssetsManager::Load<Mesh>(mesh["Mesh"].asString()))
			temp.Renderer->SubmitMesh(AssetsManager::Instance().GetRenderer().GetDevice(), meshAssets);

		return temp;
	}
}
