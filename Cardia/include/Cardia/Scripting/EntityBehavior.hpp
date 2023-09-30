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

		Component::Transform& GetTransform();
		Entity Spawn(const std::string& name, const Entity* parent = nullptr) const;
		Entity entity;
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
