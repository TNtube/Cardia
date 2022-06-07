#include "Panels/FileHierarchy.hpp"

#include <filesystem>
#include <imgui.h>
#include <set>

#include "Cardia/Core/Log.hpp"

namespace Cardia::Panel
{
        FileHierarchy::FileHierarchy()
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
                        const auto id = m_FolderIcon->getRendererID();
                        
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - button_sz.x / 2);
                        if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<size_t>(id)), button_sz, {0, 1}, {1, 0}))
                        {
                                m_CurrentPath /= path;
                        }
                        ImGui::PopStyleColor();

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - ImGui::CalcTextSize(path.c_str()).x / 2.0f + ImGui::GetStyle().ItemSpacing.x / 2);
                        ImGui::Text("%s", path.c_str());

                        ImGui::NextColumn();
                        ImGui::PopID();
                }

                for (const auto& entry: files)
                {
                        std::string path(entry.path().filename().string());
                        ImGui::PushID(path.c_str());
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                        const auto id = m_FileIcon->getRendererID();
                        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetColumnWidth() / 2.0f - button_sz.x / 2);
                        ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<size_t>(id)), button_sz, {0, 1}, {1, 0});
                        
                        if (ImGui::BeginDragDropSource())
                        {
                                const char* itemPath = (m_CurrentPath / path).string().c_str();
                                ImGui::SetDragDropPayload("FILE_PATH", itemPath, (strlen(itemPath) + 1) * sizeof(char));
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

        void FileHierarchy::updateWorkspace(const std::string& workspace)
        {
                m_Workspace = workspace;
                m_CurrentPath = workspace;
        }
}
