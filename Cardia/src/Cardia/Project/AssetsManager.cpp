#include "cdpch.hpp"
#include "Cardia/Project/AssetsManager.hpp"

constexpr std::chrono::duration<float> GC_COLLECTION_DURATION = std::chrono::duration<float>(2.0f);
constexpr std::uint32_t MAX_UNUSED_COUNT = 2;

namespace Cardia
{
	void AssetsManager::CollectGarbage(bool forceCollection)
	{
		for (auto& resource : Instance().m_Assets) {
			if (resource.second.Resource.use_count() == 1) {
				resource.second.UnusedCounter += forceCollection ? MAX_UNUSED_COUNT : 1;
			} else {
				resource.second.UnusedCounter = 0;
			}

			if (resource.second.UnusedCounter > MAX_UNUSED_COUNT) {
				Instance().m_Assets.erase(resource.first);
			}
		}
	}

	void AssetsManager::CollectionRoutine(DeltaTime& dt)
	{
		m_ElapsedTime += std::chrono::duration<float>(dt.seconds());
		if (m_ElapsedTime > GC_COLLECTION_DURATION) {
			m_ElapsedTime = std::chrono::duration<float>(0);
			CollectGarbage(false);
		}
	}

	std::filesystem::path
	AssetsManager::GetAbsolutePath(const std::filesystem::path &relative, AssetsManager::LoadType loadType)
	{
		switch (loadType) {

			case LoadType::Editor:
				return relative;
				break;
			case LoadType::Game:
				return GetAssetAbsolutePath(relative);
				break;
		}
	}
}