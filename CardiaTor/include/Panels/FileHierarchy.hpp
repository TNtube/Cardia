#pragma once
#include <string>

#include "IPanel.hpp"

namespace Cardia::Panel
{
        class FileHierarchy : public IPanel
        {
        public:
                explicit FileHierarchy(const std::string& workspace);
                ~FileHierarchy() override = default;
                void onImGuiRender(DeltaTime deltaTime) override;
                void updateWorkspace(const std::string& workspace);

        private:
                std::string m_Workspace;
                std::string m_CurrentPath;
        };
}
