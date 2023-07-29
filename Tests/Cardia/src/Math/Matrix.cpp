#include <catch2/catch_test_macros.hpp>
#include <Cardia/Math/Matrix4.hpp>


TEST_CASE("Matrix constructors", "[Math][Matrix]")
{
	constexpr Cardia::Matrix4 identity = Cardia::Matrix4f::Identity();
	Cardia::Matrix4 perspective = Cardia::Matrix4f::Perspective(Cardia::Radianf::FromDegree(60.0f), 1.0f, 0.01f, 100.0f);
	Cardia::Matrix4 orthographic = Cardia::Matrix4f::Orthographic(0.0f, 100.0f, 0.0f, 100.0f, 0.01f, 100.0f);

	CHECK(identity == Cardia::Matrix4f(1.0f));

	CHECK(perspective == Cardia::Matrix4f(
		{ 1.7320509f, 0.f, 0.f, 0.f },
		{ 0.f, 1.7320509f, 0.f, 0.f },
		{ 0.f, 0.f, -1.00010002f, -1.f },
		{ 0.f, 0.f, -0.010001f, 0.f }));

	CHECK(orthographic == Cardia::Matrix4f(
		{ 0.019999999f, 0.f, 0.f, 0.f },
		{ 0.f, 0.019999999f, 0.f, 0.f },
		{ 0.f, 0.f, -0.010001f, 0.f },
		{ -1.f, -1.f, -0.00010000999f, 1.f }));
}

TEST_CASE("Matrix accession")
{
	constexpr Cardia::Matrix4f mat4f(
		{ 1.f, 2.f, 3.f, 4.f },
		{ 5.f, 6.f, 7.f, 8.f },
		{ 9.f, 10.f, 11.f, 12.f },
		{ 13.f, 14.f, 15.f, 16.f });

	CHECK(mat4f[0] == Cardia::Vector4f(1.f, 2.f, 3.f, 4.f));
	CHECK(mat4f[1] == Cardia::Vector4f(5.f, 6.f, 7.f, 8.f));
	CHECK(mat4f[2] == Cardia::Vector4f(9.f, 10.f, 11.f, 12.f));
	CHECK(mat4f[3] == Cardia::Vector4f(13.f, 14.f, 15.f, 16.f));
}


TEST_CASE("Transformation matrices", "[Math][Matrix]")
{
	constexpr Cardia::Vector3f pos = { 1.0f, 2.0f, 3.0f };
	constexpr Cardia::Vector3f scale = { 2.0f, 3.0f, 4.0f };
	constexpr float angle = 90.0f;

	constexpr Cardia::Matrix4f translation = Cardia::Matrix4f::Identity().Translate(pos);
	constexpr Cardia::Matrix4f scaling = Cardia::Matrix4f::Identity().Scale(scale);
	Cardia::Matrix4f rotation = Cardia::Matrix4f::Identity().Rotate(Cardia::Radianf::FromDegree(angle), { 1.0f, 0.0f, 0.0f });
	Cardia::Matrix4f transformation = translation * rotation * scaling;
	Cardia::Matrix4f transpose = translation.Transpose();
	Cardia::Matrix4f inverse = scaling.Inverse();

	CHECK(translation == Cardia::Matrix4f(
		{ 1.f, 0.f, 0.f, 0.f },
		{ 0.f, 1.f, 0.f, 0.f },
		{ 0.f, 0.f, 1.f, 0.f },
		{ 1.f, 2.f, 3.f, 1.f }));

	CHECK(scaling == Cardia::Matrix4f(
		{ 2.f, 0.f, 0.f, 0.f },
		{ 0.f, 3.f, 0.f, 0.f },
		{ 0.f, 0.f, 4.f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }));

	CHECK(rotation == Cardia::Matrix4f(
		{ 1.f, 0.f, 0.f, 0.f },
		{ 0.f, -0.00000008742277f, 0.9999999f, 0.f },
		{ 0.f, -0.9999999f, -0.00000008742277f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }));

	CHECK(transformation == Cardia::Matrix4f(
		{ 2.f, 0.f, 0.f, 0.f },
		{ 0.f, -0.0000002622683f, 2.9999997f, 0.f },
		{ 0.f, -3.9999997f, -0.0000003496911f, 0.f },
		{ 1.f, 2.f, 3.f, 1.f }));

	CHECK(transpose == Cardia::Matrix4f(
		{ 1.f, 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f, 2.f },
		{ 0.f, 0.f, 1.f, 3.f },
		{ 0.f, 0.f, 0.f, 1.f }));

	CHECK(inverse == Cardia::Matrix4f(
		{ 0.5f, 0.f, 0.f, 0.f },
		{ 0.f, 0.33333334f, 0.f, 0.f },
		{ 0.f, 0.f, 0.25f, 0.f },
		{ 0.f, 0.f, 0.f, 1.f }));
}
