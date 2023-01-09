#include "Panels/SceneHierarchyPanel.hpp"

#include <imgui.h>
#include "Panels/PanelManager.hpp"
#include "Panels/InspectorPanel.hpp"


namespace Cardia :: Panel
{
	void SceneHierarchyPanel::OnImGuiRender()
	{
		drawHierarchy();
	}

	void SceneHierarchyPanel::drawHierarchy()
	{
		ImGui::Begin("Current Scene");

		if (ImGui::IsWindowFocused()) {
			m_PanelManager->SetFocused<SceneHierarchyPanel>(this);
		}

		if (!m_CurrentScene) {
			ImGui::End();
			return;
		}
		const auto view = m_CurrentScene->GetRegistry().view<Component::Name, Component::ID>();

		for (auto entity : view)
		{
			auto name = view.get<Component::Name>(entity);
			auto uuid = view.get<Component::ID>(entity);
			auto node_flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0);
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
					SetSelectedEntity(Entity(entity, m_CurrentScene));
				}
				if (ImGui::BeginPopupContextItem())
				{
					SetSelectedEntity(Entity(entity, m_CurrentScene));
					if (ImGui::MenuItem("Delete Entity"))
					{
						m_CurrentScene->DestroyEntity(entity);
						SetSelectedEntity(Entity());
					}
					ImGui::EndPopup();
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			SetSelectedEntity(Entity());
		}
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::MenuItem("Create Entity"))
				m_CurrentScene->CreateEntity();

			ImGui::EndPopup();
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::OnSceneLoad(Scene *scene)
	{
		IPanel::OnSceneLoad(scene);
		m_SelectedEntity = Entity();
	}

	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_PanelManager->GetLastFocused<InspectorPanel>()->SetSelectedEntity(entity);
		m_SelectedEntity = entity;
	}
}
