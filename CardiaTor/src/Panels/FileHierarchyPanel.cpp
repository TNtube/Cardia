#include "Panels/FileHierarchyPanel.hpp"

#include <filesystem>
#include <imgui.h>
#include <set>

#include "Cardia/Core/Log.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/Assets/AssetsManager.hpp"
#include "Panels/PanelManager.hpp"
#include "Cardia/Project/Project.hpp"
#include "CardiaTor.hpp"
#include "Cardia/Renderer/Descriptors.hpp"

#include "Cardia/ImGui/imgui_impl_vulkan.h"

namespace Cardia::Panel
{
	int FileHierarchyPanel::m_LastWindowId = 0;
	FileHierarchyPanel::FileHierarchyPanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++)
	{
		auto& app = Application::Get();

		const auto folderHandle = app.GetAssetsManager().GetHandleFromRelative("resources/icons/folder.png");
		m_FolderIcon = app.GetAssetsManager().Load<Texture>(folderHandle);

		const auto fileHandle = app.GetAssetsManager().GetHandleFromRelative("resources/icons/file.png");
		m_FileIcon = app.GetAssetsManager().Load<Texture>(fileHandle);
	}

	void FileHierarchyPanel::OnImGuiRender(CardiaTor* appContext)
	{
		if (!m_FolderIconDescriptorSet && !m_FileIconDescriptorSet && m_FileIcon && m_FolderIcon)
		{
			m_FileIconDescriptorSet = ImGui_ImplVulkan_AddTexture(m_FileIcon->GetSampler(), m_FileIcon->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			m_FolderIconDescriptorSet = ImGui_ImplVulkan_AddTexture(m_FolderIcon->GetSampler(), m_FolderIcon->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
		char buff[64];
		sprintf_s(buff, "Files##%i", m_WindowId);
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

		ImGui::Text("%s", m_PathFromAssets != "." ? m_PathFromAssets.string().c_str() : "");

		if (m_CurrentPath.compare(assetsPath) != 0)
		{
			if (ImGui::Button(".."))
			{
				m_CurrentPath = m_CurrentPath.parent_path();
				CurrentPathUpdated();
			}
		}
		constexpr ImVec2 button_sz(100, 100);

		constexpr float paddingSize = 16.0f;
		constexpr float cellSize = button_sz.x + paddingSize;


		const float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = static_cast<int>(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, nullptr, false);

		for (const auto& entry : m_Folders)
		{
			std::string path(entry.filename().string());
			ImGui::PushID(path.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - button_sz.x / 2);
			if (ImGui::ImageButton(m_FolderIconDescriptorSet, button_sz))
			{
				m_CurrentPath /= path;
				CurrentPathUpdated();
			}
			
			ImGui::PopStyleColor();

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - ImGui::CalcTextSize(path.c_str()).x / 2.0f + ImGui::GetStyle().ItemSpacing.x / 2);
			ImGui::Text("%s", path.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		for (const auto& entry: m_Files)
		{
			std::string path(entry.filename().string());
			ImGui::PushID(path.c_str());
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - button_sz.x / 2);

			ImGui::ImageButton(m_FileIconDescriptorSet, button_sz);

			if (ImGui::BeginDragDropSource())
			{
				auto handle = appContext->GetAssetsManager().GetHandleFromAbsolute(entry);
				ImGui::SetDragDropPayload("ASSET_HANDLE", &handle, sizeof(handle));
				ImGui::EndDragDropSource();
			}

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
		CurrentPathUpdated();
	}

	void FileHierarchyPanel::CurrentPathUpdated()
	{
		m_CurrentPath = canonical(m_CurrentPath);
		const auto& assetsPath = Project::GetAssetDirectory();
		m_PathFromAssets = std::filesystem::relative(m_CurrentPath, assetsPath);

		m_Folders.clear();
		m_Files.clear();
		for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
		{
			if (entry.is_directory())
				m_Folders.insert(std::filesystem::absolute(entry));
			else if (entry.path().extension() != ".imp")
				m_Files.insert(std::filesystem::absolute(entry));
		}
	}
}
