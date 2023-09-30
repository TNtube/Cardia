import cardia_native as _cd
from numbers import Number
from typing import Union


class ClassProperty:
    def __init__(self, func):
        self.func = func

    def __get__(self, instance, cls):
        return self.func(cls)


class Vector2(_cd.vec2):
    def __init__(self, x: int = 0, y: int = 0):
        self.x: int
        self.y: int
        super().__init__(x, y)

    def __add__(self, other: Union[Number, 'Vector2']):
        if isinstance(other, Number):
            return _cd.vec2._add__scalar(self, float(other))
        return super().__add__(other)

    def __sub__(self, other: Union[Number, 'Vector2']):
        if isinstance(other, Number):
            return _cd.vec2._sub__scalar(self, float(other))
        return super().__sub__(other)

    def __mul__(self, other: Union[Number, 'Vector2']):
        if isinstance(other, Number):
            return _cd.vec2._mul__scalar(self, float(other))
        return super().__mul__(other)

    def __truediv__(self, other: Union[Number, 'Vector2']):
        if isinstance(other, Number):
            return _cd.vec2._truediv__scalar(self, float(other))
        return super().__truediv__(other)

    def __len__(self):
        return super().size()

    def length(self) -> float:
        pass

    def lerp(self, end: 'Vector2', step: float):
        return super().lerp(end, step)


class Vector3(_cd.vec3):
    def __init__(self, x: int = 0, y: int = 0, z: int = 0):
        self.x: int
        self.y: int
        self.z: int
        super().__init__(x, y, z)

    def __add__(self, other: Union[Number, 'Vector3']):
        if isinstance(other, Number):
            return _cd.vec3._add__scalar(self, float(other))
        return super().__add__(other)

    def __sub__(self, other: Union[Number, 'Vector3']):
        if isinstance(other, Number):
            return _cd.vec3._sub__scalar(self, float(other))
        return super().__sub__(other)

    def __mul__(self, other: Union[Number, 'Vector3']):
        if isinstance(other, Number):
            return _cd.vec3._mul__scalar(self, float(other))
        return super().__mul__(other)

    def __truediv__(self, other: Union[Number, 'Vector3']):
        if isinstance(other, Number):
            return _cd.vec3._truediv__scalar(self, float(other))
        return super().__truediv__(other)

    def __len__(self):
        return super().size()

    def __eq__(self, other: 'Vector3'):
        return super().__eq__(other)

    def cross(self, other: 'Vector3'):
        return super().cross(other)

    def length(self) -> float:
        return super().length()

    def lerp(self, end: 'Vector3', step: float):
        return super().lerp(end, step)

    @ClassProperty
    def back(cls):
        return Vector3(0, 0, -1)

    @ClassProperty
    def down(cls):
        return Vector3(0, -1, 0)

    @ClassProperty
    def forward(cls):
        return Vector3(0, 0, 1)

    @ClassProperty
    def left(cls):
        return Vector3(-1, 0, 0)

    @ClassProperty
    def one(cls):
        return Vector3(1, 1, 1)

    @ClassProperty
    def right(cls):
        return Vector3(1, 0, 0)

    @ClassProperty
    def up(cls):
        return Vector3(0, 1, 0)

    @ClassProperty
    def zero(cls):
        return Vector3(0, 0, 0)


class Vector4(_cd.vec4):
    def __init__(self, x: int = 0, y: int = 0, z: int = 0, w: int = 0):
        self.x: int
        self.y: int
        self.z: int
        self.w: int
        super().__init__(x, y, z, w)

    def __add__(self, other: Union[Number, 'Vector4']):
        if isinstance(other, Number):
            return _cd.vec4._add__scalar(self, float(other))
        return super().__add__(other)

    def __sub__(self, other: Union[Number, 'Vector4']):
        if isinstance(other, Number):
            return _cd.vec4._sub__scalar(self, float(other))
        return super().__sub__(other)

    def __mul__(self, other: Union[Number, 'Vector4']):
        if isinstance(other, Number):
            return _cd.vec4._mul__scalar(self, float(other))
        return super().__mul__(other)

    def __truediv__(self, other: Union[Number, 'Vector4']):
        if isinstance(other, Number):
            return _cd.vec4._truediv__scalar(self, float(other))
        return super().__truediv__(other)

    def __len__(self):
        return super().size()

    def length(self) -> float:
        pass

    def lerp(self, end: 'Vector4', step: float):
        return super().lerp(end, step)
