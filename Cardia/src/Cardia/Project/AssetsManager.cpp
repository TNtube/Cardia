#include "cdpch.hpp"
#include "Cardia/Project/AssetsManager.hpp"

namespace Cardia
{
	std::shared_ptr<AssetsManager> AssetsManager::s_ActiveManager = std::make_unique<AssetsManager>();
}