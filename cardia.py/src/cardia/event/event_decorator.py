import cardia_native as _cd
import functools
from types import MethodType
from .keycode import Key, Mouse


class RegisterMethod:
    def __init__(self, func):
        functools.update_wrapper(self, func)
        self.func = func
        self.cls = None

    def __set_name__(self, cls, __):
        self.cls = cls
        # _cd.register_update_method(cls, self.func.__name__)

    def __call__(self, *args, **kwargs):
        self.func(self, *args, **kwargs)

    def __get__(self, inst, _):  # Thanks @horus-4ever
        return MethodType(self, inst)


def on_key_pressed(key: Key):
    class Inner(RegisterMethod):
        def __call__(self, *args, **kwargs):
            if _cd.is_key_pressed(int(key)):
                self.func(*args, **kwargs)

    return Inner


def on_mouse_clicked(button: Mouse):
    class Inner(RegisterMethod):
        def __call__(self, *args, **kwargs):
            if _cd.is_mouse_button_pressed(int(button)):
                self.func(*args, **kwargs)

    return Inner

