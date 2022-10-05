import cardia_native as _cd
from ..math.vector import Vector3


class Transform(_cd.Transform):
    def __init__(self, position=Vector3.zero, rotation=Vector3.zero, scale=Vector3.one):
        super().__init__(position, rotation, scale)
        self.position: Vector3 = position
        self.rotation: Vector3 = rotation
        self.scale: Vector3 = scale

    def reset(self):
        pass
