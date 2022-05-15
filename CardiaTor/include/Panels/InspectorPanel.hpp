#pragma once
#include <functional>

#include "Cardia/ECS/Entity.hpp"


namespace Cardia
{
        class InspectorPanel
        {
        public:
                static void draw(Entity selectedEntity);

        private:
                template<typename T>
                static void drawInspectorComponent(const char* name, std::function<void(T&)> func);

        private:
                inline static Entity m_SelectedEntity {};
        };
}
