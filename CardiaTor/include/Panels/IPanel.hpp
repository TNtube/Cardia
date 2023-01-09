#pragma once
#include <Cardia/Core/Time.hpp>
#include <string>
#include <Cardia/ECS/Entity.hpp>
#include <filesystem>

namespace Cardia
{
	class PanelManager;
	namespace Panel
	{
		class IPanel
		{
		public:
			explicit IPanel(PanelManager* manager) : m_PanelManager(manager) {}
			virtual ~IPanel() = default;
			virtual void OnImGuiRender() = 0;
			virtual void OnUpdateWorkspace() {}
			virtual void OnSceneLoad(Scene* scene) { m_CurrentScene = scene; }

		protected:
			Scene* m_CurrentScene {};
			PanelManager* m_PanelManager {};
		};
	}
}
