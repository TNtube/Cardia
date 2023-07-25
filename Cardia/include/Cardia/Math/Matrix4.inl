#pragma once

#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template <floating_point T>
	constexpr Matrix4<T>::Matrix4(T diagonal) noexcept
		: values {
			Vector4<T>(diagonal, 0, 0, 0),
			Vector4<T>(0, diagonal, 0, 0),
			Vector4<T>(0, 0, diagonal, 0),
			Vector4<T>(0, 0, 0, diagonal)
		} {}
	
	template <floating_point T>
	Vector4<T>& Matrix4<T>::operator[](size_t index)
	{
		CdCoreAssert(index < this->size());
		return this->values[index];
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Translate(const Vector3<T>& vec) noexcept
	{
		Matrix4 result(*this);
		result[3] = values[0] * vec[0] + values[1] * vec[1] + values[2] * vec[2] + values[3];
		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Rotate(Radian<T> angle, const Vector3<T>& vec) noexcept
	{
		
		T const a = angle.Value();
		T const c = cos(a);
		T const s = sin(a);

		Vector3<T> axis(vec.Normalize());
		Vector3<T> temp((static_cast<T>(1) - c) * axis);

		Matrix4 rotate;
		rotate[0][0] = c + temp[0] * axis[0];
		rotate[0][1] = temp[0] * axis[1] + s * axis[2];
		rotate[0][2] = temp[0] * axis[2] - s * axis[1];

		rotate[1][0] = temp[1] * axis[0] - s * axis[2];
		rotate[1][1] = c + temp[1] * axis[1];
		rotate[1][2] = temp[1] * axis[2] + s * axis[0];

		rotate[2][0] = temp[2] * axis[0] + s * axis[1];
		rotate[2][1] = temp[2] * axis[1] - s * axis[0];
		rotate[2][2] = c + temp[2] * axis[2];

		Matrix4 result;

		result[0] = values[0] * rotate[0][0] + values[1] * rotate[0][1] + values[2] * rotate[0][2];
		result[1] = values[0] * rotate[1][0] + values[1] * rotate[1][1] + values[2] * rotate[1][2];
		result[2] = values[0] * rotate[2][0] + values[1] * rotate[2][1] + values[2] * rotate[2][2];
		result[3] = values[3];

		return result;
	}


	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& vec) noexcept
	{
		Matrix4 result;
		result[0] = values[0] * vec[0];
		result[1] = values[1] * vec[1];
		result[2] = values[2] * vec[2];
		result[3] = values[3];
		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Identity() noexcept
	{
		return Matrix4(static_cast<T>(1));
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Perspective(Radian<T> fovY, T aspectRatio, T zNear, T zFar) noexcept
	{
		assert(abs(aspectRatio - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));

		T const tanHalfFovY = tan(fovY / static_cast<T>(2));

		Matrix4 result(static_cast<T>(0));

		result[0][0] = static_cast<T>(1) / (aspectRatio * tanHalfFovY);
		result[1][1] = static_cast<T>(1) / tanHalfFovY;
		result[2][2] = zFar / (zFar - zNear);
		result[2][3] = static_cast<T>(1);
		result[3][2] = -(zFar * zNear) / (zFar - zNear);

		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		Matrix4 result(1);

		glm::mat4(1.0f);

		result[0][0] = static_cast<T>(2) / (right - left);
		result[1][1] = static_cast<T>(2) / (top - bottom);
		result[2][2] = static_cast<T>(1) / (zFar - zNear);
		result[3][0] = - (right + left) / (right - left);
		result[3][1] = - (top + bottom) / (top - bottom);
		result[3][2] = - zNear / (zFar - zNear);

		return result;
	}

}
