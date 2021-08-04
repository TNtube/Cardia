#pragma once

#ifdef UT_PLATFORM_WINDOWS
	#ifdef UT_BUILD_DLL
		#define UTOPIA_API __declspec(dllexport)
	#else
		#define UTOPIA_API __declspec(dllimport)
	#endif
#else
	#error Utopia is only for Windows !
#endif

#include <type_traits>

template <typename T>
constexpr T bit(T x)
{
	return 1 << x;
}

template <typename Enumeration, std::enable_if_t<std::is_enum<Enumeration>::value, bool> = true>
constexpr auto enum_as_integer(const Enumeration value) -> typename std::underlying_type<Enumeration>::type
{
	return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}
