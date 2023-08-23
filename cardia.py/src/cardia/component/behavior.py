import abc

import cardia_native as _cd
from .transform import Transform
from typing import TypeVar

T = TypeVar("T")


class Behavior(_cd.Behavior):
    """
    Constructor is declared abstractmethod to not force the user to call it.
    It'll be implicitly called by C++ when instantiated.
    """
    @abc.abstractmethod
    def __init__(self):
        _cd.Behavior.__init__(self)

    @property
    def transform(self) -> Transform:
        return _cd.Behavior.transform.fget(self)

    def get_component(self, component_type: type[T]) -> T:
        return _cd.Behavior.get_component(self, component_type)

    def on_create(self): ...
    def on_update(self): ...
