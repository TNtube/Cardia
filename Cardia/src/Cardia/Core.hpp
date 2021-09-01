#pragma once

#ifndef CD_PLATFORM_WINDOWS
	#error Cardia is only for Windows !
#endif

#include <type_traits>
#include <cassert>
#include "Cardia/Log.hpp"



#ifdef CD_DEBUG
	template <typename T>
	constexpr void cdAssert(T x, const char* message)
	{
		if (!x)
			Cardia::Log::error("Assertion failed: {0}", message);
		assert(x);
	}
	template <typename T>
	constexpr void cdCoreAssert(T x, const char* message)
	{
		if (!x)
			Cardia::Log::coreError("Assertion failed: {0}", message);
		assert(x);
	}
#else
	template <typename T>
	constexpr void cdAssert(T x, const char* message) {}
	template <typename T>
	constexpr void cdCoreAssert(T x, const char* message) {};
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


#define CD_BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
