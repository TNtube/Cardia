#include <catch2/catch_test_macros.hpp>
#include <Cardia/Math/Angle.hpp>
#include <cmath>

namespace // anonymous
{
	constexpr Cardia::Radianf rad(std::numbers::pi_v<float>);
	constexpr Cardia::Degreef deg(60.0f);
}


TEST_CASE("Angle type can be deduced", "[Math][Angle]")
{
	constexpr Cardia::Radian radf(1.0f);
	constexpr Cardia::Degree degd(1.0);

	CHECK(std::is_same_v<std::decay_t<decltype(radf)>, Cardia::Radianf>);
	CHECK(std::is_same_v<std::decay_t<decltype(degd)>, Cardia::Degree<double>>);
}


TEST_CASE("Angle values can be accessed", "[Math][Angle]")
{
	CHECK(rad.Value() == std::numbers::pi_v<float>);
	CHECK(deg.Value() == 60.0f);
}

TEST_CASE("Angle values can be modified", "[Math][Angle]")
{
	Cardia::Radianf radCopy(rad);
	Cardia::Degreef degCopy(deg);

	radCopy.Value() = 2.0f;
	degCopy.Value() = 2.0f;

	CHECK(radCopy.Value() == 2.0f);
	CHECK(degCopy.Value() == 2.0f);
}

TEST_CASE("Angle operations", "[Math][Angle]")
{
	CHECK(rad + deg == Cardia::Radianf(4.1887903f));
	CHECK(rad - deg == Cardia::Radianf(2.0943951f));
	CHECK(rad * 2 == Cardia::Radianf(6.2831853f));
	CHECK(rad / 2 == Cardia::Radianf(1.5707963f));
	CHECK(deg + rad == Cardia::Degreef(240.0f));
	CHECK(deg - rad == Cardia::Degreef(-120.0f));
	CHECK(deg * 2 == Cardia::Degreef(120.0f));
	CHECK(deg / 2 == Cardia::Degreef(30.0f));
}