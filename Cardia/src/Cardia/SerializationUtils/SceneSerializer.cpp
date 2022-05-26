#include "cdpch.hpp"
#include "Cardia/SerializationUtils/SceneSerializer.hpp"

#include "Cardia/ECS/Components.hpp"
#include <json/json.h>


namespace Cardia::SerializerUtils
{
        std::string SerializeScene(Scene* scene)
        {
                Json::Value root;
                const auto view = scene->getRegistry().view<Component::Name>();

                for (const auto entity_id : view)
                {
                        Entity entity(entity_id, scene);
                        const auto name = entity.getComponent<Component::Name>();

                        // Transform
                        const auto& transform = entity.getComponent<Component::Transform>();
                        root[name.name]["position"]["x"] = static_cast<double>(transform.position.x);
                        root[name.name]["position"]["y"] = static_cast<double>(transform.position.y);
                        root[name.name]["position"]["z"] = static_cast<double>(transform.position.z);
                        root[name.name]["rotation"]["x"] = static_cast<double>(transform.rotation.x);
                        root[name.name]["rotation"]["y"] = static_cast<double>(transform.rotation.y);
                        root[name.name]["rotation"]["z"] = static_cast<double>(transform.rotation.z);
                        root[name.name]["scale"]["x"] = static_cast<double>(transform.scale.x);
                        root[name.name]["scale"]["y"] = static_cast<double>(transform.scale.y);
                        root[name.name]["scale"]["z"] = static_cast<double>(transform.scale.z);
                }

                std::stringstream output;

                output << root;
                
                return output.str();
        }

        Scene DeserializeScene(const std::string& serializedScene)
        {
                return Scene();
        }
}
