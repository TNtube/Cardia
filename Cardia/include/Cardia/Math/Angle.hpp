#pragma once

#include <Cardia/Core/Concepts.hpp>

#include "Cardia/Serialization/Serializer.hpp"

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

		constexpr T Value() const noexcept { return m_Value; }
		constexpr T& Value() noexcept { return m_Value; }

		constexpr bool operator==(const Radian& other) const noexcept;
		constexpr bool operator!=(const Radian& other) const noexcept;

		constexpr Radian operator-() const noexcept;

		constexpr Radian& operator+=(const Radian& other) noexcept;
		constexpr Radian operator+(const Radian& other) const noexcept;
		constexpr Radian& operator-=(const Radian& other) noexcept;
		constexpr Radian operator-(const Radian& other) const noexcept;
		constexpr Radian& operator*=(const Radian& other) noexcept;
		constexpr Radian operator*(const Radian& other) const noexcept;
		constexpr Radian& operator/=(const Radian& other) noexcept;
		constexpr Radian operator/(const Radian& other) const noexcept;

		constexpr Radian& operator+=(const Degree<T>& other) noexcept;
		constexpr Radian operator+(const Degree<T>& other) const noexcept;
		constexpr Radian& operator-=(const Degree<T>& other) noexcept;
		constexpr Radian operator-(const Degree<T>& other) const noexcept;
		constexpr Radian& operator*=(const Degree<T>& other) noexcept;
		constexpr Radian operator*(const Degree<T>& other) const noexcept;
		constexpr Radian& operator/=(const Degree<T>& other) noexcept;
		constexpr Radian operator/(const Degree<T>& other) const noexcept;

		template<typename U>
		constexpr Radian& operator+=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian operator+(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian& operator-=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian operator-(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian& operator*=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian operator*(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian& operator/=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Radian operator/(U scalar) const noexcept requires std::convertible_to<U, T>;

		constexpr Degree<T> ToDegree() const noexcept;
		static constexpr Radian FromDegree(Degree<T> degree) noexcept;
		static constexpr Radian FromDegree(T degree) noexcept;

	private:
		T m_Value {};

	public:
		constexpr static auto properties = std::make_tuple(
			property(&Radian::m_Value, "Value")
		);
	};


	template <floating_point T>
	class Degree
	{
	public:
		Degree() = default;
		explicit constexpr Degree(T value) noexcept : m_Value(value) {}

		constexpr T& Value() noexcept { return m_Value; }
		constexpr T Value() const noexcept { return m_Value; }

		constexpr bool operator==(const Degree& other) const noexcept;
		constexpr bool operator!=(const Degree& other) const noexcept;

		constexpr Degree operator-() const noexcept;

		constexpr Degree& operator+=(const Degree& other) noexcept;
		constexpr Degree operator+(const Degree& other) const noexcept;
		constexpr Degree& operator-=(const Degree& other) noexcept;
		constexpr Degree operator-(const Degree& other) const noexcept;
		constexpr Degree& operator*=(const Degree& other) noexcept;
		constexpr Degree operator*(const Degree& other) const noexcept;
		constexpr Degree& operator/=(const Degree& other) noexcept;
		constexpr Degree operator/(const Degree& other) const noexcept;

		constexpr Degree& operator+=(const Radian<T>& other) noexcept;
		constexpr Degree operator+(const Radian<T>& other) const noexcept;
		constexpr Degree& operator-=(const Radian<T>& other) noexcept;
		constexpr Degree operator-(const Radian<T>& other) const noexcept;
		constexpr Degree& operator*=(const Radian<T>& other) noexcept;
		constexpr Degree operator*(const Radian<T>& other) const noexcept;
		constexpr Degree& operator/=(const Radian<T>& other) noexcept;
		constexpr Degree operator/(const Radian<T>& other) const noexcept;

		template<typename U>
		constexpr Degree& operator+=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree operator+(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree& operator-=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree operator-(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree& operator*=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree operator*(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree& operator/=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Degree operator/(U scalar) const noexcept requires std::convertible_to<U, T>;

		constexpr Radian<T> ToRadian() const noexcept;
		static constexpr Degree FromRadian(Radian<T> radian) noexcept;
		static constexpr Degree FromRadian(T radian) noexcept;

	private:
		T m_Value {};

	public:
		constexpr static auto properties = std::make_tuple(
			property(&Degree::m_Value, "Value")
		);
	};

	using Radianf = Radian<float>;
	using Degreef = Degree<float>;
}

#include "Angle.inl"
