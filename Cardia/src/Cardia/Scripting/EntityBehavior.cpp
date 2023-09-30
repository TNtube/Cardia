#include <Cardia/Application.hpp>
#include <Cardia/ECS/Component/Label.hpp>
#include "cdpch.hpp"
#include "Cardia/Scripting/EntityBehavior.hpp"

namespace Cardia
{
	Component::Transform& Behavior::GetTransform()
	{
		return entity.GetComponent<Component::Transform>();
	}

	Entity Behavior::Spawn(const std::string& path, const Entity* parent) const
	{
		auto& am = Application::Get().GetAssetsManager();
		auto prefabHandle = am.GetHandleFromAsset(path);
		if (!prefabHandle.IsValid())
			return {};

		auto absPath = am.AbsolutePathFromHandle(prefabHandle);
		std::ifstream file(absPath);
		if (!file.is_open())
			return {};

		Json::Value root;
		file >> root;
		file.close();

		auto name = absPath.filename().replace_extension().string();

		auto outEntity = entity.GetScene()->CreateEntity(name, parent ? parent->Handle() : entt::null);

		Entity::DeserializeAndAssignComponents(root, entity.GetScene()->GetRegistry(), outEntity.Handle());

		return outEntity;
	}
}