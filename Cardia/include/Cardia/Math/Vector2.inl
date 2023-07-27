#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <json/value.h>

#include "Vector3.hpp"
#include "Cardia/Core/Concepts.hpp"

namespace Cardia
{
	template<arithmetic T>
	constexpr Vector2<T>::Vector2(const Vector3<T>& other) noexcept
		: x(other.x), y(other.y) {}
	
	template<arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator=(const Vector3<T>& other) noexcept
	{
		x = other.x;
		y = other.y;
		return *this;
	}
	
	template<arithmetic T>
	constexpr Vector2<T>::Vector2(const Vector4<T>& other) noexcept
		: x(other.x), y(other.y) {}
	
	template<arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator=(const Vector4<T>& other) noexcept
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	template<arithmetic T>
	constexpr T& Vector2<T>::operator[](size_t index) noexcept
	{
		CdCoreAssert(index < this->Size());
		switch(index)
		{
		default:
		case 0:
			return x;
		case 1:
			return y;
		}
	}

	template<arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator+=(const Vector2& other) noexcept
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator+(const Vector2& other) const noexcept
	{
		Vector2 temp(*this);
		temp += other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T>& Vector2<T>::operator+=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x += static_cast<T>(scalar);
		y += static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T> Vector2<T>::operator+(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}

	template <arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator-() const noexcept
	{
		return Vector2(-x, -y);
	}

	template<arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator-=(const Vector2<T>& other) noexcept
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const noexcept
	{
		Vector2 temp(*this);
		temp -= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T>& Vector2<T>::operator-=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x -= static_cast<T>(scalar);
		y -= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T> Vector2<T>::operator-(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator*=(const Vector2<T>& other) noexcept
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator*(const Vector2<T>& other) const noexcept
	{
		Vector2 temp(*this);
		temp *= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T>& Vector2<T>::operator*=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x *= static_cast<T>(scalar);
		y *= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T> Vector2<T>::operator*(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr Vector2<T>& Vector2<T>::operator/=(const Vector2<T>& other) noexcept
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	template<arithmetic T>
	constexpr Vector2<T> Vector2<T>::operator/(const Vector2<T>& other) const noexcept
	{
		Vector2 temp(*this);
		temp /= other;
		return temp;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T>& Vector2<T>::operator/=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		x /= static_cast<T>(scalar);
		y /= static_cast<T>(scalar);
		return *this;
	}

	template<arithmetic T>
	template<typename U>
	constexpr Vector2<T> Vector2<T>::operator/(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Vector2 temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template<arithmetic T>
	constexpr T Vector2<T>::Length() noexcept
	{
		return sqrt(this->Dot(*this));
	}

	template<arithmetic T>
	constexpr T Vector2<T>::Dot(const Vector2& other) noexcept
	{
		Vector2 temp(*this * other);
		return temp.x + temp.y;
	}

	template<arithmetic T>
	constexpr Vector2<T> Vector2<T>::Lerp(const Vector2& other, float t) noexcept
	{
		return *this + (other - *this) * t;
	}

	template<arithmetic T>
	constexpr Vector2<T> Vector2<T>::Normalize() noexcept
	{
		Vector2 temp(*this);

		T len = static_cast<T>(std::sqrt(x * x + y * y));

		if constexpr (len > 0)
		{
			temp.x /= len;
			temp.y /= len;
		}

		return temp;
	}

	template<arithmetic T>
	Json::Value Vector2<T>::Serialize() const
	{
		Json::Value root;
		root["x"] = x;
		root["y"] = y;
		return std::move(root);
	}

	template<arithmetic T>
	bool Vector2<T>::Deserialize(const Json::Value& root, Vector2& other)
	{
		if (!root.isMember("x") || !root.isMember("y"))
			return false;

		other.x = root["x"].asFloat();
		other.y = root["y"].asFloat();

		return true;
	}
}
