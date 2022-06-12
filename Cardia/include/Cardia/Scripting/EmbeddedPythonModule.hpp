#pragma once

#include <pybind11/pybind11.h>
#include <Cardia/ECS/Entity.hpp>
#include <pybind11/embed.h>

#include "EntityBehavior.hpp"
#include "Cardia/Core/KeyCodes.hpp"
#include "Cardia/Core/Input.hpp"
#include "Cardia/ECS/Components.hpp"

namespace Cardia
{
        namespace py = pybind11;

        PYBIND11_EMBEDDED_MODULE(cardia, m) {
                m.doc() = "Cardia Python Bindings";
                using namespace Cardia;

                // glm utilities
                py::class_<glm::vec2>(m, "vec2")
                        .def(py::init<float>())
                        .def(py::init<float, float>())
                        .def_readwrite("x", &glm::vec2::x, py::return_value_policy::reference)
                        .def_readwrite("y", &glm::vec2::y, py::return_value_policy::reference)
                        .def_readwrite("r", &glm::vec2::r, py::return_value_policy::reference)
                        .def_readwrite("g", &glm::vec2::g, py::return_value_policy::reference);

                py::class_<glm::vec3>(m, "vec3")
                        .def(py::init<float>())
                        .def(py::init<float, float, float>())
                        .def_readwrite("x", &glm::vec3::x, py::return_value_policy::reference)
                        .def_readwrite("y", &glm::vec3::y, py::return_value_policy::reference)
                        .def_readwrite("z", &glm::vec3::z, py::return_value_policy::reference)
                        .def_readwrite("r", &glm::vec3::r, py::return_value_policy::reference)
                        .def_readwrite("g", &glm::vec3::g, py::return_value_policy::reference)
                        .def_readwrite("b", &glm::vec3::b, py::return_value_policy::reference);
                
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
                        .def_readwrite("position", &Component::Transform::position, py::return_value_policy::reference)
                        .def_readwrite("rotation", &Component::Transform::rotation, py::return_value_policy::reference)
                        .def_readwrite("scale", &Component::Transform::scale, py::return_value_policy::reference);


                // Core objects

                auto keycodes = py::enum_<Key::Key>(m, "key");
                keycodes.value("space", Key::Space)
                        .value("apostrophe", Key::Apostrophe)
                        .value("comma", Key::Comma)
                        .value("minus", Key::Minus)
                        .value("period", Key::Period)
                        .value("slash", Key::Slash)
                        .value("n0", Key::N0)
                        .value("n1", Key::N1)
                        .value("n2", Key::N2)
                        .value("n3", Key::N3)
                        .value("n4", Key::N4)
                        .value("n5", Key::N5)
                        .value("n6", Key::N6)
                        .value("n7", Key::N7)
                        .value("n8", Key::N8)
                        .value("n9", Key::N9)
                        .value("semicolon", Key::Semicolon)
                        .value("equal", Key::Equal)
                        .value("lbracket", Key::LBracket)
                        .value("backslash", Key::BackSlash)
                        .value("rbracket", Key::RBracket)
                        .value("grave_accent", Key::GraveAccent)
                        .value("right", Key::Right)
                        .value("left", Key::Left)
                        .value("down", Key::Down)
                        .value("up", Key::Up)
                        .value("left_alt", Key::LeftAlt)
                        .value("right_alt", Key::RightAlt)
                        .value("left_ctrl", Key::LeftCtrl)
                        .value("right_ctrl", Key::RightCtrl);

                for (int v = Key::A; v < static_cast<int>(Key::Z); v++)
                {
                        const auto name = std::string(1, static_cast<char>(v));
                        keycodes.value(name.c_str(), static_cast<Key::Key>(v));
                }

                py::enum_<Mouse::Mouse>(m, "mouse")
                        .value("left", Mouse::Left)
                        .value("right", Mouse::Right)
                        .value("middle", Mouse::Middle);

                py::class_<Input>(m, "Input")
                        .def_static("is_key_pressed", &Input::isKeyPressed, py::return_value_policy::reference)
                        .def_static("is_mouse_button_pressed", &Input::isMouseButtonPressed, py::return_value_policy::reference)
                        .def_static("get_mouse_position", &Input::getMousePos, py::return_value_policy::reference)
                        .def_static("get_mouse_x", &Input::getMouseX, py::return_value_policy::reference)
                        .def_static("get_mouse_y", &Input::getMouseY, py::return_value_policy::reference);
                
                py::class_<Entity> PyEntity(m, "Entity");

                py::class_<EntityBehavior>(m, "EntityBehavior")
                        .def(py::init<const Entity&>(), py::return_value_policy::reference)
                        .def("start", &EntityBehavior::start, py::return_value_policy::reference)
                        .def("update", &EntityBehavior::update, py::return_value_policy::reference)
                        .def_property_readonly("transform", [](EntityBehavior& behavior)
                        {
                                return &behavior.m_Entity.getComponent<Component::Transform>();
                        }, py::return_value_policy::reference);
        }
}