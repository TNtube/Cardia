#include "cdpch.hpp"
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/Camera.hpp"

namespace Cardia
{
	Scene::Scene(std::string name)
		: m_Name(std::move(name))
	{

	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.addComponent<Component::Transform>();
		entity.addComponent<Component::Name>(name.empty() ? "Default Entity" : name);
		return entity;
	}

	void Scene::destroyEntity(const Entity &entity)
	{
		m_Registry.destroy(entity.m_Entity);
	}

	void Scene::onUpdate(DeltaTime deltaTime)
	{

		Camera* mainCamera = nullptr;
		glm::mat4 mainCameraTransform;

		{
			auto viewCamera = m_Registry.view<Component::Transform, Component::Camera>();
			for (auto entity: viewCamera)
			{
				auto[transform, camera] = viewCamera.get<Component::Transform, Component::Camera>(
					entity);
				if (camera.primary)
				{
					mainCamera = &camera.camera;
					mainCameraTransform = transform.getTransform();
				}
			}
		}

		Renderer2D::beginScene(*mainCamera, mainCameraTransform);

		auto view = m_Registry.view<Component::Transform, Component::SpriteRenderer>();
		for (auto entity : view)
		{
			auto [transform, spriteRenderer] = view.get<Component::Transform, Component::SpriteRenderer>(entity);
			Renderer2D::drawRect(transform.getTransform(), spriteRenderer.color);
		}

		Renderer2D::endScene();
	}
}