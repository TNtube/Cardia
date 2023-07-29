#pragma once

#include <numbers>

#include "Cardia/Core/Core.hpp"

namespace Cardia
{
	template <floating_point T>
	constexpr bool Radian<T>::operator==(const Radian& other) const noexcept
	{
		return IsAlmostEqual(m_Value, other.m_Value);
	}

	template <floating_point T>
	constexpr bool Radian<T>::operator!=(const Radian& other) const noexcept
	{
		return !(*this == other);
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator+=(const Radian& other) noexcept
	{
		m_Value += other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator+(const Radian& other) const noexcept
	{
		Radian temp(*this);
		temp += other;
		return temp;
	}

	template <floating_point T>
	constexpr Radian<T> Radian<T>::operator-() const noexcept
	{
		return Radian(-m_Value);
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator-=(const Radian& other) noexcept
	{
		m_Value -= other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator-(const Radian& other) const noexcept
	{
		Radian temp(*this);
		temp -= other;
		return temp;
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator*=(const Radian& other) noexcept
	{
		m_Value *= other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator*(const Radian& other) const noexcept
	{
		Radian temp(*this);
		temp *= other;
		return temp;
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator/=(const Radian& other) noexcept
	{
		m_Value /= other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator/(const Radian& other) const noexcept
	{
		Radian temp(*this);
		temp /= other;
		return temp;
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator+=(const Degree<T>& other) noexcept
	{
		*this += other.ToRadian();
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator+(const Degree<T>& other) const noexcept
	{
		Radian temp(*this);
		temp += other;
		return temp;
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator-=(const Degree<T>& other) noexcept
	{
		*this -= other.ToRadian();
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator-(const Degree<T>& other) const noexcept
	{
		Radian temp(*this);
		temp -= other;
		return temp;
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator*=(const Degree<T>& other) noexcept
	{
		*this *= other.ToRadian();
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator*(const Degree<T>& other) const noexcept
	{
		Radian temp(*this);
		temp *= other;
		return temp;
	}

	template<floating_point T>
	constexpr Radian<T>& Radian<T>::operator/=(const Degree<T>& other) noexcept
	{
		*this /= other.ToRadian();
		return *this;
	}

	template<floating_point T>
	constexpr Radian<T> Radian<T>::operator/(const Degree<T>& other) const noexcept
	{
		Radian temp(*this);
		temp /= other;
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Radian<T>& Radian<T>::operator+=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value += static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Radian<T> Radian<T>::operator+(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Radian temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Radian<T>& Radian<T>::operator-=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value -= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Radian<T> Radian<T>::operator-(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Radian temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Radian<T>& Radian<T>::operator*=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value *= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Radian<T> Radian<T>::operator*(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Radian temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Radian<T>& Radian<T>::operator/=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value /= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Radian<T> Radian<T>::operator/(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Radian temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template <floating_point T>
	constexpr Degree<T> Radian<T>::ToDegree() const noexcept
	{
		return Degree<T>(m_Value * (static_cast<T>(180) / static_cast<T>(std::numbers::pi)));
	}

	template <floating_point T>
	constexpr Radian<T> Radian<T>::FromDegree(Degree<T> degree) noexcept
	{
		return Radian(degree.Value() * (static_cast<T>(std::numbers::pi) / static_cast<T>(180)));
	}

	template <floating_point T>
	constexpr Radian<T> Radian<T>::FromDegree(T degree) noexcept
	{
		return Radian(degree * (static_cast<T>(std::numbers::pi) / static_cast<T>(180)));
	}

	template <floating_point T>
	constexpr bool Degree<T>::operator==(const Degree& other) const noexcept
	{
		return IsAlmostEqual(m_Value, other.m_Value);
	}

	template <floating_point T>
	constexpr bool Degree<T>::operator!=(const Degree& other) const noexcept
	{
		return !(*this == other);
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator+=(const Degree& other) noexcept
	{
		m_Value += other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator+(const Degree& other) const noexcept
	{
		Degree temp(*this);
		temp += other;
		return temp;
	}

	template <floating_point T>
	constexpr Degree<T> Degree<T>::operator-() const noexcept
	{
		return Degree(-m_Value);
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator-=(const Degree& other) noexcept
	{
		m_Value -= other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator-(const Degree& other) const noexcept
	{
		Degree temp(*this);
		temp -= other;
		return temp;
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator*=(const Degree& other) noexcept
	{
		m_Value *= other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator*(const Degree& other) const noexcept
	{
		Degree temp(*this);
		temp *= other;
		return temp;
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator/=(const Degree& other) noexcept
	{
		m_Value /= other.m_Value;
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator/(const Degree& other) const noexcept
	{
		Degree temp(*this);
		temp /= other;
		return temp;
	}

	

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator+=(const Radian<T>& other) noexcept
	{
		*this += other.ToDegree();
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator+(const Radian<T>& other) const noexcept
	{
		Degree temp(*this);
		temp += other;
		return temp;
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator-=(const Radian<T>& other) noexcept
	{
		*this -= other.ToDegree();
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator-(const Radian<T>& other) const noexcept
	{
		Degree temp(*this);
		temp -= other;
		return temp;
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator*=(const Radian<T>& other) noexcept
	{
		*this *= other.ToDegree();
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator*(const Radian<T>& other) const noexcept
	{
		Degree temp(*this);
		temp *= other;
		return temp;
	}

	template<floating_point T>
	constexpr Degree<T>& Degree<T>::operator/=(const Radian<T>& other) noexcept
	{
		*this /= other.ToDegree();
		return *this;
	}

	template<floating_point T>
	constexpr Degree<T> Degree<T>::operator/(const Radian<T>& other) const noexcept
	{
		Degree temp(*this);
		temp /= other;
		return temp;
	}

	template<floating_point T>
	template<typename U>
	constexpr Degree<T>& Degree<T>::operator+=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value += static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Degree<T> Degree<T>::operator+(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Degree temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Degree<T>& Degree<T>::operator-=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value -= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Degree<T> Degree<T>::operator-(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Degree temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Degree<T>& Degree<T>::operator*=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value *= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Degree<T> Degree<T>::operator*(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Degree temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}
	
	template<floating_point T>
	template<typename U>
	constexpr Degree<T>& Degree<T>::operator/=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Value /= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Degree<T> Degree<T>::operator/(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Degree temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template <floating_point T>
	constexpr Radian<T> Degree<T>::ToRadian() const noexcept
	{
		return Radian<T>(m_Value * (static_cast<T>(std::numbers::pi) / static_cast<T>(180)));
	}

	template <floating_point T>
	constexpr Degree<T> Degree<T>::FromRadian(Radian<T> radian) noexcept
	{
		return Degree(radian.Value() * (static_cast<T>(180) / static_cast<T>(std::numbers::pi)));
	}

	template <floating_point T>
	constexpr Degree<T> Degree<T>::FromRadian(T radian) noexcept
	{
		return Degree(radian * (static_cast<T>(180) / static_cast<T>(std::numbers::pi)));
	}
}
