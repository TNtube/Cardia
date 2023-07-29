#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include <Cardia/Math/Vector2.hpp>
#include <Cardia/Math/Vector3.hpp>
#include <Cardia/Math/Vector4.hpp>

namespace // anonymous
{
	constexpr Cardia::Vector3f vec3f1(6.12f, 82.5f, 30.763f);
	constexpr Cardia::Vector3f vec3f2(62.892f, 89.f, 47.2336f);
}


TEST_CASE("Vector type can be deduce", "[Math][Vector]")
{

	constexpr Cardia::Vector2 vec2i(1, 2);
	constexpr Cardia::Vector3 vec3u(1u, 2u, 3u);
	constexpr Cardia::Vector3 vec3f(1.0f, 2.0f, 3.0f);
	constexpr Cardia::Vector4 vec4f(1.0f, 2.0f, 3.0f, 4.0f);

	CHECK(std::is_same_v<std::decay_t<decltype(vec2i)>, Cardia::Vector2i>);
	CHECK(std::is_same_v<std::decay_t<decltype(vec3u)>, Cardia::Vector3u>);
	CHECK(std::is_same_v<std::decay_t<decltype(vec3f)>, Cardia::Vector3f>);
	CHECK(std::is_same_v<std::decay_t<decltype(vec4f)>, Cardia::Vector4f>);
}


TEST_CASE("Vector accession", "[Math][Vector]")
{
	SECTION("Indexing operator")
	{
		CHECK(vec3f1[0] == 6.12f);
		CHECK(vec3f1[1] == 82.5f);
		CHECK(vec3f1[2] == 30.763f);
	}

	SECTION("Member access")
	{
		CHECK(vec3f1.x == 6.12f);
		CHECK(vec3f1.y == 82.5f);
		CHECK(vec3f1.z == 30.763f);
	}

	SECTION("Members can be modified")
	{
		Cardia::Vector3f vec3f1Copy(vec3f1);

		vec3f1Copy.x = 1.0f;
		vec3f1Copy.y = 2.0f;
		vec3f1Copy.z = 3.0f;

		CHECK(vec3f1Copy.x == 1.0f);
		CHECK(vec3f1Copy.y == 2.0f);
		CHECK(vec3f1Copy.z == 3.0f);
		
		vec3f1Copy.x += 1.0f;
		vec3f1Copy.y += 2.0f;
		vec3f1Copy.z += 3.0f;

		CHECK(vec3f1Copy.x == 2.0f);
		CHECK(vec3f1Copy.y == 4.0f);
		CHECK(vec3f1Copy.z == 6.0f);
	}
}

TEST_CASE("Scalar operation to Vectors")
{
	CHECK(vec3f2 - 1.0f == Cardia::Vector3f(61.892f, 88.f, 46.2336f));
	CHECK(vec3f1 + 1.0f == Cardia::Vector3f(7.12f, 83.5f, 31.763f));
	CHECK(vec3f1 * 2.0f == Cardia::Vector3f(12.24f, 165.f, 61.526f));
	CHECK(vec3f2 / 2.0f == Cardia::Vector3f(31.446f, 44.5f, 23.6168f));
}

TEST_CASE("Vector operation to Vectors")
{
	SECTION("operators")
	{
		CHECK(vec3f2 - vec3f2 == Cardia::Vector3f(0.0f));
		CHECK(vec3f1 + vec3f2 == Cardia::Vector3f(69.012f, 171.5f, 77.9966f));
		CHECK(vec3f1 * vec3f2 == Cardia::Vector3f(384.899f, 7342.5f, 1453.0472f));
		CHECK(vec3f2 / vec3f1 == Cardia::Vector3f(10.27647f, 1.0787878f, 1.5354029f));
	}

	SECTION("Methods")
	{
		CHECK(vec3f1.Length() == 88.26135f);
		CHECK(vec3f1.Dot(vec3f2) == 9180.44629f);
		CHECK(vec3f1.Cross(vec3f2) == Cardia::Vector3f(1158.86499f, 1645.677f, -4643.90967f));
		CHECK(vec3f1.Lerp(vec3f2, 0.5f) == Cardia::Vector3f(34.506f, 85.75f, 38.998298f));
		CHECK(vec3f1.Normalize() == Cardia::Vector3f(0.06933952f, 0.93472391f, 0.34854438f));
	}
}
