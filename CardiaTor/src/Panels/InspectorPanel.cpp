#include "Panels/InspectorPanel.hpp"

#include <filesystem>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

#include "Cardia/ECS/Components.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "EditorUI/DragData.hpp"
#include "Panels/PanelManager.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Project/AssetsManager.hpp"

#define PYBIND11_DETAILED_ERROR_MESSAGES


namespace Cardia::Panel
{
	int InspectorPanel::m_LastWindowId = 0;
	void InspectorPanel::OnImGuiRender(CardiaTor* appContext)
	{
		char buff[64];
		sprintf(buff, "Inspector##%i", m_WindowId);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(buff, &m_IsOpen)) {
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowFocused()) {
			m_PanelManager->SetFocused<InspectorPanel>(this);
		}

		if(!m_CurrentScene)
		{
			ImGui::End();
			return;
		}

		if(!m_SelectedEntity)
		{
			ImGui::End();
			return;
		}
		// Name Component
		auto& name = m_SelectedEntity.getComponent<Component::Name>();
		auto& uuid = m_SelectedEntity.getComponent<Component::ID>();

		char buffer[128] {0};
		constexpr size_t bufferSize = sizeof(buffer)/sizeof(char);
		name.name.copy(buffer, bufferSize);

		if(EditorUI::InputText("Name", buffer, bufferSize))
		{
			name.name = std::string(buffer);
		}

		// Transform Component

		DrawInspectorComponent<Component::Transform>("Transform", [](Component::Transform& transform) {
			EditorUI::DragFloat3("Position", transform.position);

			auto rotation = glm::degrees(transform.rotation);
			EditorUI::DragFloat3("Rotation", rotation);
			transform.rotation = glm::radians(rotation);

			EditorUI::DragFloat3("Scale", transform.scale, 1);
		});

		// SpriteRenderer Component

		DrawInspectorComponent<Component::SpriteRenderer>("Sprite Renderer", [](Component::SpriteRenderer& sprite) {
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
					const auto* cStrPath = static_cast<const char*>(payload->Data);
					auto tex = AssetsManager::Load<Texture2D>(cStrPath);
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


		// MeshRendererC Component

		DrawInspectorComponent<Component::MeshRendererC>("Mesh Renderer", [](Component::MeshRendererC& meshRendererC) {
			char buffer[128] {0};
			constexpr size_t bufferSize = sizeof(buffer)/sizeof(char);
			AssetsManager::GetPathFromAsset(meshRendererC.mesh).string().copy(buffer, bufferSize);

			EditorUI::InputText("Mesh path", buffer, bufferSize, ImGuiInputTextFlags_ReadOnly);
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					auto path = std::filesystem::path(static_cast<const char*>(payload->Data));
					auto mesh = AssetsManager::Load<Mesh>(path);
					meshRendererC.mesh = mesh;
					meshRendererC.meshRenderer->SubmitMesh(*mesh);
				}
				ImGui::EndDragDropTarget();
			}

			uint32_t whiteColor = 0xffffffff;
			static const auto white = Texture2D::create(1, 1, &whiteColor);
			const auto texID = meshRendererC.texture ? meshRendererC.texture->getRendererID() : white->getRendererID();

			ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(texID)), {15, 15}, {0, 1}, {1, 0});
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
				{
					const auto* cStrPath = static_cast<const char*>(payload->Data);
					auto tex = AssetsManager::Load<Texture2D>(cStrPath);
					if (tex->isLoaded())
					{
						meshRendererC.texture = std::move(tex);
					}
				}
				ImGui::EndDragDropTarget();
			}
			ImGui::SameLine();
			ImGui::Text("Texture");
		});

		// Camera Component

		DrawInspectorComponent<Component::Camera>("Camera", [](Component::Camera& camera) {
			SceneCamera& cam = camera.camera;

			int type = static_cast<int>(cam.GetProjectionType());
			const char *cameraTypes[] = {"Perspective", "Orthographic"};
			if (EditorUI::Combo("Camera Type", &type, cameraTypes, sizeof(cameraTypes) / sizeof(char *)))
				cam.SetProjectionType(static_cast<SceneCamera::ProjectionType>(type));

			auto& isPrimary = camera.primary;

			EditorUI::Checkbox("Primary", &isPrimary);

			if (cam.GetProjectionType() == SceneCamera::ProjectionType::Perspective) {
				auto perspective = cam.GetPerspective();

				float pFov = glm::degrees(perspective.x);
				bool edited = EditorUI::DragFloat("Fov", &pFov, 0.05f);

				float pNear = perspective.y;
				edited = edited || EditorUI::DragFloat("Near", &pNear, 0.05f);

				float pFar = perspective.z;
				edited = edited || EditorUI::DragFloat("Far", &pFar, 0.05f);

				if (edited)
					cam.SetPerspective(glm::radians(pFov), pNear, pFar);
			}
			else if (cam.GetProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				auto orthographic = cam.GetOrthographic();

				float oSize = orthographic.x;
				bool edited = EditorUI::DragFloat("Size", &oSize, 0.05f);

				float oNear = orthographic.y;
				edited = edited || EditorUI::DragFloat("Near", &oNear, 0.05f);

				float oFar = orthographic.z;
				edited = edited || EditorUI::DragFloat("Far", &oFar, 0.05f);

				if (edited)
					cam.SetOrthographic(oSize, oNear, oFar);
			}
		});

		// Light Component

		DrawInspectorComponent<Component::Light>("Light", [](Component::Light& light) {
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

		DrawInspectorComponent<Component::Script>("Script", [&](Component::Script& scriptComponent) {
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

			if (!m_SelectedEntity.hasComponent<Component::MeshRendererC>() && ImGui::MenuItem("Mesh Renderer"))
			{
				m_SelectedEntity.addComponent<Component::MeshRendererC>();
				ImGui::EndPopup();
			}

			if (!m_SelectedEntity.hasComponent<Component::Script>() && ImGui::MenuItem("Entity Behavior"))
			{
				m_SelectedEntity.addComponent<Component::Script>();
				ImGui::EndPopup();
			}

			if (!m_SelectedEntity.hasComponent<Component::Light>() && ImGui::MenuItem("Light"))
			{
				m_SelectedEntity.addComponent<Component::Light>();
				ImGui::EndPopup();
			}
		}
		ImGui::End();
	}


	template<typename T>
	void InspectorPanel::DrawInspectorComponent(const char* name, std::function<void(T&)> func)
	{
		if (!m_SelectedEntity.hasComponent<T>())
			return;

		constexpr auto componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

		auto& component = m_SelectedEntity.getComponent<T>();
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
				m_SelectedEntity.removeComponent<T>();
				ImGui::EndPopup();
			}
			ImGui::EndPopup();
		}
		func(component);
		ImGui::TreePop();
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

	void InspectorPanel::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	void InspectorPanel::OnSceneLoad(Scene *scene)
	{
		IPanel::OnSceneLoad(scene);
		m_SelectedEntity = Entity();
	}
}
