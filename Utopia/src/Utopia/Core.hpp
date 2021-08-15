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
		if (!x)
			Utopia::Log::error("Assertion failed: {0}", message);
		assert(x);
	}
	template <typename T>
	constexpr void utCoreAssert(T x, const char* message)
	{
		if (!x)
			Utopia::Log::coreError("Assertion failed: {0}", message);
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

template <typename Enumeration, std::enable_if_t<std::is_enum_v<Enumeration>, bool> = true>
constexpr auto enum_as_integer(const Enumeration value) -> std::underlying_type_t<Enumeration>
{
	return static_cast<std::underlying_type_t<Enumeration>>(value);
}


#define UT_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
