#pragma once

#include <glm/gtc/quaternion.hpp>

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
		constexpr Quaternion(T x, T y, T z, T w) noexcept
			: m_Real(w), m_Imaginary(x, y, z) {}

		explicit constexpr Quaternion(T real, Vector3<T> imaginary) noexcept
			: m_Real(real), m_Imaginary(imaginary) {}

		explicit constexpr Quaternion(Vector3<T> euler) noexcept;

		constexpr Quaternion(Vector3<T> u, Vector3<T> v) noexcept;

		constexpr T Length() const noexcept;
		constexpr T Dot(const Quaternion& other) const noexcept;
		constexpr Quaternion Normalize() noexcept;

		constexpr Matrix4<T> ToMatrix() const noexcept;
	private:
		T m_Real;
		Vector3<T> m_Imaginary;
		
	};
}

#include "Quaternion.inl"
