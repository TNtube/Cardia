#include "cdpch.hpp"

#include "Cardia/ECS/Components.hpp"

namespace Cardia::Component
{
        void EntityBehavior::reloadFile()
        {
                const std::ifstream t(m_Path);
                std::stringstream buffer;
                buffer << t.rdbuf();
                m_Content = buffer.str();
        }
}
