#pragma once

namespace Cardia
{
	template <typename T> concept arithmetic = std::is_arithmetic_v<T>;
	template <typename T> concept floating_point = std::is_floating_point_v<T>;

	template <typename T, typename = void>
	struct is_map : std::false_type {};

	template <typename T>
	struct is_map<T, std::enable_if_t<
						std::is_same_v<typename T::value_type,
									std::pair<const typename T::key_type,
											  typename T::mapped_type>
						>>
		> : std::true_type {};

	template <typename T>
	constexpr bool is_map_v = is_map<T>::value;

	template<typename T>
	struct is_std_array : std::false_type {};

	template<typename T, std::size_t N>
	struct is_std_array<std::array<T, N>> : std::true_type {};

	template<typename T>
	constexpr bool is_std_array_v = is_std_array<T>::value;
}
