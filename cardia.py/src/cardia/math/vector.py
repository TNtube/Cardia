import cardia_native as _cd


class ClassProperty:
    def __init__(self, func):
        self.func = func

    def __get__(self, instance, cls):
        return self.func(cls)


class Vector2(_cd.vec2):
    def __init__(self, x: int, y: int):
        self.x: int
        self.y: int
        super().__init__(x, y)

    def length(self) -> float:
        pass

    @classmethod
    def lerp(cls, start: 'Vector2', end: 'Vector2', step: float):
        return super().lerp(start, end, step)


class Vector3(_cd.vec3):
    def __init__(self, x: int, y: int, z: int):
        self.x: int
        self.y: int
        self.z: int
        super().__init__(x, y, z)

    def length(self) -> float:
        pass

    @classmethod
    def lerp(cls, start: 'Vector3', end: 'Vector3', step: float):
        return super().lerp(start, end, step)

    @ClassProperty
    def zero(cls):
        return Vector3(0, 0, 0)

    @ClassProperty
    def one(cls):
        return Vector3(1, 1, 1)
