#pragma once

#include <numbers>

namespace Cardia
{
	template <floating_point T>
	constexpr Degree<T> Radian<T>::ToDegree() const noexcept
	{
		return Degree<T>(m_Value * (static_cast<T>(180) / static_cast<T>(std::numbers::pi)));
	}

	template <floating_point T>
	constexpr Radian<T> Radian<T>::FromDegree(const Degree<T>& degree) noexcept
	{
		return Radian(degree.Value() * (static_cast<T>(std::numbers::pi) / static_cast<T>(180)));
	}

	template <floating_point T>
	constexpr Radian<T> Degree<T>::ToRadian() const noexcept
	{
		return Radian<T>(m_Value * (static_cast<T>(std::numbers::pi) / static_cast<T>(180)));
	}

	template <floating_point T>
	constexpr Degree<T> Degree<T>::FromRadian(const Radian<T>& radian) noexcept
	{
		return Degree(radian.Value() * (static_cast<T>(180) / static_cast<T>(std::numbers::pi)));
	}
}
