import cardia_native as _cd
from ..math.vector import Vector3


class Transform(_cd.Transform):
    def __init__(self, position=Vector3.zero, rotation=Vector3.zero, scale=Vector3.one):
        super().__init__(position, rotation, scale)
        self.position: Vector3 = position
        self.rotation: Vector3 = rotation
        self.scale: Vector3 = scale

    @property
    def position(self):
        return _cd.Transform.position.fget(self)

    @position.setter
    def position(self, pos: Vector3):
        _cd.Transform.position.fset(self, pos)

    @property
    def rotation(self):
        return _cd.Transform.rotation.fget(self)

    @rotation.setter
    def rotation(self, rot: Vector3):
        _cd.Transform.rotation.fset(self, rot)

    @property
    def scale(self):
        return _cd.Transform.rotation.fget(self)

    @scale.setter
    def scale(self, scale_: Vector3):
        _cd.Transform.scale.fset(self, scale_)

    def translate(self, translation: Vector3):
        _cd.Transform.translate(self, translation)

    def rotate(self, rotation: Vector3):
        _cd.Transform.rotate(self, rotation)

    def rotate_around(self, point: Vector3, axis: Vector3, angle: float):
        _cd.Transform.rotate_around(self, point, axis, angle)

    def reset(self):
        pass
