#pragma once
#include <filesystem>


namespace Cardia
{
	class Asset
	{
	public:
		Asset() = default;
		virtual ~Asset() = default;

		const std::filesystem::path& GetPath() const { return m_Path; }

		virtual void Reload(const std::filesystem::path& path) = 0;

	protected:
		std::filesystem::path m_Path;
	};
}