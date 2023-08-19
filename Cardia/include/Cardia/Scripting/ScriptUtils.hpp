#pragma once

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace Cardia
{
	template <typename T>
	inline bool IsSubclass(const py::type& cls)
	{
		if (!PyObject_IsInstance(cls.ptr(), (PyObject *)&PyType_Type)) {
			return false; // not a type
		}
		const auto pyType = py::type::of<T>();
		if (!pyType) return false;
		auto out = PyType_IsSubtype(reinterpret_cast<PyTypeObject*>(cls.ptr()), reinterpret_cast<PyTypeObject*>(pyType.ptr()));
		return out > 0;
	}

	template<>
	inline bool IsSubclass<int>(const py::type& cls)
	{
		if (!PyObject_IsInstance(cls.ptr(), (PyObject *)&PyType_Type)) {
			return false; // not a type
		}

		return PyType_FastSubclass(reinterpret_cast<PyTypeObject*>(cls.ptr()), Py_TPFLAGS_LONG_SUBCLASS) > 0;
	}

	template <>
	inline bool IsSubclass<float>(const py::type& cls)
	{
		return PyType_IsSubtype(reinterpret_cast<PyTypeObject*>(cls.ptr()), &PyFloat_Type) > 0;
	}

	template<>
	inline bool IsSubclass<std::string>(const py::type& cls)
	{
		return PyType_IsSubtype(reinterpret_cast<PyTypeObject*>(cls.ptr()), &PyUnicode_Type) > 0;
	}

	inline bool IsSubclass(const py::type& cls, const py::type& base)
	{
		return PyType_IsSubtype(reinterpret_cast<PyTypeObject*>(cls.ptr()), reinterpret_cast<PyTypeObject*>(base.ptr())) > 0;
	}
}