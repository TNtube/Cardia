#include "Panels/InspectorPanel.hpp"

#include <filesystem>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Cardia/ECS/Components.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "EditorUI/DragData.hpp"

#define PYBIND11_DETAILED_ERROR_MESSAGES


namespace Cardia::Panel
{
        void InspectorPanel::OnImGuiRender()
        {
                ImGui::Begin("Inspector");

		if(!m_CurrentScene)
		{
			ImGui::End();
			return;
		}

		auto selectedEntity = m_CurrentScene->GetCurrentEntity();
		if(!selectedEntity)
		{
			ImGui::End();
			return;
		}
		// Name Component
		auto& name = selectedEntity.getComponent<Component::Name>();
		auto& uuid = selectedEntity.getComponent<Component::ID>();

		char buffer[128] {0};
		constexpr size_t bufferSize = sizeof(buffer)/sizeof(char);
		name.name.copy(buffer, bufferSize);

		if(EditorUI::InputText("Name", buffer, bufferSize))
		{
			name.name = std::string(buffer);
		}

		// Transform Component

		DrawInspectorComponent<Component::Transform>("Transform", selectedEntity, [](Component::Transform& transform) {
			EditorUI::DragFloat3("Position", transform.position);

			auto rotation = glm::degrees(transform.rotation);
			EditorUI::DragFloat3("Rotation", rotation);
			transform.rotation = glm::radians(rotation);

			EditorUI::DragFloat3("Scale", transform.scale, 1);
		});

		// SpriteRenderer Component

		DrawInspectorComponent<Component::SpriteRenderer>("Sprite Renderer", selectedEntity, [](Component::SpriteRenderer& sprite) {
			EditorUI::ColorEdit4("Color", glm::value_ptr(sprite.color));
			EditorUI::DragFloat("Tiling Factor", &sprite.tillingFactor, 0.05f, 0);
			uint32_t whiteColor = 0xffffffff;

			const auto white = Texture2D::create(1, 1, &whiteColor);
			const auto texID = sprite.texture ? sprite.texture->getRendererID() : white->getRendererID();

			ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(texID)), {15, 15}, {0, 1}, {1, 0});
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					const auto* path = static_cast<const char*>(payload->Data);
					auto tex = Texture2D::create(path);
					if (tex->isLoaded())
					{
						sprite.texture = std::move(tex);
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Texture");
			EditorUI::DragInt("zIndex", &sprite.zIndex, 0.05f);
		});

		// Camera Component

		DrawInspectorComponent<Component::Camera>("Camera", selectedEntity, [](Component::Camera& camera) {
			SceneCamera& cam = camera.camera;

			int type = static_cast<int>(cam.getProjectionType());
			const char *cameraTypes[] = {"Perspective", "Orthographic"};
			if (EditorUI::Combo("Camera Type", &type, cameraTypes, sizeof(cameraTypes) / sizeof(char *)))
				cam.setProjectionType(static_cast<SceneCamera::ProjectionType>(type));

			auto& isPrimary = camera.primary;

			EditorUI::Checkbox("Primary", &isPrimary);

			if (cam.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float pFov = glm::degrees(cam.getPerspectiveFov());
				if (EditorUI::DragFloat("Fov", &pFov, 0.05f))
					cam.setPerspectiveFov(glm::radians(pFov));
				float pFar = cam.getPerspectiveFar();
				if (EditorUI::DragFloat("Far", &pFar, 0.05f))
					cam.setPerspectiveFar(pFar);
				float pNear = cam.getPerspectiveNear();
				if (EditorUI::DragFloat("Near", &pNear, 0.05f))
					cam.setPerspectiveNear(pNear);
			}
			else if (cam.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float oSize = cam.getOrthographicSize();
				if (EditorUI::DragFloat("Size", &oSize, 0.05f))
					cam.setOrthographicSize(oSize);
				float oFar = cam.getOrthographicFar();
				if (EditorUI::DragFloat("Far", &oFar, 0.05f))
					cam.setOrthographicFar(oFar);
				float oNear = cam.getOrthographicNear();
				if (EditorUI::DragFloat("Near", &oNear, 0.05f))
					cam.setOrthographicNear(oNear);
			}
		});

		// Light Component

		DrawInspectorComponent<Component::Light>("Light", selectedEntity, [](Component::Light& light) {
			int item_current = light.lightType;
			const char* items[] = { "Directional Light", "Point Light", "Spot Light" };
			EditorUI::Combo("Light Type", &item_current, items, IM_ARRAYSIZE(items));

			light.lightType = item_current;

			EditorUI::ColorEdit3("Color", glm::value_ptr(light.color));

			if (item_current == 0) return;
			EditorUI::DragFloat("Range", &light.range, 0.01f, 0.0f);

			if (item_current == 1) return;
			EditorUI::DragFloat("Angle", &light.angle, 0.5f, 0.0f);
			EditorUI::DragFloat("Smoothness", &light.smoothness, 0.5f);
		});

		DrawInspectorComponent<Component::Script>("Script", selectedEntity, [&](Component::Script& scriptComponent) {
			std::filesystem::path filepath = scriptComponent.getPath();
			auto path = filepath.filename().string();

			char* buffer = &path[0];
			EditorUI::InputText("Script Name", buffer, path.size(), ImGuiInputTextFlags_ReadOnly);

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					const std::filesystem::path behaviorPath = static_cast<const char*>(payload->Data);
					if (behaviorPath.extension() == ".py")
					{
						scriptComponent.setPath(behaviorPath.string());
					}
					else
					{
						Log::warn("Could not load file {0}", behaviorPath.string());
					}
				}
				ImGui::EndDragDropTarget();
			}

			auto& attributes = scriptComponent.scriptClass.Attributes();

			for (auto& attribute: attributes)
			{
				auto fieldName = attribute.name;
				auto instance = ScriptEngine::Instance().GetInstance(uuid.uuid);
				auto type = attribute.type;
				DrawField(instance, type, fieldName.c_str(), attribute.instance.object());
				switch (type) {
					case ScriptFieldType::List:
					{
						py::list list;
						if (instance) {
							list = instance->GetAttrOrMethod(fieldName.c_str());
						} else {
							list = attribute.instance.object();
						}
						if(ImGui::TreeNodeEx(static_cast<void*>(list.ptr()), ImGuiTreeNodeFlags_SpanAvailWidth, "%s", fieldName.c_str()))
						{
							auto len = py::len(list);
							auto index_to_del = -1;
							for (int index = 0; index < len; index++) {
								py::object object(list[index]);
								auto str = std::to_string(index);
								if (ImGui::Button(" - ")) {
									index_to_del = index;
								}
								ImGui::SameLine();
								if (DrawField(nullptr, attribute.valueType, str.c_str(), object)) {
									list[index] = object;
								}
							}
							if (index_to_del >= 0) {
								list.attr("pop")(index_to_del);
							}
							const auto textWidth = ImGui::CalcTextSize("  +  ").x;

							ImGui::SetCursorPosX((ImGui::GetWindowSize().x - textWidth) * 0.5f);
							if (ImGui::Button("  +  ")) {
								list.append(DefaultObjectFromScriptFieldType(attribute.valueType));
							}

							ImGui::TreePop();
						}
						break;
					}
					case ScriptFieldType::Dict: {
						auto dict = py::dict(instance->GetAttrOrMethod(fieldName.c_str()));
						break;
					}

					default: break;
				}

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

			if (!selectedEntity.hasComponent<Component::Script>() && ImGui::MenuItem("Entity Behavior"))
			{
				selectedEntity.addComponent<Component::Script>();
				ImGui::EndPopup();
			}

			if (!selectedEntity.hasComponent<Component::Light>() && ImGui::MenuItem("Light"))
			{
				selectedEntity.addComponent<Component::Light>();
				ImGui::EndPopup();
			}
		}
		ImGui::End();
        }


        template<typename T>
	void InspectorPanel::DrawInspectorComponent(const char* name, Entity entity, std::function<void(T&)> func)
	{
		if (!entity.hasComponent<T>())
			return;

		constexpr auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		auto& component = entity.getComponent<T>();
		if(!ImGui::TreeNodeEx(static_cast<void*>(&component), componentFlags, "%s", name))
			return;


		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Reset Component"))
			{
				component.reset();
				ImGui::EndPopup();
			}
			if (!std::is_same_v<T, Component::Transform> && ImGui::MenuItem("Remove Component"))
			{
				entity.removeComponent<T>();
				ImGui::EndPopup();
			}
			ImGui::EndPopup();
		}
		func(component);
		ImGui::TreePop();
	}

	void InspectorPanel::OnSceneLoad(Cardia::Scene *scene)
	{
		m_CurrentScene = scene;
	}

	bool InspectorPanel::DrawField(ScriptInstance* behaviorInstance, ScriptFieldType type, const char* fieldName, py::object& field) {
		py::object value;
		if (behaviorInstance) {
			value = behaviorInstance->GetAttrOrMethod(fieldName);
		} else {
			value = field;
		}

		switch (type) {
			case ScriptFieldType::Int:
			{
				auto castedField = value.cast<int>();
				if (!EditorUI::DragInt(fieldName, &castedField, 0.1f))
					return false;

				auto pyField = py::cast(castedField);
				if (behaviorInstance)
				{
					behaviorInstance->SetAttr(fieldName, pyField);
				}
				field = pyField;
				return true;
			}
			case ScriptFieldType::Float:
			{
				auto castedField = value.cast<float>();
				if (!EditorUI::DragFloat(fieldName, &castedField, 0.1f))
					return false;

				auto pyField = py::cast(castedField);
				if (behaviorInstance)
				{
					behaviorInstance->SetAttr(fieldName, pyField);
				}
				field = pyField;
				return true;
			}
			case ScriptFieldType::String:
			{
				auto castedField = value.cast<std::string>();
				char buff[128] {0};
				constexpr size_t bufferSize = sizeof(buff) / sizeof(char);
				castedField.copy(buff, bufferSize);
				if (!EditorUI::InputText(fieldName, buff, IM_ARRAYSIZE(buff)))
					return false;
				auto pyField = py::cast(std::string(buff));
				if (behaviorInstance)
				{
					behaviorInstance->SetAttr(fieldName, pyField);
				}
				field = pyField;
				return true;
			}
			case ScriptFieldType::PyBehavior:
			{
				char buff[128]{0};
				constexpr size_t bufferSize = sizeof(buff) / sizeof(char);
				auto id = field.cast<std::string>();
				try {
					auto entity = m_CurrentScene->GetEntityByUUID(UUID::fromString(id));

					if (entity) {
						auto instanceName = entity.getComponent<Component::Name>().name;
						instanceName.copy(buff, bufferSize);
					}
				} catch (std::exception& e) {

				}
				EditorUI::InputText(fieldName, buff, bufferSize, ImGuiInputTextFlags_ReadOnly);
				if (!ImGui::BeginDragDropTarget())
					return false;

				const ImGuiPayload* payload;
				if (!(payload = ImGui::AcceptDragDropPayload("ENTITY_UUID"))){
					ImGui::EndDragDropTarget();
					return false;
				}
				const char* str = static_cast<const char*>(payload->Data);
				field = py::cast(str);
				auto script = ScriptEngine::Instance().GetInstance(UUID::fromString(str));
				if (script && behaviorInstance)
				{
					behaviorInstance->SetAttr(fieldName, *script);
				}
				ImGui::EndDragDropTarget();
				return true;
			}
			case ScriptFieldType::Vector2:
			{
				auto castedField = value.cast<glm::vec2>();
				if (!EditorUI::DragFloat2(fieldName, castedField, 0.1f))
					return false;
				py::setattr(field, "x", py::cast(castedField.x));
				py::setattr(field, "y", py::cast(castedField.y));
			}
			case ScriptFieldType::Vector3:
			{
				auto castedField = value.cast<glm::vec3>();
				if (!EditorUI::DragFloat3(fieldName, castedField, 0.1f))
					return false;
				py::setattr(field, "x", py::cast(castedField.x));
				py::setattr(field, "y", py::cast(castedField.y));
				py::setattr(field, "z", py::cast(castedField.z));
			}
			case ScriptFieldType::Vector4:
			{
				auto castedField = value.cast<glm::vec4>();
				if (!EditorUI::DragFloat4(fieldName, castedField, 0.1f))
					return false;
				py::setattr(field, "x", py::cast(castedField.x));
				py::setattr(field, "y", py::cast(castedField.y));
				py::setattr(field, "z", py::cast(castedField.z));
				py::setattr(field, "w", py::cast(castedField.w));
			}
			default:
				return false;
		}

	}
}
