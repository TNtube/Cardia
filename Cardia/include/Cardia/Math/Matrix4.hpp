#pragma once

#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template<arithmetic T>
	struct Matrix4
	{
		Vector4<T> values[4];

		static constexpr size_t length() { return 4; }
		Vector4<T>& operator[](size_t index);


		Matrix4 Translate(const Vector3<T>& vec);
		
		Matrix4 Scale(const Vector3<T>& vec);
	};
}

#include "Matrix4.inl"
