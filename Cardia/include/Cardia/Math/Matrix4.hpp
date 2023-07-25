#pragma once

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

		explicit constexpr Matrix4(T diagonal) noexcept;

		static constexpr size_t size() noexcept { return 4; }
		constexpr Vector4<T>& operator[](size_t index) noexcept;


		constexpr Matrix4 Translate(const Vector3<T>& vec) noexcept;
		constexpr Matrix4 Rotate(Radian<T> angle, const Vector3<T>& vec) noexcept;
		constexpr Matrix4 Scale(const Vector3<T>& vec) noexcept;

		static constexpr Matrix4 Identity() noexcept;
		static constexpr Matrix4 Perspective(Radian<T> fovY, T aspectRatio, T zNear, T zFar) noexcept;
		static constexpr Matrix4 Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept;
	};
}

#include "Matrix4.inl"
