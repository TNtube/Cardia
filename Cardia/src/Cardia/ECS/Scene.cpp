#include "cdpch.hpp"
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"

namespace Cardia
{
	Scene::Scene(std::string name)
		: m_Name(std::move(name))
	{

	}

	Entity Scene::createEntity(std::string name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.addComponent<Component::Transform>();
		entity.addComponent<Component::Name>(name.empty() ? "Default Entity" : std::move(name));
		return entity;
	}

	void Scene::onUpdate(DeltaTime deltaTime)
	{
		using namespace Component;

		Renderer2D::

		auto view = m_Registry.view<Transform, SpriteRenderer>();
		for (auto entity : view)
		{
			auto [transform, spriteRenderer] = view.get<Transform, SpriteRenderer>(entity);

		}
	}
}