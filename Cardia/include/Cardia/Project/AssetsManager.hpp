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
		AssetsManager();
		virtual ~AssetsManager();

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
			if (path.empty()) return nullptr;

			std::filesystem::path absPath;
			switch (loadType)
			{

				case LoadType::Editor:
					absPath = path;
					break;
				case LoadType::Game:
					absPath = GetAssetAbsolutePath(path);
					break;
			}
			return s_ActiveManager->LoadImpl<T>(absPath);
		}

		static void CollectGarbage(bool forceCollection = true);

	private:
		template<typename T>
		std::shared_ptr<T> LoadImpl(const std::filesystem::path& path);

		std::unordered_map<TypeID, AssetRefCounter> m_Assets;

		// Threading related
		void CollectThread();
		std::thread m_CollectThread;
		std::mutex m_CounterMutex;
		std::condition_variable m_CollectCV;
		bool m_ShouldStopCollecting = false;

		static std::shared_ptr<AssetsManager> s_ActiveManager;
	};

	template<typename T>
	inline std::shared_ptr<T> AssetsManager::LoadImpl(const std::filesystem::path& path)
	{
		cdCoreAssert(false, std::format("Unknown assets type {}", typeid(T).name()));
		return std::shared_ptr<T>();
	}

	template<>
	inline std::shared_ptr<Shader> AssetsManager::LoadImpl(const std::filesystem::path& path)
	{
		TypeID id {typeid(Shader), path.string()};

		if (!m_Assets.contains(id)) {
			AssetRefCounter res(Shader::create({path.string() + ".vert", path.string() + ".frag"}));
			m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Shader>(m_Assets[id].Resource);
	}

	template<>
	inline std::shared_ptr<Texture2D> AssetsManager::LoadImpl(const std::filesystem::path& path)
	{
		TypeID id {typeid(Texture2D), path.string()};

		if (!m_Assets.contains(id)) {
			AssetRefCounter res(Texture2D::create(path.string()));
			m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Texture2D>(m_Assets[id].Resource);
	}

	template<>
	inline std::shared_ptr<Mesh> AssetsManager::LoadImpl(const std::filesystem::path& path)
	{
		TypeID id {typeid(Mesh), path.string()};

		if (!m_Assets.contains(id)) {
			AssetRefCounter res(std::make_unique<Mesh>(Mesh::ReadMeshFromFile(path.string())));
			m_Assets.insert_or_assign(id, res);
		}

		return std::static_pointer_cast<Mesh>(m_Assets[id].Resource);

	}
}