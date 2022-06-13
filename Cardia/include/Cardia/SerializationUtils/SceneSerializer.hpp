#pragma once

#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"

namespace Cardia::SerializerUtils
{
        std::string SerializeScene(Scene* scene, const std::string& workspace);

        bool DeserializeScene(const std::string& serializedScene, Scene& scene, const std::string& workspace);
}