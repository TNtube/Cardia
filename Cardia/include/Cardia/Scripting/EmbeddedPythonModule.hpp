#pragma once

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <Cardia/ECS/Entity.hpp>
#include <pybind11/embed.h>

#include "Cardia/Core/KeyCodes.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/ECS/Components.hpp"
#include "ScriptEngine.hpp"
#include "Cardia/Math/Vector2.hpp"
#include "Cardia/Core/Time.hpp"
#include "EntityBehavior.hpp"
#include "Cardia/Scripting/ScriptUtils.hpp"

namespace Cardia
{
	namespace py = pybind11;

	template<typename... Cpn>
	py::object GetComponent(ComponentGroup<Cpn...>, Entity entity, py::type& cls)
	{
		py::object out = py::none();

		([&](){
			if (IsSubclass<Cpn>(cls)) {
				out = py::cast(entity.GetComponent<Cpn>(), py::return_value_policy::reference);
			}
		}(), ...);

		return out;
	}

	template<typename... Cpn>
	py::object AddComponent(ComponentGroup<Cpn...>, Entity entity, py::type& cls)
	{
		py::object out = py::none();

		([&](){
			if (IsSubclass<Cpn>(cls)) {
				out = py::cast(entity.AddComponent<Cpn>(), py::return_value_policy::reference);
			}
		}(), ...);

		return out;
	}

	template<typename... Cpn>
	bool HasComponent(ComponentGroup<Cpn...>, Entity entity, py::type& cls)
	{
		bool out = false;
		([&](){
			if (IsSubclass<Cpn>(cls)) {
				out = entity.HasComponent<Cpn>();
			}
		}(), ...);

		return out;
	}

	PYBIND11_EMBEDDED_MODULE(cardia_native, m) {
		m.doc() = "Cardia Python Bindings";

		// behavior
		py::class_<Behavior, PyBehavior>(m, "Behavior")
			.def(py::init<>())
			.def("on_create", &Behavior::on_create)
			.def("on_update", &Behavior::on_update)
			.def_property_readonly("transform", &Behavior::GetTransform, py::return_value_policy::reference)
			.def_property_readonly("entity", [](Behavior& self){ return self.entity; }, py::return_value_policy::reference)
			.def("spawn", &Behavior::Spawn, py::arg("path"), py::arg("parent").none(true));

		// entity
		py::class_<Entity>(m, "Entity")
			.def(py::init<>())
			.def_property_readonly("transform", [](Entity& self) -> Component::Transform& {
				return self.GetComponent<Component::Transform>();
			}, py::return_value_policy::reference)
			.def_property("name", [](Entity& self) {
				return self.GetComponent<Component::Label>().Name;
			}, [](Entity& self, std::string newName) {
				return self.GetComponent<Component::Label>().Name = std::move(newName);
			})
			.def("get_component", [](Entity& self, py::type& cls) -> py::object {
				return GetComponent(ScriptableComponents{}, self, cls);
			}, py::return_value_policy::reference)
			.def("add_component", [](Entity& self, py::type& cls) -> py::object {
				return AddComponent(ScriptableComponents{}, self, cls);
			}, py::return_value_policy::reference)
			.def("has_component", [](Entity& self, py::type& cls) -> bool {
				return HasComponent(ScriptableComponents{}, self, cls);
			}, py::return_value_policy::reference)
			.def("__bool__", &Entity::IsValid)
			.def("kill", &Entity::Kill, py::return_value_policy::reference);

		// math utilities
		py::class_<Vector2f>(m, "vec2")
			.def(py::init<float>())
			.def(py::init<float, float>())
			.def_readwrite("x", &Vector2f::x, py::return_value_policy::reference)
			.def_readwrite("y", &Vector2f::y, py::return_value_policy::reference)
			.def("__add__", [](Vector2f& self, Vector2f& other) { return self + other; })
			.def("_add__scalar", [](Vector2f& self, float scalar) { return self + scalar; })
			.def("__sub__", [](Vector2f& self, Vector2f& other) { return self - other; })
			.def("_sub__scalar", [](Vector2f& self, float scalar) { return self - scalar; })
			.def("__mul__", [](Vector2f& self, Vector2f& other) { return self * other; })
			.def("_mul__scalar", [](Vector2f& self, float scalar) { return self * scalar; })
			.def("__truediv__", [](Vector2f& self, Vector2f& other) { return self / other; })
			.def("_truediv__scalar", [](Vector2f& self, float scalar) { return self / scalar; })
			.def("__eq__", [](Vector2f& self, Vector2f& other) { return self == other; })
			.def("length", &Vector2f::Length,py::return_value_policy::reference)
			.def("size", &Vector2f::Size,py::return_value_policy::reference)
			.def("lerp", &Vector2f::Lerp, py::return_value_policy::reference);

		py::class_<Vector3f>(m, "vec3")
			.def(py::init<float>())
			.def(py::init<float, float, float>())
			.def_readwrite("x", &Vector3f::x, py::return_value_policy::reference)
			.def_readwrite("y", &Vector3f::y, py::return_value_policy::reference)
			.def_readwrite("z", &Vector3f::z, py::return_value_policy::reference)
			.def("__add__", [](Vector3f& self, Vector3f& other) { return self + other; })
			.def("_add__scalar", [](Vector3f& self, float scalar) { return self + scalar; })
			.def("__sub__", [](Vector3f& self, Vector3f& other) { return self - other; })
			.def("_sub__scalar", [](Vector3f& self, float scalar) { return self - scalar; })
			.def("__mul__", [](Vector3f& self, Vector3f& other) { return self * other; })
			.def("_mul__scalar", [](Vector3f& self, float scalar) { return self * scalar; })
			.def("__truediv__", [](Vector3f& self, Vector3f& other) { return self / other; })
			.def("_truediv__scalar", [](Vector3f& self, float scalar) { return self / scalar; })
			.def("__eq__", [](Vector3f& self, Vector3f& other) { return self == other; })
			.def("length", &Vector3f::Length,py::return_value_policy::reference)
			.def("size", &Vector3f::Size,py::return_value_policy::reference)
			.def("lerp", &Vector3f::Lerp, py::return_value_policy::reference)
			.def("cross", &Vector3f::Cross, py::return_value_policy::reference);

		py::class_<Vector4f>(m, "vec4")
			.def(py::init<float>())
			.def(py::init<float, float, float, float>())
			.def_readwrite("x", &Vector4f::x, py::return_value_policy::reference)
			.def_readwrite("y", &Vector4f::y, py::return_value_policy::reference)
			.def_readwrite("z", &Vector4f::z, py::return_value_policy::reference)
			.def_readwrite("w", &Vector4f::w, py::return_value_policy::reference)
			.def("__add__", [](Vector4f& self, Vector4f& other) { return self + other; })
			.def("__add__scalar", [](Vector4f& self, float scalar) { return self + scalar; })
			.def("__sub__", [](Vector4f& self, Vector4f& other) { return self - other; })
			.def("__sub__scalar", [](Vector4f& self, float scalar) { return self - scalar; })
			.def("__mul__", [](Vector4f& self, Vector4f& other) { return self * other; })
			.def("_mul__scalar", [](Vector4f& self, float scalar) { return self * scalar; })
			.def("__truediv__", [](Vector4f& self, Vector4f& other) { return self / other; })
			.def("__truediv__scalar", [](Vector4f& self, float scalar) { return self / scalar; })
			.def("__eq__", [](Vector4f& self, Vector4f& other) { return self == other; })
			.def("length", &Vector4f::Length,py::return_value_policy::reference)
			.def("size", &Vector4f::Size,py::return_value_policy::reference)
			.def("lerp", &Vector4f::Lerp, py::return_value_policy::reference);

		py::class_<Quatf>(m, "Quaternion")
		    .def(py::init<float, Vector3f>())
			.def(py::init<float, float, float, float>())
			.def("__mul__", [](Quatf& self, Quatf& other) {
				return self * other;
			});


		// Components

		py::class_<Component::ID>(m, "ID")
			.def(py::init<>())
			.def_readwrite("uuid", &Component::ID::Uuid, py::return_value_policy::reference);

		py::class_<Component::Label>(m, "Label")
			.def(py::init<>())
			.def_readwrite("name", &Component::Label::Name, py::return_value_policy::reference)
			.def_readwrite("color", &Component::Label::Color, py::return_value_policy::reference);

		py::class_<Component::Transform>(m, "Transform")
			.def(py::init<>())
			.def(py::init<Vector3f, Vector3f, Vector3f>())
			.def_property("position",
				&Component::Transform::GetPosition,
				&Component::Transform::SetPosition,
				py::return_value_policy::reference)
			.def_property("rotation",
				&Component::Transform::GetRotation,
				&Component::Transform::SetRotation,
				py::return_value_policy::reference)
			.def_property("scale",
				&Component::Transform::GetScale,
				&Component::Transform::SetScale,
				py::return_value_policy::reference)
			.def_property_readonly("forward", &Component::Transform::Forward, py::return_value_policy::reference)
			.def_property_readonly("up", &Component::Transform::Up, py::return_value_policy::reference)
			.def("translate", &Component::Transform::Translate, py::return_value_policy::reference)
			.def("rotate", &Component::Transform::Rotate, py::return_value_policy::reference)
			.def("rotate_around", &Component::Transform::RotateAround, py::return_value_policy::reference)
			.def("reset", &Component::Transform::Reset, py::return_value_policy::reference);

		py::class_<Component::Camera>(m, "Camera")
			.def(py::init<>());

		py::class_<Component::Light>(m, "Light")
			.def(py::init<>())
			.def_readwrite("type", &Component::Light::LightType, py::return_value_policy::reference)
			.def_readwrite("color", &Component::Light::Color, py::return_value_policy::reference)
			.def_readwrite("range", &Component::Light::Range, py::return_value_policy::reference)
			.def_readwrite("angle", &Component::Light::Angle, py::return_value_policy::reference)
			.def_readwrite("smoothness", &Component::Light::Smoothness, py::return_value_policy::reference)
			.def("reset", &Component::Light::Reset, py::return_value_policy::reference);

		// API Calls

		m.def("is_key_pressed", &Input::IsKeyPressed, py::return_value_policy::reference);
		m.def("is_mouse_button_pressed", &Input::IsMouseButtonPressed, py::return_value_policy::reference);
		m.def("get_mouse_position", &Input::GetMousePos, py::return_value_policy::reference);
		m.def("get_mouse_x", &Input::GetMouseX, py::return_value_policy::reference);
		m.def("get_mouse_y", &Input::GetMouseY, py::return_value_policy::reference);

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
