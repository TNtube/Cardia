import cardia_native as _cd
from .keycode import Key, Mouse
from ..math import Vector2


class Input:
    @staticmethod
    def is_key_pressed(key: Key) -> bool:
        return _cd.is_key_pressed(int(key))

    @staticmethod
    def is_mouse_button_pressed(key: Mouse) -> bool:
        return _cd.is_mouse_button_pressed(int(key))

    @staticmethod
    def get_mouse_position() -> Vector2:
        return _cd.get_mouse_position()

    @staticmethod
    def get_mouse_x() -> float:
        return _cd.get_mouse_x()

    @staticmethod
    def get_mouse_y() -> float:
        return _cd.get_mouse_y()
