#pragma once
#include <pybind11/embed.h>
#include <Cardia/ECS/Entity.hpp>
#include <Cardia/ECS/Component/Transform.hpp>

namespace py = pybind11;


namespace Cardia
{
	class Behavior
	{
	public:
		virtual ~Behavior() = default;
		virtual void on_create() {};
		virtual void on_update() {};
		virtual void on_destroy() {};


		void OnDestroy()
		{
			try {
				on_destroy();
				for (auto& task : m_Tasks)
				{
					task.attr("cancel")();
				}
			} catch (const std::exception& e) {
				Log::Error("On Destroy : {0}", e.what());
			}
		}

		Component::Transform& GetTransform();
		Entity Spawn(const std::string& name, const Entity* parent = nullptr) const;
		py::object RunCoroutine(const py::object& coroutine);
		Entity entity;
	private:
		py::list m_Tasks;
	};

	class PyBehavior : public Behavior
	{
	public:
		void on_create() override
		{
			PYBIND11_OVERRIDE(
				void,
				Behavior,
				on_create
			);
		}
		void on_update() override
		{
			PYBIND11_OVERRIDE(
				void,
				Behavior,
				on_update
			);
		}
		void on_destroy() override
		{
			PYBIND11_OVERRIDE(
				void,
				Behavior,
				on_destroy
			);
		}
	};
}
