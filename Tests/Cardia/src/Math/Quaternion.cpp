#include <catch2/catch_test_macros.hpp>
#include <Cardia/Math/Quaternion.hpp>
#include <cmath>

namespace // anonymous
{
	constexpr Cardia::Quaternion q1(1.f, 2.f, 3.f, 4.f);
	constexpr Cardia::Quaternion q2(2.f, 3.f, 4.f, 5.f);
}


TEST_CASE("Quaternion type can be deduced", "[Math][Quaternion]")
{

	constexpr Cardia::Quaternion qf(1.0f, 2.0f, 3.0f, 4.0f);
	constexpr Cardia::Quaternion qd(1.0, 2.0, 3.0, 4.0);

	CHECK(std::is_same_v<std::decay_t<decltype(qf)>, Cardia::Quaternion<float>>);
	CHECK(std::is_same_v<std::decay_t<decltype(qd)>, Cardia::Quaternion<double>>);
}

TEST_CASE("Quaternion values can be accessed", "[Math][Quaternion]")
{
	CHECK(q1.w() == 1.f);
	CHECK(q1.x() == 2.f);
	CHECK(q1.y() == 3.f);
	CHECK(q1.z() == 4.f);
}

TEST_CASE("Quaternion values can be modified", "[Math][Quaternion]")
{
	Cardia::Quaternion q1Copy(q1);

	q1Copy.w() = 2.0f;
	q1Copy.x() = 3.0f;
	q1Copy.y() = 4.0f;
	q1Copy.z() = 5.0f;

	CHECK(q1Copy.w() == 2.0f);
	CHECK(q1Copy.x() == 3.0f);
	CHECK(q1Copy.y() == 4.0f);
	CHECK(q1Copy.z() == 5.0f);
}

TEST_CASE("Scalar operations on Quaternions", "[Math][Quaternion]")
{
	CHECK(q2 - 1.0f == Cardia::Quaternion(1.f, 2.f, 3.f, 4.f));
	CHECK(q1 + 1.0f == Cardia::Quaternion(2.f, 3.f, 4.f, 5.f));
	CHECK(q1 * 2 == Cardia::Quaternion(2.f, 4.f, 6.f, 8.f));
	CHECK(q2 / 2 == Cardia::Quaternion(1.f, 1.5f, 2.f, 2.5f));
}

TEST_CASE("Quaternion operations", "[Math][Quaternion]")
{
	CHECK(q1 + q2 == Cardia::Quaternion(3.f, 5.f, 7.f, 9.f));
	CHECK(q1 - q2 == Cardia::Quaternion(-1.f, -1.f, -1.f, -1.f));
	CHECK(q1 * q2 == Cardia::Quaternion(-36.f, 6.f, 12.f, 12.f));
	CHECK(q1.Dot(q2) == 40.f);
	CHECK(q1.Length() == std::sqrt(30.f));
	CHECK(q1.Normalize() == q1 * (1 / q1.Length()));
	CHECK(q1.Conjugate() == Cardia::Quaternion(1.f, -2.f, -3.f, -4.f));
	CHECK(q1.Inverse() == Cardia::Quaternion(1.f/30.0f, -2.f/30.0f, -3.f/30.0f, -4.f/30.0f));
}

TEST_CASE("Quaternion comparison", "[Math][Quaternion]")
{
	CHECK(q1 == Cardia::Quaternion(1.f, 2.f, 3.f, 4.f));
	CHECK(q1 != Cardia::Quaternion(2.f, 3.f, 4.f, 5.f));
}

TEST_CASE("Quaternion to Matrix", "[Math][Quaternion]")
{
	CHECK(q1.ToMatrix() == Cardia::Matrix4f(
		{-49.f, 20.f, 10.f, 0.f},
		{4.f, -39.f, 28.f, 0.f},
		{22.f, 20.f, -25.f, 0.f},
		{0.f, 0.f, 0.f, 1.f}
	));
}