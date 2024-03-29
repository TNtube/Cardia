import abc
import typing

import cardia_native as _cd
from .transform import Transform
from typing import TypeVar, Union
import asyncio

from ..core.entity import Entity

T = TypeVar("T")


class Behavior(_cd.Behavior):
    """
    Constructor is declared abstractmethod to not force the user to call it.
    It'll be implicitly called by C++ when instantiated.
    """

    @property
    def entity(self) -> Entity:
        return _cd.Behavior.entity.fget(self)

    @property
    def transform(self) -> Transform:
        return self.entity.transform

    def get_component(self, component_type: type[T]) -> T:
        return self.entity.get_component(component_type)

    def add_component(self, component_type: type[T]) -> T:
        return self.entity.add_component(component_type)

    def has_component(self, component_type: type[T]) -> bool:
        return self.entity.has_component(component_type)

    def spawn(self, name: str, parent: Entity = None) -> Entity:
        return _cd.Behavior.spawn(self, name, parent)

    def on_create(self): ...
    def on_update(self): ...
    def on_destroy(self): ...

    def run_coroutine(self, coroutine: Union[typing.Coroutine, typing.Awaitable, typing.Generator]) -> asyncio.Task:
        return _cd.Behavior.run_coroutine(self, coroutine)

