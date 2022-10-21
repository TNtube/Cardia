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
                virtual void OnImGuiRender() = 0;
                virtual void OnUpdateWorkspace() {};
		virtual void OnSceneLoad(Scene* scene) {};
        };
}
