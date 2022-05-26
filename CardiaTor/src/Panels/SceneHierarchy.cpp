#include "Panels/SceneHierarchy.hpp"

#include <entt/entt.hpp>
#include <imgui.h>


namespace Cardia :: Panel
{

	SceneHierarchy::SceneHierarchy(Scene* scene)
		: m_Scene(scene)
	{

	}

	void SceneHierarchy::onImGuiRender(DeltaTime deltaTime)
	{
		drawHierarchy();
	}

	void SceneHierarchy::drawHierarchy()
	{
		ImGui::Begin("Current Scene");
		const auto view = m_Scene->m_Registry.view<Component::Name>();

		for (auto entity : view)
		{
			auto name = view.get<Component::Name>(entity);
			auto node_flags = ((m_EntityClicked == entity) ? ImGuiTreeNodeFlags_Selected : 0);
			node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_Leaf;
			//node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
			if (ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), node_flags, "%s", name.name.c_str())) {
				if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
					m_EntityClicked = Entity(entity, m_Scene);
				}
				if (ImGui::BeginPopupContextItem())
				{
					m_EntityClicked = Entity(entity, m_Scene);
					if (ImGui::MenuItem("Delete Entity"))
					{
						m_Scene->destroyEntity(entity);
						resetEntityClicked();
					}
					ImGui::EndPopup();
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
			resetEntityClicked();
		}
		if (ImGui::BeginPopupContextWindow(nullptr, 1, false))
		{
			if (ImGui::MenuItem("Create Entity"))
				m_Scene->createEntity();

			ImGui::EndPopup();
		}
		ImGui::End();
	}
}
