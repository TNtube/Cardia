#pragma once
#include <filesystem>
#include <set>
#include <string>

#include "IPanel.hpp"
#include "Cardia/Renderer/Texture.hpp"


namespace Cardia::Panel
{
	class FileHierarchyPanel : public IPanel
	{
	public:
		explicit FileHierarchyPanel(PanelManager* manager);
		~FileHierarchyPanel() override = default;
		void OnImGuiRender(CardiaTor* appContext) override;
		void OnUpdateWorkspace() override;

	private:
		void CurrentPathUpdated();
		std::filesystem::path m_CurrentPath;
		std::filesystem::path m_PathFromAssets;

		std::set<std::filesystem::directory_entry> m_Folders;
		std::set<std::filesystem::directory_entry> m_Files;

		std::shared_ptr<Texture2D> m_FileIcon;
		std::shared_ptr<Texture2D> m_FolderIcon;

		static int m_LastWindowId;
	};
}
