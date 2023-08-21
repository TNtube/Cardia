import math

from cardia import Behavior, Key, on_key_pressed, Time, Vector3, Input, Mouse
import cardia.log as log
import colorsys
from Greeting import Greeting


def default_bool():
    return True


class Moving(Behavior):
    @staticmethod
    def default_velocity():
        return 5

    def __init__(self):
        super().__init__()
        self.velocity: int = Moving.default_velocity()
        self._tick_count: float = 5.27
        self.text: str = "Hello Cardia"
        self.color: Vector3 = Vector3(17, 15, 13)
        self.show_text: bool = default_bool()

    def on_create(self):
        self.velocity = 5
        pass

    def on_update(self):
        self.transform.rotate(
            Vector3(0, self.velocity * Time.delta_time.seconds(), 0)
        )

        self.transform.translate(
            Vector3(math.sin(self._tick_count) * self.velocity * Time.delta_time.seconds(), 0, 0)
        )

        if Input.is_mouse_button_pressed(Mouse.Left) and self.show_text:
            log.trace(f"message from python {Input.get_mouse_x()} {Input.get_mouse_y()}")

        self._tick_count += Time.delta_time.seconds()
