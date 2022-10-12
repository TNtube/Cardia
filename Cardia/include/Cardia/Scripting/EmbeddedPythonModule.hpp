#pragma once

#include <pybind11/pybind11.h>
#include <Cardia/ECS/Entity.hpp>
#include <pybind11/embed.h>
#include <glm/vec2.hpp>

#include "Cardia/Core/KeyCodes.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/ECS/Components.hpp"
#include "ScriptEngine.hpp"


namespace pybind11::detail
{
}


namespace Cardia
{
	namespace py = pybind11;

	template<typename T>
	bool GetComponent(Entity& entity, py::object& cls, py::object& out)
	{
		auto issubclass = ScriptEngine::getPythonBuiltins().attr("issubclass");
		if (issubclass(cls, py::detail::get_type_handle(typeid(T), false)).cast<bool>()) {
			out["output"] = py::cast(entity.getComponent<T>(), py::return_value_policy::reference);
			return true;
		}
		return false;
	}

	PYBIND11_EMBEDDED_MODULE(cardia_native, m) {
		m.doc() = "Cardia Python Bindings";
		using namespace Cardia;

		// glm utilities
		py::class_<glm::vec2>(m, "vec2")
			.def(py::init<float>())
			.def(py::init<float, float>())
			.def_readwrite("x", &glm::vec2::x, py::return_value_policy::reference)
			.def_readwrite("y", &glm::vec2::y, py::return_value_policy::reference)
			.def_readwrite("r", &glm::vec2::r, py::return_value_policy::reference)
			.def_readwrite("g", &glm::vec2::g, py::return_value_policy::reference)
			.def("length", [](glm::vec2& self) {
					return glm::length(self);
				},py::return_value_policy::reference)
			.def_static("lerp", [](glm::vec2& start, glm::vec2& end, float step){
					return glm::mix(start, end, step);
				}, py::return_value_policy::reference);

		py::class_<glm::vec3>(m, "vec3")
			.def(py::init<float>())
			.def(py::init<float, float, float>())
			.def_readwrite("x", &glm::vec3::x, py::return_value_policy::reference)
			.def_readwrite("y", &glm::vec3::y, py::return_value_policy::reference)
			.def_readwrite("z", &glm::vec3::z, py::return_value_policy::reference)
			.def_readwrite("r", &glm::vec3::r, py::return_value_policy::reference)
			.def_readwrite("g", &glm::vec3::g, py::return_value_policy::reference)
			.def_readwrite("b", &glm::vec3::b, py::return_value_policy::reference)
			.def("length", [](glm::vec3& self) {
					return glm::length(self);
				},py::return_value_policy::reference)
			.def_static("lerp", [](glm::vec3& start, glm::vec3& end, float step){
					return glm::mix(start, end, step);
				}, py::return_value_policy::reference);

		py::class_<glm::vec4>(m, "vec4")
			.def(py::init<float>())
			.def(py::init<float, float, float, float>())
			.def_readwrite("x", &glm::vec4::x, py::return_value_policy::reference)
			.def_readwrite("y", &glm::vec4::y, py::return_value_policy::reference)
			.def_readwrite("z", &glm::vec4::z, py::return_value_policy::reference)
			.def_readwrite("w", &glm::vec4::w, py::return_value_policy::reference)
			.def_readwrite("r", &glm::vec4::r, py::return_value_policy::reference)
			.def_readwrite("g", &glm::vec4::g, py::return_value_policy::reference)
			.def_readwrite("b", &glm::vec4::b, py::return_value_policy::reference)
			.def_readwrite("a", &glm::vec4::a, py::return_value_policy::reference);


		// Components

		py::class_<Component::Transform>(m, "Transform")
		        .def(py::init<>())
		        .def(py::init<glm::vec3, glm::vec3, glm::vec3>())
			.def_readwrite("position", &Component::Transform::position, py::return_value_policy::reference)
			.def_readwrite("rotation", &Component::Transform::rotation, py::return_value_policy::reference)
			.def_readwrite("scale", &Component::Transform::scale, py::return_value_policy::reference)
			.def("reset", &Component::Transform::reset, py::return_value_policy::reference);

		// API Calls

		m.def("is_key_pressed", &Input::isKeyPressed, py::return_value_policy::reference);
		m.def("is_mouse_button_pressed", &Input::isMouseButtonPressed, py::return_value_policy::reference);
		m.def("get_mouse_position", &Input::getMousePos, py::return_value_policy::reference);
		m.def("get_mouse_x", &Input::getMouseX, py::return_value_policy::reference);
		m.def("get_mouse_y", &Input::getMouseY, py::return_value_policy::reference);

		m.def("get_native_transform", [](std::string& id) -> Component::Transform& {
			auto scene = ScriptEngine::getSceneContext();
			Entity entity = scene->getEntityByUUID(UUID::fromString(id));
			return entity.getComponent<Component::Transform>();
		}, py::return_value_policy::reference);

		m.def("set_native_transform", [](std::string& id, Component::Transform transform){
			auto scene = ScriptEngine::getSceneContext();
			Entity entity = scene->getEntityByUUID(UUID::fromString(id));
			auto& t = entity.getComponent<Component::Transform>();
			t.position = transform.position;
			t.rotation = transform.rotation;
			t.scale = transform.scale;
		}, py::return_value_policy::reference);

		m.def("get_component", [&](std::string& id, py::object& cls, py::object& out) {
			auto scene = ScriptEngine::getSceneContext();
			Entity entity = scene->getEntityByUUID(UUID::fromString(id));
			if (GetComponent<Component::Transform>(entity, cls, out)) {
				return;
			}
		});

		m.def("register_update_method", [](py::object& cls, std::string& name) {
		    ScriptEngine::registerUpdateMethod(cls, name);
		});

		m.def("register_update_function", [](py::object& func) {
			ScriptEngine::registerUpdateFunction(func);
		});

		m.def("get_delta_time_seconds", []() {
			return Time::deltaTime().seconds();
		});

		m.def("get_delta_time_milliseconds", []() {
			return Time::deltaTime().milliseconds();
		});
	}
}