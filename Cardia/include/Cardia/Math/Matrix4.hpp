#pragma once

#include <glm/fwd.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "Angle.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	// Matrix4 is left handed, zero to one depth range and Y up
	template<floating_point T>
	struct Matrix4
	{
		Vector4<T> values[4];

		Matrix4() = default;

		explicit constexpr Matrix4(Vector4<T> col1, Vector4<T> col2, Vector4<T> col3, Vector4<T> col4) noexcept
			: values{ col1, col2, col3, col4 } {}
		explicit constexpr Matrix4(T diagonal) noexcept;

		static constexpr size_t Size() noexcept { return 4; }
		constexpr Vector4<T> operator[](size_t index) const noexcept;
		constexpr Vector4<T>& operator[](size_t index) noexcept;

		constexpr Matrix4 operator*(const Matrix4& other) const noexcept;
		constexpr Matrix4 operator*(T scalar) const noexcept;


		constexpr Matrix4 Translate(const Vector3<T>& vec) const noexcept;
		constexpr Matrix4 Rotate(Radian<T> angle, const Vector3<T>& vec) const noexcept;
		constexpr Matrix4 Scale(const Vector3<T>& vec) const noexcept;

		constexpr Matrix4 Inverse() const noexcept;
		constexpr Matrix4 Transpose() const noexcept;

		constexpr T* Data() noexcept { return &values[0].x; }
		constexpr T* Data() const noexcept { return const_cast<T*>(&values[0].x); }

		static constexpr Matrix4 Identity() noexcept;
		static constexpr Matrix4 Perspective(Radian<T> fovY, T aspectRatio, T zNear, T zFar) noexcept;
		static constexpr Matrix4 Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept;
	};
	
	using Matrix4f = Matrix4<float>;
	using Matrix4d = Matrix4<double>;
}

#include "Matrix4.inl"
