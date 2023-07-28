#include "cdpch.hpp"

#include "Cardia/ECS/Components.hpp"
#include <json/json.h>
#include <Cardia/Serialization/SceneSerializer.hpp>
#include <Cardia/Project/Project.hpp>
#include <Cardia/Asset/AssetsManager.hpp>

#include "Cardia/Math/Vector2.hpp"


namespace Json {
	using namespace Cardia;

	template <> Cardia::ScriptField Value::as<ScriptField>() const {
		auto& node = *this;
		Cardia::ScriptField out;

		out.name = node["name"].asString();
		out.type = static_cast<ScriptFieldType>(node["type"].asInt());

		switch (out.type) {
			case ScriptFieldType::Int:
				out.instance = py::cast(node["value"].asInt());
				break;
			case ScriptFieldType::Float:
				out.instance = py::cast(node["value"].asFloat());
				break;
			case ScriptFieldType::String:
				out.instance = py::cast(node["value"].asString());
				break;
			case ScriptFieldType::List:
			{
				py::list list;
				for (auto& subNode: node["value"]) {
					auto subField = subNode.as<ScriptField>();
					out.valueType = subField.type;
					list.attr("append")(subField.instance.object());
				}
				out.instance = list;
				break;
			}
			case ScriptFieldType::Dict:break;
			case ScriptFieldType::PyBehavior:
				out.instance = py::cast(node["value"].asString());
				break;
			case ScriptFieldType::Vector2:
				Vector2f vec2;
				Vector2f::Deserialize(node["value"], vec2);
				out.instance = py::cast(vec2);
				break;
			case ScriptFieldType::Vector3:
				Vector3f vec3;
				Vector3f::Deserialize(node["value"], vec3);
				out.instance = py::cast(vec3);
				break;
			case ScriptFieldType::Vector4:
				Vector4f vec4;
				Vector4f::Deserialize(node["value"], vec4);
				out.instance = py::cast(vec4);
				break;
			case ScriptFieldType::Unserializable:break;
		}
		return out;
	}
}


namespace Cardia::Serialization
{
	Json::Value ToJson(const ScriptField& field) {
		Json::Value out;
		out["type"] = static_cast<int>(field.type);
		out["name"] = field.name;

		switch (field.type) {
			case ScriptFieldType::Int:
				out["value"] = py::handle(field.instance).cast<int>();
				break;
			case ScriptFieldType::Float:
				out["value"] = py::handle(field.instance).cast<float>();
				break;
			case ScriptFieldType::String:
				out["value"] = py::handle(field.instance).cast<std::string>();
				break;
			case ScriptFieldType::List:
			{
				py::list list(field.instance);
				for (int index = 0; index < py::len(list); index++) {
					ScriptField subField;
					subField.name = std::to_string(index);
					subField.instance = py::object(list[index]);
					switch (subField.valueType) {
						case ScriptFieldType::List:
						case ScriptFieldType::Dict:
							subField.type = ScriptFieldType::Unserializable;
							break;
						default:
							subField.type = field.valueType;
							break;
					}
					out["value"].append(ToJson(subField));
				}
				break;
			}
			case ScriptFieldType::Dict:break;
			case ScriptFieldType::PyBehavior:
				out["value"] = py::handle(field.instance).cast<std::string>();
				break;
			case ScriptFieldType::Vector2:
				out["value"] = py::handle(field.instance).cast<Vector2f>().Serialize();
				break;
			case ScriptFieldType::Vector3:
				out["value"] = py::handle(field.instance).cast<Vector3f>().Serialize();
				break;
			case ScriptFieldType::Vector4:
				out["value"] = py::handle(field.instance).cast<Vector4f>().Serialize();
				break;
			case ScriptFieldType::Unserializable:break;
		}

		return out;

	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::ID& component)
	{
		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::ID::ClassName()] = std::string(component.uuid);
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Transform& component)
	{
		Json::Value node;
		node["position"] = component.position.Serialize();
		node["rotation"] = component.rotation.Serialize();
		node["scale"] = component.scale.Serialize();

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Transform::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Name& component)
	{

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Name::ClassName()] = component.name;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::MeshRendererC& component)
	{

		auto mesh = component.meshRenderer->GetMesh();
		Json::Value node;

		if (mesh)
		{
			node["path"] = AssetsManager::GetPathFromAsset(component.meshRenderer->GetMesh()).string();
			Json::Value materials;
			for (const auto& material : component.meshRenderer->GetMesh()->GetMaterials()) {
				materials.append(AssetsManager::GetPathFromAsset(material).string());
			}
			node["materials"] = materials;
		}

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::MeshRendererC::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::SpriteRenderer& component)
	{

		Json::Value node;

		node["color"] = component.color.Serialize();
		node["texture"] = AssetsManager::GetPathFromAsset(component.texture).string();
		node["tillingFactor"] = component.tillingFactor;
		node["zIndex"] = component.zIndex;

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::SpriteRenderer::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Camera &component)
	{

		Json::Value node;

		node["type"] = static_cast<int>(component.camera.GetProjectionType());
		const auto pers = component.camera.GetPerspective();
		node["perspective"] = pers.Serialize();

		const auto ortho = component.camera.GetOrthographic();
		node["orthographic"] = ortho.Serialize();

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Camera::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Light &component)
	{

		Json::Value node;

		node["type"] = component.lightType;
		node["color"] = component.color.Serialize();

		node["range"] = component.range;
		node["angle"] = component.angle;
		node["smoothness"] = component.smoothness;

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Light::ClassName()] = node;
	}

	void SceneArchiveOutput::operator()(entt::entity entity, const Component::Script &component)
	{

		Json::Value node;

		node["path"] = component.GetPath();

		for (auto& item : component.scriptClass.Attributes()) {
			node["attributes"].append(ToJson(item));
		}

		auto idx = static_cast<uint32_t>(entity);
		m_Root[idx][Component::Script::ClassName()] = node;
	}

	void SceneArchiveOutput::Finalize() {
		Json::Value root;
		for (const auto & idx : m_Root) {
			if (!idx.isNull()) {
				root.append(idx);
			}
		}
		m_Root = root;
	}

	std::string SceneArchiveOutput::ToString()
	{
		std::stringstream output;
		output << m_Root;
		return output.str();
	}

	std::ostream &operator<<(std::ostream &stream, const SceneArchiveOutput& archiveOutput)
	{
		stream << archiveOutput.m_Root;
		return stream;
	}

	template<typename... Cpn>
	static SceneArchiveOutput SerializeComponents(ComponentGroup<Cpn...>, entt::registry& src)
	{
		SceneArchiveOutput archiveOutput;
		entt::snapshot snapshot{src};
		snapshot.component<Cpn...>(archiveOutput);

		archiveOutput.Finalize();
		return std::move(archiveOutput);
	}


	bool Serialization::SceneSerializer::Serialize(const std::filesystem::path &path)
	{
		std::ofstream file(path);
		if (!file.is_open())
			return false;

		file << SerializeComponents(AllComponents{}, m_Scene.GetRegistry());
		return true;
	}



	bool SceneSerializer::Serialize( Json::Value &root)
	{
		auto archive = SerializeComponents(AllComponents{}, m_Scene.GetRegistry());
		root = archive.m_Root;
		return true;
	}

	bool Serialization::SceneSerializer::Deserialize(Renderer& renderer, const std::filesystem::path &path)
	{
		Json::Value root;
		std::ifstream file(path);

		std::string err;
		const Json::CharReaderBuilder builder;
		if (!file.is_open() || !Json::parseFromStream(builder, file, &root, &err))
		{
			Log::CoreError("Could not load Scene {0} : {1}", path.string(), err);
			return false;
		}

		return Deserialize(renderer, root);
	}

	bool SceneSerializer::Deserialize(Renderer& renderer, const Json::Value &root)
	{
		for (const auto& node: root)
		{
			std::string uuid = node[Component::ID::ClassName()].asString();
			UUID id;
			Entity entity;
			try {
				id = UUID::FromString(uuid);
				entity = m_Scene.CreateEntityFromId(id);
			}
			catch (const std::invalid_argument& e) {
				Log::Warn("Entity with invalid UUID found");
				entity = m_Scene.CreateEntity();
			}


			auto& name = entity.GetComponent<Component::Name>();
			name.name = node[Component::Name::ClassName()].asString();

			auto& component = entity.GetComponent<Component::Transform>();
			Vector3f::Deserialize(node[Component::Transform::ClassName()]["position"], component.position);
			Vector3f::Deserialize(node[Component::Transform::ClassName()]["rotation"], component.rotation);
			Vector3f::Deserialize(node[Component::Transform::ClassName()]["scale"], component.scale);


			auto currComponent = Component::SpriteRenderer::ClassName();
			if (node.isMember(currComponent))
			{
				auto& spriteRenderer = entity.AddComponent<Component::SpriteRenderer>();
				Vector4f::Deserialize(node[currComponent]["color"], spriteRenderer.color);

				spriteRenderer.texture =
					std::make_shared<Texture2D>(
						renderer.GetDevice(),
						renderer,
						node[currComponent]["texture"].asString());
				// if (texture && texture->IsLoaded())
				// {
				// 	spriteRenderer.texture = std::move(texture);
				// }

				spriteRenderer.tillingFactor = node[currComponent]["tillingFactor"].asFloat();
				spriteRenderer.zIndex = node[currComponent]["zIndex"].asInt();
			}

			currComponent = Component::MeshRendererC::ClassName();
			if (node.isMember(currComponent)) {
				auto& meshRenderer = entity.AddComponent<Component::MeshRendererC>();

				auto mesh = AssetsManager::Load<Mesh>(node[currComponent]["path"].asString());

				auto& materials = node[currComponent]["materials"];
				for (const auto& texturePath : materials) {
					std::filesystem::path path = texturePath.asString();
					if (path.empty())
					{
						continue;
					}
					
					auto texture = 
					std::make_shared<Texture2D>(
						renderer.GetDevice(),
						renderer,
						AssetsManager::GetAssetAbsolutePath(path));
					mesh->GetMaterials().push_back(std::move(texture));
					// if (texture && texture->IsLoaded())
					// {
					// 	meshRenderer.meshRenderer->GetMesh()->GetMaterials().push_back(std::move(texture));
					// }
				}
				meshRenderer.meshRenderer->SubmitMesh(renderer.GetDevice(), mesh);
			}

			currComponent = Component::Camera::ClassName();
			if (node.isMember(currComponent))
			{
				auto& camera = entity.AddComponent<Component::Camera>();

				PerspectiveData pers;
				PerspectiveData::Deserialize(node[currComponent]["perspective"], pers);
				camera.camera.SetPerspective(pers);

				OrthographicData ortho;
				OrthographicData::Deserialize(node[currComponent]["orthographic"], ortho);
				camera.camera.SetOrthographic(ortho);

				camera.camera.SetProjectionType(
					static_cast<SceneCamera::ProjectionType>(node[currComponent]["type"].asInt()));
			}


			currComponent = Component::Light::ClassName();
			if (node.isMember(currComponent))
			{
				auto& light = entity.AddComponent<Component::Light>();
				light.lightType = node[currComponent]["type"].asInt();
				Vector3f::Deserialize(node[currComponent]["color"], light.color);

				light.range = node[currComponent]["range"].asFloat();
				light.angle = node[currComponent]["angle"].asFloat();
				light.smoothness = node[currComponent]["smoothness"].asFloat();

			}

			currComponent = Component::Script::ClassName();
			if (node.isMember(currComponent))
			{
				auto& behavior = entity.AddComponent<Component::Script>();
				behavior.SetPath(node[currComponent]["path"].asString());

				auto& attrsNode = node[currComponent]["attributes"];
				auto& attrs = behavior.scriptClass.Attributes();
				for (const auto& subNode: attrsNode) {
					auto field = subNode.as<ScriptField>();
					auto attrPair = std::find_if(attrs.begin(), attrs.end(), [&](auto& attr) {
						return attr == field;
					});
					if (attrPair != attrs.end()) {
						*attrPair = field;
					}
				}
			}
		}
		return true;
	}
}
