#include "Panels/FileHierarchyPanel.hpp"

#include <filesystem>
#include <imgui.h>
#include <set>

#include "Cardia/Core/Log.hpp"
#include "Cardia/Application.hpp"
#include "Panels/PanelManager.hpp"
#include "Cardia/Project/Project.hpp"

namespace Cardia::Panel
{
	int FileHierarchyPanel::m_LastWindowId = 0;
	FileHierarchyPanel::FileHierarchyPanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++)
	{
		m_FolderIcon = Texture2D::create("resources/icons/folder.png");
		m_FileIcon = Texture2D::create("resources/icons/file.png");
	}

	void FileHierarchyPanel::OnImGuiRender(CardiaTor* appContext)
	{
		char buff[64];
		sprintf(buff, "Files##%i", m_WindowId);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(buff, &m_IsOpen)) {
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowFocused()) {
			m_PanelManager->SetFocused<FileHierarchyPanel>(this);
		}

		const auto& assetsPath = Project::GetAssetDirectory();

		if (assetsPath.empty())
		{
			ImGui::End();
			return;
		}

		const auto pathFromAssets = std::filesystem::relative(m_CurrentPath, assetsPath);
		ImGui::Text("%s", pathFromAssets != "." ? pathFromAssets.string().c_str() : "");

		if (m_CurrentPath != assetsPath)
		{
			if (ImGui::Button(".."))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
			}
		}
		const ImVec2 button_sz(100, 100);

		constexpr float paddingSize = 16.0f;
		const float cellSize = button_sz.x + paddingSize;


		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = static_cast<int>(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;


		std::set<std::filesystem::directory_entry> folders;
		std::set<std::filesystem::directory_entry> files;

		for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			if (entry.is_directory())
				folders.insert(entry);
			else
				files.insert(entry);
		}

		ImGui::Columns(columnCount, nullptr, false);

		for (const auto& entry : folders)
		{
			std::string path(entry.path().filename().string());
			ImGui::PushID(path.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			const auto id = 0; // m_FolderIcon->GetRendererID();

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - button_sz.x / 2);
			// if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<size_t>(id)), button_sz, {0, 1}, {1, 0}))
			// {
			// 	m_CurrentPath /= path;
			// }
			ImGui::PopStyleColor();

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - ImGui::CalcTextSize(path.c_str()).x / 2.0f + ImGui::GetStyle().ItemSpacing.x / 2);
			if (ImGui::Button(path.c_str()))
			{
				m_CurrentPath /= path;
			}

			ImGui::NextColumn();
			ImGui::PopID();
		}

		for (const auto& entry: files)
		{
			std::string path(entry.path().filename().string());
			ImGui::PushID(path.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			const auto id = 0; // m_FileIcon->GetRendererID();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - button_sz.x / 2);
			// ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<size_t>(id)), button_sz, {0, 1}, {1, 0});

			// if (ImGui::BeginDragDropSource())
			// {
			// 	std::string itemPath = (pathFromAssets / path).string();
			// 	ImGui::SetDragDropPayload("FILE_PATH", itemPath.c_str(), (strlen(itemPath.c_str()) + 1) * sizeof(char));
			// 	ImGui::EndDragDropSource();
			// }

			ImGui::PopStyleColor();
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - ImGui::CalcTextSize(path.c_str()).x / 2.0f + ImGui::GetStyle().ItemSpacing.x / 2);
			ImGui::Text("%s", path.c_str());
			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::End();
	}

	void FileHierarchyPanel::OnUpdateWorkspace()
	{
		m_CurrentPath = Project::GetAssetDirectory();
	}
}
