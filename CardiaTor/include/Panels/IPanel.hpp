#pragma once
#include "Cardia/Core/Time.hpp"


namespace Cardia
{
        class IPanel
        {
        public:
                virtual ~IPanel() = default;
                virtual void onImGuiRender(DeltaTime deltaTime) = 0;
        };
}
