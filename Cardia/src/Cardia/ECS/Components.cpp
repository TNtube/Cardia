#include "cdpch.hpp"

#include "Cardia/ECS/Components.hpp"

#include "Cardia/Asset/AssetsManager.hpp"

namespace Cardia::Component
{

	Json::Value Label::Serialize() const
	{
		Json::Value root;

		auto& label = root["Label"];

		label["Name"] = Name;
		label["Color"] = Color.Serialize();

		return root;
	}

	std::optional<Label> Label::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Label"))
			return std::nullopt;

		const auto& label = root["Label"];

		if (!label.isMember("Name") ||
			!label.isMember("Color"))
			return std::nullopt;

		Label temp;

		temp.Name = label["Name"].asString();

		if (const auto& color = Vector4f::Deserialize(label["Color"]))
			temp.Color = color.value();
		else
			return std::nullopt;

		return temp;
	}

	Json::Value ID::Serialize() const
	{
		Json::Value root;

		root["ID"] = Uuid.ToString();

		return root;
	}

	std::optional<ID> ID::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("ID"))
			return std::nullopt;

		ID temp;
		temp.Uuid = UUID::FromString(root["ID"].asString());

		return temp;
	}

	Vector3f Transform::Forward() const
	{
		Vector3f forward;

		forward.x = cos(Rotation.x) * sin(Rotation.y);
		forward.y = -sin(Rotation.x);
		forward.z = cos(Rotation.x) * cos(Rotation.y);

		return forward;
	}

	Vector3f Transform::Up() const
	{
		return Forward().Cross(Right());
	}

	Vector3f Transform::Right() const
	{
		Vector3f right;
		right.x =  cos(Rotation.y);
		right.y =  0;
		right.z = -sin(Rotation.y);
		return right;
	}

	Json::Value Transform::Serialize() const
	{
		Json::Value root;

		auto& transform = root["Transform"];

		transform["Position"] = Position.Serialize();
		transform["Rotation"] = Rotation.Serialize();
		transform["Scale"] = Scale.Serialize();

		return root;
	}

	std::optional<Transform> Transform::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Transform"))
			return std::nullopt;

		const auto& transform = root["Transform"];

		if (!transform.isMember("Position") ||
			!transform.isMember("Rotation") ||
			!transform.isMember("Scale"))
			return std::nullopt;

		Transform temp;

		if (const auto pos = Vector3f::Deserialize(transform["Position"]))
			temp.Position = pos.value();
		else
			return std::nullopt;

		if (const auto rot = Vector3f::Deserialize(transform["Rotation"]))
			temp.Rotation = rot.value();
		else
			return std::nullopt;

		if (const auto scale = Vector3f::Deserialize(transform["Scale"]))
			temp.Scale = scale.value();
		else
			return std::nullopt;

		return temp;
	}

	Json::Value SpriteRenderer::Serialize() const
	{
		Json::Value root;

		auto& sprite = root["SpriteRenderer"];

		sprite["Color"] = Color.Serialize();
		sprite["Texture"] = AssetsManager::GetPathFromAsset(Texture).string();
		sprite["TilingFactor"] = TillingFactor;
		sprite["ZIndex"] = ZIndex;

		return root;
	}

	std::optional<SpriteRenderer> SpriteRenderer::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("SpriteRenderer"))
			return std::nullopt;

		const auto& sprite = root["SpriteRenderer"];

		if (!sprite.isMember("Color") ||
			!sprite.isMember("Texture") ||
			!sprite.isMember("TilingFactor") ||
			!sprite.isMember("ZIndex"))
			return std::nullopt;

		SpriteRenderer temp;

		if (const auto color = Vector4f::Deserialize(sprite["Color"]))
			temp.Color = color.value();
		else
			return std::nullopt;

		temp.Texture = AssetsManager::Load<Texture2D>(sprite["Texture"].asString());
		temp.TillingFactor = sprite["TilingFactor"].asFloat();
		temp.ZIndex = sprite["ZIndex"].asInt();

		return temp;
	}

	Json::Value MeshRendererC::Serialize() const
	{
		Json::Value root;

		auto& mesh = root["MeshRenderer"];
		mesh["Mesh"] = AssetsManager::GetPathFromAsset(Renderer->GetMesh()).string();

		return root;
	}

	std::optional<MeshRendererC> MeshRendererC::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("MeshRenderer"))
			return std::nullopt;

		const auto& mesh = root["MeshRenderer"];

		if (!mesh.isMember("Mesh"))
			return std::nullopt;

		MeshRendererC temp;

		if (const auto meshAssets = AssetsManager::Load<Mesh>(mesh["Mesh"].asString()))
			temp.Renderer->SubmitMesh(AssetsManager::Instance().GetRenderer().GetDevice(), meshAssets);

		return temp;
	}

	Json::Value Camera::Serialize() const
	{
		Json::Value root;

		auto& camera = root["Camera"];

		camera["CameraData"] = CameraData.Serialize();
		camera["Primary"] = Primary;

		return root;
	}

	std::optional<Camera> Camera::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Camera"))
			return std::nullopt;

		const auto& cameraNode = root["Camera"];

		if (!cameraNode.isMember("CameraData") ||
			!cameraNode.isMember("Primary"))
			return std::nullopt;

		Camera temp;

		if (const auto camera = SceneCamera::Deserialize(cameraNode["CameraData"]))
			temp.CameraData = camera.value();
		else
			return std::nullopt;

		temp.Primary = cameraNode["Primary"].asBool();

		return temp;
	}

	Json::Value Light::Serialize() const
	{
		Json::Value root;

		auto& light = root["Light"];
		light["LightType"] = LightType;
		light["Color"] = Color.Serialize();
		light["Range"] = Range;
		light["Angle"] = Angle;
		light["Smoothness"] = Smoothness;

		return root;
	}

	std::optional<Light> Light::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Light"))
			return std::nullopt;

		const auto& lightNode = root["Light"];
		if (!lightNode.isMember("LightType") ||
			!lightNode.isMember("Color") ||
			!lightNode.isMember("Range") ||
			!lightNode.isMember("Angle") ||
			!lightNode.isMember("Smoothness"))
			return std::nullopt;

		Light temp;

		if (const auto color = Vector4f::Deserialize(lightNode["Color"]))
			temp.Color = color.value();
		else
			return std::nullopt;

		temp.LightType = lightNode["LightType"].asInt();
		temp.Range = lightNode["Range"].asFloat();
		temp.Angle = lightNode["Angle"].asFloat();
		temp.Smoothness = lightNode["Smoothness"].asFloat();

		return temp;
	}


	Json::Value Script::Serialize() const
	{
		Json::Value root;
		auto& script = root["Script"];
		
		script["Path"] = GetPath();

		for (auto& item : Class.Attributes()) {
			script["Attributes"].append(item.Serialize());
		}

		return root;
	}

	std::optional<Script> Script::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Script"))
			return std::nullopt;

		auto& script = root["Script"];

		if (!script.isMember("Path"))
			return std::nullopt;

		Script temp;

		temp.SetPath(script["Path"].asString());

		if (!script.isMember("Attributes"))
			return temp;

		auto& attrsNode = script["Attributes"];
		auto& attrs = temp.Class.Attributes();
		for (const auto& subNode: attrsNode) {
			auto field = ScriptField::Deserialize(subNode);
			if (!field) continue;
			auto attrPair = std::ranges::find_if(attrs, [&](const ScriptField& attr) {
				return attr == field.value();
			});
			if (attrPair != attrs.end()) {
				*attrPair = field.value();
			}
		}

		return temp;
	}

	void Script::ReloadFile()
	{
		if (m_Path.empty()) return;
		Log::Info("Loading path {0}", m_Path);
		const std::ifstream t(m_Path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		auto filePath = std::filesystem::path(m_Path);
		Class = ScriptEngine::Instance().GetClassFromPyFile(filePath);
		Class.RegisterAttributes();
	}
}
