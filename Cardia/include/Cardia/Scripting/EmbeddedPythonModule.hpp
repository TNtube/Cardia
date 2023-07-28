#pragma once

#include <pybind11/pybind11.h>
#include <Cardia/ECS/Entity.hpp>
#include <pybind11/embed.h>

#include "Cardia/Core/KeyCodes.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/ECS/Components.hpp"
#include "ScriptEngine.hpp"
#include "Cardia/Math/Vector2.hpp"


namespace Cardia
{
	namespace py = pybind11;

	template<typename T>
	bool GetComponent(Entity& entity, py::object& cls, py::object& out)
	{
		if (ScriptEngine::IsSubClass(cls, py::detail::get_type_handle(typeid(T), false))) {
			out["output"] = py::cast(entity.GetComponent<T>(), py::return_value_policy::reference);
			return true;
		}
		return false;
	}

	PYBIND11_EMBEDDED_MODULE(cardia_native, m) {
		m.doc() = "Cardia Python Bindings";
		using namespace Cardia;

		// glm utilities
		py::class_<Vector2f>(m, "vec2")
			.def(py::init<float>())
			.def(py::init<float, float>())
			.def_readwrite("x", &Vector2f::x, py::return_value_policy::reference)
			.def_readwrite("y", &Vector2f::y, py::return_value_policy::reference)
			.def_readwrite("r", &Vector2f::r, py::return_value_policy::reference)
			.def_readwrite("g", &Vector2f::g, py::return_value_policy::reference)
			.def("length", &Vector2f::Length,py::return_value_policy::reference)
			.def("size", &Vector2f::Size,py::return_value_policy::reference)
			.def("lerp", &Vector2f::Lerp, py::return_value_policy::reference);

		py::class_<Vector3f>(m, "vec3")
			.def(py::init<float>())
			.def(py::init<float, float, float>())
			.def_readwrite("x", &Vector3f::x, py::return_value_policy::reference)
			.def_readwrite("y", &Vector3f::y, py::return_value_policy::reference)
			.def_readwrite("z", &Vector3f::z, py::return_value_policy::reference)
			.def_readwrite("r", &Vector3f::r, py::return_value_policy::reference)
			.def_readwrite("g", &Vector3f::g, py::return_value_policy::reference)
			.def_readwrite("b", &Vector3f::b, py::return_value_policy::reference)
			.def("length", &Vector3f::Length,py::return_value_policy::reference)
			.def("size", &Vector3f::Size,py::return_value_policy::reference)
			.def("lerp", &Vector3f::Lerp, py::return_value_policy::reference);

		py::class_<Vector4f>(m, "vec4")
			.def(py::init<float>())
			.def(py::init<float, float, float, float>())
			.def_readwrite("x", &Vector4f::x, py::return_value_policy::reference)
			.def_readwrite("y", &Vector4f::y, py::return_value_policy::reference)
			.def_readwrite("z", &Vector4f::z, py::return_value_policy::reference)
			.def_readwrite("w", &Vector4f::w, py::return_value_policy::reference)
			.def_readwrite("r", &Vector4f::r, py::return_value_policy::reference)
			.def_readwrite("g", &Vector4f::g, py::return_value_policy::reference)
			.def_readwrite("b", &Vector4f::b, py::return_value_policy::reference)
			.def_readwrite("a", &Vector4f::a, py::return_value_policy::reference)
			.def("length", &Vector4f::Length,py::return_value_policy::reference)
			.def("size", &Vector4f::Size,py::return_value_policy::reference)
			.def("lerp", &Vector4f::Lerp, py::return_value_policy::reference);


		// Components

		py::class_<Component::Transform>(m, "Transform")
			.def(py::init<>())
			.def(py::init<Vector3f, Vector3f, Vector3f>())
			.def_readwrite("position", &Component::Transform::position, py::return_value_policy::reference)
			.def_readwrite("rotation", &Component::Transform::rotation, py::return_value_policy::reference)
			.def_readwrite("scale", &Component::Transform::scale, py::return_value_policy::reference)
			.def("reset", &Component::Transform::Reset, py::return_value_policy::reference);

		py::class_<Component::Light>(m, "Light")
			.def(py::init<>())
			.def_readwrite("type", &Component::Light::lightType, py::return_value_policy::reference)
			.def_readwrite("color", &Component::Light::color, py::return_value_policy::reference)
			.def_readwrite("range", &Component::Light::range, py::return_value_policy::reference)
			.def_readwrite("angle", &Component::Light::angle, py::return_value_policy::reference)
			.def_readwrite("smoothness", &Component::Light::smoothness, py::return_value_policy::reference)
			.def("reset", &Component::Light::Reset, py::return_value_policy::reference);

		// API Calls

		m.def("is_key_pressed", &Input::IsKeyPressed, py::return_value_policy::reference);
		m.def("is_mouse_button_pressed", &Input::IsMouseButtonPressed, py::return_value_policy::reference);
		m.def("get_mouse_position", &Input::GetMousePos, py::return_value_policy::reference);
		m.def("get_mouse_x", &Input::GetMouseX, py::return_value_policy::reference);
		m.def("get_mouse_y", &Input::GetMouseY, py::return_value_policy::reference);

		m.def("get_native_transform", [](std::string& id) -> Component::Transform& {
			auto& scene = ScriptEngine::Instance().GetSceneContext();
			Entity entity = scene.GetEntityByUUID(UUID::FromString(id));
			return entity.GetComponent<Component::Transform>();
		}, py::return_value_policy::reference);

		m.def("set_native_transform", [](std::string& id, Component::Transform transform){
			auto& scene = ScriptEngine::Instance().GetSceneContext();
			Entity entity = scene.GetEntityByUUID(UUID::FromString(id));
			auto& t = entity.GetComponent<Component::Transform>();
			t.position = transform.position;
			t.rotation = transform.rotation;
			t.scale = transform.scale;
		}, py::return_value_policy::reference);

		m.def("get_component", [&](std::string& id, py::object& cls, py::object& out) {
			auto& scene = ScriptEngine::Instance().GetSceneContext();
			Entity entity = scene.GetEntityByUUID(UUID::FromString(id));
			if (GetComponent<Component::Transform>(entity, cls, out)) {
				return;
			}
			if (GetComponent<Component::Light>(entity, cls, out)) {
				return;
			}
		});

		m.def("register_update_method", [](py::object& cls, std::string& name) {
			ScriptEngine::Instance().RegisterUpdateMethod(cls, name);
		});

		m.def("get_delta_time_seconds", []() {
			return Time::GetDeltaTime().AsSeconds();
		});

		m.def("get_delta_time_milliseconds", []() {
			return Time::GetDeltaTime().AsMilliseconds();
		});

		auto log = m.def_submodule("log");

		log.def("trace", &Log::Trace<std::string>);
		log.def("warn", &Log::Warn<std::string>);
		log.def("error", &Log::Error<std::string>);
	}
}
