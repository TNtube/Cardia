#include "cdpch.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/ECS/Component/ModelRenderer.hpp"

namespace Cardia::Component
{
	Json::Value ModelRenderer::Serialize() const
	{
		Json::Value root;

		auto& mesh = root["MeshRenderer"];
		mesh["Model"] = Renderer->GetHandle().ID.ToString();

		return root;
	}

	std::optional<ModelRenderer> ModelRenderer::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("MeshRenderer"))
			return std::nullopt;

		const auto& mesh = root["MeshRenderer"];

		if (!mesh.isMember("Model"))
			return std::nullopt;

		ModelRenderer temp;

		AssetHandle handle(UUID::FromString(mesh["Model"].asString()));
		temp.Renderer = Application::Get().GetAssetsManager().Load<MeshRenderer>(handle);

		return temp;
	}
}
