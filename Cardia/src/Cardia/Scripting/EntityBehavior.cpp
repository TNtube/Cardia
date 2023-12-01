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
		const auto prefabUuid = am.GetUUIDFromAsset(path);
		if (!prefabUuid.IsValid())
			return {};

		const auto absPath = am.AbsolutePathFromUUID(prefabUuid);
		std::ifstream file(absPath);
		if (!file.is_open())
			return {};

		Json::Value root;
		file >> root;
		file.close();

		const auto name = absPath.filename().replace_extension().string();

		const auto outEntity = entity.GetScene()->CreateEntity(name, parent ? parent->Handle() : entt::null);

		Entity::DeserializeAndAssignComponents(root, entity.GetScene()->GetRegistry(), outEntity.Handle());

		return outEntity;
	}

	py::object Behavior::RunCoroutine(const py::object &coroutine)
	{
		try {
			const auto loop = py::module::import("asyncio").attr("get_event_loop")();
			auto task = loop.attr("create_task")(coroutine);
			m_Tasks.append(task);
			return task;
		} catch (py::error_already_set& e) {
			Log::CoreError("Failed to run coroutine : {0}", e.what());
		}
		return py::none();
	}
}