#include "cdpch.hpp"
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/Camera.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include <pybind11/embed.h>


namespace Cardia
{
	namespace py = pybind11;
	using namespace py::literals;

	Scene::Scene(std::string name)
		: m_Name(std::move(name))
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = {m_Registry.create(), this};
		entity.addComponent<Component::Transform>();
		entity.addComponent<Component::ID>();
		entity.addComponent<Component::Name>(name.empty() ? "Default Entity" : name);
		return entity;
	}

	Entity Scene::CreateEntityFromId(UUID uuid) {
		Entity entity = {m_Registry.create(), this};
		entity.addComponent<Component::Transform>();
		entity.addComponent<Component::ID>(uuid);
		entity.addComponent<Component::Name>("Default Entity");
		return entity;
	}

	void Scene::DestroyEntity(entt::entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeUpdate()
	{
		ScriptEngine::Instance().OnRuntimeUpdate();

		SceneCamera* mainCamera = nullptr;
		glm::mat4 mainCameraTransform;

		{
			const auto viewCamera = m_Registry.view<Component::Transform, Component::Camera>();
			for (const auto entity: viewCamera)
			{
				auto[transform, cam] = viewCamera.get<Component::Transform, Component::Camera>(
					entity);
				if (cam.primary)
				{
					mainCamera = &cam.camera;
					mainCameraTransform = transform.getTransform();
				}
			}
		}

		if (!mainCamera)
		{
			Log::error("Scene hierarchy should have a primary camera. Either create one or set the existing one to primary");
			return;
		}

		Renderer2D::beginScene(*mainCamera, mainCameraTransform);

		const auto view = m_Registry.view<Component::Transform, Component::SpriteRenderer>();
		for (const auto entity : view)
		{
			auto [transform, spriteRenderer] = view.get<Component::Transform, Component::SpriteRenderer>(entity);
			Renderer2D::drawRect(transform.getTransform(), spriteRenderer.texture.get(), spriteRenderer.color, spriteRenderer.tillingFactor, spriteRenderer.zIndex);
		}

		const auto lightView = m_Registry.view<Component::Transform, Component::PointLight2D>();
		for (const auto entity : lightView)
		{
			auto [transform, pointLight] = lightView.get<Component::Transform, Component::PointLight2D>(entity);

			Renderer2D::addLight(transform.position, pointLight);
		}

		Renderer2D::endScene();
	}

	void Scene::OnUpdateEditor(Camera& editorCamera)
	{
		Renderer2D::beginScene(editorCamera, editorCamera.getPosition());

		const auto view = m_Registry.view<Component::Transform, Component::SpriteRenderer>();
		for (const auto entity : view)
		{
			auto [transform, spriteRenderer] = view.get<Component::Transform, Component::SpriteRenderer>(entity);
			Renderer2D::drawRect(transform.getTransform(), spriteRenderer.texture.get(), spriteRenderer.color, spriteRenderer.tillingFactor, spriteRenderer.zIndex);
		}

		const auto lightView = m_Registry.view<Component::Transform, Component::PointLight2D>();
		for (const auto entity : lightView)
		{
			auto [transform, pointLight] = lightView.get<Component::Transform, Component::PointLight2D>(entity);
			Renderer2D::addLight(transform.position, pointLight);
		}


		Renderer2D::endScene();
	}

	void Scene::OnViewportResize(float width, float height)
	{
		auto view = m_Registry.view<Component::Camera>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<Component::Camera>(entity);
			cameraComponent.camera.setViewportSize((float)width, (float)height);
		}
	}

	void Scene::clear()
	{
		m_Registry.clear();
	}

	void Scene::OnRuntimeStart()
	{
		ScriptEngine::Instance().OnRuntimeStart(this);
	}

	Entity Scene::GetEntityByUUID(const UUID& uuid)
	{
		auto view = m_Registry.view<Component::ID>();
		Entity result;
		for (auto entity : view)
		{
			auto& idComponent = view.get<Component::ID>(entity);
			if (idComponent.uuid == uuid) {
				result = Entity(entity, this);
			}
		}
		return result;
	}

	void Scene::OnRuntimeStop()
	{
	}

	Entity Scene::GetCurrentEntity()
	{
		return GetEntityByUUID(m_CurrentEntity);
	}
}
