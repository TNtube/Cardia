#pragma once

namespace Cardia
{
	template <typename T> concept arithmetic = std::is_arithmetic_v<T>;
}
