#include "Panels/SceneHierarchy.hpp"

#include <imgui.h>


namespace Cardia :: Panel
{

	SceneHierarchy::SceneHierarchy(Scene* scene)
		: m_Scene(scene)
	{

	}

	void SceneHierarchy::OnImGuiRender()
	{
		drawHierarchy();
	}

	void SceneHierarchy::drawHierarchy()
	{
		ImGui::Begin("Current Scene");
		const auto view = m_Scene->GetRegistry().view<Component::Name, Component::ID>();

		for (auto entity : view)
		{
			auto selectedEntity = m_Scene->GetCurrentEntity();
			auto name = view.get<Component::Name>(entity);
			auto uuid = view.get<Component::ID>(entity);
			auto node_flags = ((selectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
			node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			//node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), node_flags, "%s", name.name.c_str())) {
				if (ImGui::BeginDragDropSource())
				{
					std::string itemUuid = uuid.uuid;
					ImGui::SetDragDropPayload("ENTITY_UUID", itemUuid.c_str(), (strlen(itemUuid.c_str()) + 1) * sizeof(char));
					ImGui::EndDragDropSource();
				}
				if (ImGui::IsItemClicked(ImGuiMouseButton_Middle)) {
					m_Scene->SetCurrentEntity(view.get<Component::ID>(entity).uuid);
				}
				if (ImGui::BeginPopupContextItem())
				{
					m_Scene->SetCurrentEntity(view.get<Component::ID>(entity).uuid);
					if (ImGui::MenuItem("Delete Entity"))
					{
						m_Scene->DestroyEntity(entity);
						m_Scene->SetCurrentEntity(UUID());
					}
					ImGui::EndPopup();
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			m_Scene->SetCurrentEntity(UUID());
		}
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::MenuItem("Create Entity"))
				m_Scene->CreateEntity();

			ImGui::EndPopup();
		}
		ImGui::End();
	}

	void SceneHierarchy::OnSceneLoad(Cardia::Scene *scene)
	{
		m_Scene = scene;
	}
}
