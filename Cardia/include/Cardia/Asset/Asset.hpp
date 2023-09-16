#pragma once
#include <filesystem>
#include <utility>
#include "Cardia/Core/UUID.hpp"


namespace Cardia
{
	struct AssetHandle
	{
		UUID ID;
		std::filesystem::path Path;

		bool IsValid() const { return ID.IsValid(); }

		bool operator==(const AssetHandle& other) const { return ID == other.ID; }
	};

	class Asset
	{
	public:
		explicit Asset(AssetHandle assetHandle) : m_Handle(std::move(assetHandle)) {};
		Asset() = default;
		virtual ~Asset() = default;

		const AssetHandle& GetHandle() const { return m_Handle; }

		virtual void Reload() = 0;

	protected:
		AssetHandle m_Handle;
	};
}