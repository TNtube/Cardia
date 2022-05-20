#include "Panels/FileHierarchy.hpp"

#include <filesystem>
#include <imgui.h>

#include "Cardia/Core/Log.hpp"

namespace Cardia::Panel
{
        FileHierarchy::FileHierarchy(const std::string& workspace) : m_Workspace(workspace), m_CurrentPath(workspace)
        {
                m_FolderIcon = Texture2D::create("assets/icons/folder.png");
                m_FileIcon = Texture2D::create("assets/icons/file.png");
        }

        void FileHierarchy::onImGuiRender(DeltaTime deltaTime)
        {
                ImGui::Begin("Files");

                if (m_Workspace.string().length() == 0)
                {
                        ImGui::End();
                        return;
                }
                
                const auto pathFromWorkspace = std::filesystem::relative(m_CurrentPath, m_Workspace).string();
                ImGui::Text("%s", pathFromWorkspace != "." ? pathFromWorkspace.c_str() : "");

                if (m_CurrentPath != m_Workspace)
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

                ImGui::Columns(columnCount, nullptr, false);

                for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
                {
                        std::string path(entry.path().filename().string());
                        
                        ImGui::PushID(path.c_str());
                        
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                        const auto id = entry.is_directory() ? m_FolderIcon->getRendererID() : m_FileIcon->getRendererID();
                        if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(id), button_sz, {0, 1}, {1, 0}) && entry.is_directory())
                        {
                                m_CurrentPath /= path;
                        }

                        if (ImGui::BeginDragDropSource())
                        {
                                const wchar_t* itemPath = (m_CurrentPath / path).c_str();
                                ImGui::SetDragDropPayload("FILE_PATH", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
                                ImGui::EndDragDropSource();
                        }

                        ImGui::PopStyleColor();

                        ImGui::TextWrapped("%s", path.c_str());

                        ImGui::NextColumn();
                        ImGui::PopID();
                }
                
                ImGui::Columns(1);
                ImGui::End();
        }

        void FileHierarchy::updateWorkspace(const std::string& workspace)
        {
                m_Workspace = workspace;
                m_CurrentPath = workspace;
        }
}
