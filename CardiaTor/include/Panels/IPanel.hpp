#pragma once
#include <Cardia/Core/Time.hpp>
#include <string>
#include <Cardia/ECS/Entity.hpp>
#include <filesystem>

namespace Cardia
{
	class PanelManager;
	class CardiaTor;
	namespace Panel
	{
		class IPanel
		{
		public:
			explicit IPanel(PanelManager* manager, int windowId) : m_PanelManager(manager), m_WindowId(windowId) {}
			virtual ~IPanel() = default;
			virtual void OnImGuiRender(CardiaTor* appContext) = 0;
			virtual void OnUpdateWorkspace() {}
			virtual void OnSceneLoad(Scene* scene) { m_CurrentScene = scene; }
			bool IsOpen() const { return m_IsOpen; }

			virtual void Open() { m_IsOpen = true; }
			virtual void Close() { m_IsOpen = false; }

		protected:
			bool m_IsOpen = true;

			int m_WindowId;
			Scene* m_CurrentScene {};
			PanelManager* m_PanelManager {};
		};
	}
}
