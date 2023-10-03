import cardia_native as _cd

from ..component.transform import Transform
from typing import TypeVar

T = TypeVar("T")


class Entity(_cd.Entity):
    @property
    def transform(self) -> Transform:
        return _cd.Entity.transform.fget(self)

    def get_component(self, component_type: type[T]) -> T:
        return _cd.Entity.get_component(self, component_type)

    def add_component(self, component_type: type[T]) -> T:
        return _cd.Entity.add_component(self, component_type)

    def has_component(self, component_type: type[T]) -> bool:
        return _cd.Entity.has_component(self, component_type)

    def kill(self):
        return _cd.Entity.kill(self)
