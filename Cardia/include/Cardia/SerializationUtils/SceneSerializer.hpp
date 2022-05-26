#pragma once

#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"

namespace Cardia::SerializerUtils
{
        std::string SerializeScene(Scene* scene);

        bool DeserializeScene(const std::string& serializedScene, Scene& scene);
}