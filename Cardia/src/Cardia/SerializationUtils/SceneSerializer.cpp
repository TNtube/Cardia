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

                        Json::Value node;
                        // Transform
                        const auto& transform = entity.getComponent<Component::Transform>();
                        node["position"]["x"] = transform.position.x;
                        node["position"]["y"] = transform.position.y;
                        node["position"]["z"] = transform.position.z;
                        node["rotation"]["x"] = transform.rotation.x;
                        node["rotation"]["y"] = transform.rotation.y;
                        node["rotation"]["z"] = transform.rotation.z;
                        node["scale"]["x"] = transform.scale.x;
                        node["scale"]["y"] = transform.scale.y;
                        node["scale"]["z"] = transform.scale.z;

                        root[name.name]["transform"] = node;
                        node.clear();

                        // SpriteRenderer
                        if (entity.hasComponent<Component::SpriteRenderer>())
                        {
                                const auto& spriteRenderer = entity.getComponent<Component::SpriteRenderer>();
                                node["color"]["r"] = spriteRenderer.color.r;
                                node["color"]["g"] = spriteRenderer.color.g;
                                node["color"]["b"] = spriteRenderer.color.b;
                                node["color"]["a"] = spriteRenderer.color.a;
                                node["texture"] = spriteRenderer.texture ? spriteRenderer.texture->getPath() : "";
                                node["tillingFactor"] = spriteRenderer.tillingFactor;
                                node["zIndex"] = spriteRenderer.zIndex;
                        
                                root[name.name]["spriteRenderer"] = node;
                                node.clear();
                        }
                        
                        // Camera
                        if (entity.hasComponent<Component::Camera>())
                        {
                                const auto& camera = entity.getComponent<Component::Camera>();
                                node["type"] = static_cast<int>(camera.camera.getProjectionType());
                                node["perspectiveFov"] = camera.camera.getPerspectiveFov();
                                node["perspectiveFar"] = camera.camera.getPerspectiveFar();
                                node["perspectiveNear"] = camera.camera.getPerspectiveNear();
                                
                                node["orthoSize"] =camera.camera.getOrthographicSize();
                                node["orthoNear"] = camera.camera.getOrthographicNear();
                                node["orthoFar"] = camera.camera.getOrthographicFar();
                                
                                root[name.name]["camera"] = node;
                                node.clear();
                        }
                }

                std::stringstream output;

                output << root;
                
                return output.str();
        }

        bool DeserializeScene(const std::string& serializedScene, Scene& scene)
        {
                Json::Value root;

                std::string err;
                const Json::CharReaderBuilder builder;
                const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
                if (!reader->parse(serializedScene.c_str(), serializedScene.c_str() + serializedScene.length(), &root, &err))
                {
                        Log::coreError("Could not load scene {0} : {1}", scene.getName(), err);
                        return false;
                }

                scene.clear();

                for (const auto& entityName: root.getMemberNames())
                {
                        auto entity = scene.createEntity(entityName);

                        auto& node = root[entityName];

                        auto& component = entity.getComponent<Component::Transform>();
                        component.position.x = node["transform"]["position"]["x"].asFloat();
                        component.position.y = node["transform"]["position"]["y"].asFloat();
                        component.position.z = node["transform"]["position"]["z"].asFloat();
                        
                        component.rotation.x = node["transform"]["rotation"]["x"].asFloat();
                        component.rotation.y = node["transform"]["rotation"]["y"].asFloat();
                        component.rotation.z = node["transform"]["rotation"]["z"].asFloat();
                        
                        component.scale.x = node["transform"]["scale"]["x"].asFloat();
                        component.scale.y = node["transform"]["scale"]["y"].asFloat();
                        component.scale.z = node["transform"]["scale"]["z"].asFloat();

                        if (node.isMember("spriteRenderer"))
                        {
                                auto& spriteRenderer = entity.addComponent<Component::SpriteRenderer>();
                                spriteRenderer.color.r = node["spriteRenderer"]["color"]["r"].asFloat();
                                spriteRenderer.color.g = node["spriteRenderer"]["color"]["g"].asFloat();
                                spriteRenderer.color.b = node["spriteRenderer"]["color"]["b"].asFloat();
                                spriteRenderer.color.a = node["spriteRenderer"]["color"]["a"].asFloat();

                                auto texture = Texture2D::create(node["spriteRenderer"]["texture"].asString());
                                if (texture->isLoaded())
                                {
                                        spriteRenderer.texture = std::move(texture);
                                }
                                
                                spriteRenderer.color.a = node["spriteRenderer"]["tillingFactor"].asFloat();
                                spriteRenderer.zIndex = node["spriteRenderer"]["zIndex"].asInt();
                        }

                        if (node.isMember("camera"))
                        {
                                auto& camera = entity.addComponent<Component::Camera>();
                                camera.camera.setProjectionType(static_cast<SceneCamera::ProjectionType>(node["camera"]["type"].asInt()));
                                camera.camera.setPerspectiveFov(node["camera"]["perspectiveFov"].asFloat());
                                camera.camera.setPerspectiveFar(node["camera"]["perspectiveNear"].asFloat());
                                camera.camera.setPerspectiveFar(node["camera"]["perspectiveFar"].asFloat());

                                camera.camera.setOrthographicSize(node["camera"]["orthoSize"].asFloat());
                                camera.camera.setOrthographicNear(node["camera"]["orthoNear"].asFloat());
                                camera.camera.setOrthographicFar(node["camera"]["orthoFar"].asFloat());
                        }
                }
                
                return true;
        }
}
