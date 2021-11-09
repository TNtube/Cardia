#pragma once

#if !defined(_WIN64) && !defined(__linux__)
	#error Cardia is only for Windows x64 or Linux !
#endif

#include <type_traits>
#include <cassert>
#include "Log.hpp"



#ifdef CD_DEBUG
	template <typename T,  typename... Args>
	constexpr void cdAssert(T x, Args&&... args)
	{
		if (!x)
			Cardia::Log::error("Assertion failed: {0}", std::forward<Args>(args)...);
		assert(x);
	}
	template <typename T, typename... Args>
	constexpr void cdCoreAssert(T x, Args&&... args)
	{
		if (!x)
			Cardia::Log::coreError("Assertion failed: {0}", std::forward<Args>(args)...);
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


#define CD_BIND_EVENT_FN(x) [this](auto && PH1) { return (x)(PH1); }
