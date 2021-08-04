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

template <typename T>
constexpr T bit(T x)
{
	return 1 << x;
}
