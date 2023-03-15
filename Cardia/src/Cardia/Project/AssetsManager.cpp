#include "cdpch.hpp"
#include "Cardia/Project/AssetsManager.hpp"

constexpr float GC_COLLECTION_DURATION = 10.0f;
constexpr std::uint32_t MAX_UNUSED_COUNT = 2;

namespace Cardia
{
	std::shared_ptr<AssetsManager> AssetsManager::s_ActiveManager = std::make_unique<AssetsManager>();

	AssetsManager::AssetsManager()
	{
		m_CollectThread = std::thread(&AssetsManager::CollectThread, this);
	}

	AssetsManager::~AssetsManager()
	{
		m_ShouldStopCollecting = true;
		m_CollectCV.notify_all();
		m_CollectThread.join();
	}

	void AssetsManager::CollectGarbage(bool forceCollection)
	{
		for (auto& resource : s_ActiveManager->m_Assets) {
			if (resource.second.Resource.use_count() == 1) {
				resource.second.UnusedCounter++;
			}

			if (forceCollection || resource.second.UnusedCounter > MAX_UNUSED_COUNT) {
				resource.second.Resource.reset();
			}
		}
	}

	void AssetsManager::CollectThread()
	{
		while (true) {
			auto dur = std::chrono::duration<float>(GC_COLLECTION_DURATION);
			std::unique_lock<std::mutex> lock(m_CounterMutex);
			m_CollectCV.wait_for(lock, dur, [&]() {
				return m_ShouldStopCollecting;
			});

			if (m_ShouldStopCollecting) {
				return;
			}

			CollectGarbage(false);
		}
	}
}