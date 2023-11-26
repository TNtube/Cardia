#pragma once

#include <fstream>
#include <json/json.h>
#include <filesystem>

#include "Cardia/Math/Vector4.hpp"

namespace Cardia
{
	template <typename T, T... S, typename F>
	constexpr void for_sequence(std::integer_sequence<T, S...>, F&& f) {
		(static_cast<void>(f(std::integral_constant<T, S>{})), ...);
	}

	template <typename T>
	constexpr bool has_properties()
	{
		if constexpr (requires (T t) {t.properties;})
			return true;
		else
			return false;
	}

	namespace Serialization
	{
		template<typename T>
		Json::Value ToJson(const T& object)
		{
			Json::Value data;

			if constexpr (!has_properties<T>()) {
				if constexpr (std::is_enum_v<T>)
					data = static_cast<std::underlying_type_t<T>>(object);
				else
					data = object;
				return data;
			} else {
				constexpr auto nbProperties = std::tuple_size_v<decltype(T::properties)>;
				for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i){
					constexpr auto property = std::get<i>(T::properties);

					using Type = typename decltype(property)::Type;
					data[property.name] = ToJson<Type>(object.*(property.member));
				});
			}

			return data;
		}

		template<typename T>
		T FromJson(const Json::Value& data)
		{
			if constexpr (!has_properties<T>()) {
				return data.as<T>();
			} else {
				T object;
				constexpr auto nbProperties = std::tuple_size_v<decltype(T::properties)>;
				for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i){
					constexpr auto property = std::get<i>(T::properties);

					using Type = typename decltype(property)::Type;

					if constexpr (std::is_enum_v<Type>)
						object.*(property.member) = static_cast<Type>(FromJson<std::underlying_type_t<Type>>(data[property.name]));
					else
						object.*(property.member) = FromJson<Type>(data[property.name]);
				});

				return object;
			}
		}
	}

	template <typename T>
	class Serializer
	{
	public:
		explicit Serializer(T& serializable)
			: m_Serializable(serializable) {}

		void Serialize(const std::filesystem::path& path)
		{
			const Json::Value root = Serialization::ToJson(m_Serializable);
			std::ofstream file(path);
			file << root;
			file.close();
		}

		static T Deserialize(const std::filesystem::path& path)
		{
			std::ifstream file(path);
			if (!file.is_open())
				throw std::exception(("No file at " + path.string()).c_str());

			Json::Value root;
			file >> root;
			file.close();

			return Serialization::FromJson<T>(root);
		}

	private:
		T& m_Serializable;
	};


}
