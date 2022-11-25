import cardia_native as _cd
from ..math.vector import Vector3


class PointLight2D(_cd.PointLight2D):
    def __init__(self):
        super().__init__()
        self.color: Vector3 = Vector3.one
        self.range: float = 1

    def reset(self):
        pass
