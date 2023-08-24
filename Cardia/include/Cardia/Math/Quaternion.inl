#pragma once

#include "Cardia/Core/Concepts.hpp"
#include "Vector3.hpp"


namespace Cardia
{
	template <floating_point T>
	constexpr Quaternion<T>::Quaternion(Vector3<T> euler) noexcept
	{
		auto halfEuler = euler * static_cast<T>(0.5);

		Vector3<T> c { cos(halfEuler.x), cos(halfEuler.y), cos(halfEuler.z) };
		Vector3<T> s { sin(halfEuler.x), sin(halfEuler.y), sin(halfEuler.z) };

		this->m_Real = c.x * c.y * c.z + s.x * s.y * s.z;
		this->m_Imaginary = {
			s.x * c.y * c.z - c.x * s.y * s.z,
			c.x * s.y * c.z + s.x * c.y * s.z,
			c.x * c.y * s.z - s.x * s.y * c.z
		};
	}

	// http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
	template <floating_point T>
	constexpr Quaternion<T>::Quaternion(Vector3<T> u, Vector3<T> v) noexcept
	{
		T norm_u_norm_v = sqrt(u.Dot(u) * v.Dot(v));
		T real_part = norm_u_norm_v + u.Dot(v);
		Vector3<T> t;

		if(real_part < static_cast<T>(1.e-6f) * norm_u_norm_v)
		{
			// If u and v are exactly opposite, rotate 180 degrees
			// around an arbitrary orthogonal axis. Axis normalisation
			// can happen later, when we normalise the quaternion.
			real_part = static_cast<T>(0);
			t = abs(u.x) > abs(u.z) ? Vector3<T>(-u.y, u.x, static_cast<T>(0))
						: Vector3<T>(static_cast<T>(0), -u.z, u.y);
		}
		else
		{
			// Otherwise, build quaternion the standard way.
			t = u.Cross(v);
		}

		*this = Quaternion(real_part, t).Normalize();
	}

	template <floating_point T>
	constexpr bool Quaternion<T>::operator==(const Quaternion& other) const noexcept
	{
		return IsAlmostEqual(m_Real, other.m_Real) && m_Imaginary == other.m_Imaginary;
	}

	template <floating_point T>
	constexpr bool Quaternion<T>::operator!=(const Quaternion& other) const noexcept
	{
		return !(*this == other);
	}

	template<floating_point T>
	constexpr Quaternion<T>& Quaternion<T>::operator+=(const Quaternion& other) noexcept
	{
		m_Real += other.m_Real;
		m_Imaginary += other.m_Imaginary;
		return *this;
	}

	template<floating_point T>
	constexpr Quaternion<T> Quaternion<T>::operator+(const Quaternion& other) const noexcept
	{
		Quaternion temp(*this);
		temp += other;
		return temp;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T>& Quaternion<T>::operator+=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Real += static_cast<T>(scalar);
		m_Imaginary += static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T> Quaternion<T>::operator+(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Quaternion temp(*this);
		temp += static_cast<T>(scalar);
		return temp;
	}

	template <floating_point T>
	constexpr Quaternion<T> Quaternion<T>::operator-() const noexcept
	{
		return Quaternion(-m_Real, -m_Imaginary);
	}

	template<floating_point T>
	constexpr Quaternion<T>& Quaternion<T>::operator-=(const Quaternion& other) noexcept
	{
		m_Real -= other.m_Real;
		m_Imaginary -= other.m_Imaginary;
		return *this;
	}

	template<floating_point T>
	constexpr Quaternion<T> Quaternion<T>::operator-(const Quaternion& other) const noexcept
	{
		Quaternion temp(*this);
		temp -= other;
		return temp;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T>& Quaternion<T>::operator-=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Real -= static_cast<T>(scalar);
		m_Imaginary -= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T> Quaternion<T>::operator-(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Quaternion temp(*this);
		temp -= static_cast<T>(scalar);
		return temp;
	}

	template<floating_point T>
	constexpr Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& other) noexcept
	{
		Quaternion p(*this);
		Quaternion q(other);

		T real = p.m_Real * q.m_Real - p.m_Imaginary.Dot(q.m_Imaginary);
		Vector3<T> imaginary = q.m_Imaginary * p.m_Real + p.m_Imaginary * q.m_Real + p.m_Imaginary.Cross(q.m_Imaginary);

		m_Real = real;
		m_Imaginary = imaginary;

		return *this;
	}

	template<floating_point T>
	constexpr Quaternion<T> Quaternion<T>::operator*(const Quaternion& other) const noexcept
	{
		Quaternion temp(*this);
		temp *= other;
		return temp;
	}

	template<floating_point T>
	constexpr Vector3<T> Quaternion<T>::operator*(const Vector3<T>& other) const noexcept
	{
		auto t = m_Imaginary.Cross(other) * static_cast<T>(2.0);
		return other + t * m_Real + m_Imaginary.Cross(t);
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T>& Quaternion<T>::operator*=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Real *= static_cast<T>(scalar);
		m_Imaginary *= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T> Quaternion<T>::operator*(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Quaternion temp(*this);
		temp *= static_cast<T>(scalar);
		return temp;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T>& Quaternion<T>::operator/=(U scalar) noexcept requires std::convertible_to<U, T>
	{
		m_Real /= static_cast<T>(scalar);
		m_Imaginary /= static_cast<T>(scalar);
		return *this;
	}

	template<floating_point T>
	template<typename U>
	constexpr Quaternion<T> Quaternion<T>::operator/(U scalar) const noexcept requires std::convertible_to<U, T>
	{
		Quaternion temp(*this);
		temp /= static_cast<T>(scalar);
		return temp;
	}

	template <floating_point T>
	constexpr T Quaternion<T>::Length() const noexcept
	{
		return sqrt(this->Dot(*this));
	}

	template <floating_point T>
	constexpr T Quaternion<T>::Dot(const Quaternion& other) const noexcept
	{
		return m_Real * other.m_Real + m_Imaginary.Dot(other.m_Imaginary);
	}

	template <floating_point T>
	constexpr Quaternion<T> Quaternion<T>::Conjugate() const noexcept
	{
		return Quaternion(m_Real, -m_Imaginary);
	}

	template <floating_point T>
	constexpr Quaternion<T> Quaternion<T>::Inverse() const noexcept
	{
		T dot = this->Dot(*this);
		if (dot <= static_cast<T>(0)) // Problem
			return Quaternion(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));

		return Conjugate() / dot;
	}

	template <floating_point T>
	constexpr Quaternion<T> Quaternion<T>::Normalize() const noexcept
	{
		T len = Length();
		if (len <= static_cast<T>(0)) // Problem
			return Quaternion(static_cast<T>(1), static_cast<T>(0), static_cast<T>(0), static_cast<T>(0));
		T oneOverLen = static_cast<T>(1) / len;

		return Quaternion(m_Real * oneOverLen, m_Imaginary * oneOverLen);
	}

	template <floating_point T>
	constexpr Matrix4<T> Quaternion<T>::ToMatrix() const noexcept
	{
		Matrix4<T> result(static_cast<T>(1));
		T xx(m_Imaginary.x * m_Imaginary.x);
		T yy(m_Imaginary.y * m_Imaginary.y);
		T zz(m_Imaginary.z * m_Imaginary.z);
		T xz(m_Imaginary.x * m_Imaginary.z);
		T xy(m_Imaginary.x * m_Imaginary.y);
		T yz(m_Imaginary.y * m_Imaginary.z);
		T wx(m_Real * m_Imaginary.x);
		T wy(m_Real * m_Imaginary.y);
		T wz(m_Real * m_Imaginary.z);

		result[0][0] = static_cast<T>(1) - static_cast<T>(2) * (yy +  zz);
		result[0][1] = static_cast<T>(2) * (xy + wz);
		result[0][2] = static_cast<T>(2) * (xz - wy);

		result[1][0] = static_cast<T>(2) * (xy - wz);
		result[1][1] = static_cast<T>(1) - static_cast<T>(2) * (xx +  zz);
		result[1][2] = static_cast<T>(2) * (yz + wx);

		result[2][0] = static_cast<T>(2) * (xz + wy);
		result[2][1] = static_cast<T>(2) * (yz - wx);
		result[2][2] = static_cast<T>(1) - static_cast<T>(2) * (xx +  yy);

		return result;
	}

	template<floating_point T>
	constexpr T Quaternion<T>::roll() const noexcept
	{
		const T y = static_cast<T>(2) * (m_Imaginary.x * m_Imaginary.y + m_Real * m_Imaginary.z);
		const T x = m_Real * m_Real + m_Imaginary.x * m_Imaginary.x - m_Imaginary.y * m_Imaginary.y - m_Imaginary.z * m_Imaginary.z;

		if (Vector2<T>(x, y) == Vector2<T>(0)) // avoid atan2(0,0) - handle singularity - Matiis
			return static_cast<T>(0);

		return static_cast<T>(std::atan2(y, x));
	}

	template<floating_point T>
	constexpr T Quaternion<T>::pitch() const noexcept
	{
		const T y = static_cast<T>(2) * (m_Imaginary.y * m_Imaginary.z - m_Real * m_Imaginary.x);
		const T x = m_Real * m_Real - m_Imaginary.x * m_Imaginary.x - m_Imaginary.y * m_Imaginary.y + m_Imaginary.z * m_Imaginary.z;

		if (Vector2<T>(x, y) == Vector2<T>(0)) // avoid atan2(0,0) - handle singularity - Matiis
			return static_cast<T>(0);

		return static_cast<T>(std::atan2(y, x));
	}

	template<floating_point T>
	constexpr T Quaternion<T>::yaw() const noexcept
	{
		return std::asin(
			std::clamp(
				static_cast<T>(-2) * (m_Imaginary.x * m_Imaginary.z - m_Real * m_Imaginary.y),
				static_cast<T>(-1),
				static_cast<T>(1)));
	}

	template<floating_point T>
	constexpr Quaternion<T> Quaternion<T>::Identity() noexcept
	{
		return Quaternion(static_cast<T>(1), Vector3(static_cast<T>(0)));
	}


	template<floating_point T>
	constexpr Quaternion<T> Quaternion<T>::FromAxisAngle(const Vector3<T>& axis, Radian<T> angle) noexcept
	{
		T const a(angle.Value());
		T const s = std::sin(a * static_cast<T>(0.5));

		return Quaternion(std::cos(a * static_cast<T>(0.5)), axis * s);
	}

	template <floating_point T>
	constexpr Vector3<T> Quaternion<T>::ToEuler() const noexcept
	{
		return Vector3<T>(pitch(), yaw(), roll());
	}

	template<floating_point T>
	Json::Value Quaternion<T>::Serialize() const
	{
		Json::Value root;
		root["x"] = m_Imaginary.x;
		root["y"] = m_Imaginary.y;
		root["z"] = m_Imaginary.z;
		root["w"] = m_Real;
		return root;
	}

	template<floating_point T>
	std::optional<Quaternion<T>> Quaternion<T>::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("x") || !root.isMember("y") || !root.isMember("z") || !root.isMember("w"))
			return std::nullopt;

		Quaternion temp;
		temp.m_Imaginary.x = root["x"].asFloat();
		temp.m_Imaginary.y = root["y"].asFloat();
		temp.m_Imaginary.z = root["z"].asFloat();
		temp.m_Real = root["w"].asFloat();

		return temp;
	}

}
