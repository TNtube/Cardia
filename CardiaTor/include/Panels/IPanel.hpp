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
                virtual void onImGuiRender() = 0;
                virtual void updateWorkspace() {}
                virtual Entity& getSelectedEntity()
                {
                        return m_SelectedEntity;
                }

                virtual void updateSelectedEntity(const Entity& entity)
                {
                        m_SelectedEntity = entity;
                }
        protected:
                Entity m_SelectedEntity;
        };
}
