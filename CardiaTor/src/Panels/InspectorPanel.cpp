#include "Panels/InspectorPanel.hpp"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Cardia/ECS/Components.hpp"
#include "Cardia/ECS/Entity.hpp"


namespace Cardia
{
        void InspectorPanel::draw(Entity selectedEntity)
        {
        	m_SelectedEntity = selectedEntity;
                ImGui::Begin("Inspector");
		if(selectedEntity) 
		{
			// Name Component
			auto& name = selectedEntity.getComponent<Component::Name>();

			char buffer[128] {0};
			size_t bufferSize = sizeof(buffer)/sizeof(char);
			name.name.copy(buffer, bufferSize);

			if(ImGui::InputText("Name", buffer, bufferSize))
			{
				name.name = std::string(buffer, bufferSize);
			}

			// Transform Component

			drawInspectorComponent<Component::Transform>("Transform", [](Component::Transform& transform) {
				ImGui::DragFloat3("Position", glm::value_ptr(transform.position), 0.05f);
				auto rotation = glm::degrees(transform.rotation);
				if (ImGui::DragFloat3("Rotation", glm::value_ptr(rotation), 0.2f))
					transform.rotation = glm::radians(rotation);

				ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.05f);
			});

			// SpriteRenderer Component

			drawInspectorComponent<Component::SpriteRenderer>("Sprite Renderer", [](Component::SpriteRenderer& sprite) {
				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
				ImGui::DragFloat("Tiling Factor", &sprite.tillingFactor, 0.05f, 0, 5);
			});

			// Camera Component

			drawInspectorComponent<Component::Camera>("Camera", [](Component::Camera& camera) {
				SceneCamera& cam = camera.camera;

				int type = static_cast<int>(cam.getProjectionType());
				const char *cameraTypes[] = {"Perspective", "Orthographic"};
				if (ImGui::Combo("Camera Type", &type, cameraTypes,
						 sizeof(cameraTypes) / sizeof(char *)))
					cam.setProjectionType(static_cast<SceneCamera::ProjectionType>(type));
				if (cam.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
					float pFov = glm::degrees(cam.getPerspectiveFov());
					if (ImGui::DragFloat("Fov", &pFov, 0.05f))
						cam.setPerspectiveFov(glm::radians(pFov));
					float pFar = cam.getPerspectiveFar();
					if (ImGui::DragFloat("Far", &pFar, 0.05f))
						cam.setPerspectiveFar(pFar);
					float pNear = cam.getPerspectiveNear();
					if (ImGui::DragFloat("Near", &pNear, 0.05f))
						cam.setPerspectiveNear(pNear);
				}
				else if (cam.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
					float oSize = cam.getOrthographicSize();
					if (ImGui::DragFloat("Size", &oSize, 0.05f))
						cam.setOrthographicSize(oSize);
					float oFar = cam.getOrthographicFar();
					if (ImGui::DragFloat("Far", &oFar, 0.05f))
						cam.setOrthographicFar(oFar);
					float oNear = cam.getOrthographicNear();
					if (ImGui::DragFloat("Near", &oNear, 0.05f))
						cam.setOrthographicNear(oNear);
				}
			});

			ImGui::Separator();

			auto windowWidth = ImGui::GetWindowSize().x;
			auto textWidth   = ImGui::CalcTextSize("Add Components...").x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			if (ImGui::Button("Add Component...")) {
				ImGui::OpenPopup("Add Component");
			}


			// Add component button
			if (ImGui::BeginPopup("Add Component"))
			{
				if (!selectedEntity.hasComponent<Component::Camera>() && ImGui::MenuItem("Camera"))
				{
					selectedEntity.addComponent<Component::Camera>();
					ImGui::EndPopup();
				}

				if (!selectedEntity.hasComponent<Component::SpriteRenderer>() && ImGui::MenuItem("Sprite Renderer"))
				{
					selectedEntity.addComponent<Component::SpriteRenderer>();
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();
        }

	
	template<typename T>
	void InspectorPanel::drawInspectorComponent(const char* name, std::function<void(T&)> func)
        {
        	auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

        	if (m_SelectedEntity.hasComponent<T>()) {
        		auto& component = m_SelectedEntity.getComponent<T>();
        		if(ImGui::TreeNodeEx(static_cast<void*>(&component), componentFlags, "%s", name))
        		{
        			if (ImGui::BeginPopupContextItem())
        			{
        				if (ImGui::MenuItem("Reset Component"))
        				{
        					component.reset();
        					ImGui::EndPopup();
        				}

        				if (!std::is_same<T, Component::Transform>::value && ImGui::MenuItem("Remove Component"))
        				{
        					m_SelectedEntity.removeComponent<T>();
        					ImGui::EndPopup();
        				}
        				ImGui::EndPopup();
        			}
        			func(component);
        			ImGui::TreePop();
        		}
        	}
        }
}
