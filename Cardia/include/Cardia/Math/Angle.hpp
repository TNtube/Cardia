#pragma once

#include <Cardia/Core/Concepts.hpp>

namespace Cardia
{

	template <floating_point T>
	class Degree;
	
	template <floating_point T>
	class Radian
	{
	public:
		Radian() = default;
		explicit constexpr Radian(T value) noexcept : m_Value(value) {}

		constexpr T& Value() noexcept { return m_Value; }

		constexpr Degree<T> ToDegree() const noexcept;
		static constexpr Radian FromDegree(const Degree<T>& degree) noexcept;

	private:
		T m_Value {};
	};


	template <floating_point T>
	class Degree
	{
	public:
		Degree() = default;
		explicit constexpr Degree(T value) noexcept : m_Value(value) {}

		constexpr T& Value() noexcept { return m_Value; }

		constexpr Radian<T> ToRadian() const noexcept;
		static constexpr Degree FromRadian(const Radian<T>& radian) noexcept;

	private:
		T m_Value {};
	};
}

#include "Angle.inl"
