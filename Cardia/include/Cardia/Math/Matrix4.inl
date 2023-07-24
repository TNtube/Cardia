#pragma once

#include <glm/mat4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <json/value.h>
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template <arithmetic T>
	Vector4<T>& Matrix4<T>::operator[](size_t index)
	{
		CdCoreAssert(index < this->length());
		return this->values[index];
	}

	template <arithmetic T>
	Matrix4<T> Matrix4<T>::Translate(const Vector3<T>& vec)
	{
		Matrix4 result(*this);
		result[3] = values[0] * vec[0] + values[1] * vec[1] + values[2] * vec[2] + values[3];
		return result;
	}

	template <arithmetic T>
	Matrix4<T> Matrix4<T>::Scale(const Vector3<T>& vec)
	{
		Matrix4 result;
		result[0] = values[0] * vec[0];
		result[1] = values[1] * vec[1];
		result[2] = values[2] * vec[2];
		result[3] = values[3];
		return result;
	}

}
