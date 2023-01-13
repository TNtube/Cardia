#include "Panels/SceneHierarchyPanel.hpp"

#include <imgui.h>
#include "Panels/PanelManager.hpp"
#include "Panels/InspectorPanel.hpp"
#include "CardiaTor.hpp"


namespace Cardia :: Panel
{
	int SceneHierarchyPanel::m_LastWindowId = 0;
	void SceneHierarchyPanel::OnImGuiRender(CardiaTor* appContext)
	{
		char buff[64];
		sprintf(buff, "Current Scene##%i", m_WindowId);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(buff, &m_IsOpen)) {
			m_PanelManager->DeletePanel(this);
			ImGui::End();
			return;
		}

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
					SetSelectedEntityFromItself(Entity(entity, m_CurrentScene), appContext);
				}
				if (ImGui::BeginPopupContextItem())
				{
					SetSelectedEntityFromItself(Entity(entity, m_CurrentScene), appContext);
					if (ImGui::MenuItem("Delete Entity"))
					{
						m_CurrentScene->DestroyEntity(entity);
						SetSelectedEntityFromItself(Entity(), appContext);
					}
					ImGui::EndPopup();
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			SetSelectedEntityFromItself(Entity(), appContext);
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

	// Should be called only from outside
	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void SceneHierarchyPanel::SetSelectedEntityFromItself(Entity entity, CardiaTor* appCtx)
	{
		appCtx->SetSelectedEntity(entity);
		m_PanelManager->GetLastFocused<InspectorPanel>()->SetSelectedEntity(entity);
		m_SelectedEntity = entity;
	}
}
