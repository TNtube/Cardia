from cardia import Behavior, Key, on_key_pressed, Time, Vector3, Input, Mouse
import cardia.log as log
import colorsys
from Greeting import Greeting


class Moving(Behavior):
    def __init__(self):
        self.velocity: int
        self._tick_count: float
        self.text: str
        self.color: Vector3
        self.greeting: Greeting
        self.lst: list[str]

    def on_create(self):
        # self.velocity = 5
        pass

    def on_update(self):
        self.transform.rotation.y += self.velocity * Time.delta_time.seconds()
        if Input.is_mouse_button_pressed(Mouse.Left):
            log.trace("message from python")
            log.trace("|".join(map(str, self.lst)))

    @on_key_pressed(Key.Left)
    def move_left(self):
        self.transform.position.x -= self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Right)
    def move_right(self):
        self.transform.position.x += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Down)
    def move_down(self):
        self.transform.position.z -= self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Up)
    def move_top(self):
        self.transform.position.z += self.velocity * Time.delta_time.seconds()

    @on_key_pressed(Key.Space)
    def greet(self):
        self.greeting.hello_world()
