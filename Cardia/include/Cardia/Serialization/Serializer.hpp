#pragma once

#include "Serializable.hpp"
#include <filesystem>
#include <json/json.h>

namespace Cardia
{
	template <Serializable T>
	class Serializer
	{
	public:
		explicit Serializer(T& serializable)
			: m_Serializable(serializable) {}

		void Serialize(const std::filesystem::path& path)
		{
			const Json::Value root = m_Serializable.Serialize();
			std::ofstream file(path);
			file << root;
			file.close();
		}

		static std::optional<T> Deserialize(const std::filesystem::path& path)
		{
			std::ifstream file(path);
			if (!file.is_open())
				return std::nullopt;

			Json::Value root;
			file >> root;
			file.close();

			return T::Deserialize(root);
		}

	private:
		T& m_Serializable;
	};
}
