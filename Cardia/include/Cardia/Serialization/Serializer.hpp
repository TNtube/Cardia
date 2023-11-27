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
		template <typename T> T convert_to (const std::string &str)
		{
			std::istringstream ss(str);
			T num;
			ss >> num;
			return num;
		}

		template<typename T>
		Json::Value ToJson(const T& object)
		{
			Json::Value data;

			if constexpr (is_map_v<T>)
			{
				data = Json::Value(Json::objectValue);
				for (const auto& [key, value] : object)
					data[std::to_string(key)] = ToJson(value);
				return data;
			}
			else if constexpr (is_std_array_v<T>)
			{
				data = Json::Value(Json::arrayValue);
				for (std::size_t i = 0; i < object.size(); ++i)
					data.append(ToJson(object[i]));
				return data;
			}
			else if constexpr (std::input_or_output_iterator<T>)
			{
				data = Json::Value(Json::arrayValue);
				for (const auto& value : object)
					data.append(ToJson(value));
				return data;
			}
			else if constexpr (!has_properties<T>())
			{
				if constexpr (std::is_enum_v<T>)
					data = static_cast<std::underlying_type_t<T>>(object);
				else
					data = object;
				return data;
			}
			else
			{
				constexpr auto nbProperties = std::tuple_size_v<decltype(T::properties)>;
				for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i){
					constexpr auto property = std::get<i>(T::properties);

					using Type = typename decltype(property)::Type;
					data[property.name] =
						property.isGetSet
						? ToJson<Type>((object.*(property.getter))())
						: ToJson<Type>(object.*(property.member));
				});

				return data;
			}
		}

		template<typename T>
		T FromJson(const Json::Value& data)
		{
			if constexpr (is_map_v<T>)
			{
				T object;
				for (const auto& key : data.getMemberNames())
					object[convert_to<typename T::key_type>(key)] = FromJson<typename T::mapped_type>(data[key]);
				return object;
			}
			else if constexpr (is_std_array_v<T>)
			{
				T object;
				for (std::size_t i = 0; i < object.size(); ++i)
					object[i] = FromJson<typename T::value_type>(data.get(i, Json::Value::null));
				return object;
			}
			else if constexpr (std::input_or_output_iterator<T>)
			{
				T object;
				for (const auto& value : data)
					object.push_back(FromJson<typename T::value_type>(value));
				return object;
			}
			else if constexpr (std::is_integral_v<T>)
			{
				return static_cast<T>(data.as<std::int64_t>());
			}
			else if constexpr (!has_properties<T>()) {
				if constexpr (std::is_enum_v<T>)
					return static_cast<T>(data.as<std::underlying_type_t<T>>());
				else
					return data.as<T>();
			}
			else
			{
				T object;

				constexpr auto nbProperties = std::tuple_size_v<decltype(T::properties)>;
				for_sequence(std::make_index_sequence<nbProperties>{}, [&](auto i){
					constexpr auto property = std::get<i>(T::properties);

					using Type = typename decltype(property)::Type;

					Type member;
					if constexpr (std::is_enum_v<Type>)
						member = static_cast<Type>(FromJson<std::underlying_type_t<Type>>(data[property.name]));
					else
						member = std::move(FromJson<Type>(data[property.name]));

					if constexpr (property.isGetSet)
						(object.*(property.setter))(std::move(member));
					else
						object.*(property.member) = std::move(member);
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
