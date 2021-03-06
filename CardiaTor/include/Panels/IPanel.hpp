#pragma once
#include "Cardia/Core/Time.hpp"
#include <string>
#include <Cardia/ECS/Entity.hpp>
#include <filesystem>

namespace Cardia
{
        class IPanel
        {
        public:
                virtual ~IPanel() = default;
                virtual void onImGuiRender(DeltaTime deltaTime) = 0;
                virtual void updateWorkspace(const std::string& workspace)
                {
                        m_Workspace = workspace;
                }
                virtual Entity& getSelectedEntity()
                {
                        return m_SelectedEntity;
                }

                virtual void updateSelectedEntity(const Entity& entity)
                {
                        m_SelectedEntity = entity;
                }
        protected:
                std::filesystem::path m_Workspace;
                Entity m_SelectedEntity;
        };
}
