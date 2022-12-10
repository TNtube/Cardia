import cardia_native as _cd
from ..math.vector import Vector3
import enum


class LightType(enum.IntEnum):
    Directional = 0
    Point = 0
    Spot = 0


class PointLight(_cd.Light):
    def __init__(self):
        super().__init__()
        self.type: LightType = LightType.Directional
        self.color: Vector3 = Vector3.one
        self.range: float = 5
        self.angle: float = 35
        self.smoothness: float = 1

    def reset(self):
        pass
