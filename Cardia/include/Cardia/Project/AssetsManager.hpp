#pragma once

#include <memory>
#include <string>
#include <typeindex>
#include <utility>
#include "Cardia/Core/Core.hpp"
#include "Cardia/Renderer/Texture.hpp"
#include "Cardia/DataStructure/Mesh.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Core/Time.hpp"

namespace {
	struct TypeID
	{
		std::type_index type_index;
		std::string ID;
		bool operator==(const TypeID& other) const {
			return ID == other.ID && type_index == other.type_index;
		}
	};

	struct AssetRefCounter
	{
		AssetRefCounter() = default;
		explicit AssetRefCounter(std::shared_ptr<void> resource) : Resource(std::move(resource)) {}
		std::shared_ptr<void> Resource;
		uint32_t UnusedCounter = 0;
	};
}

namespace std {
	template<>
	struct hash<TypeID> {
		auto operator()(const TypeID& typeId) const -> ::size_t {
			return hash<std::string>{}(typeId.ID) ^ hash<std::type_index>{}(typeId.type_index);
		}
	};
}

namespace Cardia
{
	class AssetsManager
	{
	public:
		enum class LoadType {
			Editor,
			Game
		};

	public:
		static std::filesystem::path GetPathFromAsset(const std::shared_ptr<void>& resource);

		static std::filesystem::path GetAssetAbsolutePath(const std::filesystem::path& relative) {
			return Project::GetAssetDirectory() / relative;
		}

		template<typename T>
		static std::shared_ptr<T> Load(const std::filesystem::path& path)
		{
			return Load<T>(path, LoadType::Game);
		}

		template<typename T>
		static std::shared_ptr<T> Load(const std::filesystem::path& path, LoadType loadType)
		{
			return Instance().LoadImpl<T>(path, loadType);
		}

		static void CollectGarbage(bool forceCollection = true);

		static AssetsManager& Instance() { static AssetsManager instance; return instance; }
	private:
		AssetsManager() = default;

		static std::filesystem::path GetAbsolutePath(const std::filesystem::path& relative, LoadType loadType);
		template<typename T>
		std::shared_ptr<T> LoadImpl(const std::filesystem::path& path, LoadType loadType);
		std::unordered_map<TypeID, AssetRefCounter> m_Assets;

		// Collection related
		friend Application;
		void CollectionRoutine(DeltaTime& dt);
		std::chrono::duration<float> m_ElapsedTime {};
	};

	inline std::filesystem::path AssetsManager::GetPathFromAsset(const std::shared_ptr<void>& resource)
	{
		for (auto& res : Instance().m_Assets)
		{
			if (res.second.Resource == resource) {
				return res.first.ID;
			}
		}
		return "";
	}

	template<typename T>
	inline std::shared_ptr<T> AssetsManager::LoadImpl(const std::filesystem::path& path, LoadType loadType)
	{
		cdCoreAssert(false, std::format("Unknown assets type {}", typeid(T).name()));
		return std::shared_ptr<T>();
	}

	template<>
	inline std::shared_ptr<Shader> AssetsManager::LoadImpl(const std::filesystem::path& path, LoadType loadType)
	{
		std::filesystem::path absPath = GetAbsolutePath(path, loadType);
		TypeID id {typeid(Shader), path.string()};

		if (!m_Assets.contains(id)) {
			AssetRefCounter res(Shader::create({absPath.string() + ".vert", absPath.string() + ".frag"}));
			m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Shader>(m_Assets[id].Resource);
	}

	template<>
	inline std::shared_ptr<Texture2D> AssetsManager::LoadImpl(const std::filesystem::path& path, LoadType loadType)
	{
		std::filesystem::path absPath = GetAbsolutePath(path, loadType);
		TypeID id {typeid(Texture2D), path.string()};

		if (!m_Assets.contains(id)) {
			AssetRefCounter res(Texture2D::create(absPath.string()));
			m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Texture2D>(m_Assets[id].Resource);
	}

	template<>
	inline std::shared_ptr<Mesh> AssetsManager::LoadImpl(const std::filesystem::path& path, LoadType loadType)
	{
		std::filesystem::path absPath = GetAbsolutePath(path, loadType);
		TypeID id {typeid(Mesh), path.string()};

		if (!m_Assets.contains(id)) {
			AssetRefCounter res(std::make_unique<Mesh>(Mesh::ReadMeshFromFile(absPath.string())));
			m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Mesh>(m_Assets[id].Resource);

	}
}