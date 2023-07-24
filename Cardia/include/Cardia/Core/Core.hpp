#pragma once
#if !defined(_WIN64) && !defined(__linux__)
	#error Cardia is only for Windows x64 or Linux !
#endif

#include <source_location>
#include <type_traits>
#include <cassert>
#include "Log.hpp"
#include <uuid.h>


#if defined(__cpp_consteval) && !defined(NDEBUG) // Trick because CLion can't find std::source_location
	template <typename T>
	constexpr void CdAssert(T x, const std::string& message = "invalid", const std::source_location location = std::source_location::current())
	{
		if (!x)
			Cardia::Log::CoreError("{0} : ({1}:{2}) Assertion failed: {3}",
				location.file_name(), location.line(), location.column(), message);
		assert(x);
	}
	template <typename T>
	constexpr void CdCoreAssert(T x, const std::string& message = "invalid", const std::source_location location = std::source_location::current())
	{
		if (!x)
			Cardia::Log::CoreError("{0} : ({1}:{2}) Assertion failed: {3}",
				location.file_name(), location.line(), location.column(), message);
		assert(x);
	}
#else
	template <typename T>
	constexpr void CdAssert(T x, const char* message) {}
	template <typename T>
	constexpr void CdCoreAssert(T x, const char* message) {};
#endif

template <typename T>
constexpr T Bit(T x)
{
	return 1 << x;
}

template <typename Enumeration, std::enable_if_t<std::is_enum_v<Enumeration>, bool> = true>
constexpr auto enum_as_integer(const Enumeration value) -> std::underlying_type_t<Enumeration>
{
	return static_cast<std::underlying_type_t<Enumeration>>(value);
}


#define CD_BIND_EVENT_FN(x) [this](auto && PH1) { return (x)(PH1); }
