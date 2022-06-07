#include <pybind11/pybind11.h>
#include <Cardia/ECS/Entity.hpp>

#include "Cardia/ECS/Components.hpp"

namespace py = pybind11;

PYBIND11_MODULE(cardia, m) {
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
        
        py::class_<Entity>(m, "Entity")
                .def(py::init<>())
                .def(py::init<const Entity&>())
                .def("get_transform", &Entity::getComponent<Component::Transform>, py::return_value_policy::reference);
}