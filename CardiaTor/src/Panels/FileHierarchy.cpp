#include "Panels/FileHierarchy.hpp"

#include <filesystem>
#include <imgui.h>

namespace Cardia::Panel
{
        FileHierarchy::FileHierarchy(const std::string& workspace) : m_Workspace(workspace), m_CurrentPath(workspace)
        {
        }

        void FileHierarchy::onImGuiRender(DeltaTime deltaTime)
        {
                ImGui::Begin("Files");

                if (m_Workspace.length() == 0)
                {
                        ImGui::End();
                        return;
                }

                if (m_CurrentPath != m_Workspace && ImGui::Button(".."))
                {
                        m_CurrentPath = m_CurrentPath.substr(0, m_CurrentPath.find_last_of("\\/"));
                }

                int i = 1;
                for (const auto& entry : std::filesystem::directory_iterator(m_CurrentPath))
                {
                        std::string path(entry.path().filename().string());
                        
                        if (ImGui::Button(path.c_str(), {100, 50}))
                        {
                                if (entry.is_directory())
                                {
                                        m_CurrentPath.append("/" + path);
                                }
                        }
                        if (i%8 != 0)
                        {
                                ImGui::SameLine();
                        }
                        i++;
                }
                ImGui::End();
        }

        void FileHierarchy::updateWorkspace(const std::string& workspace)
        {
                m_Workspace = workspace;
                m_CurrentPath = workspace;
        }
}
