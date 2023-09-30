#pragma once
#include <filesystem>
#include <set>
#include <string>
#include <imgui.h>

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

		std::set<std::filesystem::path> m_Folders;
		std::set<std::filesystem::path> m_Files;

		std::shared_ptr<Texture> m_FileIcon;
		ImTextureID m_FileIconDescriptorSet {};
		std::shared_ptr<Texture> m_FolderIcon;
		ImTextureID m_FolderIconDescriptorSet {};

		static int m_LastWindowId;
	};
}
