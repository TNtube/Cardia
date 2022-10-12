import cardia_native as _cd
from .transform import Transform
from typing import TypeVar

T = TypeVar("T")


class Behavior:
    def __init__(self):
        self.id = None

    @property
    def transform(self) -> Transform:
        return _cd.get_native_transform(self.id)

    @transform.setter
    def transform(self, t: Transform):
        _cd.set_native_transform(self.id, t)

    def get_component(self, component_type: type[T]) -> T:
        out = {'output': None}
        _cd.get_component(self.id, component_type, out)
        return out['output']

    def on_create(self):
        pass

    def on_update(self):
        pass
