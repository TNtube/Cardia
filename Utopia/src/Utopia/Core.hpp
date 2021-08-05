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
#include <cassert>
#include "Utopia/Log.hpp"



#ifdef UT_DEBUG
	template <typename T>
	constexpr void utAssert(T x, const char* message) 
	{
		if (!(x))
			UT_ERROR("Assertion failed: {0}", message);
		assert(x);
	}
	template <typename T>
	constexpr void utCoreAssert(T x, const char* message)
	{
		if (!(x))
			UT_CORE_ERROR("Assertion failed: {0}", message);
		assert(x);
	}
#else
	template <typename T>
	constexpr void utAssert(T x, const char* message) {}
	template <typename T>
	constexpr void utCoreAssert(T x, const char* message) {};
#endif

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
