#include "Panels/InspectorPanel.hpp"

#include <filesystem>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Cardia/ECS/Components.hpp"
#include "Cardia/ECS/Entity.hpp"


namespace Cardia::Panel
{
        void InspectorPanel::onImGuiRender(DeltaTime deltaTime)
        {
                ImGui::Begin("Inspector");
		if(m_SelectedEntity) 
		{
			// Name Component
			auto& name = m_SelectedEntity.getComponent<Component::Name>();

			char buffer[128] {0};
			constexpr size_t bufferSize = sizeof(buffer)/sizeof(char);
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
				uint32_t whiteColor = 0xffffffff;
				const auto white = Texture2D::create(1, 1, &whiteColor);
				const auto texID = sprite.texture ? sprite.texture->getRendererID() : white->getRendererID();
				ImGui::ImageButton(reinterpret_cast<ImTextureID>(texID), {15, 15}, {0, 1}, {1, 0}, 0);
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH", ImGuiDragDropFlags_AcceptBeforeDelivery))
					{
						Log::coreInfo("Dropped Here");
						const auto* path = static_cast<const wchar_t*>(payload->Data);
						const std::filesystem::path texturePath = path;
						auto tex = Texture2D::create(texturePath.string());
						sprite.texture = std::move(tex);
					}
					ImGui::EndDragDropTarget();
				}
				ImGui::SameLine();
				ImGui::Text("Texture");
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

			const auto windowWidth = ImGui::GetWindowSize().x;
			const auto textWidth   = ImGui::CalcTextSize("Add Components...").x;

			ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
			if (ImGui::Button("Add Component...")) {
				ImGui::OpenPopup("Add Component");
			}


			// Add component button
			if (ImGui::BeginPopup("Add Component"))
			{
				if (!m_SelectedEntity.hasComponent<Component::Camera>() && ImGui::MenuItem("Camera"))
				{
					m_SelectedEntity.addComponent<Component::Camera>();
					ImGui::EndPopup();
				}

				if (!m_SelectedEntity.hasComponent<Component::SpriteRenderer>() && ImGui::MenuItem("Sprite Renderer"))
				{
					m_SelectedEntity.addComponent<Component::SpriteRenderer>();
					ImGui::EndPopup();
				}
			}
		}
		ImGui::End();
        }

        void InspectorPanel::updateSelectedEntity(const Entity& entity) const
        {
        	m_SelectedEntity = entity;
        }


        template<typename T>
	void InspectorPanel::drawInspectorComponent(const char* name, std::function<void(T&)> func)
        {
        	constexpr auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

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

        				if (!std::is_same_v<T, Component::Transform> && ImGui::MenuItem("Remove Component"))
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
