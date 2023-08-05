#pragma once

#include "Matrix4.hpp"
#include "Cardia/Core/Concepts.hpp"
#include "Vector3.hpp"


namespace Cardia
{
	template <floating_point T>
	class Quaternion
	{
	public:
		Quaternion() = default;
		constexpr Quaternion(T w, T x, T y, T z) noexcept
			: m_Real(w), m_Imaginary(x, y, z) {}
		explicit constexpr Quaternion(T real, Vector3<T> imaginary) noexcept
			: m_Real(real), m_Imaginary(imaginary) {}
		explicit constexpr Quaternion(Vector3<T> euler) noexcept;
		constexpr Quaternion(Vector3<T> u, Vector3<T> v) noexcept;

		constexpr bool operator==(const Quaternion& other) const noexcept;
		constexpr bool operator!=(const Quaternion& other) const noexcept;

		constexpr T x() const noexcept { return m_Imaginary.x; }
		constexpr T y() const noexcept { return m_Imaginary.y; }
		constexpr T z() const noexcept { return m_Imaginary.z; }
		constexpr T w() const noexcept { return m_Real; }

		constexpr T& x() noexcept { return m_Imaginary.x; }
		constexpr T& y() noexcept { return m_Imaginary.y; }
		constexpr T& z() noexcept { return m_Imaginary.z; }
		constexpr T& w() noexcept { return m_Real; }

		constexpr Quaternion operator-() const noexcept;

		constexpr Quaternion& operator+=(const Quaternion& other) noexcept;
		constexpr Quaternion operator+(const Quaternion& other) const noexcept;
		constexpr Quaternion& operator-=(const Quaternion& other) noexcept;
		constexpr Quaternion operator-(const Quaternion& other) const noexcept;
		constexpr Quaternion& operator*=(const Quaternion& other) noexcept;
		constexpr Quaternion operator*(const Quaternion& other) const noexcept;

		constexpr Vector3<T> operator*(const Vector3<T>& other) const noexcept;

		template<typename U>
		constexpr Quaternion& operator+=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion operator+(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion& operator-=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion operator-(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion& operator*=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion operator*(U scalar) const noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion& operator/=(U scalar) noexcept requires std::convertible_to<U, T>;
		template<typename U>
		constexpr Quaternion operator/(U scalar) const noexcept requires std::convertible_to<U, T>;

		constexpr T Length() const noexcept;
		constexpr T Dot(const Quaternion& other) const noexcept;
		constexpr Quaternion Conjugate() const noexcept;
		constexpr Quaternion Inverse() const noexcept;
		constexpr Quaternion Normalize() const noexcept;

		constexpr Matrix4<T> ToMatrix() const noexcept;
	private:
		T m_Real;
		Vector3<T> m_Imaginary;
		
	};

	using Quatf = Quaternion<float>;
	using Quatd = Quaternion<double>;
}

#include "Quaternion.inl"
