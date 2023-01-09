#pragma once
#include <filesystem>
#include <string>

#include "IPanel.hpp"
#include "Cardia/Renderer/Texture.hpp"

namespace Cardia
{
	class PanelManager;
	namespace Panel
	{
		class FileHierarchyPanel : public IPanel
		{
		public:
			explicit FileHierarchyPanel(PanelManager* manager);
			~FileHierarchyPanel() override = default;
			void OnImGuiRender() override;
			void OnUpdateWorkspace() override;

		private:
			std::filesystem::path m_CurrentPath;
			std::unique_ptr<Texture2D> m_FileIcon;
			std::unique_ptr<Texture2D> m_FolderIcon;
		};
	}
}
