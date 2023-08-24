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
			ImGui::End();
			return;
		}
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Create Entity"))
				m_CurrentScene->CreateEntity();

			ImGui::EndPopup();
		}

		if (ImGui::IsWindowFocused()) {
			m_PanelManager->SetFocused<SceneHierarchyPanel>(this);
		}

		if (!m_CurrentScene) {
			ImGui::End();
			return;
		}

		if (!ImGui::TreeNodeEx(m_CurrentScene, ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_DefaultOpen, "%s", m_CurrentScene->GetName())) {
			ImGui::TreePop();
			ImGui::End();
			return;
		}

		const auto view = m_CurrentScene->GetRegistry().view<Component::Relationship>();

		for (const auto enttEntity : view)
		{
			auto entity = Entity(enttEntity, m_CurrentScene);
			if (!entity.GetParent().IsValid()) {
				DrawEntityNode(entity, appContext);
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			SetSelectedEntityFromItself(Entity(), appContext);
		}
		ImGui::TreePop();
		ImGui::End();
	}

	// Should be called only from outside
	void SceneHierarchyPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void SceneHierarchyPanel::OnSceneLoad(Scene *scene)
	{
		IPanel::OnSceneLoad(scene);
		m_SelectedEntity = Entity();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity, CardiaTor* appCtx)
	{
		auto node_flags = m_SelectedEntity == entity.Handle() ? ImGuiTreeNodeFlags_Selected : 0;
		node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow;

		if (!entity.GetChildren().begin()->IsValid()) {
			node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_DefaultOpen;
		}

		const auto& label = entity.GetComponent<Component::Label>();
		const auto& uuid = entity.GetComponent<Component::ID>();

		// Push label color
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{label.Color.x, label.Color.y, label.Color.z, label.Color.w});


		// If node opened
		if (ImGui::TreeNodeEx(uuid.Uuid.ToString().c_str(), node_flags, "%s", label.Name.c_str())) {
			ImGui::PopStyleColor();
			if (ImGui::BeginPopupContextItem())
			{
				if (ImGui::MenuItem("Add Child"))
				{
					const auto child = m_CurrentScene->CreateEntity("Entity", entity.Handle());
					SetSelectedEntityFromItself(child, appCtx);
				}
				if (ImGui::MenuItem("Delete Entity"))
				{
					m_CurrentScene->DestroyEntity(entity);
					SetSelectedEntityFromItself(Entity(), appCtx);
				}
				ImGui::EndPopup();
			}

			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("ENTITY", &entity, sizeof(Entity));
				ImGui::EndDragDropSource();
			}
			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				SetSelectedEntityFromItself(entity, appCtx);
			}


			for (auto child : entity.GetChildren()) {
				DrawEntityNode(child, appCtx);
			}

			ImGui::TreePop();
		} else {
			ImGui::PopStyleColor();
		}
	}

	void SceneHierarchyPanel::SetSelectedEntityFromItself(Entity entity, CardiaTor* appCtx)
	{
		appCtx->SetSelectedEntity(entity);
		m_PanelManager->GetLastFocused<InspectorPanel>()->SetSelectedEntity(entity);
		m_SelectedEntity = entity;
	}
}
