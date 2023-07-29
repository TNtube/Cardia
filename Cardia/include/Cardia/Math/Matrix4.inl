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
	constexpr bool Matrix4<T>::operator==(const Matrix4& other) const noexcept
	{
		return values[0] == other[0] && values[1] == other[1] && values[2] == other[2] && values[3] == other[3];
	}

	template <floating_point T>
	constexpr bool Matrix4<T>::operator!=(const Matrix4& other) const noexcept
	{
		return !(*this == other);
	}

	template <floating_point T>
	constexpr Vector4<T>& Matrix4<T>::operator[](size_t index) noexcept
	{
		CdCoreAssert(index < this->Size());
		return this->values[index];
	}

	template <floating_point T>
	constexpr Vector4<T> Matrix4<T>::operator[](size_t index) const noexcept
	{
		CdCoreAssert(index < this->Size());
		return this->values[index];
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::operator*(const Matrix4& other) const noexcept
	{
		const Vector4<T> srcA0 = values[0];
		const Vector4<T> srcA1 = values[1];
		const Vector4<T> srcA2 = values[2];
		const Vector4<T> srcA3 = values[3];
		const Vector4<T> srcB0 = other[0];
		const Vector4<T> srcB1 = other[1];
		const Vector4<T> srcB2 = other[2];
		const Vector4<T> srcB3 = other[3];

		Matrix4 result;

		result[0] = srcA0 * srcB0[0] + srcA1 * srcB0[1] + srcA2 * srcB0[2] + srcA3 * srcB0[3];
		result[1] = srcA0 * srcB1[0] + srcA1 * srcB1[1] + srcA2 * srcB1[2] + srcA3 * srcB1[3];
		result[2] = srcA0 * srcB2[0] + srcA1 * srcB2[1] + srcA2 * srcB2[2] + srcA3 * srcB2[3];
		result[3] = srcA0 * srcB3[0] + srcA1 * srcB3[1] + srcA2 * srcB3[2] + srcA3 * srcB3[3];

		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::operator*(T scalar) const noexcept
	{
		return Matrix4f(
			values[0] * scalar,
			values[1] * scalar,
			values[2] * scalar,
			values[3] * scalar);
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Translate(const Vector3<T>& vec) const noexcept
	{
		Matrix4 result(*this);
		result[3] = values[0] * vec[0] + values[1] * vec[1] + values[2] * vec[2] + values[3];
		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Rotate(Radian<T> angle, const Vector3<T>& vec) const noexcept
	{
		
		T const a = angle.Value();
		T const c = cos(a);
		T const s = sin(a);

		Vector3<T> axis(vec.Normalize());
		Vector3<T> temp(axis * (static_cast<T>(1) - c));

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
	constexpr Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& vec) const noexcept
	{
		Matrix4 result;
		result[0] = values[0] * vec[0];
		result[1] = values[1] * vec[1];
		result[2] = values[2] * vec[2];
		result[3] = values[3];
		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Inverse() const noexcept
	{
		T coef00 = values[2][2] * values[3][3] - values[3][2] * values[2][3];
		T coef02 = values[1][2] * values[3][3] - values[3][2] * values[1][3];
		T coef03 = values[1][2] * values[2][3] - values[2][2] * values[1][3];

		T coef04 = values[2][1] * values[3][3] - values[3][1] * values[2][3];
		T coef06 = values[1][1] * values[3][3] - values[3][1] * values[1][3];
		T coef07 = values[1][1] * values[2][3] - values[2][1] * values[1][3];

		T coef08 = values[2][1] * values[3][2] - values[3][1] * values[2][2];
		T coef10 = values[1][1] * values[3][2] - values[3][1] * values[1][2];
		T coef11 = values[1][1] * values[2][2] - values[2][1] * values[1][2];

		T coef12 = values[2][0] * values[3][3] - values[3][0] * values[2][3];
		T coef14 = values[1][0] * values[3][3] - values[3][0] * values[1][3];
		T coef15 = values[1][0] * values[2][3] - values[2][0] * values[1][3];

		T coef16 = values[2][0] * values[3][2] - values[3][0] * values[2][2];
		T coef18 = values[1][0] * values[3][2] - values[3][0] * values[1][2];
		T coef19 = values[1][0] * values[2][2] - values[2][0] * values[1][2];

		T coef20 = values[2][0] * values[3][1] - values[3][0] * values[2][1];
		T coef22 = values[1][0] * values[3][1] - values[3][0] * values[1][1];
		T coef23 = values[1][0] * values[2][1] - values[2][0] * values[1][1];

		Vector4<T> fac0(coef00, coef00, coef02, coef03);
		Vector4<T> fac1(coef04, coef04, coef06, coef07);
		Vector4<T> fac2(coef08, coef08, coef10, coef11);
		Vector4<T> fac3(coef12, coef12, coef14, coef15);
		Vector4<T> fac4(coef16, coef16, coef18, coef19);
		Vector4<T> fac5(coef20, coef20, coef22, coef23);

		Vector4<T> vec0(values[1][0], values[0][0], values[0][0], values[0][0]);
		Vector4<T> vec1(values[1][1], values[0][1], values[0][1], values[0][1]);
		Vector4<T> vec2(values[1][2], values[0][2], values[0][2], values[0][2]);
		Vector4<T> vec3(values[1][3], values[0][3], values[0][3], values[0][3]);

		Vector4<T> inv0(vec1 * fac0 - vec2 * fac1 + vec3 * fac2);
		Vector4<T> inv1(vec0 * fac0 - vec2 * fac3 + vec3 * fac4);
		Vector4<T> inv2(vec0 * fac1 - vec1 * fac3 + vec3 * fac5);
		Vector4<T> inv3(vec0 * fac2 - vec1 * fac4 + vec2 * fac5);

		Vector4<T> signA(+1, -1, +1, -1);
		Vector4<T> signB(-1, +1, -1, +1);
		Matrix4 inverse(inv0 * signA, inv1 * signB, inv2 * signA, inv3 * signB);

		Vector4<T> row0(inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0]);

		Vector4<T> dot0(values[0] * row0);
		T dot1 = (dot0.x + dot0.y) + (dot0.z + dot0.w);

		T dneOverDeterminant = static_cast<T>(1) / dot1;

		return inverse * dneOverDeterminant;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Transpose() const noexcept
	{
		Matrix4 result;

		result[0][0] = values[0][0];
		result[0][1] = values[1][0];
		result[0][2] = values[2][0];
		result[0][3] = values[3][0];

		result[1][0] = values[0][1];
		result[1][1] = values[1][1];
		result[1][2] = values[2][1];
		result[1][3] = values[3][1];

		result[2][0] = values[0][2];
		result[2][1] = values[1][2];
		result[2][2] = values[2][2];
		result[2][3] = values[3][2];

		result[3][0] = values[0][3];
		result[3][1] = values[1][3];
		result[3][2] = values[2][3];
		result[3][3] = values[3][3];

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

		const T tanHalfFovY = tan(fovY.Value() / static_cast<T>(2));

		Matrix4 result(static_cast<T>(0));

		result[0][0] = static_cast<T>(1) / (aspectRatio * tanHalfFovY);
		result[1][1] = static_cast<T>(1) / tanHalfFovY;
		result[2][2] = zFar / (zNear - zFar);
		result[2][3] = - static_cast<T>(1);
		result[3][2] = -(zFar * zNear) / (zFar - zNear);

		return result;
	}

	template <floating_point T>
	constexpr Matrix4<T> Matrix4<T>::Orthographic(T left, T right, T bottom, T top, T zNear, T zFar) noexcept
	{
		Matrix4 result(1);

		result[0][0] = static_cast<T>(2) / (right - left);
		result[1][1] = static_cast<T>(2) / (top - bottom);
		result[2][2] = - static_cast<T>(1) / (zFar - zNear);
		result[3][0] = - (right + left) / (right - left);
		result[3][1] = - (top + bottom) / (top - bottom);
		result[3][2] = - zNear / (zFar - zNear);

		return result;
	}

}
